/*
 * destbytime.c - btree for destination files (sorted by time)
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include "common/conventions.h"
#include "common/fastmem.h"
#include "common/spool.h"
#include "common/utils.h"
#include "base.h"

#include "dirlist.h"
#include "desttree.h"
#include "destbytime.h"

#define node_treeskel filenode_desttree
#define find2_treeskel find2_destbytime
#define addnode2_treeskel addnode2_destbytime
#define rmnode2_treeskel rmnode2_destbytime
#define LEFT(a)	((a)->bytime.left)
#define RIGHT(a)	((a)->bytime.right)
#define BALANCE(a)	((a)->bytime.balance)
#define cmp destbytimecmp

static int destbytimecmp(struct filenode_desttree *a, struct filenode_desttree *b) {
if (a->st_mtim.tv_sec==b->st_mtim.tv_sec) {
	if (a->st_mtim.tv_nsec<b->st_mtim.tv_nsec) return -1;
	if (a->st_mtim.tv_nsec==b->st_mtim.tv_nsec) return 0;
	return 1;
}
if (a->st_mtim.tv_sec<b->st_mtim.tv_sec) return -1;
return 1;
}

static void printdirs(struct dirlist *dir) {
if (!dir) return;
printdirs(dir->parent);
fprintf(stderr,"%s ",dir->nameslash.str);
}

static void printnodes(struct filenode_desttree *root, unsigned int indent) {
unsigned int plus;
plus=indent+1;
if (RIGHT(root)) printnodes(RIGHT(root),plus);
	while (indent) {
		fputs("    ",stderr);
		indent--;
	}
	if (root->isinsrcdir) fprintf(stderr,"MATCHED ");
	fprintf(stderr,"%s/",root->topdir.str);
	printdirs(root->dir);
	fprintf(stderr,"%s (/%d) %"PRIu64" bytes %s",
		root->name.str,
		BALANCE(root),
		root->st_size,
		ctime(&(root->st_mtim.tv_sec))
		);
if (LEFT(root)) printnodes(LEFT(root),plus);
}
void printnodes_destbytime(struct filenode_desttree *root) {
if (!root) return;
(void)printnodes(root,0);
}

#if 0
static uint64_t addpossible(struct filenode_desttree *root) {
uint64_t u=0;
if (RIGHT(root)) u+=addpossible(RIGHT(root));
if (!root->isinsrcdir) u+=root->st_size;
if (LEFT(root)) u+=addpossible(LEFT(root));
return u;
}

uint64_t addpossible_destbytime(struct filenode_desttree *root) {
if (!root) return 0;
return addpossible(root);
}
#endif

uint64_t addunmatched_destbytime(struct desttree *dest) {
struct filenode_desttree *dnode;
uint64_t u=0;

for (dnode=dest->firstbytime;dnode;dnode=dnode->nextbytime) {
	if (dnode->isinsrcdir) continue;
	if (dnode->isdeleted) continue;

	u+=dnode->st_size;
}
return u;
}

#line 1 "destbytime.c/common/treeskel.c"
#include "common/treeskel.c"
