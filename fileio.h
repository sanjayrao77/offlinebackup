
/*
 * fileio.h
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
struct backupstats_fileio {
	uint64_t filescopied;
	uint64_t bytescopied;
};
H_CLEARFUNC(backupstats_fileio);

int deletemismatches_fileio(struct desttree *dest, struct options *options);
int expiresourcefile_fileio(struct options *options, char *filename);
int makespace_fileio(uint64_t *spacemade_out, struct options *options, struct desttree *dest, uint64_t spacewanted);
int createdestdir_fileio(struct options *options, char *destpath);
int copyfile_fileio(uint64_t *written_out, struct options *options, struct stats_backupfiles *sbf, struct filenode_srctree *fn,
		uint64_t already, unsigned char *buffer, unsigned int blen, int isverifylater);
int verifyfile_fileio(struct options *options, struct stats_backupfiles *sbf, struct filenode_srctree *fn);
int verifyfile2_fileio(int *ismatch_out, char *sfilename, struct desttree *dest, struct filenode_desttree *dnode,
		struct options *options);
