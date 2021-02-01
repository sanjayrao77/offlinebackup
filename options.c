/*
 * options.c - runtime options
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "common/conventions.h"
#include "base.h"
#include "srctodest.h"
#include "options.h"
CLEARFUNC(options);

void setinteractive_options(struct options *options, int isyes) {
options->isinteractive=(isyes)?1:0;
}
void setverbose_options(struct options *options, int isyes) {
options->isverbose=(isyes)?1:0;
}
void setverifycopy_options(struct options *options, int isyes) {
options->isverifycopy=(isyes)?1:0;
}
void setverifymatch_options(struct options *options, int isyes) {
options->isverifymatch=(isyes)?1:0;
}
void setdryrun_options(struct options *options, int isyes) {
options->isdryrun=(isyes)?1:0; // this will clear other options on init
}
void setcopyfiles_options(struct options *options, int isyes) {
options->iscopyfiles=(isyes)?1:0;
}
void setdeletemismatch_options(struct options *options, int isyes) {
options->isdeletemismatch=(isyes)?1:0;
}
void setdeletetofree_options(struct options *options, int isyes) {
options->isdeletetofree=(isyes)?1:0;
}
void setallow0match_options(struct options *options, int isyes) {
options->allow0match=(isyes)?1:0;
}
void setmemstats_options(struct options *options, int isyes) {
options->ismemstats=(isyes)?1:0;
}
void setmismatchquit_options(struct options *options, int isyes) {
options->ismismatchquit=(isyes)?1:0;
if (isyes) {
	setverifymatch_options(options,1);
}
}
void setexpirationdays_options(struct options *options, unsigned int days) {
time_t t;
if (!days) {
	options->maximumage=0;
	return;
}
t=time(NULL);

options->isverifymatch=1;
options->allow0match=1;

options->maximumage=days;
options->srcexpiration=t-(days*24*60*60);
}
void setiobuffersize_options(struct options *options, unsigned int bytes) {
options->iobuffersize=bytes;
}
void setfsmarginsize_options(struct options *options, unsigned int bytes) {
options->fsmargin=bytes;
}
int init_options(struct options *options) {
if (options->iobuffersize<16384) options->iobuffersize=16384; // this will be split in two for verifying
if (!(options->iobuffer=malloc(options->iobuffersize))) GOTOERROR;
if (options->isdryrun) {
	options->isdeletemismatch=0;
	options->isdeletetofree=0;
	options->iscopyfiles=0;
	options->isverifycopy=0;
}
return 0;
error:
	return -1;
}
void deinit_options(struct options *options) {
iffree(options->iobuffer);
}

int parsecmdline_options(struct options *options, struct holder_srctodest *hstd, int argc, char **argv) {
char *bname;
int ret=0;
int i;

options->iscopyfiles=1;
options->isdeletemismatch=1;
options->isdeletetofree=1;
options->allow0match=1; // this is defaulted to avoid accidental resync (overwrite) if --allow0match is forgotten
options->fsmargin=16*1024*1024; // 16mb margin, bail if no matter what we do, we'll end up with less than this free
options->ismismatchquit=1; // defaulted in case someone has corrupted online backups

if (argc<1) { options->bname="output"; return 0; }

bname=strrchr(argv[0],'/');
if (!bname) bname=argv[0]; else bname+=1;
options->bname=bname;


if (!strcmp(bname,"test")) {
	(void)setinteractive_options(options,1);
	(void)setverbose_options(options,1);
	(void)setdryrun_options(options,1);
	(void)setallow0match_options(options,1);
	(void)setexpirationdays_options(options,180); // => 0match
}

for (i=1;i<argc;i++) {
	if (!strcmp(argv[i],"--srcdest")) {
		if (i+2>=argc) {
			fprintf(stderr,"Not enough arguments to --srcdest, should be \"--srcdest (SRCDIR) (DESTDIR)\"\n");
			ret=-1;
		} else {
			if (add_srctodest(hstd,argv[i+1],argv[i+2])) {
				fprintf(stderr,"Error adding --srcdest %s %s\n",argv[i+1],argv[i+2]);
				ret=-1;
			}
			i+=2;
		}
	} else if (!strcmp(argv[i],"--interactive")) setinteractive_options(options,1);
	else if (!strcmp(argv[i],"--not-interactive")) setinteractive_options(options,0);
	else if (!strcmp(argv[i],"--verbose")) setverbose_options(options,1);
	else if (!strcmp(argv[i],"--not-verbose")) setverbose_options(options,0);
	else if (!strcmp(argv[i],"--verifycopy")) setverifycopy_options(options,1);
	else if (!strcmp(argv[i],"--not-verifycopy")) setverifycopy_options(options,0);
	else if (!strcmp(argv[i],"--verifymatch")) setverifymatch_options(options,1);
	else if (!strcmp(argv[i],"--not-verifymatch")) setverifymatch_options(options,0);
	else if (!strcmp(argv[i],"--dryrun")) setdryrun_options(options,1);
	else if (!strcmp(argv[i],"--not-dryrun")) setdryrun_options(options,0);
	else if (!strcmp(argv[i],"--copyfiles")) setcopyfiles_options(options,1);
	else if (!strcmp(argv[i],"--not-copyfiles")) setcopyfiles_options(options,0);
	else if (!strcmp(argv[i],"--deletemismatch")) setdeletemismatch_options(options,1);
	else if (!strcmp(argv[i],"--not-deletemismatch")) setdeletemismatch_options(options,0);
	else if (!strcmp(argv[i],"--deletetofree")) setdeletetofree_options(options,1);
	else if (!strcmp(argv[i],"--not-deletetofree")) setdeletetofree_options(options,0);
	else if (!strcmp(argv[i],"--allow0match")) setallow0match_options(options,1);
	else if (!strcmp(argv[i],"--not-allow0match")) setallow0match_options(options,0);
	else if (!strcmp(argv[i],"--max6months")) setexpirationdays_options(options,183);
	else if (!strcmp(argv[i],"--max1year")) setexpirationdays_options(options,366);
	else if (!strcmp(argv[i],"--max18months")) setexpirationdays_options(options,365+183);
	else if (!strcmp(argv[i],"--max2years")) setexpirationdays_options(options,366+365);
	else if (!strcmp(argv[i],"--nomaxage")) setexpirationdays_options(options,0);
	else if (!strcmp(argv[i],"--nomargin")) setfsmarginsize_options(options,4096); // 4k margin, not 0
	else if (!strcmp(argv[i],"--memstats")) setmemstats_options(options,1);
	else if (!strcmp(argv[i],"--not-memstats")) setmemstats_options(options,0);
	else if (!strcmp(argv[i],"--mismatchquit")) setmismatchquit_options(options,1);
	else if (!strcmp(argv[i],"--not-mismatchquit")) setmismatchquit_options(options,0);
	else if (!strcmp(argv[i],"--fullbackup_mode")) {
		setverbose_options(options,1);
		setverifycopy_options(options,1);
		setverifymatch_options(options,1);
		setallow0match_options(options,1);
		setexpirationdays_options(options,183);
	} else if (!strcmp(argv[i],"--verify_mode")) {
		setcopyfiles_options(options,0);
		setverbose_options(options,1);
		setverifymatch_options(options,1);
		setdeletemismatch_options(options,0);
		setallow0match_options(options,1);
	} else if (!strcmp(argv[i],"--help")) {
		ret=1;
		fprintf(stdout,"Usage: %s (OPTIONS)\n",argv[0]);
		fputs("  --srcdest A B         Copy files from directory A to B\n",stdout);
		fputs("                        If you provide multiple --srcdest arguments, then PLEASE\n",stdout);
		fputs("                        make sure no A's or B's overlap in the filesystem. Also\n",stdout);
		fputs("                        ensure that all B directories are on the same partition.\n",stdout);
		fputs("  --interactive         Prompt user before deleting or copying\n",stdout);
		fputs("  --not-interactive       disable --interactive (default)\n",stdout);
		fputs("  --verbose             Print progress to stdout\n",stdout);
		fputs("  --not-verbose           disable --verbose (default)\n",stdout);
		fputs("  --memstats            Print memory stats to stdout\n",stdout);
		fputs("  --not-memstats          disable --memstats (default)\n",stdout);
		fputs("  --verifycopy          Re-read files after writing them to verify contents\n",stdout);
		fputs("  --not-verifycopy        disable --verifycopy (default)\n",stdout);
		fputs("  --verifymatch         Compare existing destination files to source files\n",stdout);
		fputs("  --not-verifymatch       disable --verifymatch (default)\n",stdout);
		fputs("  --mismatchquit        Quit out if existing files don't verify, requires/enables --verifymatch (default)\n",stdout);
		fputs("  --not-mismatchquit       disable --mismatchquit\n",stdout);
		fputs("  --dryrun              Go through the motions but don't delete or copy\n",stdout);
		fputs("  --not-dryrun            disable --dryrun (default)\n",stdout);
		fputs("  --copyfiles           Copy files from source to destination (default)\n",stdout);
		fputs("  --not-copyfiles         disable --copyfiles\n",stdout);
		fputs("  --deletemismatch      Delete destination files that don't match source (default)\n",stdout);
		fputs("  --not-deletemismatch    disable --deletemistmatch\n",stdout);
		fputs("  --deletetofree        Delete oldest destination files to make space on device (default)\n",stdout);
		fputs("  --not-deletetofree      disable -deletetofree\n",stdout);
		fputs("  --allow0match         Allow 0-byte files on source to match destination files (default)\n",stdout);
		fputs("  --not-allow0match       disable --allow0match\n",stdout);
		fputs("  --max6months          Truncate source files older than 183 days to 0 bytes\n",stdout);
		fputs("  --max1year            Truncate source files older than 366 days to 0 bytes \n",stdout);
		fputs("  --max18months         Truncate source files older than 548 days to 0 bytes\n",stdout);
		fputs("  --max2years           Truncate source files older than 731 days to 0 bytes \n",stdout);
		fputs("                          all --maxXXX options also enable --allow0match --verifymatch\n",stdout);
		fputs("  --nomaxage            Do not truncate source files (default)\n",stdout);
		fputs("  --nomargin            Attempt copying even if it looks like we'll run out of space\n",stdout);
		fputs("  --verify_mode         Shortcut for --not-copyfiles --verbose --verifymatch ...\n",stdout);
		fputs("                        ...  --not-deletemismatch --allow0match\n",stdout);
		fputs("  --fullbackup_mode     Shortcut for --verbose --verifycopy --verifymatch ...\n",stdout);
		fputs("                        ...  --allow0match --max6months\n",stdout);
	} else {
		fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
		ret=-1;
	}
}
options->iobuffersize=1024*1024; // 1M buffer
return ret;
}
