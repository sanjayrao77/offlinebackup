/*
 * common/spool.c - linked list byte stream
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include "conventions.h"
#include "utils.h"
#include "fastmem.h"
#include "spool.h"

//static char hexchars[16]="0123456789abcdef"; // in utils.c
static int htmlsafe[256]={
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  1,  1,  0,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  0,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1
};

static int _ustring_spool(struct content_spool *first, unsigned char *data, unsigned int n) {
if (spool_fastmem(&first->fm,data,n)) {
	first->error=1;
	GOTOERROR;
}
return 0;
error:
	return -1;
}

static int _char_spool(struct content_spool *first, char ch) {
if (spool1_fastmem(&first->fm,(unsigned char)ch)) {
	first->error=1;
	GOTOERROR;
}
return 0;
error:
	return -1;
}

int char_spool(struct content_spool *spool, char ch) {
if (spool->error) return spool->error;
return _char_spool(spool,ch);
}

unsigned char *export_spool(unsigned int *len_out, struct content_spool *first) {
// export_fastmem calls malloc
if (!first) return NULL;
if (first->error) return NULL;
return export_fastmem(len_out,&first->fm);
}

int jsonspool_spool(struct content_spool *dest, struct content_spool *first) {
struct node_fastmem *node;
if (!first) return 0;
if (first->error) return -1;
node=&first->fm.node;
while (node) {
	if (ustring_json_spool(dest,node->data,node->num)) return -1;
	node=node->next;
}
return dest->error;
}
int spool_spool(struct content_spool *dest, struct content_spool *first) {
struct node_fastmem *node;
if (!first) return 0;
if (first->error) return -1;
node=&first->fm.node;
while (node) {
	if (_ustring_spool(dest,node->data,node->num)) return -1;
	node=node->next;
}
return dest->error;
}

int i_spool(struct content_spool *spool, int i) {
char str[25];
sprintf(str,"%d",i);
return content_spool(spool,str);
}

int ui_spool(struct content_spool *spool, unsigned int ui) {
char str[25];
sprintf(str,"%u",ui);
return content_spool(spool,str);
}

int ui64_spool(struct content_spool *spool, uint64_t ui) {
char str[25];
sprintf(str,"%"PRIu64,ui);
return content_spool(spool,str);
}
int i64_spool(struct content_spool *spool, int64_t i) {
char str[25];
sprintf(str,"%"PRId64,i);
return content_spool(spool,str);
}

int fmt_double_spool(struct content_spool *spool, double d) {
char str[100];
if (d>.01) snprintf(str,100,"%.02f",d);
else snprintf(str,100,"%f",d);
return content_spool(spool,str);
}

int double_spool(struct content_spool *spool, double d) {
char str[100];
snprintf(str,100,"%f",d);
return content_spool(spool,str);
}

void free_spool(struct content_spool *first) {
if (!first) return;
deinit_fastmem(&first->fm); // first no longer valid pointer
//free_fastmem(first->fastmem);
}

void reset_spool(struct content_spool *first) {
if (!first) return;
reset_fastmem(&first->fm);
first->error=0;
}

struct content_spool *new_spool(void) {
static struct content_spool blank;
struct content_spool *ret;
void *ptr;

if (!(ptr=malloc(32768+sizeof(struct content_spool)))) GOTOERROR;
ret=(struct content_spool *)(((char *)ptr)+32768);
*ret=blank;
(void)init2_fastmem(&ret->fm,ptr,32768);
return ret;
error:
	return NULL;
#if 0
struct fastmem *fastmem;
struct content_spool *ret;

fastmem=new2_fastmem(0,(void **)&ret,sizeof(struct content_spool));
if (!fastmem) GOTOERROR;
ret->error=0;
ret->fastmem=fastmem;
return ret;
error:
	return NULL;
#endif
}

int max_string_json_spool(struct content_spool *first, char *data, unsigned int max) {
unsigned int ui;
ui=strlen(data);
if (ui>max) ui=max;
return ustring_json_spool(first,(unsigned char *)data,ui);
}

int ustring_asciijson_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
// this mirrors common/jsonprint.c:fprintstring2_json
unsigned char buff[6];
unsigned char buff2[2];
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char*)"NULL",4);

memcpy(buff,"\\u00",4);
buff2[0]='\\';
runstart=data;
while (1) {
	switch (*data) {
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: /* case 8:  case 9: */
		/* case 10: */ case 11: /* case 12:  case 13: */ case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29:
		case 30: case 31:
