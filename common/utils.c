/*
 * common/utils.c - common functions
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
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>
#include "conventions.h"
#include "fastmem.h"
#include "spool.h"

#if 0
#define DEBUG
#endif

#include "utils.h"

char hexchars[16]="0123456789abcdef";

int64_t slownto64s(char *str, uint64_t len) {
int64_t ret=0;
int isneg=0;

if (!len) return 0;

if (str[0]=='-') {
	isneg=1;
	str++;
	len--;
	if (!len) return 0;
}

switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
len--;
while (1) {
	if (!len) {
			if (isneg) return -ret;
			return ret;
	}
	len--;
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default:
			if (isneg) return -ret;
			return ret;
			break;
	}
}
}
int64_t slowto64s(char *str) {
int64_t ret=0;
int isneg=0;

if (str[0]=='-') {
	isneg=1;
	str++;
}

switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
while (1) {
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default:
			if (isneg) return -ret;
			return ret;
			break;
	}
}
}

unsigned int slowntou(unsigned char *str, unsigned int len) {
unsigned int ret=0;
if (!len) return 0;
switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
len--;
while (1) {
	if (!len) return ret;
	len--;
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default: return ret; break;
	}
}
return ret;
}
uint64_t slownto64(char *str, uint64_t len) {
uint64_t ret=0;
if (!len) return 0;
switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
len--;
while (1) {
	if (!len) return ret;
	len--;
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default: return ret; break;
	}
}
return ret;
}
uint64_t slowto64(char *str) {
uint64_t ret=0;
switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
while (1) {
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default: return ret; break;
	}
}
return ret;
}
unsigned int slowtou(char *str) {
unsigned int ret=0;
switch (*str) {
	case '1': ret=1; break;
	case '2': ret=2; break;
	case '3': ret=3; break;
	case '4': ret=4; break;
	case '5': ret=5; break;
	case '6': ret=6; break;
	case '7': ret=7; break;
	case '8': ret=8; break;
	case '9': ret=9; break;
	case '+':
	case '0': break;
	default: return 0; break;
}
while (1) {
	str++;
	switch (*str) {
		case '9': ret=ret*10+9; break;
		case '8': ret=ret*10+8; break;
		case '7': ret=ret*10+7; break;
		case '6': ret=ret*10+6; break;
		case '5': ret=ret*10+5; break;
		case '4': ret=ret*10+4; break;
		case '3': ret=ret*10+3; break;
		case '2': ret=ret*10+2; break;
		case '1': ret=ret*10+1; break;
		case '0': ret=ret*10; break;
		default: return ret; break;
	}
}
return ret;
}

unsigned char slow3tou8(char **str_out, char *str) {
int i;
switch (*str) {
	case '0': i=0; break;
	case '1': i=100; break;
	case '2': i=200; break;
	default:
		*str_out=NULL;
		return 0;
		break;
}
str++;
switch (*str) {
	case '0': break; case '1': i+=10; break; case '2': i+=20; break; case '3': i+=30; break; case '4': i+=40; break;
	case '5': i+=50; break; case '6': i+=60; break; case '7': i+=70; break; case '8': i+=80; break; case '9': i+=90; break;
	default:
		*str_out=NULL;
		return 0;
		break;
}
str++;
switch (*str) {
	case '0': break; case '1': i+=1; break; case '2': i+=2; break; case '3': i+=3; break; case '4': i+=4; break;
	case '5': i+=5; break; case '6': i+=6; break; case '7': i+=7; break; case '8': i+=8; break; case '9': i+=9; break;
	default:
		*str_out=NULL;
		return 0;
		break;
}
str++;
*str_out=str;
return (unsigned char)i;
}

int chomp(char *str) {
while (1) {
	switch (*str) {
		case '\0':
			return -1;
		case '\r':
		case '\n':
			*str='\0';
			return 0;

	}
	str++;
}
return -1;
}

int dumpfile(unsigned char *data, unsigned int datalen, char *filename) {
FILE *ff=NULL;
ff=fopen(filename,"wb");
if (!ff) goto error;
if (fwrite(data,datalen,1,ff)!=1) goto error;
if (fileclose(&ff)) GOTOERROR;
return 0;
error:
	iffclose(ff);
	return -1;
}

int catfile(char *srcfilename, char *destfilename) {
unsigned char buffer[512];
int i;
int fdin=-1,fdout=-1;

fdin=open(srcfilename,O_RDONLY);
if (fdin<0) GOTOERROR;
fdout=open(destfilename,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
if (fdout<0) GOTOERROR;

while (1) {
	i=read(fdin,buffer,512);
	if (i<=0) {
		if (!i) break;
		GOTOERROR;
	}
	if (writen(fdout,buffer,i)) GOTOERROR;
}

(ignore)close(fdin);
if (fdclose(&fdout)) GOTOERROR;
return 0;
error:
	ifclose(fdin);
	ifclose(fdout);
	return -1;
}

off_t getsizefromfd(int fd) {
struct stat st;
if (fstat(fd,&st)) return -1;
return st.st_size;
}

int filename_readn(unsigned char *dest, unsigned int len, char *filename) {
int fd=-1;
fd=open(filename,O_RDONLY);
if (fd<0) GOTOERROR;
if (readn(fd,dest,len)) GOTOERROR;
(ignore)close(fd);
return 0;
error:
	ifclose(fd);
	return -1;
}

int fd_bufferfile2(char *data_out, unsigned int *len_out, unsigned int datamax, int fd) {
struct stat st;
uint64_t len;
if (lseek(fd,0,SEEK_SET)) GOTOERROR;
if (fstat(fd,&st)) GOTOERROR;
len=st.st_size;
if (len>=datamax) GOTOERROR;
data_out[len]='\0';
if (readn(fd,(unsigned char *)data_out,len)) GOTOERROR;

*len_out=len;
return 0;
error:
	return -1;
}
int fd_bufferfile(unsigned char **data_out, uint64_t *len_out, int fd) {
struct stat st;
uint64_t len;
unsigned char *data=NULL;
unsigned char *temp;
int i;
if (lseek(fd,0,SEEK_SET)) GOTOERROR;
if (fstat(fd,&st)) GOTOERROR;
len=st.st_size;
temp=data=malloc(len+1);
if (!data) GOTOERROR;
data[len]='\0';
while (len) {
	i=read(fd,temp,len);
	if (i<1) GOTOERROR;
	len-=i;
	temp+=i;
}
len=st.st_size;

*data_out=data;
*len_out=len;
return 0;
error:
	iffree(data);
	return -1;
}

int bufferfile2(char *data_out, unsigned int *len_out, unsigned int datamax, char *filename) {
int fd;
fd=open(filename,O_RDONLY);
if (fd<0) GOTOERROR;
if (fd_bufferfile2(data_out,len_out,datamax,fd)) GOTOERROR;
(ignore)close(fd);
return 0;
error:
	ifclose(fd);
	return -1;
}
int bufferfile(unsigned char **data_out, uint64_t *len_out, char *filename) {
int fd;
fd=open(filename,O_RDONLY);
if (fd<0) GOTOERROR;
if (fd_bufferfile(data_out,len_out,fd)) GOTOERROR;
(ignore)close(fd);
return 0;
error:
	ifclose(fd);
	return -1;
}
#if 0
int bufferfile(unsigned char **data_out, uint64_t *len_out, char *filename) {
FILE *ff=NULL;
unsigned int len;
unsigned char *data=NULL;

ff=fopen(filename,"rb");
if (!ff) goto error;
if (fseek(ff,0,SEEK_END)) goto error;
len=ftell(ff);
(void)rewind(ff);
data=malloc(len+1);
if (!data) goto error;
data[len]='\0';
if (fread(data,len,1,ff)!=1) goto error;
fclose(ff);
*data_out=data;
*len_out=len;
return 0;
error:
	iffree(data);
	iffclose(ff);
	return -1;
}
#endif

static unsigned char _hexval(unsigned int a, unsigned int b) {
	/* a is high 4, b is low 4 */
