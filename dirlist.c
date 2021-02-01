/*
 * dirlist.c - keeping track of directory names
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

struct dirlist *make_dirlist(struct fastmem *fastmem, struct dirlist *parent, char *name) {
struct dirlist *dir;
unsigned int ui;
if (!(dir=FCALLOC(fastmem,struct dirlist,1))) return NULL;
dir->parent=parent;
ui=strlen(name);
if (!(dir->nameslash.str=alloc_fastmem(fastmem,ui+2))) return NULL;
memcpy(dir->nameslash.str,name,ui);
dir->nameslash.str[ui]='/';
ui++;
dir->nameslash.str[ui]='\0';
dir->nameslash.ui=ui;
return dir;
}

static int fillfullpath(unsigned int *len_out, char *buffer, unsigned int bmax, struct dirlist *dt) {
unsigned int len=0;
unsigned int n,m;

n=dt->nameslash.ui;
if (dt->parent) {
	if (fillfullpath(&len,buffer,bmax,dt->parent)) GOTOERROR;
	m=len+n;
	if (m>bmax) GOTOERROR;
	memcpy(buffer+len,dt->nameslash.str,n);
} else {
	m=n;
	if (m>bmax) GOTOERROR;
	memcpy(buffer,dt->nameslash.str,n);
}
*len_out=m;
return 0;
error:
	return -1;
}

static int fillfullpath2(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirlist *dt) {
unsigned int n,m;
m=topdir->ui;
if (m+1>=bmax) GOTOERROR;
memcpy(buffer,topdir->str,m);
if (!dt) {
	*len_out=m;
	return 0;
}
if (fillfullpath(&n,buffer+m,bmax-m,dt)) GOTOERROR;
*len_out=m+n;
return 0;
error:
	return -1;
}

int fillpath_dirlist(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirlist *dt) {
return fillfullpath2(len_out,buffer,bmax,topdir,dt);
}

#if 0
// works, no longer used
int open_dirlist(DIR **dir_out, char *buffer, unsigned int blen, struct uistr *topdir, struct dirlist *dt) {
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
#endif

static void printpath(FILE *fout, struct dirlist *dl) {
if (dl->parent) (void)printpath(fout,dl->parent);
(ignore)fputuistr(&dl->nameslash,fout);
}

void printpath_dirlist(FILE *fout, struct dirlist *dl) {
(void)printpath(fout,dl);
}
