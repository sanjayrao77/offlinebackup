
/*
 * desttree.h
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
struct desttree {
	struct filenode_desttree *topnodebyname,*topnodebytime;
	struct filenode_desttree *firstbytime,*cursor_bytime;

	struct {
		uint64_t all; // bytes
	} space;
	struct {
		uint64_t all; // number of files
	} counts;

	char *onepathmax; // buffer to store full paths, PATH_MAX+1 bytes
	char *twopathmax; // used for freeing space while copying
	struct fastmem *fastmem,*fastmemtofree;
};

struct filenode_desttree {
	struct uistr topdir; // this is used for treecmp
	struct dirlist *dir;

	struct uistr name;
	struct timespec st_mtim;
	off_t st_size;

	char isinsrcdir; // is also in a source directory, possibly different size or mtime (possibly not in srctree)
	char ismismatch; // is in srctree, this hapens when size or mtime mismatch (=>isinsrcdir)
	char isdeleted; // if the file has been deleted by us

	struct {
		struct filenode_desttree *left,*right;
		signed char balance;
	} byname,bytime;
	struct filenode_desttree *nextbytime;
};

H_CLEARFUNC(desttree);
int init_desttree(struct desttree *dt, struct fastmem *fastmem_in);
void deinit_desttree(struct desttree *dt);
int fill_desttree(struct desttree *tree_inout, char *path);
int adddir_desttree(struct desttree *ft, struct uistr *fullpath);
void printcounts_desttree(FILE *fout, struct desttree *dt);
void fixtimelist_desttree(struct desttree *dt);
void printfilename_desttree(FILE *fout, struct filenode_desttree *fn);
int fillfilename_desttree(char *buffer, unsigned int blen, struct filenode_desttree *fn);
