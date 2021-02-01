
/*
 * dirtree.h
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
struct dirtree {
	struct dirlist dirlist;

	// to save memory, std and parent could be union'd (exactly one is used at a time)
	struct srctodest *std;
	struct dirtree *parent;

	struct dirtree *firstchild,*nextsibling;

	struct filenode_srctree *firstfile;
};

struct dirtree *make_dirtree(struct fastmem *fastmem, struct dirtree *parent, char *name, struct srctodest *std);
int fillpath_dirtree(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirtree *dt);

#ifdef _DIRENT_H
int open_dirtree(DIR **dir_out, char *buffer, unsigned int blen, struct uistr *topdir, struct dirtree *dt);
#endif
void addfile_dirtree(struct dirtree *d, struct filenode_srctree *fns);
void print_dirtree(FILE *fout, struct dirtree *firstdir);
void printdestpath_dirtree(FILE *fout, struct dirtree *dt);
void printsrcpath_dirtree(FILE *fout, struct dirtree *dt);
