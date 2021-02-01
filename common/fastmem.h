
/*
 * common/fastmem.h
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
#define DEFAULTSIZE_FASTMEM	65536
#define _COMMON_FASTMEM_H

struct node_fastmem {
	unsigned char *data;
	unsigned int num,max;

	union {
		struct node_fastmem *next;
		struct node_fastmem *next_recycle;
	};
};

struct fastmem {
	struct node_fastmem node;
	union {
		struct node_fastmem *current;
		struct fastmem *next_recycle;
	};
};
H_CLEARFUNC(fastmem);

#define strdup_fastmem(a,b) ((char *)memdup_fastmem(a,(unsigned char *)b,strlen(b)+1))
#define strdup2_fastmem(a,b) ((char *)memdup2_fastmem(a,(unsigned char *)b,strlen(b)+1))

#define FALLOC(a,b) ((b*)alloc_fastmem(a,sizeof(b)))
#define FCALLOC(a,b,m) ((b*)calloc_fastmem(a,(m)*sizeof(b)))
#define FTALLOC(a,b,m) ((b*)alloc_fastmem(a,(m)*sizeof(b)))
#define FALLOC2(a,b) ((b*)alloc2_fastmem(a,sizeof(b)))
#define new_fastmem(a) new2_fastmem(a,0,0)

void free_fastmem(struct fastmem *fastmem);
void *alloc_fastmem(struct fastmem *fastmem, unsigned int size);
void *calloc_fastmem(struct fastmem *fastmem, unsigned int size);
void *reserve_fastmem(unsigned int *available_out, struct fastmem *fastmem, unsigned int size);
int advance_fastmem(struct fastmem *fastmem, unsigned int size);
struct fastmem *new2_fastmem(unsigned int size, void **extra_out, unsigned int extra);
#define ifreset_fastmem(a) do { if (a) reset_fastmem(a); } while (0)
void reset_fastmem(struct fastmem *fastmem);
int spool_fastmem(struct fastmem *fastmem, unsigned char *data, unsigned int len);
unsigned int sizeof_fastmem(struct fastmem *fastmem);

unsigned char *memdup_fastmem(struct fastmem *fastmem, unsigned char *data, unsigned int datalen);
void *alloc2_fastmem(struct fastmem **fastmem_inout, unsigned int size);
unsigned char *memdup2_fastmem(struct fastmem **fastmem_inout, unsigned char *data, unsigned int datalen);
unsigned char *memdup2z_fastmem(struct fastmem **fastmem_inout, unsigned char *data, unsigned int datalen);
unsigned char *memdupz_fastmem(struct fastmem *fastmem, unsigned char *data, unsigned int datalen);
unsigned char *export_fastmem(unsigned int *len_out, struct fastmem *fastmem);
int spool1_fastmem(struct fastmem *fastmem, unsigned char ch);
void writeout_fastmem(unsigned char *dest, struct fastmem *fastmem);
// don't mix init_/deinit_ and new_/free_ calls, stay consistent or crash
int init_fastmem(struct fastmem *fastmem, unsigned int size);
void init2_fastmem(struct fastmem *fastmem, void *data, unsigned int databytes);
void deinit_fastmem(struct fastmem *fastmem);
void recycle_node_fastmem(struct node_fastmem **first, struct node_fastmem *node);
void recycle_fastmem(struct fastmem **first_fastmem, struct node_fastmem **first_node, struct fastmem *fastmem);
struct node_fastmem *unrecycle_node_fastmem(struct node_fastmem **first);
struct fastmem *unrecycle_fastmem(struct fastmem **first);