if (a&64) a=((a&31)+9)<<4;
else a=(a&15)<<4;
if (b&64) b=(b&31)+9;
else b=b&15;
return (unsigned char)(a|b);
}
static unsigned char _hexval1(unsigned int b) {
if (!(b&64)) return (unsigned char)b&15;
return (unsigned char)(b&31)+9;
}

unsigned char hexval(unsigned int a, unsigned int b) {
return _hexval(a,b);
}

unsigned int slowhextoi(char *str) {
unsigned int ret=0;
while (*str) {
	ret<<=4;
	ret|=_hexval1(*str);
	str++;
}
return ret;
}

uint64_t slowhexto64(char *str) {
uint64_t ret=0;
while (*str) {
	ret<<=4;
	ret|=_hexval1(*str);
	str++;
}
return ret;
}

void stain2(unsigned int *len_out, unsigned char *str, unsigned int len_in) {
unsigned int i,j;

j=len_in;
for (i=0;i<j;i++) {
	if (str[0]=='%') {
		if (i+2>=j) break;
		{
			unsigned int x,y;
			x=str[1]; y=str[2];

			if (x&64) x=((x&31)+9)<<4;
			else x=(x&15)<<4;

			if (y&64) y=(y&31)+9;
			else y=y&15;

			str[0]=(unsigned char)(x|y);
		}
//		if (str[0]=='\0') str[0]=' '; // don't do this
		j-=2;
		memmove(str+1,str+3,j-i);
	} 
	str++;
}
*len_out=j;
}
void stain(char *str_in) {
int i,j;
unsigned char *str;
unsigned int x,y;

str=(unsigned char *)str_in;
j=strlen(str_in);
for (i=0;i<j;i++) {
	if (str[0]=='%') {
		if (i+2>=j) return;
		{
			x=str[1]; y=str[2];

			if (x&64) x=((x&31)+9)<<4;
			else x=(x&15)<<4;

			if (y&64) y=(y&31)+9;
			else y=y&15;

			str[0]=(unsigned char)(x|y);
		}
		if (str[0]=='\0') str[0]=' ';
		j-=2;
		memmove(str+1,str+3,j-i);
	} 
	str++;
}
}

