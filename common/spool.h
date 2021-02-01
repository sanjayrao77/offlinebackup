/*
 * common/spool.h
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
#define _COMMON_SPOOL_H
#define string_spool(a,b) ustring_spool(a,(unsigned char *)b,strlen(b))
#define SPOOL(a) string_spool(spool,a)
#define SPOOLCH(a) char_spool(spool,a)
#define SPOOLC(a) char_spool(spool,a[0])
#define IFSPOOL2(a,b) ((b)?string_spool(a,b):ustring_spool(a,(unsigned char *)"NULL",4))
#define IFSPOOL(a) IFSPOOL2(spool,a)
#define string_javascript_spool(a,b) ustring_javascript_spool(a,(unsigned char *)b,strlen(b))
#define JSPOOL(a) string_javascript_spool(spool,a)
#define IFJSPOOL2(a,b) ((b)?string_javascript_spool(a,b):(void)0)
#define IFJSPOOL(a) IFJSPOOL2(spool,a)
#define string_json_spool(a,b) ustring_json_spool(a,(unsigned char*)b,strlen(b))
#define JSONSPOOL(a) string_json_spool(spool,a)
#define IFJSONSPOOL(a) ((a)?JSONSPOOL(a):(void)0)
#define string_html_spool(a,b) ustring_html_spool(a,(unsigned char *)b,strlen(b))
#define HTMLSPOOL(a) string_html_spool(spool,a)
#define string_html2_spool(a,b,c) ustring_html2_spool(a,(unsigned char *)b,strlen(b),c)
#define HTMLSPOOL2(a,b) string_html2_spool(spool,a,b)

// these should be removed
#define ifcontentjs_spool(a,b) IFJSPOOL2(a,b)
#define contentjson3_spool(a,b,c) max_string_json_spool(a,b,c)

#define ISPOOL(a) i_spool(spool,a)
#define UISPOOL(a) ui_spool(spool,a)

#define sizeof_spool(a) sizeof_fastmem(&(a)->fm)
#define reserve_spool(a,b,c) reserve_fastmem(a,&(b)->fm,c)
#define advance_spool(a,b) advance_fastmem(&(a)->fm,b)
#define unspool_spool(a,b) writeout_fastmem(a,&(b)->fm)

struct content_spool {
	struct fastmem fm;
	int error;
};

#ifdef _STDINT_H
#define u64_spool(a,b) ui64_spool(a,b)
#define s64_spool(a,b) i64_spool(a,b)
#define S64SPOOL(a) s64_spool(spool,a) 
#define U64SPOOL(a) u64_spool(spool,a)
int ui64_spool(struct content_spool *spool, uint64_t ui);
int i64_spool(struct content_spool *spool, int64_t ui);
#endif

int char_spool(struct content_spool *spool, char ch);
unsigned char *export_spool(unsigned int *len_out, struct content_spool *first);
int jsonspool_spool(struct content_spool *dest, struct content_spool *first);
int spool_spool(struct content_spool *dest, struct content_spool *first);
int i_spool(struct content_spool *spool, int i);
int ui_spool(struct content_spool *spool, unsigned int ui);
int fmt_double_spool(struct content_spool *spool, double d);
int double_spool(struct content_spool *spool, double d);
void free_spool(struct content_spool *first);
void reset_spool(struct content_spool *first);
struct content_spool *new_spool(void);
int max_string_json_spool(struct content_spool *first, char *data, unsigned int max);
int ustring_json_spool(struct content_spool *first, unsigned char *data, unsigned int len);
int ustring_javascript_spool(struct content_spool *first, unsigned char *data, unsigned int len);
#define alnum_string_underscore_spool(a,b) alnum_ustring_underscore_spool(a,(unsigned char *)b,strlen(b))
int alnum_ustring_underscore_spool(struct content_spool *first, unsigned char *data, unsigned int len);
int ustring_spool(struct content_spool *first, unsigned char *data, unsigned int n);
int content_spool(struct content_spool *first, char *data);
int fwrite_spool(struct content_spool *first, FILE *ff);
int write_spool(int fd, struct content_spool *first);
int ustring_html_spool(struct content_spool *first, unsigned char *data, int l);
int ustring_html2_spool(struct content_spool *first, unsigned char *data, unsigned int len, unsigned int dobrs);
int htmltable_spool(struct content_spool *first, char *data);
int url_spool(struct content_spool *first, char *data, int maxlen);
int file_spool(struct content_spool *spool, char *filename);
int fread_spool(struct content_spool *spool, FILE *fin);
#define underscore_spool(a,b) string_underscore_spool(a,b)
int string_underscore_spool(struct content_spool *spool, char *str_in);
int dump_spool(char *filename, struct content_spool *first);
int strcmp_spool(struct content_spool *first, char *str);
int isempty_spool(struct content_spool *first);
int hex2_spool(struct content_spool *spool, unsigned char *src, unsigned int len);
int readstream_spool(struct content_spool *spool, int fd);

#define string_asciijson_spool(a,b) ustring_asciijson_spool(a,(unsigned char *)b,strlen(b))
int ustring_asciijson_spool(struct content_spool *first, unsigned char *data, unsigned int len);
#define string_asciihtml_spool(a,b) ustring_asciihtml_spool(a,(unsigned char *)b,strlen(b))
int ustring_asciihtml_spool(struct content_spool *first, unsigned char *data, unsigned int len);
#define string_url_spool(a,b) ustring_url_spool(a,(unsigned char *)b,strlen(b))
int ustring_url_spool(struct content_spool *first, unsigned char *data, unsigned int len);
void ctime_spool(struct content_spool *spool, time_t t);
