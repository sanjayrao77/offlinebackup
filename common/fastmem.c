/*
 * common/fastmem.c - way to bunch small memory allocations into fewer mallocs
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "conventions.h"
#include "fastmem.h"

CLEARFUNC(fastmem);

void deinit_fastmem(struct fastmem *fastmem) {
// don't mix init_/deinit_ and new_/free_ versions!!
struct node_fastmem *cur;
iffree(fastmem->node.data);
cur=fastmem->node.next;
while (cur) {
	struct node_fastmem *next;
	next=cur->next;
	free(cur);
	cur=next;
}
}
int init_fastmem(struct fastmem *fastmem, unsigned int size) {
if (!size) size=DEFAULTSIZE_FASTMEM;
fastmem->current=&fastmem->node;
if (!(fastmem->node.data=malloc(size))) GOTOERROR;
fastmem->node.max=size;
return 0;
error:
	return -1;
}
void init2_fastmem(struct fastmem *fastmem, void *data, unsigned int databytes) {
fastmem->current=&fastmem->node;
fastmem->node.data=data;
fastmem->node.max=databytes;
}

void free_fastmem(struct fastmem *fastmem) {
// don't mix init_/deinit_ and new_/free_ versions!!
struct node_fastmem *cur;
if (!fastmem) return;
cur=fastmem->node.next; // skip first one as it's freed with free(fastmem)
while (cur) {
	struct node_fastmem *next;
	next=cur->next;
	free(cur);
	cur=next;
}
free(fastmem);
}

static struct node_fastmem *new_node_fastmem(unsigned int size) {
struct node_fastmem *node;
if (!size) size=DEFAULTSIZE_FASTMEM;
node=malloc(sizeof(struct node_fastmem)+size);
if (!node) return NULL;
node->data=(unsigned char *)node+sizeof(struct node_fastmem);
node->num=0;
node->max=size;
node->next=NULL;
return node;
}

struct fastmem *new2_fastmem(unsigned int size, void **extra_out, unsigned int extra) {
struct fastmem *fastmem;
if (!size) size=DEFAULTSIZE_FASTMEM;
fastmem=malloc(sizeof(struct fastmem)+size+extra);
if (!fastmem) return NULL;
fastmem->node.data=(unsigned char *)fastmem+sizeof(struct fastmem);
fastmem->node.num=0;
fastmem->node.max=size;
fastmem->node.next=NULL;
fastmem->current=&fastmem->node;
if (extra_out) *extra_out=(unsigned char *)fastmem+sizeof(struct fastmem)+size;
#if 0
fprintf(stderr,"Allocated %u bytes at %p\n",size,fastmem->mem);
#endif
return fastmem;
}

void reset_fastmem(struct fastmem *fastmem) {
struct node_fastmem *node;
node=fastmem->current=&fastmem->node;
while (1) {
	node->num=0;
	node=node->next;
	if (!node) break;
}
}

void *calloc_fastmem(struct fastmem *fastmem, unsigned int size) {
void *ptr;
ptr=alloc_fastmem(fastmem,size);
if (!ptr) return NULL;
memset(ptr,0,size);
return ptr;
}

void *alloc_fastmem(struct fastmem *fastmem, unsigned int size) {
/* alloc returns a block of contiguous memory */
struct node_fastmem *node;
void *toret;
node=fastmem->current;
while (1) {
	if (node->num+size<=node->max) {
		toret=(void *)node->data+node->num;
		node->num+=size;
		break;
	} else if (node->next) {
		node=node->next;
		fastmem->current=node;
	} else {
		node->next=new_node_fastmem(size+DEFAULTSIZE_FASTMEM);
		if (!node->next) GOTOERROR;
		node=node->next;
		fastmem->current=node;
	}
}
return toret;
error:
	return NULL;
}
void *reserve_fastmem(unsigned int *available_out, struct fastmem *fastmem, unsigned int size) {
// reserve_ is alloc_ but without node->num+=size, call advance_ afterward to claim the memory
// available may be larger than size
struct node_fastmem *node;
void *toret;
node=fastmem->current;
while (1) {
	if (node->num+size<=node->max) {
		toret=(void *)node->data+node->num;
		*available_out=node->max-node->num;
		break;
	} else if (node->next) {
		node=node->next;
		fastmem->current=node;
	} else {
		node->next=new_node_fastmem(size+DEFAULTSIZE_FASTMEM);
		if (!node->next) GOTOERROR;
		node=node->next;
		fastmem->current=node;
	}
}
return toret;
error:
	return NULL;
}

int advance_fastmem(struct fastmem *fastmem, unsigned int size) {
struct node_fastmem *node;
node=fastmem->current;
if (node->num+size>node->max) GOTOERROR;
node->num+=size;
error:
	return -1;
}