int postescape(FILE *ff, unsigned char *data, unsigned int l) {
unsigned int ui,uk;
unsigned int last=0;
char buff[3]={'%',0,0};

for (ui=0;ui<l;ui++) {
	uk=data[ui];
	switch (uk) {
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8: case 9:
		case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29:
		case 30: case 31: 
		case '+':
		case 127: 
		case '%':
			(ignore)fwrite(data+last,ui-last,1,ff);
			buff[1]=hexchars[uk>>4];
			buff[2]=hexchars[uk&15];
			(ignore)fwrite(buff,3,1,ff);
			last=ui+1;
		break;
		case 32:
			(ignore)fwrite(data+last,ui-last,1,ff);
			(ignore)fputc('+',ff);
			last=ui+1;
		break;
	}
}
(ignore)fwrite(data+last,l-last,1,ff);
if (ferror(ff)) goto error;
return 0;
error:
	return -1;
}

#if 0
// obsolete, use fgets3 instead
char *fgets2(char **line_inout, int *len_inout, FILE *ff) {
char *line=*line_inout;
int m,n=*len_inout;
if (!fgets(line,n,ff)) goto error;
while (chomp(line)) {
	char *temp;
	m=n+1024;
	temp=realloc(line,m);
	if (!temp) goto error;
	line=*line_inout=temp;
	if (!fgets(line+n-1,1024,ff)) goto error;
	n=*len_inout=m;
}
return line;
error:
	return NULL;
}
#endif


#define FGETSBUFFER	256

