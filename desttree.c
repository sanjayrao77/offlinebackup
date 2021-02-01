/*
 * desttree.c - destination file info
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

#include "dirlist.h"
#include "desttree.h"
#include "destbyname.h"
#include "destbytime.h"

CLEARFUNC(desttree);

int init_desttree(struct desttree *dt, struct fastmem *fastmem_in) {
struct fastmem *fastmem;

if (fastmem_in) {
	fastmem=dt->fastmem=fastmem_in;
} else {
	if (!(fastmem=new_fastmem(0))) GOTOERROR;
	dt->fastmem=fastmem;
	dt->fastmemtofree=fastmem;
}

if (!(dt->onepathmax=alloc_fastmem(fastmem,PATH_MAX+1))) GOTOERROR;
if (!(dt->twopathmax=alloc_fastmem(fastmem,PATH_MAX+1))) GOTOERROR;
return 0;
error:
	return -1;
}

void deinit_desttree(struct desttree *dt) {
if (dt->fastmemtofree) free_fastmem(dt->fastmemtofree);
}

static int adddir(struct desttree *ft, struct uistr *dirsofar, struct dirlist *parent, char *name, struct uistr *topdir);

static int addfile(struct desttree *ft, struct uistr *dirsofar, struct dirlist *dft, char *name, struct uistr *topdir) {
struct filenode_desttree *node;
struct stat st;
unsigned int n;

n=strlen(name);
if (dirsofar->ui+n>=PATH_MAX+1) GOTOERROR;
memcpy(dirsofar->str+dirsofar->ui,name,n+1); // if we update dirsofar->ui then we'll have to reset it later
if (stat(dirsofar->str,&st)) {
	if (errno==ENOENT) {
		fprintf(stderr,"Can't stat destination %s\n",ft->onepathmax);
		return 0;
	}
	GOTOERROR;
}

if (!(node=FCALLOC(ft->fastmem,struct filenode_desttree,1))) GOTOERROR;
if (!(node->name.str=(char *)memdup_fastmem(ft->fastmem,(unsigned char *)name,n+1))) GOTOERROR;
node->name.ui=n;

ft->space.all+=st.st_size;
ft->counts.all+=1;

node->topdir=*topdir; // used for tree cmp function, so avoid pointer
node->dir=dft;
node->st_mtim=st.st_mtim;
node->st_size=st.st_size;

(void)addnode2_destbyname(&ft->topnodebyname,node);
(void)addnode2_destbytime(&ft->topnodebytime,node);
return 0;
error:
	return -1;
}

static int addfiles(struct desttree *ft, struct uistr *dirsofar, struct dirlist *dft, struct uistr *topdir) {
DIR *d=NULL;
unsigned int dirsofarui_in;

dirsofarui_in=dirsofar->ui;
if (dft) {
	if (dirsofarui_in+dft->nameslash.ui>=PATH_MAX+1) GOTOERROR;
	memcpy(dirsofar->str+dirsofarui_in,dft->nameslash.str,dft->nameslash.ui);
	dirsofarui_in+=dft->nameslash.ui;
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
#if 0
			fprintf(stderr,"Adding file %s\n",de->d_name);
#endif
#if 0
printnodes_destbyname(ft->topnodebyname);
#endif
			if (addfile(ft,dirsofar,dft,de->d_name,topdir)) GOTOERROR;
//			dirsofar->ui=dirsofarui_in; // no need
			break;
		case DT_DIR:
			if (de->d_name[0]=='.') {
				if (!de->d_name[1]) continue;
				if ((de->d_name[1]=='.')&&(de->d_name[2]=='\0')) continue;
			}
#if 0
			fprintf(stderr,"Adding subdir %s\n",de->d_name);
#endif
			if (adddir(ft,dirsofar,dft,de->d_name,topdir)) GOTOERROR;
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

static int adddir(struct desttree *ft, struct uistr *dirsofar, struct dirlist *parent, char *name, struct uistr *topdir) {
struct dirlist *dft;
if (!(dft=make_dirlist(ft->fastmem,parent,name))) GOTOERROR;
if (addfiles(ft,dirsofar,dft,topdir)) GOTOERROR;
return 0;
error:
	return -1;
}

int adddir_desttree(struct desttree *ft, struct uistr *topdir) {
struct uistr uistr;
// passed dirs must be separate in the fs ; redundant trees will generate warnings and/or bomb out
// passed dirs must be on the same device for unlinking to make sense
#if 0
fprintf(stderr,"Adding dir %s\n",topdir->str);
#endif
uistr.str=ft->onepathmax;
uistr.ui=topdir->ui;
memcpy(uistr.str,topdir->str,uistr.ui);
if (addfiles(ft,&uistr,NULL,topdir)) GOTOERROR;
return 0;
error:
	return -1;
}

void printcounts_desttree(FILE *fout, struct desttree *dt) {
fprintf(fout,"destination_stats: { allfiles: %"PRIu64" },\n",
		dt->counts.all);
}

static void fixtimelist(struct filenode_desttree ***lastptr, struct filenode_desttree *node) {
if (node->bytime.left) (void)fixtimelist(lastptr,node->bytime.left);
**lastptr=node;
*lastptr=&node->nextbytime;
if (node->bytime.right) (void)fixtimelist(lastptr,node->bytime.right);
}

void fixtimelist_desttree(struct desttree *dt) {
// sets dt->firstbytime/nextbytime list to make it easier to delete them sequentially
struct filenode_desttree **fnd;
if (!dt->topnodebytime) return;
fnd=&dt->firstbytime;
(void)fixtimelist(&fnd,dt->topnodebytime);
}

void printfilename_desttree(FILE *fout, struct filenode_desttree *fn) {
(ignore)fputuistr(&fn->topdir,fout);
(void)printpath_dirlist(fout,fn->dir);
(ignore)fputuistr(&fn->name,fout);
}

int fillfilename_desttree(char *buffer, unsigned int blen, struct filenode_desttree *fn) {
unsigned int len,n;
n=fn->name.ui;
if (fillpath_dirlist(&len,buffer,blen,&fn->topdir,fn->dir)) GOTOERROR;
if (len+n>=blen) GOTOERROR;
memcpy(buffer+len,fn->name.str,n+1);
return 0;
error:
	return -1;
}
