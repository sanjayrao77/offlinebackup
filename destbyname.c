
/*
 * destbyname.c - btree for destination files (sorted by name)
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

#include "srctodest.h"
#include "dirlist.h"
#include "desttree.h"
#include "destbyname.h"

#define node_treeskel filenode_desttree
#define find2_treeskel find2_destbyname
#define addnode2_treeskel addnode2_destbyname
#define rmnode2_treeskel rmnode2_destbyname
#define LEFT(a)	((a)->byname.left)
#define RIGHT(a)	((a)->byname.right)
#define BALANCE(a)	((a)->byname.balance)
#define cmp destbynamecmp

static int dircmp(struct dirlist *a, struct dirlist *b) {
int i;
if (!a) {
	if (!b) return 0;
	return -1;
}
if (!b) return 1;
i=dircmp(a->parent,b->parent);
if (i) return i;
return strcmp(a->nameslash.str,b->nameslash.str);
}

static int destbynamecmp(struct filenode_desttree *a, struct filenode_desttree *b) {
int i;
//fprintf(stderr,"Comparing %s and %s\n",a->topdir,b->topdir);
i=strcmp(a->topdir.str,b->topdir.str);
if (i) return i;
//fprintf(stderr,"Comparing %p and %p\n",a->dir,b->dir);
i=dircmp(a->dir,b->dir);
if (i) return i;
//fprintf(stderr,"Comparing %s and %s\n",a->name,b->name);
return strcmp(a->name.str,b->name.str);
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
printdirs(root->dir);
fprintf(stderr,"%s (/%d) %"PRIu64" bytes %s",
	root->name.str,
	BALANCE(root),
	root->st_size,
	ctime(&(root->st_mtim.tv_sec))
	);
if (LEFT(root)) printnodes(LEFT(root),plus);
}
void printnodes_destbyname(struct filenode_desttree *root) {
if (!root) return;
(void)printnodes(root,0);
}

#line 1 "destbyname.c/common/treeskel.c"
#include "common/treeskel.c"