int fd_fgets3(char **line_out, unsigned int *len_out, unsigned char **buffer_inout, unsigned int *bufferlen_inout, int fd) {
char *line=(char *)*buffer_inout;
unsigned int bytesread=0;
unsigned int bytesfree;

{
	unsigned int bufferlen=*bufferlen_inout;
	if (bufferlen) {
		bytesfree=bufferlen-1;
	} else bytesfree=0;
}

while (1) {
	int i,k;
	if (bytesfree) {
		k=_BADMIN(bytesfree,FGETSBUFFER);
	} else {
		unsigned int bufferlen;
		bytesfree=*bufferlen_inout+FGETSBUFFER;
		bufferlen=(*bufferlen_inout)+bytesfree+1;
		line=realloc(line,bufferlen);
		if (!line) GOTOERROR;
		*bufferlen_inout=bufferlen;
		*buffer_inout=(unsigned char *)line;
		k=FGETSBUFFER;
	}
	i=read(fd,line+bytesread,k);
	if (i<0) GOTOERROR;
	if (!i) {
		if (!bytesread) { *line_out=NULL; if (len_out) *len_out=0; return 0; }
		line[bytesread]='\0';
		*line_out=line;
		if (len_out) *len_out=bytesread;
		return 0;
	}
	{
		char *temp;
		temp=line+bytesread;
		k=i;
		while (1) {
			i--;
			if (*temp=='\n') {
				temp[1]='\0';
				*line_out=line;
				if (len_out) *len_out=bytesread+k-i;
				(ignore)lseek(fd,-i,SEEK_CUR);
				return 0;
			}
			if (!i) break;
			temp++;
		}
		bytesread+=k;
		bytesfree-=k;
	}
}
error:
	return -1;
}

int fd_fgets(char **line_out, unsigned int *len_out, unsigned char *buffer, unsigned int bufferlen, int fd) {
// untested
char *line=(char *)buffer;
unsigned int bytesread=0;
unsigned int bytesfree;

if (bufferlen<2) return -1;
bytesfree=bufferlen-1;

while (1) {
	int i,k;
	if (bytesfree) {
		k=_BADMIN(bytesfree,FGETSBUFFER);
	} else {
		line[bytesread]='\0';
		*line_out=line;
		if (len_out) *len_out=bytesread;
		return 0;
	}
	i=read(fd,line+bytesread,k);
	if (i<0) GOTOERROR;
	if (!i) {
		if (!bytesread) { *line_out=NULL; if (len_out) *len_out=0; return 0; }
		line[bytesread]='\0';
		*line_out=line;
		if (len_out) *len_out=bytesread;
		return 0;
	}
	{
		char *temp;
		temp=line+bytesread;
		k=i;
		while (1) {
			i--;
			if (*temp=='\n') {
				temp[1]='\0';
				*line_out=line;
				if (len_out) *len_out=bytesread+k-i;
				(ignore)lseek(fd,-i,SEEK_CUR);
				return 0;
			}
			if (!i) break;
			temp++;
		}
		bytesread+=k;
		bytesfree-=k;
	}
}
error:
	return -1;
}

int fgets3(char **line_out, unsigned char **buffer_inout, unsigned int *bufferlen_inout, FILE *ff) {
char *line=(char *)*buffer_inout;
unsigned int bufferlen=*bufferlen_inout;
unsigned int bytesread;

if (!bufferlen) {
	bufferlen=256;
	line=realloc(NULL,bufferlen);
	if (!line) GOTOERROR;
	*bufferlen_inout=bufferlen;
	*buffer_inout=(unsigned char *)line;
}

if (!fgets(line,bufferlen,ff)) {
	*line_out=NULL;
	return 0;
}
while (1) {
	if (strchr(line,'\n')) break;
	if (feof(ff)) break;
	bytesread=bufferlen-1;
	bufferlen*=2;
	line=realloc(line,bufferlen);
	if (!line) GOTOERROR;
	*bufferlen_inout=bufferlen;
	*buffer_inout=(unsigned char *)line;

	if (!fgets(line+bytesread,bufferlen-bytesread,ff)) break;
}

*line_out=line;
return 0;
error:
	return -1;
}