#if 1
		// for >7bit input, useful for filenames
		case 128: case 129:
		case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139:
		case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149:
		case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159:
		case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169:
		case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179:
		case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189:
		case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199:
		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209:
		case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219:
		case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229:
		case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239:
		case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249:
		case 250: case 251: case 252: case 253: case 254: case 255:
#endif
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff[4]=hexchars[*data>>4];
			buff[5]=hexchars[*data&15];
			(ignore)_ustring_spool(first,buff,6);
			break;
		case '\"':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='\"';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\\':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='\\';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '/':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='/';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\b':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='b';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\f':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='f';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\n':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='n';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\r':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='r';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\t':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='t';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		default:
			run++;
			break;
	}
	len--;
	if (!len) break;
	data++;
}
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}
int ustring_json_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
// this mirrors common/jsonprint.c:fprintstring2_json
unsigned char buff[6];
unsigned char buff2[2];
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char*)"NULL",4);

memcpy(buff,"\\u00",4);
buff2[0]='\\';
runstart=data;
while (1) {
	switch (*data) {
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: /* case 8:  case 9: */
		/* case 10: */ case 11: /* case 12:  case 13: */ case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29:
		case 30: case 31:
#if 0
		case 128: case 129:
		case 130: case 131: case 132: case 133: case 134: case 135: case 136: case 137: case 138: case 139:
		case 140: case 141: case 142: case 143: case 144: case 145: case 146: case 147: case 148: case 149:
		case 150: case 151: case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159:
		case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: case 168: case 169:
		case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: case 178: case 179:
		case 180: case 181: case 182: case 183: case 184: case 185: case 186: case 187: case 188: case 189:
		case 190: case 191: case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199:
		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: case 208: case 209:
		case 210: case 211: case 212: case 213: case 214: case 215: case 216: case 217: case 218: case 219:
		case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229:
		case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239:
		case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: case 248: case 249:
		case 250: case 251: case 252: case 253: case 254: case 255:
#endif
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff[4]=hexchars[*data>>4];
			buff[5]=hexchars[*data&15];
			(ignore)_ustring_spool(first,buff,6);
			break;
		case '\"':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='\"';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\\':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='\\';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '/':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='/';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\b':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='b';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\f':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='f';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\n':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='n';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\r':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='r';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		case '\t':
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff2[1]='t';
			(ignore)_ustring_spool(first,buff2,2);
			break;
		default:
			run++;
			break;
	}
	len--;
	if (!len) break;
	data++;
}
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}

