
/*
 * dirlist.h
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
struct dirlist {
	struct uistr nameslash; // dirname+'/'
	struct dirlist *parent;
};

struct dirlist *make_dirlist(struct fastmem *fastmem, struct dirlist *parent, char *name);
int fillpath_dirlist(unsigned int *len_out, char *buffer, unsigned int bmax, struct uistr *topdir, struct dirlist *dt);
void printpath_dirlist(FILE *fout, struct dirlist *dl);

#ifdef _DIRENT_H
int open_dirlist(DIR **dir_out, char *buffer, unsigned int blen, struct uistr *topdir, struct dirlist *dt);
#endif