void unpostescape(char *str_in) {
int i,j;
unsigned char *str;
unsigned int x,y;

str=(unsigned char *)str_in;
for (j=0;str[j];j++) if (str[j]==' ') { j++; break; }
for (i=0;i<j;i++) {
	switch (str[0]) {
		case '%':
			if (i+2>=j) return;
			{
				x=str[1]; y=str[2];

				if (x&64) x=((x&31)+9)<<4;
				else x=(x&15)<<4;

				if (y&64) y=(y&31)+9;
				else y=y&15;

				str[0]=(unsigned char)(x|y);
			}
			if (str[0]=='\0') str[0]=' ';
			j-=2;
			memmove(str+1,str+3,j-i);
		break;
		case '+':
			str[0]=' ';
		break;
		case ' ':
			str[0]='\0';
			return;
		break;

	}
	str++;
}
}

int case_endswith(char *str, char *end) {
unsigned int n,m;
m=strlen(str);
n=strlen(end);
if (m<n) return 0;
return (!memcasecmp((unsigned char *)str+m-n,(unsigned char *)end,n));
}
int endswith(char *str, char *end) {
unsigned int n,m;
m=strlen(str);
n=strlen(end);
if (m<n) return 0;
return (!memcmp(str+m-n,end,n));
}

int strescape(FILE *ff, char *str) {
return postescape(ff,(unsigned char *)str,strlen(str));
}

int forkexecwait(char *newdir, char *program, char *parameter) {
return forkexecwait2(newdir,program,parameter,NULL);
}

int forkexecwait2(char *newdir, char *program, char *param1, char *param2) {
int i;
int fd;

i=fork();
if (i==-1) return -1;
if (i) {
	waitpid(i,NULL,0);
	return 0;
}

/* child now */
fd=open("/dev/null",O_WRONLY);
if (fd==-1) goto done;

if (dup2(fd,1)==-1) goto done;
if (dup2(fd,2)==-1) goto done;

if ((newdir)&&(newdir[0])) chdir(newdir);

execlp(program,program,param1,param2,NULL);

done:
exit(0);
return -1;
}

#if 0
// should be updated to u64
int getfilesize(unsigned int *fs_out, char *filename) {
struct stat st;
if (stat(filename,&st)) goto error;
*fs_out=st.st_size;
return 0;
error:
	return -1;
}
#endif

int istrchr(char *str, char ch) {
int i=0;
while (*str) {
	if (*str==ch) return i;
	i++;
	str++;
}
return -1;
}

int64_t istrnchr(unsigned char *str_in, uint64_t n, char ch) {
unsigned char *str=str_in;
while (n) {
	if (*str==ch) return (int64_t)(str-str_in);
	n--;
	str++;
}
return -1;
}
int uistrnchr(uint64_t *count_out, unsigned char *str_in, uint64_t n, char ch) {
unsigned char *str=str_in;
while (n) {
	if (*str==ch) {
		*count_out=(uint64_t)(str-str_in);
		return 0;
	}
	n--;
	str++;
}
return -1;
}

int istrcasestr(char *haystack, char *needle) {
int m,n,i=0;
n=strlen(needle);
m=strlen(haystack);
if (m<n) return -1;
m=m-n+1;
while (m) {
	if (!strncasecmp(haystack,needle,n)) return i;
	m--;
	i++;
	haystack++;
}
return -1;
}

int openexecpipeb(int *readfd_out, int *writefd_out, char *cmd, char *cmds[]) {
int filedes[4]={-1,-1,-1,-1};
pid_t pid;
int i;
int ischild=0;

if (pipe(&filedes[0])) GOTOERROR;
if (pipe(&filedes[2])) GOTOERROR;

(ignore)fflush(stdout);

pid=fork();
if (pid<0) GOTOERROR;
if (pid) {
	/* parent */
	close(filedes[0]); filedes[0]=-1;
	close(filedes[3]); filedes[3]=-1;
	*writefd_out=filedes[1];
	*readfd_out=filedes[2];
} else {
	/* child */
	ischild=1;
	close(filedes[1]); filedes[1]=-1;
	close(filedes[2]); filedes[2]=-1;
	if (dup2(filedes[0],STDIN_FILENO)==-1) GOTOERROR;
	if (dup2(filedes[3],STDOUT_FILENO)==-1) GOTOERROR;
	if (execvp(cmd,cmds)) {
#ifdef DEBUG
		fprintf(stderr,"Error running \"%s\"\n",cmd); 
#endif
		GOTOERROR;
	}
}

return 0;
error:
	for (i=0;i<4;i++) if (filedes[i]>=0) close(filedes[i]);
	if (ischild) exit(0);
	return -1;
}