int ustring_url_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
unsigned char buff[3];
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char*)"NULL",4);
buff[0]='%';
runstart=data;
while (1) {
	if (!isalnum(*data)) {
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff[1]=hexchars[*data>>4];
			buff[2]=hexchars[*data&15];
			(ignore)_ustring_spool(first,buff,3);
	} else run++;
	len--;
	if (!len) break;
	data++;
}
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}
int ustring_javascript_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
unsigned char buff[3];
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char*)"NULL",4);
buff[0]='%';
runstart=data;
while (1) {
	switch (*data) {
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8: case 9:
		case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29:
		case 30: case 31:
		case 127: 
		case '%': case '\'': case ',': case ';':
		case '\"':
		case 128: case 129:
		case 130: case 131: case 132: case 133: case 134:
		case 135: case 136: case 137: case 138: case 139:
		case 140: case 141: case 142: case 143: case 144:
		case 145: case 146: case 147: case 148: case 149:
		case 150: case 151: case 152: case 153: case 154:
		case 155: case 156: case 157: case 158: case 159:
		case 160: case 161: case 162: case 163: case 164:
		case 165: case 166: case 167: case 168: case 169:
		case 170: case 171: case 172: case 173: case 174:
		case 175: case 176: case 177: case 178: case 179:
		case 180: case 181: case 182: case 183: case 184:
		case 185: case 186: case 187: case 188: case 189:
		case 190: case 191: case 192: case 193: case 194:
		case 195: case 196: case 197: case 198: case 199:
		case 200: case 201: case 202: case 203: case 204:
		case 205: case 206: case 207: case 208: case 209:
		case 210: case 211: case 212: case 213: case 214:
		case 215: case 216: case 217: case 218: case 219:
		case 220: case 221: case 222: case 223: case 224:
		case 225: case 226: case 227: case 228: case 229:
		case 230: case 231: case 232: case 233: case 234:
		case 235: case 236: case 237: case 238: case 239:
		case 240: case 241: case 242: case 243: case 244:
		case 245: case 246: case 247: case 248: case 249:
		case 250: case 251: case 252: case 253: case 254:
		case 255:
			if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
			runstart=data+1;
			buff[1]=hexchars[*data>>4];
			buff[2]=hexchars[*data&15];
			(ignore)_ustring_spool(first,buff,3);
			break;
		default:
			run++;
			break;
	}
	len--;
	if (!len) break;
	data++;
}
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}

int alnum_ustring_underscore_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char *)"NULL",4);
runstart=data;
while (1) {
	if (!isalnum(*data)) {
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		runstart=data+1;
		(ignore)_char_spool(first,'_');
	} else run++;

	len--;
	if (!len) break;
	data++;
}
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}

int ustring_spool(struct content_spool *first, unsigned char *data, unsigned int n) {
if (first->error) return first->error;
return _ustring_spool(first,data,n);
}

int content_spool(struct content_spool *first, char *data) {
if (first->error) return first->error;
if (data) return _ustring_spool(first,(unsigned char *)data,strlen(data));
else return _ustring_spool(first,(unsigned char *)"NULL",4);
}

int fwrite_spool(struct content_spool *first, FILE *ff) {
struct node_fastmem *node;
int i;
if (first->error) return -1;
node=&first->fm.node;
while (node) {
	i=fwrite(node->data,node->num,1,ff);
	if ((i!=1)&&(node->num)) return -1;
	node=node->next;
}
return 0;
}

int write_spool(int fd, struct content_spool *first) {
struct node_fastmem *node;
int i;
if (first->error) return -1;
node=&first->fm.node;
while (node) {
	i=write(fd,node->data,node->num);
	if (i!=node->num) return -1;
	node=node->next;
}
return 0;
}

int ustring_html_spool(struct content_spool *first, unsigned char *data, int l) {
return ustring_html2_spool(first,data,l,1);
}

