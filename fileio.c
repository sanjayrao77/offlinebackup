/*
 * fileio.c - functions to handle file io
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <inttypes.h>
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
#include "destbytime.h"
#include "srctree.h"
#include "spacestats.h"
#include "yesno.h"
#include "backupfiles.h"

#include "fileio.h"

//#define SAFEMODE
#ifdef SAFEMODE
#warning SAFEMODE fs changes disabled
#endif

static void printsrctopdir(FILE *fout, struct dirtree *dt) {
while (dt->parent) dt=dt->parent;
(ignore)fputuistr(&dt->std->srcpath,fout);
}
static void printdesttopdir(FILE *fout, struct dirtree *dt) {
while (dt->parent) dt=dt->parent;
(ignore)fputuistr(&dt->std->destpath,fout);
}

static void printpartialpath(FILE *fout, struct dirtree *dt) {
if (!dt->parent) return;
(void)printpartialpath(fout,dt->parent);
(ignore)fputuistr(&dt->dirlist.nameslash,fout);
}

#ifndef SAFEMODE
static int writen2(int *written_out, int fd, unsigned char *buffer, int num) {
int num_in;
num_in=num;
while (num) {
	int k;
	k=write(fd,buffer,num);
	if (k<0) {
		if (errno==ENOSPC) break;
		GOTOERROR;
	}
	buffer+=k;
	num-=k;
}
*written_out=num_in-num;;
return 0;
error:
	return -1;
}
static int copyfile3(uint64_t *written_out, char *infilename, int infd, char *outfilename, int outfd,
		uint64_t already, uint64_t st_size, struct timespec *st_mtim, unsigned char *buffer, unsigned int blen) {
// returns:-2=>ENOSPC(see written_out)
uint64_t togo;

togo=st_size-already;
if (already) {
	if (0>lseek(infd,already,SEEK_SET)) GOTOERROR;
	if (0>lseek(outfd,already,SEEK_SET)) GOTOERROR;
}
while (togo) {
	int m,n;
	if (blen>togo) blen=togo;
	m=read(infd,buffer,blen);
	if (m<=0) {
		fprintf(stderr,"Error reading from file %s\n",infilename);
		GOTOERROR;
	}
	if (writen2(&n,outfd,buffer,m)) {
		fprintf(stderr,"Error writing to file %s\n",outfilename);
		GOTOERROR;
	}
	already+=n;
	if (m!=n) {
		*written_out=already;
		return -2;
	}
	togo-=n;
}

{
	struct timespec times[2];
	times[0].tv_sec=time(NULL);
	times[0].tv_nsec=0;
	times[1]=*st_mtim;
#if 0
	fprintf(stderr,"Writing with st_mtim.tv_sec: %"PRIu64" .tv_nsec:%"PRIu64"\n",st_mtim->tv_sec,st_mtim->tv_nsec);
#endif
	if (futimens(outfd,times)) GOTOERROR;
}

*written_out=already;
return 0;
error:
	return -1;
}

static int copyfile2(uint64_t *written_out, struct options *options, struct stats_backupfiles *sbf, struct filenode_srctree *fn,
		uint64_t already, unsigned char *buffer, unsigned int blen) {
// returns:1=>copy declined by user, -2=>ENOSPC(see written_out), -3=>src changed size (or gone)
int infd=-1,outfd=-1;
unsigned int ui;
struct stat st;
int ret=0;

ui=fn->name.ui;
ui+=1;
if (sbf->buffleft < ui) GOTOERROR;
memcpy(sbf->sbuff+sbf->slen,fn->name.str,ui);
memcpy(sbf->dbuff+sbf->dlen,fn->name.str,ui);

if (stat(sbf->sbuff,&st)) {
	if (errno==ENOENT) return -3;
	GOTOERROR;
}
if (st.st_size!=fn->st_size) return -3;

infd=open(sbf->sbuff,O_RDONLY);
if (infd<0) {
	if (errno==ENOENT) return -3;
	GOTOERROR;
}
outfd=open(sbf->dbuff,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR); // |O_EXCL will fail if we run out of space on creation
if (outfd<0) GOTOERROR;

switch (copyfile3(written_out,sbf->sbuff,infd,sbf->dbuff,outfd,already,st.st_size,&fn->st_mtim,buffer,blen)) {
	case 0: break;
	case -2: ret=-2; break;
	default: GOTOERROR;
}

if (close(infd)) GOTOERROR; infd=-1;
if (close(outfd)) GOTOERROR; outfd=-1;
return ret;
error:
	ifclose(infd);
	ifclose(outfd);
	return -1;
}
#endif

int copyfile_fileio(uint64_t *written_out, struct options *options, struct stats_backupfiles *sbf, struct filenode_srctree *fn,
		uint64_t already, unsigned char *buffer, unsigned int blen, int isverifylater) {
// returns:1=>copy declined by user, -2=>ENOSPC(see written_out), -3=>src changed size (or gone)
// already: bytes already written to dest, should start after that
// written: total bytes (from 0) written to dest, not necessarily the number written in this call
// need to create dest dirs if needed
// need to ftruncate dest if needed
// update mtime at end (set atime equal)
if (!options->iscopyfiles) {
	if (options->isverbose) {
		(ignore)fputs("{ action: \"(NOT) copy\", name: \"",stdout);
		(void)printdestfilename_srctree(stdout,fn);
		(ignore)fputs("\" },\n",stdout);
	}
	*written_out=0;
	return 0;
}
if (options->isinteractive) {
	int isyes;
	fputs("Copy (",stdout);
	(void)printsrctopdir(stdout,fn->dir);
	fputs(" -> ",stdout);
	(void)printdesttopdir(stdout,fn->dir);
	fputs(") ",stdout);
	(void)printpartialpath(stdout,fn->dir);
	fputuistr(&fn->name,stdout);
	fputs(" ? ",stdout);
	if (getyes(&isyes)) GOTOERROR;
	if (!isyes) return 1; // user declined to copy file, no error
} else if (options->isverbose) {
	if (!isverifylater) { // verify will print this
		(ignore)fputs("{ action: \"copy\", name: \"",stdout);
		(void)printdestfilename_srctree(stdout,fn);
		(ignore)fputs("\", verified: 0 },\n",stdout);
	}
}

if (ferror(stdout)) GOTOERROR;

#ifdef SAFEMODE
*written_out=fn->st_size;
return 0;
#else
return copyfile2(written_out,options,sbf,fn,already,buffer,blen);
#endif
error:
	return -1;
}

int makespace_fileio(uint64_t *spacemade_out, struct options *options, struct desttree *dest, uint64_t spacewanted) {
// if spacemade < spacewanted then we ran out of potential deletes
int isverbose=options->isverbose;
int isinteractive=options->isinteractive;
struct filenode_desttree *cursor;
uint64_t spacemade=0;
for (cursor=dest->cursor_bytime;cursor;cursor=cursor->nextbytime) {
	if (cursor->isinsrcdir) continue; // mismatches
	if (cursor->isdeleted) continue;
	if (fillfilename_desttree(dest->twopathmax,PATH_MAX+1,cursor)) GOTOERROR;
	if (!options->isdeletetofree) {
		if (isverbose) {
			fputs("{ action: \"(NOT) delete\", name: \"",stdout);
			(ignore)fputs(dest->twopathmax,stdout);
			(ignore)fputs("\" },\n",stdout);
		}
		continue;
	}
	if (isinteractive) {
		int isyes;
		fputs("Delete for space ",stdout);
		(ignore)fputs(dest->twopathmax,stdout);
		(ignore)fputs(" ? ",stdout);
		if (getyes(&isyes)) GOTOERROR;
		if (!isyes) continue;
	} else if (isverbose) {
		fputs("{ action: \"delete\", name: \"",stdout);
		(ignore)fputs(dest->twopathmax,stdout);
		(ignore)fputs("\" },\n",stdout);
	}
#ifdef SAFEMODE
	fprintf(stderr,"Unlinking currently disabled (SAFEMODE)\n");
	spacemade+=cursor->st_size;
#else
	{
		struct stat st;
		if (stat(dest->twopathmax,&st)) {
			if (errno==ENOENT) {
				fprintf(stderr,"File removed from under us: %s\n",dest->twopathmax);
				continue;
			}
			GOTOERROR;
		}
		if (unlink(dest->twopathmax)) GOTOERROR;
		cursor->isdeleted=1;
		spacemade+=st.st_size;
	}
#endif
	if (spacemade>=spacewanted) break;
}
dest->cursor_bytime=cursor;
*spacemade_out=spacemade;
return 0;
error:
	return -1;
}

int deletemismatches_fileio(struct desttree *dest, struct options *options) {
struct filenode_desttree *fn;
int isverbose=options->isverbose;
int isinteractive=options->isinteractive;
for (fn=dest->firstbytime;fn;fn=fn->nextbytime) {
	if (!fn->ismismatch) continue;
	if (fn->isdeleted) continue; // not currently possible
	if (fillfilename_desttree(dest->onepathmax,PATH_MAX+1,fn)) GOTOERROR;
	if (!options->isdeletemismatch) {
		if (isverbose) {
			fputs("{ action: \"(NOT) delete mismatch\", name: \"",stdout);
			(ignore)fputs(dest->onepathmax,stdout);
			(ignore)fputs("\" },\n",stdout);
		}
		continue;
	}
	if (isinteractive) {
		int isyes=1;
		fputs("Delete mismatch ",stdout);
		(ignore)fputs(dest->onepathmax,stdout);
		(ignore)fputs(" ? ",stdout);
		if (getyes(&isyes)) GOTOERROR;
		if (!isyes) continue;
	} else if (isverbose) {
		fputs("{ action: \"delete mismatch\", name: \"",stdout);
		(ignore)fputs(dest->onepathmax,stdout);
		(ignore)fputs("\" },\n",stdout);
	}
#ifdef SAFEMODE
	fprintf(stderr,"Unlinking currently disabled (SAFEMODE)\n");
#else
	if (unlink(dest->onepathmax)) GOTOERROR;
#endif
}
return 0;
error:
	return -1;
}

int expiresourcefile_fileio(struct options *options, char *filename) {
if (!options->isdeletetofree) {
	if (options->isverbose) {
		fputs("{ action: \"(NOT) truncate\", name: \"",stdout);
		fputs(filename,stdout);
		fputs("\" },\n",stdout);
	}
	return 0;
}
if (options->isinteractive) {
	int isyes;
	fputs("Truncate ",stdout);
	fputs(filename,stdout);
	fputs(" ? ",stdout);
	if (ferror(stdout)) GOTOERROR;
	if (getyes(&isyes)) GOTOERROR;
	if (!isyes) return 0;
} else if (options->isverbose) {
	fputs("{ action: \"truncate\", name: \"",stdout);
	fputs(filename,stdout);
	fputs("\" },\n",stdout);
}
#ifdef SAFEMODE
fprintf(stderr,"Truncation currently disabled (SAFEMODE)\n");
#else
if (truncate(filename,0)) GOTOERROR;
#endif
return 0;
error:
	return -1;
}

int createdestdir_fileio(struct options *options, char *destpath) {
struct stat st;
if (!stat(destpath,&st)) return 0;
if (errno!=ENOENT) GOTOERROR;
if (!options->iscopyfiles) {
	if (options->isverbose) {
		(ignore)fputs("{ action: \"(NOT) create directory\", name: \"",stdout);
		(ignore)fputs(destpath,stdout);
		(ignore)fputs("\" },\n",stdout);
	}
	return 0;
}
if (options->isinteractive) {
	int isyes;
	(ignore)fputs("Create directory ",stdout);
	(ignore)fputs(destpath,stdout);
	fputs(" ? ",stdout);
	if (getyes(&isyes)) GOTOERROR;
	if (!isyes) return 0;
} else if (options->isverbose) {
	(ignore)fputs("{ action: \"create directory\", name: \"",stdout);
	(ignore)fputs(destpath,stdout);
	(ignore)fputs("\" },\n",stdout);
}
#ifdef SAFEMODE
	fprintf(stderr,"Directory creation disabled (SAFEMODE)\n");
#else
if (mkdir(destpath,S_IRWXU)) GOTOERROR;
#endif
return 0;
error:
	return -1;
}

static int readn(int fd, unsigned char *buffer, unsigned int blen) {
while (blen) {
	int k;
	k=read(fd,buffer,blen);
	if (k<=0) GOTOERROR;
	blen-=k;
	buffer+=k;
}
return 0;
error:
	return -1;
}

static int verifyfile(char *sfilename, char *dfilename, uint64_t st_size, unsigned char *buffer, unsigned int blen, int isverbose) {
// returns: -2: source read error, -3: dest read error, -4: data mismatch
uint64_t togo;
int sfd=-1,dfd=-1;
unsigned char *buffer2;
int err=-1;

blen=blen/2;
buffer2=buffer+blen;

sfd=open(sfilename,O_RDONLY);
if (sfd<0) {
	err=-2;
	GOTOERROR;
}
dfd=open(dfilename,O_RDONLY);
if (dfd<0) {
	err=-3;
	GOTOERROR;
}

togo=st_size;
while (togo) {
	if (blen>togo) blen=togo;
	if (readn(sfd,buffer,blen)) {
		err=-2;
		GOTOERROR;
	}
	if (readn(dfd,buffer2,blen)) {
		err=-3;
		GOTOERROR;
	}
	if (memcmp(buffer,buffer2,blen)) {
		err=-4;
		GOTOERROR;
	}
	togo-=blen;
}
(ignore)close(sfd);
(ignore)close(dfd);
return 0;
error:
	if (isverbose) {
		switch (err) {
			case -2: fprintf(stdout,"{ action: \"error\", type: \"Source read error\", name: \"%s\" },\n",sfilename); break;
			case -3: fprintf(stdout,"{ action: \"error\", type: \"Destination read error\", name: \"%s\" },\n",dfilename); break;
			case -4: fprintf(stdout,"{ action: \"error\", type: \"Data mismatch\", source: \"%s\", dest: \"%s\"},\n",sfilename,dfilename); break;
		}
	}
	ifclose(sfd);
	ifclose(dfd);
	return err;
}

int verifyfile_fileio(struct options *options, struct stats_backupfiles *sbf, struct filenode_srctree *fn) {
// this really needs to be called from backupfiles (sbf needs to be filled)
#ifdef SAFEMODE
	fprintf(stderr,"Copy verification disabled (SAFEMODE)\n");
return 0;
#else
	if (verifyfile(sbf->sbuff,sbf->dbuff,fn->st_size,options->iobuffer,options->iobuffersize,options->isverbose)) GOTOERROR;
if (options->isverbose) {
	(ignore)fputs("{ action: \"copy\", name: \"",stdout);
	(ignore)fputs(sbf->dbuff,stdout);
	(ignore)fputs("\", verified: 1 },\n",stdout);
}
return 0;
error:
	return -1;
#endif
}
int verifyfile2_fileio(int *ismatch_out, char *sfilename, struct desttree *dest, struct filenode_desttree *dnode,
		struct options *options) {
// returns: -2: source read error, -3: dest read error
int r;

if (fillfilename_desttree(dest->onepathmax,PATH_MAX+1,dnode)) GOTOERROR;
r=verifyfile(sfilename,dest->onepathmax,dnode->st_size,options->iobuffer,options->iobuffersize,options->isverbose);
switch (r) {
	case 0: *ismatch_out=1; r=0; break;
	case -4: *ismatch_out=0; r=0; break;
	case -3: break;
	default: GOTOERROR;
}

return r;
error:
	return -1;
}