int openexecpipe(char *cmd, struct content_spool *spool, char *cmds[]) {
char *spare[2];
int readfd=-1,writefd=-1;
pid_t pid;

if (!cmds) {
	cmds=spare;
	spare[0]=cmd;
	spare[1]=NULL;
}

if (openexecpipeb(&readfd,&writefd,cmd,cmds)) GOTOERROR;
pid=fork();
if (pid<0) GOTOERROR;
if (!pid) {
	if (spool) (ignore)write_spool(writefd,spool);
	_exit(0);
}
(ignore)close(writefd);

return readfd;
error:
	return -1;
}

int openexecpipe2(struct content_spool *out, char *cmd, 
		struct content_spool *in, char *cmds[]) {
int fd=-1;
unsigned char buffer[512];
fd=openexecpipe(cmd,in,cmds);
if (fd<0) GOTOERROR;
while (1) {
	int i;
	i=read(fd,buffer,512);
	if (i<0) GOTOERROR;
	if (!i) break;
	if (ustring_spool(out,buffer,i)) GOTOERROR;
}
(ignore)close(fd);
return 0;
error:
	ifclose(fd);
	return -1;
}

void killzombies() {
while (waitpid(-1,NULL,WNOHANG)>0);
}


void decodehex2(unsigned char *dest, unsigned char *src, unsigned int len) {
/* len is number of 8bits */
while (len) {
	*dest=_hexval(src[0],src[1]);
	dest++;
	src+=2;
	len--;
}
}

int shiftstring(char *str) {
int r;
r=*str;
if (!r) return 0;
while (1) {
	*str=str[1];
	if (!*str) break;
	str++;
}
return r;
}

void dos2unix(char *str) {
while (1) {
	switch (*str) {
		case 0: return;
		case '\r':
			(void)shiftstring(str);
			break;
	}
	str++;
}
}

int readn(int fd, unsigned char *msg, unsigned int len) {
while (len) {
	ssize_t k;
	k=read(fd,(char *)msg,len);
	if (k<1) {
		if ((k<0) && (errno==EINTR)) continue;
		return -1;
	}
	len-=k;
	msg+=k;
}
return 0;
}
int writen(int fd, unsigned char *msg, unsigned int len) {
while (len) {
	ssize_t k;
	k=write(fd,(char *)msg,len);
	if (k<1) {
		if ((k<0) && (errno==EINTR)) continue;
		return -1;
	}
	len-=k;
	msg+=k;
}
return 0;
}
int fwriten(FILE *ff, unsigned char *msg, unsigned int len) {
if (!len) return 0;
if (fwrite(msg,len,1,ff)!=1) return -1;
return 0;
}

int isvalidfd(int fd) {
// this doesn't work
if (fcntl(fd,F_GETFL)==-1) return 0;
return 1;
}

int memcasecmp(unsigned char *one, unsigned char *two, unsigned int len) {
if (len) while (1) {
	int i;
	i=tolower(*one)-tolower(*two);
	if (i) return i;
	len--;
	if (!len) break;
	one++;
	two++;
}
return 0;
}

unsigned char *memcasemem(unsigned char *haystack, unsigned int hlen, unsigned char *needle, unsigned int nlen) {
unsigned int n;
if (nlen>hlen) return NULL;
n=hlen-nlen;
while (1) {
	if (!memcasecmp(haystack,needle,nlen)) return haystack;
	if (!n) break;
	n--;
	haystack++;
}
return NULL;
}

