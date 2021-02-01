/*
 * dirtree.c - keeping track of directories
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

struct dirtree *make_dirtree(struct fastmem *fastmem, struct dirtree *parent, char *name, struct srctodest *std) {
struct dirtree *dir;
unsigned int n;

if (!(dir=FCALLOC(fastmem,struct dirtree,1))) GOTOERROR;
if (!parent) {
	dir->std=std;
	return dir;
}

n=strlen(name);
if (!(dir->dirlist.nameslash.str=alloc_fastmem(fastmem,n+2))) GOTOERROR;
memcpy(dir->dirlist.nameslash.str,name,n);
dir->dirlist.nameslash.str[n]='/';
n++;
dir->dirlist.nameslash.str[n]='\0';
dir->dirlist.nameslash.ui=n;

if (parent->dirlist.nameslash.str) dir->dirlist.parent=&parent->dirlist;
dir->parent=parent;
dir->nextsibling=parent->firstchild;
parent->firstchild=dir;
return dir;
error:
	return NULL;
}

void addfile_dirtree(struct dirtree *d, struct filenode_srctree *fns) {
fns->nextfile=d->firstfile;
d->firstfile=fns;
}

static int fillfullpath(unsigned int *len_out, char *buffer, unsigned int bmax, struct dirtree *dt) {
unsigned int len=0;
unsigned int n,m;

n=dt->dirlist.nameslash.ui;
if (dt->dirlist.parent) {
	if (fillfullpath(&len,buffer,bmax,dt->parent)) GOTOERROR;
	m=len+n;
	if (m>bmax) GOTOERROR;
	memcpy(buffer+len,dt->dirlist.nameslash.str,n);
} else {
	m=n;
	if (m>bmax) GOTOERROR;
	memcpy(buffer,dt->dirlist.nameslash.str,n);
}
*len_out=m;
return 0;
error:
	return -1;
}

static int fillfullpath2(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirtree *dt) {
unsigned int n,m;
m=topdir->ui;
if (m+1>=bmax) GOTOERROR;
memcpy(buffer,topdir->str,m);
if (!dt) {
	*len_out=m;
	return 0;
}
if (!dt->dirlist.nameslash.str) {
	*len_out=m;
} else {
	if (fillfullpath(&n,buffer+m,bmax-m,dt)) GOTOERROR;
	*len_out=m+n;
}
return 0;
error:
	return -1;
}

int fillpath_dirtree(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirtree *dt) {
return fillfullpath2(len_out,buffer,bmax,topdir,dt);
}

int open_dirtree(DIR **dir_out, char *buffer, unsigned int blen, struct uistr *topdir, struct dirtree *dt) {
unsigned int len;
DIR *d=NULL;
if (fillfullpath2(&len,buffer,blen,topdir,dt)) GOTOERROR;
buffer[len]='\0';
d=opendir(buffer);
if (!d) {
	fprintf(stderr,"Unable to open \"%s\"\n",buffer);
	GOTOERROR;
}
*dir_out=d;
return 0;
error:
	if (d) closedir(d);
	return -1;
}

static void printnodes(FILE *fout, struct dirtree *dir, int indent) {
struct filenode_srctree *file;
int i;
file=dir->firstfile;
while (file) {
	if (dir->dirlist.nameslash.str) {
		for (i=0;i<indent;i++) {
			fputs("  ",fout);
		}
		fputuistr(&dir->dirlist.nameslash,fout);
		fputs("/ ",fout);
	} else fputs("  ",fout);
	fputuistr(&file->name,fout);
	fputc('\n',fout);

	file=file->nextfile;
}

indent+=1;
dir=dir->firstchild;
while (dir) {
	(void)printnodes(fout,dir,indent);
	dir=dir->nextsibling;
}
}

void print_dirtree(FILE *fout, struct dirtree *firstdir) {
while (firstdir) {
	(void)printnodes(fout,firstdir,0);
	firstdir=firstdir->nextsibling;
}
}

static void printdestpath(FILE *fout, struct dirtree *dt) {
#if 0
fprintf(stderr,"dirtree\n\t.dirlist.name: %s\n\t.dirlist.parent: %p\n\t.std: %p\n\t.parent: %p\n\t.firstchild: %p\n"\
"\t.nextsibling: %p\n\t.firstfile: %p\n", dt->dirlist.name, dt->dirlist.parent, dt->std, dt->parent,
dt->firstchild, dt->nextsibling, dt->firstfile);
#endif

if (!dt->parent) {
	(ignore)fputuistr(&dt->std->destpath,fout);
	return;
}
(void)printdestpath(fout,dt->parent);
(ignore)fputuistr(&dt->dirlist.nameslash,fout);
}

static void printsrcpath(FILE *fout, struct dirtree *dt) {
if (!dt->parent) {
	(ignore)fputuistr(&dt->std->srcpath,fout);
	return;
}
(void)printsrcpath(fout,dt->parent);
(ignore)fputuistr(&dt->dirlist.nameslash,fout);
}

void printdestpath_dirtree(FILE *fout, struct dirtree *dt) {
(void)printdestpath(fout,dt);
}

void printsrcpath_dirtree(FILE *fout, struct dirtree *dt) {
(void)printsrcpath(fout,dt);
}
