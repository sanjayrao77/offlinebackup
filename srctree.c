/*
 * srctree.c - keep track of origin files
 * Copyright (C) 2021 Sanjay Rao
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#define _FILE_OFFSET_BITS	64
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#define DEBUG
#include "common/conventions.h"
#include "common/fastmem.h"
#include "base.h"
#include "srctodest.h"
#include "options.h"
#include "dirlist.h"
#include "dirtree.h"
#include "desttree.h"
#include "destbyname.h"
#include "srctree.h"
#include "backupfiles.h"
#include "fileio.h"


CLEARFUNC(srctree);

int init_srctree(struct srctree *st, struct fastmem *fastmem_in) {
struct fastmem *fastmem;

if (fastmem_in) {
	fastmem=st->fastmem=fastmem_in;
} else {
	if (!(fastmem=new_fastmem(0))) GOTOERROR;
	st->fastmem=fastmem;
	st->fastmemtofree=fastmem;
}

if (!(st->onepathmax=alloc_fastmem(fastmem,PATH_MAX+1))) GOTOERROR;
return 0;
error:
	return -1;
}

void deinit_srctree(struct srctree *st) {
if (st->fastmemtofree) free_fastmem(st->fastmemtofree);
}

static int addfile(struct srctree *ft, struct dirtree *dft, char *name, struct srctodest *std, struct desttree *dest,
		struct options *options, struct uistr *dirsofar) {
struct filenode_srctree *node;
struct filenode_desttree *dnode;
struct stat st;
unsigned int n;

n=strlen(name);
if (dirsofar->ui+n>=PATH_MAX+1) GOTOERROR;
memcpy(dirsofar->str+dirsofar->ui,name,n+1); // if we update dirsofar->ui then we'll have to reset it later
if (stat(dirsofar->str,&st)) {
	if (errno==ENOENT) {
		fprintf(stderr,"Skipping %s\n",ft->onepathmax);
		return 0;
	}
	GOTOERROR;
}

{
	struct filenode_desttree m;
	m.topdir=std->destpath;
	m.name.str=name;
	m.name.ui=n; // unused
	if (dft->dirlist.nameslash.str) m.dir=&dft->dirlist;
	else m.dir=NULL;

//	fprintf(stderr,"Looking for \"%s\" \"%s\"\n",m.topdir,m.name);

	dnode=find2_destbyname(dest->topnodebyname,&m);

#if 0
	if (dnode) {
		fprintf(stderr,"Found dnode %s\n",ft->onepathmax);
	} else {
		fprintf(stderr,"Didn't find dnode %s\n",ft->onepathmax);
	}
#endif

	if (dnode) {
		dnode->isinsrcdir=1; // prevents dest filename from being deleted after new data is copied over it
		ft->space.matched+=dnode->st_size;

// ext4 supports tv_nsec, ext2/3 will have 0 for tv_nsec
// it can't really matter
//#define CHECKNSEC
		if ( (dnode->st_size==st.st_size)
				&& (dnode->st_mtim.tv_sec==st.st_mtim.tv_sec) 
#ifdef CHECKNSEC
				&& (dnode->st_mtim.tv_nsec==st.st_mtim.tv_nsec) 
#endif
				) {
			int ismatch=1;
			int didverifycheck=0;
			int r;
			if (options->isverifymatch) {
				didverifycheck=1;
				r=verifyfile2_fileio(&ismatch,dirsofar->str,dest,dnode,options);
				if (r) {
					if (r==-3) { // dest read error
						ismatch=0;
						ft->counts.readerrors+=1;
						if (options->isverbose) {
							fputs("{ action: \"readerror\", name: \"",stdout);
							(void)printfilename_desttree(stdout,dnode);
							fputs("\" },\n",stdout);
						}
					} else GOTOERROR;
				} else {
					if (!ismatch) {
						if (options->ismismatchquit) {
							fprintf(stderr,"Data mismatch! Quiting on %s\n",dirsofar->str);
							GOTOERROR;
						}
					}
				}
				if (ismatch) {
					ft->counts.matchesverified+=1;
					if (options->isinteractive) fprintf(stderr,"%"PRIu64": Verified file %s\r",ft->counts.matchesverified,name);
				}
			}
			if (ismatch) {
				ft->counts.matched+=1;
	//			fprintf(stderr,"Matched %s\n",ft->onepathmax);
				if (options->maximumage) {
					if (st.st_mtim.tv_sec < options->srcexpiration) {
						if (st.st_size) {
							if (didverifycheck) { // only delete if we _know_ we have another copy on dest
								if (expiresourcefile_fileio(options,ft->onepathmax)) GOTOERROR;
							}
						}
					}
				}
				return 0;
			}
		}
		if (options->allow0match) {
			if (0==st.st_size) { // special case, allow blank source to be matched as well (this allows offline backups to persist)
	//			fprintf(stderr,"Matched 0-byte %s\n",ft->onepathmax);
				ft->counts.matched+=1;
				return 0;
			}
		}

		if (options->isverbose) {
			if ((dnode->st_size!=st.st_size) && (dnode->st_mtim.tv_sec!=st.st_mtim.tv_sec)) {
				fprintf(stdout,"{ action: \"mismatch\", name: \"%s\", dest_size: %"PRIu64", src_size: %"PRIu64", dest_mtime: %"PRIu64", "\
						"src_mtime: %"PRIu64" },\n",
						dirsofar->str, dnode->st_size, st.st_size, (uint64_t)dnode->st_mtim.tv_sec,(uint64_t)st.st_mtim.tv_sec);
			} else if (dnode->st_mtim.tv_sec!=st.st_mtim.tv_sec) {
				fprintf(stdout,"{ action: \"mismatch\", name: \"%s\", dest_mtime: %"PRIu64", src_mtime: %"PRIu64" },\n",
						dirsofar->str, (uint64_t)dnode->st_mtim.tv_sec,(uint64_t)st.st_mtim.tv_sec);
			} else if (dnode->st_size!=st.st_size) {
				fprintf(stdout,"{ action: \"mismatch\", name: \"%s\", dest_size: %"PRIu64", src_size: %"PRIu64" },\n",
						dirsofar->str, dnode->st_size, st.st_size);
			} else {
				fprintf(stdout,"{ action: \"mismatch\", name: \"%s\" },\n", dirsofar->str);
			}
		}

		dnode->ismismatch=1;
		ft->counts.mismatched+=1;

		if (dnode->st_size > st.st_size) {
			ft->space.mismatched+=dnode->st_size - st.st_size;
		} else if (dnode->st_size != st.st_size) { // dest < ssrc
			ft->space.unmatched+=st.st_size - dnode->st_size;
		}
	} else {
		// only exists in source dir
		ft->space.unmatched+=st.st_size;
		ft->counts.unmatched+=1;
	}
}

if (!(node=FCALLOC(ft->fastmem,struct filenode_srctree,1))) GOTOERROR;
if (!(node->name.str=(char *)memdup_fastmem(ft->fastmem,(unsigned char *)name,n+1))) GOTOERROR;
node->name.ui=n;

node->dir=dft;
node->st_mtim=st.st_mtim;
node->st_size=st.st_size;

(void)addfile_dirtree(dft,node);
return 0;
error:
	return -1;
}

static int adddir(struct srctree *ft, struct dirtree *parent, char *name, struct srctodest *std, struct desttree *dest,
		struct options *options, struct uistr *dirsofar);

static int addfiles(struct srctree *ft, struct dirtree *dft, struct srctodest *std, struct desttree *dest, struct options *options,
		struct uistr *dirsofar) {
DIR *d=NULL;
unsigned int dirsofarui_in;

dirsofarui_in=dirsofar->ui;
if (dft->dirlist.nameslash.str) {
	if (dirsofarui_in+dft->dirlist.nameslash.ui>=PATH_MAX+1) GOTOERROR;
	memcpy(dirsofar->str+dirsofarui_in,dft->dirlist.nameslash.str,dft->dirlist.nameslash.ui);
	dirsofarui_in+=dft->dirlist.nameslash.ui;
	dirsofar->ui=dirsofarui_in;
}
dirsofar->str[dirsofarui_in]='\0';
d=opendir(dirsofar->str);
if (!d) {
	fprintf(stderr,"Unable to open \"%s\"\n",dirsofar->str);
	GOTOERROR;
}
while (1) {
	struct dirent *de;
	de=readdir(d);
	if (!de) break;
	switch (de->d_type) {
		case DT_REG:
//			fprintf(stderr,"Checking file %s\n",de->d_name);
#if 0
printnodes_srcbyname(ft->topnodebyname);
#endif
			if (addfile(ft,dft,de->d_name,std,dest,options,dirsofar)) GOTOERROR;
			break;
		case DT_DIR:
			if (de->d_name[0]=='.') {
				if (!de->d_name[1]) continue;
				if ((de->d_name[1]=='.')&&(de->d_name[2]=='\0')) continue;
			}
//			fprintf(stderr,"Checking subdir %s\n",de->d_name);
			if (adddir(ft,dft,de->d_name,std,dest,options,dirsofar)) GOTOERROR;
			dirsofar->ui=dirsofarui_in;
			break;
	}
}
(ignore)closedir(d);
return 0;
error:
	if (d) (ignore)closedir(d);
	return -1;
}

static int adddir(struct srctree *ft, struct dirtree *parent, char *name, struct srctodest *std, struct desttree *dest,
		struct options *options, struct uistr *dirsofar) {
struct dirtree *dst;
if (options->isinteractive) fprintf(stderr,"Entering directory %s\r",name);
if (!(dst=make_dirtree(ft->fastmem,parent,name,NULL))) GOTOERROR;
if (addfiles(ft,dst,std,dest,options,dirsofar)) GOTOERROR;
return 0;
error:
	return -1;
}


int adddir_srctree(struct srctree *ft, struct srctodest *std, struct desttree *dest, struct options *options) {
struct dirtree *dt;
struct uistr dirsofar;
// passed dirs must be separate in the fs ; redundant trees will cause all sorts of weirdness
//fprintf(stderr,"Adding dir %s to %s\n",std->srcpath,std->destpath);
if (!(dt=make_dirtree(ft->fastmem,NULL,NULL,std))) GOTOERROR;

dt->nextsibling=ft->firstdir;
ft->firstdir=dt;

dirsofar.str=ft->onepathmax;
dirsofar.ui=std->srcpath.ui;
memcpy(dirsofar.str,std->srcpath.str,dirsofar.ui);
if (addfiles(ft,dt,std,dest,options,&dirsofar)) GOTOERROR;
if (options->isinteractive) fputs("\n",stderr); // for \r interactive status
return 0;
error:
	return -1;
}

void printcounts_srctree(FILE *fout, struct srctree *st) {
fprintf(fout,"source_counts: { matched: %"PRIu64", unmatched: %"PRIu64", mismatched: %"PRIu64", matchesverified: %"PRIu64", readerrors: %"PRIu64" },\n",
		st->counts.matched,st->counts.unmatched,st->counts.mismatched,st->counts.matchesverified,st->counts.readerrors);
}
void print_srctree(FILE *fout, struct srctree *src) {
(void)print_dirtree(fout,src->firstdir);
}

void printsrcfilename_srctree(FILE *fout, struct filenode_srctree *fn) {
(void)printsrcpath_dirtree(fout,fn->dir);
(ignore)fputuistr(&fn->name,fout);
}

void printdestfilename_srctree(FILE *fout, struct filenode_srctree *fn) {
(void)printdestpath_dirtree(fout,fn->dir);
(ignore)fputuistr(&fn->name,fout);
}
