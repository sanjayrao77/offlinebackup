/*
 * backupfiles.c - track what files to process
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
#include "desttree.h"
#include "dirlist.h"
#include "dirtree.h"
#include "srctree.h"
#include "yesno.h"
#include "backupfiles.h"
#include "fileio.h"

CLEARFUNC(stats_backupfiles);

static int backupfiles(struct stats_backupfiles *bsdt, struct options *options, struct dirtree *dt,
		unsigned char *buffer, unsigned int blen, struct desttree *dest) {
struct dirtree *subdt;
struct filenode_srctree *file;
int isverifycopy=options->isverifycopy;

#if 0
fprintf(stderr,"Directory %s\n",dt->dirlist.name);
#endif

dest->cursor_bytime=dest->firstbytime;
file=dt->firstfile;
while (file) {
	uint64_t spacemade,written,already=0,spacewanted=1024*1024;
	while (1) {
		switch (copyfile_fileio(&written,options,bsdt,file,already,buffer,blen,isverifycopy)) {
			case 0: // all good
				bsdt->filescopied+=1;
				bsdt->bytescopied+=written;
				if (isverifycopy) {
					if (verifyfile_fileio(options,bsdt,file)) GOTOERROR;
					bsdt->copiesverified+=1;
				}
//					goto doublebreak;
//					break;
			case 1: // file skipped by user choice
				goto doublebreak;
				break;
			case -2: // need space
				already=written;
				break;
			case -3: // file changed under us or is missing
				fputs("{ action: \"skip\", name: \"",stdout);
				(void)printsrcfilename_srctree(stdout,file);
				fputs("\" },\n",stdout);
				goto doublebreak;
				break;
			default: GOTOERROR; // other error
		}

		if (makespace_fileio(&spacemade,options,dest,spacewanted)) GOTOERROR;
		if (!spacemade) {
			fprintf(stderr,"We've run out of disk space, can't make room for source files.\n");
			GOTOERROR;
		}
	}
	doublebreak:
	file=file->nextfile;
}

subdt=dt->firstchild;
while (subdt) {
	unsigned int oldslen,olddlen,oldbleft,ui;
	oldslen=bsdt->slen;
	olddlen=bsdt->dlen;
	oldbleft=bsdt->buffleft;

	ui=subdt->dirlist.nameslash.ui;
	if (ui>=bsdt->buffleft) GOTOERROR;

	memcpy(bsdt->sbuff+oldslen,subdt->dirlist.nameslash.str,ui);
	bsdt->slen+=ui;

	memcpy(bsdt->dbuff+olddlen,subdt->dirlist.nameslash.str,ui);
	bsdt->dlen+=ui;

	bsdt->buffleft-=ui;

	bsdt->dbuff[bsdt->dlen]='\0';
	if (createdestdir_fileio(options,bsdt->dbuff)) GOTOERROR;

	if (backupfiles(bsdt,options,subdt,buffer,blen,dest)) GOTOERROR;

	bsdt->slen=oldslen;
	bsdt->dlen=olddlen;
	bsdt->buffleft=oldbleft;
	subdt=subdt->nextsibling;
}
return 0;
error:
	return -1;
}

static int gettopdirs(struct stats_backupfiles *bsdt, struct dirtree *dt) {
unsigned int ui;

if (dt->parent) GOTOERROR;
//while (dir->parent) dir=dir->parent;

ui=dt->std->srcpath.ui;
if (ui>=bsdt->buffleft) GOTOERROR;
memcpy(bsdt->sbuff,dt->std->srcpath.str,ui);
bsdt->slen=ui;

ui=dt->std->destpath.ui;
if (ui>=bsdt->buffleft) GOTOERROR;
memcpy(bsdt->dbuff,dt->std->destpath.str,ui);
bsdt->dlen=ui;

if (bsdt->slen>bsdt->dlen) bsdt->buffleft-=bsdt->slen;
else bsdt->buffleft-=bsdt->dlen;
return 0;
error:
	return -1;
}

int walktree_backupfiles(struct stats_backupfiles *bsdt, struct options *options, struct srctree *src, struct desttree *dest) {
// src.onepathmax and dest.onepathmax are used
// dest needed to unlink old files to make room
struct dirtree *dt;
bsdt->sbuff=src->onepathmax;
bsdt->dbuff=dest->onepathmax;
bsdt->buffleft=PATH_MAX+1;
dt=src->firstdir;
while (dt) {
	if (gettopdirs(bsdt,dt)) GOTOERROR; // can't really error
	if (backupfiles(bsdt,options,dt,options->iobuffer,options->iobuffersize,dest)) GOTOERROR;
	dt=dt->nextsibling;
}
return 0;
error:
	return -1;
}

int print_stats_backupfiles(FILE *fout, struct stats_backupfiles *bsdt) {
fprintf(fout,"backup_stats: { filescopied: %"PRIu64", bytescopied: %"PRIu64", copiesverified: %"PRIu64", unlinkable: %"PRIu64" }",
	bsdt->filescopied,bsdt->bytescopied,bsdt->copiesverified,bsdt->stillunmatched);
return 0;
}

