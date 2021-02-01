
/*
 * srctodest.c - keep track of jobs
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
#include <errno.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#define DEBUG
#include "common/conventions.h"
#include "common/fastmem.h"
#include "base.h"
#include "srctodest.h"

CLEARFUNC(holder_srctodest);

int init_holder_srctodest(struct holder_srctodest *hstd) {
if (!(hstd->fastmem=new_fastmem(0))) GOTOERROR;
return 0;
error:
	return -1;
}
void deinit_holder_srctodest(struct holder_srctodest *hstd) {
if (hstd->fastmem) free_fastmem(hstd->fastmem);
}

int add_srctodest(struct holder_srctodest *hstd, char *srcpath, char *destpath) {
struct srctodest *std;
unsigned int ui;
if (!(std=FALLOC(hstd->fastmem,struct srctodest))) GOTOERROR;

ui=strlen(srcpath);
if (ui<1) GOTOERROR;
if (!(std->srcpath.str=alloc_fastmem(hstd->fastmem,ui+2))) GOTOERROR;
memcpy(std->srcpath.str,srcpath,ui+1);
std->srcpath.ui=ui;
if (std->srcpath.str[ui-1]!='/') {
	std->srcpath.str[ui]='/';
	std->srcpath.str[ui+1]='\0';
	std->srcpath.ui=ui+1;
}

ui=strlen(destpath);
if (ui<2) GOTOERROR;
if (!(std->destpath.str=alloc_fastmem(hstd->fastmem,ui+2))) GOTOERROR;
memcpy(std->destpath.str,destpath,ui+1);
std->destpath.ui=ui;
if (std->destpath.str[ui-1]!='/') {
	std->destpath.str[ui]='/';
	std->destpath.str[ui+1]='\0';
	std->destpath.ui=ui+1;
}

std->next=hstd->first;
hstd->first=std;
return 0;
error:
	return -1;
}

