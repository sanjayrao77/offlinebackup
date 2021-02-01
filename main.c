/*
 * main.c - entry and top procedure
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
#include <time.h>
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
#include "backupfiles.h"
#include "fileio.h"
#include "yesno.h"
#include "performance.h"

int main(int argc, char **argv) {
struct desttree dest;
struct srctree src;
struct holder_srctodest hstd;
struct spacestats ss;
struct stats_backupfiles bsdt;
struct options options;
time_t time_start,time_finish;

time_start=time(NULL);

clear_desttree(&dest);
clear_srctree(&src);
clear_holder_srctodest(&hstd);
clear_spacestats(&ss);
clear_stats_backupfiles(&bsdt);
clear_options(&options);

if (init_holder_srctodest(&hstd)) GOTOERROR;
if (parsecmdline_options(&options,&hstd,argc,argv)) {
	deinit_holder_srctodest(&hstd);
	return 0;
}
#if 0
if (options.isinteractive) {
	int isyes;
	fprintf(stdout,"Testing interactive, hit a letter.\n");
	if (getyes(&isyes)) GOTOERROR;
	fprintf(stdout,"Received %s\n",(isyes)?"Yes":"No");
}
#endif
if (!hstd.first) {
	fprintf(stderr,"No source directories found. Nothing to do!\n");
	deinit_holder_srctodest(&hstd);
	return 0;
}
if (init_options(&options)) GOTOERROR;
(void)vinit_spacestats(&ss,options.fsmargin); 
if (init_srctree(&src,hstd.fastmem)) GOTOERROR;
if (init_desttree(&dest,hstd.fastmem)) GOTOERROR;

if (options.isverbose) fprintf(stdout,"var %s={ ",options.bname);
{
	struct srctodest *std;
	if (options.isverbose) fputs("firstpass: [ ",stdout);
	std=hstd.first; // already checked, !NULL
	while (std) {
		if (options.isverbose) {
			fprintf(stdout,"{ action: \"addsource\", source: \"%s\", destination: \"%s\" },\n",std->srcpath.str,std->destpath.str);
		}
		if (adddir_desttree(&dest,&std->destpath)) GOTOERROR;
		if (adddir_srctree(&src,std,&dest,&options)) GOTOERROR; // can print truncates
		std=std->next;
	}
	(void)fixtimelist_desttree(&dest); // set .firstbytime/.nextbytime
	if (options.isverbose) fputs("{ action: \"finished\" } ],\n",stdout);
}
if (ferror(stdout)) GOTOERROR;

// assumes (hstd.first)
if (getfreespace_spacestats(&ss,&hstd.first->destpath)) GOTOERROR;
(void)addtrees_spacestats(&ss,&src,&dest,&options);

if (options.isverbose) {
	(void)printcounts_desttree(stdout,&dest);
}
(void)printcounts_srctree(stdout,&src);
if (options.isverbose) {
	(void)print_spacestats(stdout,&ss);
}

if (options.iscopyfiles) {
	if (ss.possible+ss.available < ss.needed) {
		fputs("Error, insufficient space.\n",stderr);
		fprintf(stderr,"stderr: { text: \"Insufficient space\", needed:%"PRIu64", available:%"PRIu64", deletable:%"PRIu64" } };\n",
			ss.needed,ss.available,ss.possible);
		fprintf(stdout,"{ text: \"Insufficient space\", needed:%"PRIu64", available:%"PRIu64", deletable:%"PRIu64" } };\n",
			ss.needed,ss.available,ss.possible);
		GOTOERROR;
	}
}

if (ferror(stdout)) GOTOERROR;
if (src.counts.mismatched) { // pretty rare occurance
	if (options.isdryrun || options.isdeletemismatch) {
		int isyes=1;
		if (options.isinteractive && options.isdeletemismatch) { // dryrun doesn't need approval
			fprintf(stdout,"Mismatches were found. Delete them now? ");
			if (getyes(&isyes)) GOTOERROR;
		}
		if (isyes) {
			if (options.isverbose) {
				fputs("mismatches: [\n",stdout);
			}
			if (deletemismatches_fileio(&dest,&options)) GOTOERROR;
			if (options.isverbose) {
				fputs("{ action: \"finished\" } ],\n",stdout);
			}
		}
	}
}

if (ferror(stdout)) GOTOERROR;
if (options.isdryrun || options.iscopyfiles) {
//	fprintf(stderr,"Backing up files\n");
	if (options.isverbose) {
		fputs("fschanges: [\n",stdout);
	}
	if (walktree_backupfiles(&bsdt,&options,&src,&dest)) GOTOERROR;
	if (options.isverbose) {
		fputs("{ action: \"finished\" }],\n",stdout);
	}
}

if (options.ismemstats) {
	(ignore)printstats_performance(stdout);
}

bsdt.stillunmatched=addunmatched_destbytime(&dest);
(void)print_stats_backupfiles(stdout,&bsdt);
if (options.isverbose) {
	time_finish=time(NULL);
	fprintf(stdout,",\nduration: { seconds: %"PRId64" } };\n",(int64_t)(time_finish - time_start));
} else fputs(" };\n",stdout);

deinit_holder_srctodest(&hstd);
deinit_srctree(&src);
deinit_desttree(&dest);
deinit_options(&options);
return 0;
error:
	deinit_holder_srctodest(&hstd);
	deinit_srctree(&src);
	deinit_desttree(&dest);
	deinit_options(&options);
	return -1;
}