uint64_t octaltou64(char *str) {
uint64_t ui;
switch (*str) {
	case '0': ui=0; break;
	case '1': ui=1; break;
	case '2': ui=2; break;
	case '3': ui=3; break;
	case '4': ui=4; break;
	case '5': ui=5; break;
	case '6': ui=6; break;
	case '7': ui=7; break;
	default: return 0;
}
str++;
while (1) {
	switch (*str) {
		case '0': ui=ui*8; break;
		case '1': ui=ui*8+1; break;
		case '2': ui=ui*8+2; break;
		case '3': ui=ui*8+3; break;
		case '4': ui=ui*8+4; break;
		case '5': ui=ui*8+5; break;
		case '6': ui=ui*8+6; break;
		case '7': ui=ui*8+7; break;
		default: return ui;
	}
	str++;
}
}
unsigned int octaltoui(char *str) {
unsigned int ui;
switch (*str) {
	case '0': ui=0; break;
	case '1': ui=1; break;
	case '2': ui=2; break;
	case '3': ui=3; break;
	case '4': ui=4; break;
	case '5': ui=5; break;
	case '6': ui=6; break;
	case '7': ui=7; break;
	default: return 0;
}
str++;
while (1) {
	switch (*str) {
		case '0': ui=ui*8; break;
		case '1': ui=ui*8+1; break;
		case '2': ui=ui*8+2; break;
		case '3': ui=ui*8+3; break;
		case '4': ui=ui*8+4; break;
		case '5': ui=ui*8+5; break;
		case '6': ui=ui*8+6; break;
		case '7': ui=ui*8+7; break;
		default: return ui;
	}
	str++;
}
}

int safe_fputs(char *str, FILE *fout) {
while (1) {
	if (!*str) break;
	if (isgraph(*str)) fputc(*str,fout);
	else fputc('_',fout);
	str++;
}
return 0;
}

int fdclose(int *fd_inout) {
int fd;
fd=*fd_inout;
*fd_inout=-1;
if (close(fd)) {
	if (errno==EBADF) return -1;
	if (errno==EIO) return -1;
}
return 0;
}

int fileclose(FILE **ff_inout) {
FILE *ff;
ff=*ff_inout;
*ff_inout=NULL;
if (fclose(ff)) {
	if (errno==EBADF) return -1;
	if (errno==EIO) return -1;
}
return 0;
}

int short_uint64tohex(char *dest, uint64_t u64) {
// dest should have at least 17 bytes
int n;
#define DOIT(a) do { dest+=a; dest[0]='\0'; for (n=a-1;n>=0;n--) { dest--; dest[0]=hexchars[u64&15]; u64=u64>>4; } return a; } while (0)
if (u64&0xFFFFFFFF00000000) {
	if (u64&0xFFFF000000000000) {
		if (u64&0xFF00000000000000) {
			if (u64&0xF000000000000000) {
				DOIT(16);
			}
			DOIT(15);
		}	
		if (u64&0x00F0000000000000) {
			DOIT(14);
		}
		DOIT(13);
	} else { // 0x0000FFFF00000000
		if (u64&0x0000FF0000000000) {
			if (u64&0x0000F00000000000) {
				DOIT(12);
			} else { // 0x00000F0000000000
				DOIT(11);
			}
		} else { // 0x000000FF00000000
			if (u64&0x000000F000000000) {
				DOIT(10);
			} else { // 0x0000000F00000000
				DOIT(9);
			}
		}
	}
} else { // lower 32
	if (u64&0xFFFF0000) {
		if (u64&0xFF000000) {
			if (u64&0xF0000000) {
				DOIT(8);
			} else { // 0x0F000000
				DOIT(7);
			}
		} else { // 0x00FF0000
			if (u64&0xF00000) {
				DOIT(6);
			} else { // 0xF0000
				DOIT(5);
			}
		}
	} else { // 0xFFFF
		if (u64&0xFF00) {
			if (u64&0xF000) {
				DOIT(4);
			} else { // 0xF00
				DOIT(3);
			}
		} else { // 0xFF
			if (u64&0xF0) {
				DOIT(2);
			} else {
				DOIT(1);
			}
		}
	}
}
#undef DOIT
}

int fwrite_uint64tohex(FILE *fout, uint64_t u64) {
char buff[17];
int n;
n=short_uint64tohex(buff,u64);
if (fwrite(buff,n,1,fout)!=1) return -1;
return n;
}