int ustring_asciihtml_spool(struct content_spool *first, unsigned char *data, unsigned int len) {
	/* spool_content3, reborn -- htmlsafe*/
// dobrs => convert >=2 linefeeds to a <br><br> if anything follows the linefeeds
int numbadchs=4;
unsigned int badchs[]={ (unsigned int)'<' , (unsigned int)'>' , (unsigned int)'&' ,(unsigned int) '"' };
unsigned char *repstr[]={(unsigned char *)"&lt;",(unsigned char *)"&gt;",(unsigned char *)"&amp;",(unsigned char *)"&quot;"};
unsigned int replen[]={4,4,5,6};
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char *)"NULL",4);
runstart=data;
while (1) {
	unsigned int k;
//doublecontinue:
	k=*data;
#if 1
	// ascii input, good for filenames
	if (k&128) {
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		runstart=data+1;
		(ignore)_ustring_spool(first,(unsigned char *)"&#",2);
		(ignore)ui_spool(first,k);
		(ignore)_char_spool(first,';');
	} else 
#endif
#if 0
	if (k=='\n') {
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		if (len<3) goto doublebreak;
		if (data[1]=='\n') {
			data+=2;
			len-=2;
			while (1) {
				if (*data!='\n') {
					if (dobrs) (ignore)_ustring_spool(first,(unsigned char *)"<br><br>",8);
					runstart=data;
					goto doublecontinue;
				}
				len--;
				if (!len) goto doublebreak;
				data++;
			}
		}
		runstart=data+1;
	} else if (!htmlsafe[k]) {
#else
	if (!htmlsafe[k]) {
#endif
		int j;
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		runstart=data+1;
	
		for (j=0;j<numbadchs;j++) {
			if (k==badchs[j]) {
				(ignore)_ustring_spool(first,repstr[j],replen[j]);
				goto found;
			}
		}
		(ignore)_char_spool(first,'.');
		found:{}
	} else {
		run++;
	}

	len--;
	if (!len) break;
	data++;
}
//doublebreak:
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}
int ustring_html2_spool(struct content_spool *first, unsigned char *data, unsigned int len, unsigned int dobrs) {
	/* spool_content3, reborn -- htmlsafe*/
// dobrs => convert >=2 linefeeds to a <br><br> if anything follows the linefeeds
int numbadchs=4;
unsigned int badchs[]={ (unsigned int)'<' , (unsigned int)'>' , (unsigned int)'&' ,(unsigned int) '"' };
unsigned char *repstr[]={(unsigned char *)"&lt;",(unsigned char *)"&gt;",(unsigned char *)"&amp;",(unsigned char *)"&quot;"};
unsigned int replen[]={4,4,5,6};
unsigned char *runstart;
unsigned int run=0;

if (first->error) return first->error;
if (!len) return 0;
if (!data) return _ustring_spool(first,(unsigned char *)"NULL",4);
runstart=data;
while (1) {
	unsigned int k;
doublecontinue:
	k=*data;
#if 0
	// This isn't UTF8, let's let go of this
	if (k&128) {
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		runstart=data+1;
		(ignore)_ustring_spool(first,(unsigned char *)"&#",2);
		(ignore)ui_spool(first,k);
		(ignore)_char_spool(first,';');
	} else 
#endif
	if (k=='\n') {
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		if (len<3) goto doublebreak;
		if (data[1]=='\n') {
			data+=2;
			len-=2;
			while (1) {
				if (*data!='\n') {
					if (dobrs) (ignore)_ustring_spool(first,(unsigned char *)"<br><br>",8);
					runstart=data;
					goto doublecontinue;
				}
				len--;
				if (!len) goto doublebreak;
				data++;
			}
		}
		runstart=data+1;
	} else if (!htmlsafe[k]) {
		int j;
		if (run) { (ignore)_ustring_spool(first,runstart,run); run=0; }
		runstart=data+1;
	
		for (j=0;j<numbadchs;j++) {
			if (k==badchs[j]) {
				(ignore)_ustring_spool(first,repstr[j],replen[j]);
				goto found;
			}
		}
		(ignore)_char_spool(first,'.');
		found:{}
	} else {
		run++;
	}

	len--;
	if (!len) break;
	data++;
}
doublebreak:
if (run) (ignore)_ustring_spool(first,runstart,run);
return first->error;
}

int htmltable_spool(struct content_spool *first, char *data) {
int i;
/* use this for display (tables) */
if (first->error) return first->error;
if (!data) return _ustring_spool(first,(unsigned char *)"NULL",4);
else if (!data[0]) return _ustring_spool(first,(unsigned char *)"&nbsp;",6);
i=strlen(data);
return ustring_html_spool(first,(unsigned char *)data,i);
}

