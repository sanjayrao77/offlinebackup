/*
 * backupfiles.h
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
struct stats_backupfiles {
	uint64_t filescopied,copiesverified;
	uint64_t bytescopied;
	uint64_t stillunmatched;

	char *sbuff,*dbuff;
	unsigned int buffleft;
	unsigned int slen,dlen;
};
H_CLEARFUNC(stats_backupfiles);

int print_stats_backupfiles(FILE *fout, struct stats_backupfiles *bsdt);
int walktree_backupfiles(struct stats_backupfiles *bsdt, struct options *options, struct srctree *src, struct desttree *dest);
