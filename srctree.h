
/*
 * srctree.h
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
struct srctree {
	struct filenode_srctree *topnodebyname;
	char *onepathmax; // buffer to store full paths
	struct {
		uint64_t matched,unmatched; // space needed, matched already allocated in dest
		uint64_t mismatched; // count of space where dest is larger than source
	} space;
	struct {
		uint64_t unmatched; // files that are only in source
		uint64_t matched; // number of files that match cleanly
		uint64_t mismatched; // number of files that exist in source and dest but don't match
		uint64_t matchesverified; // number of files, doesn't include 0match
		uint64_t readerrors; // number of destination files with i/o errors, these mark as mismatched
	} counts;

	struct dirtree *firstdir;

	struct fastmem *fastmem,*fastmemtofree;
};

struct filenode_srctree {
	struct dirtree *dir;
	struct filenode_srctree *nextfile; // list from dir->firstfile

	struct uistr name;
	struct timespec st_mtim;
	off_t st_size;

// int ismatched; // matched files won't be stored in src

	struct {
		struct filenode_srctree *left,*right;
		signed char balance;
	} byname;
};

H_CLEARFUNC(srctree);
int init_srctree(struct srctree *st, struct fastmem *fastmem_in);
void deinit_srctree(struct srctree *st);
int fill_srctree(struct srctree *tree_inout, char *path);
void printcounts_srctree(FILE *fout, struct srctree *st);
void print_srctree(FILE *fout, struct srctree *src);
void printsrcfilename_srctree(FILE *fout, struct filenode_srctree *fn);
void printdestfilename_srctree(FILE *fout, struct filenode_srctree *fn);
int adddir_srctree(struct srctree *ft, struct srctodest *std, struct desttree *dest, struct options *options);