int url_spool(struct content_spool *first, char *data, int maxlen) {
int i;
if (first->error) return first->error;
i=strlen(data);
if (i<=maxlen) return ustring_html_spool(first,(unsigned char *)data,i);
if (ustring_html_spool(first,(unsigned char *)data,maxlen)) return -1;
return _ustring_spool(first,(unsigned char *)"...",3);
}

int fread_spool(struct content_spool *spool, FILE *fin) {
while (1) {
	unsigned char *dest;
	unsigned int n;
	int k;
	dest=reserve_spool(&n,spool,1024);
	if (!dest) GOTOERROR;
	k=fread(dest,1,n,fin);
	if (k<1) {
		if (!k) break;
		goto error;
	}
	if (_ustring_spool(spool,dest,(unsigned int)k)) goto error;
}
return 0;
error:
	return -1;
}

int file_spool(struct content_spool *spool, char *filename) {
FILE *ff=NULL;
if (spool->error) return spool->error;
ff=fopen(filename,"rb");
if (!ff) goto error;
if (fread_spool(spool,ff)) GOTOERROR;
(ignore)fclose(ff);
return 0;
error:
	iffclose(ff);
	return -1;
}

int string_underscore_spool(struct content_spool *spool, char *str_in) {
unsigned char *str,*runstart;
unsigned int run=0;

if (spool->error) return spool->error;
if (!str_in) return _ustring_spool(spool,(unsigned char*)"NULL",4);
runstart=str=(unsigned char *)str_in;
while (1) {
	switch (*str) {
		case 0: goto doublebreak;
		case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8: case 9:
		case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29:
		case 30: case 31: case 32:
		case 127: 
		case '\'': case ',':
			if (run) { (ignore)_ustring_spool(spool,runstart,run); run=0; }
			runstart=str+1;
			(ignore)_char_spool(spool,'_');
			break;
		default:
			run++;
			break;
	}
	str++;
}
doublebreak:
if (run) (ignore)_ustring_spool(spool,runstart,run);
return spool->error;
}

int dump_spool(char *filename, struct content_spool *first) {
FILE *ff;
ff=fopen(filename,"wb");
if (!ff) GOTOERROR;
fwrite_spool(first,ff);
if (fileclose(&ff)) GOTOERROR;
return 0;
error:
	iffclose(ff);
	return -1;
}

int strcmp_spool(struct content_spool *first, char *str) {
struct node_fastmem *node;
if (!first) return -1;
if (first->error) return -1;
node=&first->fm.node;
while (node) {
	if (node->num) {
		int i;
		i=strncmp(str,(char *)node->data,node->num);
		if (i) return i;
		str+=node->num;
	}
	node=node->next;
}
return 0;
}

int isempty_spool(struct content_spool *first) {
struct node_fastmem *node;
if (!first) return 1;
node=&first->fm.node;
while (node) {
	if (node->num) return 0;
	node=node->next;
}
return 1;
}

int hex2_spool(struct content_spool *spool, unsigned char *src, unsigned int len) {
unsigned char buff2[2];

if (spool->error) return spool->error;
while (len) {
	buff2[0]=hexchars[src[0]/16];
	buff2[1]=hexchars[src[0]%16];
	if (_ustring_spool(spool,buff2,2)) return -1;
	src++;
	len--;
}
return 0;
}

int readstream_spool(struct content_spool *spool, int fd) {
while (1) {
	unsigned char *dest;
	unsigned int n;
	int k;
	dest=reserve_spool(&n,spool,1);
	if (!dest) GOTOERROR;
	k=read(fd,dest,n);
	if (k<=0) {
		if (!k) break;
		GOTOERROR;
	}
	(ignore)advance_spool(spool,k);
}
return 0;
error:
	return -1;
}

void ctime_spool(struct content_spool *spool, time_t t) {
char *temp;
temp=ctime(&t);
if (!temp) return; // can this happen?
(ignore)_ustring_spool(spool,(unsigned char *)temp,24);
}