int spool1_fastmem(struct fastmem *fastmem, unsigned char ch) {
/* spool copies data in -- might not be contiguous */
struct node_fastmem *node;
node=fastmem->current;
while (1) {
	if (node->max!=node->num) {
		node->data[node->num]=ch;
		node->num+=1;
		break;
	} else if (node->next) {
		node=node->next;
		fastmem->current=node;
	} else {
		node->next=new_node_fastmem(DEFAULTSIZE_FASTMEM);
		if (!node->next) GOTOERROR;
		node=node->next;
		fastmem->current=node;
	}
}
return 0;
error:
	return -1;
}
int spool_fastmem(struct fastmem *fastmem, unsigned char *data, unsigned int len) {
/* spool copies data in -- might not be contiguous */
struct node_fastmem *node;
node=fastmem->current;
while (len) {
	unsigned int d;
	d=node->max-node->num;
	if (d) {
		if (d>len) d=len;
		memcpy(node->data+node->num,data,d);
		node->num+=d;
		data+=d;
		len-=d;
	} else if (node->next) {
		node=node->next;
		fastmem->current=node;
	} else {
		node->next=new_node_fastmem(DEFAULTSIZE_FASTMEM);
		if (!node->next) GOTOERROR;
		node=node->next;
		fastmem->current=node;
	}
}
return 0;
error:
	return -1;
}


void *alloc2_fastmem(struct fastmem **fastmem_inout, unsigned int size) {
struct fastmem *fastmem;

fastmem=*fastmem_inout;
if (fastmem) return alloc_fastmem(fastmem,size);

fastmem=*fastmem_inout=new_fastmem(DEFAULTSIZE_FASTMEM);
if (!fastmem) return NULL;

return alloc_fastmem(fastmem,size);
}

unsigned char *memdup_fastmem(struct fastmem *fastmem,
		unsigned char *data, unsigned int datalen) {
unsigned char *ret;
ret=alloc_fastmem(fastmem,datalen);
if (!ret) goto error;
memcpy(ret,data,datalen);
return ret;
error:
	return NULL;
}

unsigned char *memdupz_fastmem(struct fastmem *fastmem, unsigned char *data, unsigned int datalen) {
unsigned char *ret;
ret=alloc_fastmem(fastmem,datalen+1);
if (!ret) goto error;
memcpy(ret,data,datalen);
ret[datalen]='\0';
return ret;
error:
	return NULL;
}

unsigned char *memdup2_fastmem(struct fastmem **fastmem_inout,
		unsigned char *data, unsigned int datalen) {
unsigned char *ret;
ret=alloc2_fastmem(fastmem_inout,datalen);
if (!ret) goto error;
memcpy(ret,data,datalen);
return ret;
error:
	return NULL;
}
unsigned char *memdup2z_fastmem(struct fastmem **fastmem_inout,
	unsigned char *data, unsigned int datalen) {
unsigned char *ptr;
ptr=alloc2_fastmem(fastmem_inout,datalen+1);
if (!ptr) return NULL;
memcpy(ptr,data,datalen);
ptr[datalen]='\0';
return ptr;
}

#if 0
char *strdup_fastmem(struct fastmem *fastmem, char *str) {
return (char *)memdup_fastmem(fastmem,(unsigned char *)str,strlen(str)+1);
}
#endif

unsigned int sizeof_fastmem(struct fastmem *fastmem) {
struct node_fastmem *node;
unsigned int total=0;
node=&fastmem->node;
while (node) {
	total+=node->num;
	node=node->next;
}
return total;
}


unsigned char *export_fastmem(unsigned int *len_out, struct fastmem *fastmem) {
unsigned int num;
unsigned char *temp,*data;
struct node_fastmem *node;

node=&fastmem->node;
num=sizeof_fastmem(fastmem);
data=malloc(num+1);
if (!data) return NULL;
data[num]='\0';

temp=data;
while (node) {
	memcpy(temp,node->data,node->num);
	temp+=node->num;
	node=node->next;
}

if (len_out) *len_out=num;
return data;
}

void writeout_fastmem(unsigned char *dest, struct fastmem *fastmem) {
unsigned char *temp;
struct node_fastmem *node;
node=&fastmem->node;
temp=dest;
while (node) {
	memcpy(temp,node->data,node->num);
	temp+=node->num;
	node=node->next;
}
}

void recycle_node_fastmem(struct node_fastmem **first, struct node_fastmem *node) {
while (node) {
	struct node_fastmem *next;
	next=node->next;
	node->next_recycle=*first;
	*first=node;
	node=next;
}
}
void recycle_fastmem(struct fastmem **first_fastmem, struct node_fastmem **first_node, struct fastmem *fastmem) {
(void)recycle_node_fastmem(first_node,fastmem->node.next);
fastmem->next_recycle=*first_fastmem;
*first_fastmem=fastmem;
}
struct node_fastmem *unrecycle_node_fastmem(struct node_fastmem **first) {
struct node_fastmem *node;
node=*first;
if (!node) return NULL;
*first=node->next_recycle;
node->num=0;
node->next=NULL;
return node;
}
struct fastmem *unrecycle_fastmem(struct fastmem **first) {
struct fastmem *f;
f=*first;
if (!f) return NULL;
*first=f->next_recycle;
f->current=&f->node;
f->node.num=0;
f->node.next=NULL;
return f;
}
