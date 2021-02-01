
/*
 * options.h
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
struct options {
	int isinteractive;
	int isverbose;
	int isverifycopy;
	int isverifymatch;
	int isdryrun; // dryrun => !isdeletemismatch, !isdeletetofree, !iscopyfiles
	int iscopyfiles;
	int isdeletemismatch;
	int isdeletetofree;
	int allow0match; // src is matched to dest if src is 0-byte (allows copy to remain in offline disk after removal)
	int ismemstats;
	int ismismatchquit;
	unsigned int maximumage; // if >0, if a matched src file is older than .maximumage, then truncate it to 0
	uint64_t srcexpiration; // determined by maximumage, don't modify

	unsigned char *iobuffer;
	unsigned int iobuffersize;

	unsigned int fsmargin;
	char *bname;
};

H_CLEARFUNC(options);
void setinteractive_options(struct options *options, int isyes);
void setverbose_options(struct options *options, int isyes);
void setverifycopy_options(struct options *options, int isyes);
void setverifymatch_options(struct options *options, int isyes);
void setdryrun_options(struct options *options, int isyes);
void setcopyfiles_options(struct options *options, int isyes);
void setdeletemismatch_options(struct options *options, int isyes);
void setdeletetofree_options(struct options *options, int isyes);
void setallow0match_options(struct options *options, int isyes);
void setexpirationdays_options(struct options *options, unsigned int days);
void setiobuffersize_options(struct options *options, unsigned int bytes);
int init_options(struct options *options);
void deinit_options(struct options *options);
int parsecmdline_options(struct options *options, struct holder_srctodest *hstd, int argc, char **argv);
void setmemstats_options(struct options *options, int isyes);
void setfsmarginsize_options(struct options *options, unsigned int bytes);
void setmismatchquit_options(struct options *options, int isyes);
