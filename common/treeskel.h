/*
 * common/treeskel.h
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
#define _COMMON_TREESKEL_H

// this is for reference
// you probably want to include btree.h instead

struct node_treeskel {
	struct {
		unsigned int ui;
	} data;

	signed char balance;
	struct node_treeskel *left,*right;
};

H_NEWFUNC(node_treeskel);
H_CLEARFUNC(node_treeskel);
void addnode_treeskel(struct node_treeskel **root_inout, struct node_treeskel *node,
	int (*cmp)(struct node_treeskel *a, struct node_treeskel *b));
void printnodes_treeskel(struct node_treeskel *root);
unsigned int findmaxdepth_treeskel(struct node_treeskel *root);
struct node_treeskel *find_treeskel(struct node_treeskel *root, struct node_treeskel *node,
	int (*cmp)(struct node_treeskel *a, struct node_treeskel *b));
void rmnode_treeskel(struct node_treeskel **root_inout, struct node_treeskel *node, struct node_treeskel **found_out,
	int (*cmp)(struct node_treeskel *a, struct node_treeskel *b));
int verifynodes_treeskel(struct node_treeskel *root, unsigned int size);
unsigned int countnodes_treeskel(struct node_treeskel *root);
