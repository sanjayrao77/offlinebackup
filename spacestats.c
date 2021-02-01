/*
 * spacestats.c - calculate disk usage
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
#include <sys/statvfs.h>
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

CLEARFUNC(spacestats);

void vinit_spacestats(struct spacestats *ss, unsigned int margin) {
ss->margin=margin;
}
void addtrees_spacestats(struct spacestats *ss, struct srctree *st, struct desttree *dt, struct options *options) {
ss->needed=st->space.unmatched + ss->margin;
ss->possible=dt->space.all - st->space.matched;
if (options->isdeletemismatch) ss->possible+=st->space.mismatched;
}
static int getfreespace(uint64_t *sz_out, char *filename) {
struct statvfs sv;
if (statvfs(filename,&sv)) {
	GOTOERROR;
}
*sz_out=sv.f_bsize*sv.f_bavail;
return 0;
error:
	return -1;
}

int getfreespace_spacestats(struct spacestats *ss, struct uistr *fullpath) {
if (getfreespace(&ss->available,fullpath->str)) GOTOERROR;
return 0;
error:
	return -1;
}
int print_spacestats(FILE *fout, struct spacestats *ss) {
fprintf(fout,"space_stats: { "\
"needed: %"PRIu64", "\
"available: %"PRIu64", "\
"possible: %"PRIu64", "\
"margin: %"PRIu64" },\n",ss->needed,ss->available,ss->possible,ss->margin);

return 0;
}
