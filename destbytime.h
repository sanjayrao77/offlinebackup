
/*
 * destbytime.h
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
void addnode2_destbytime(struct filenode_desttree **root_inout, struct filenode_desttree *node);
struct filenode_desttree *find2_destbytime(struct filenode_desttree *root, struct filenode_desttree *match);
unsigned int findmaxdepth_destbytime(struct filenode_desttree *root);
void rmnode2_destbytime(struct filenode_desttree **root_inout, struct filenode_desttree *node, struct filenode_desttree **found_out);
void printnodes_destbytime(struct filenode_desttree *root);
uint64_t addunmatched_destbytime(struct desttree *dest);
