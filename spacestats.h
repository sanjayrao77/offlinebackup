
/*
 * spacestats.h
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
struct spacestats {
	uint64_t needed,available,possible;
	uint64_t margin;
};

H_CLEARFUNC(spacestats);
void vinit_spacestats(struct spacestats *ss, unsigned int margin);
void addtrees_spacestats(struct spacestats *ss, struct srctree *st, struct desttree *dt, struct options *options);
int print_spacestats(FILE *fout, struct spacestats *ss);
int getfreespace_spacestats(struct spacestats *ss, struct uistr *fullpath);
