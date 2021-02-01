/*
 * common/utils.h
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
#define fileexists(a) (!access(a,F_OK))

extern char hexchars[];

#ifdef LEGACY
#warning change slowtoi to slowtou
#define slowtoi(a) slowtou(a)
#endif
unsigned int slowtou(char *str);
unsigned int slowntou(unsigned char *str, unsigned int len);
int chomp(char *str);
#ifdef _STDINT_H
int bufferfile(unsigned char **data_out, uint64_t *len_out, char *filename);
int fd_bufferfile(unsigned char **data_out, uint64_t *len_out, int fd);
#endif
int fd_bufferfile2(char *data_out, unsigned int *len_out, unsigned int datamax, int fd);
int bufferfile2(char *data_out, unsigned int *len_out, unsigned int datamax, char *filename);
void stain(char *str_in);
void stain2(unsigned int *len_out, unsigned char *str, unsigned int len_in);
unsigned char hexval(unsigned int a, unsigned int b);
int dumpfile(unsigned char *data, unsigned int datalen, char *filename);
int postescape(FILE *ff, unsigned char *data, unsigned int l);
int strescape(FILE *ff, char *str);
char *fgets2(char **line_inout, int *len_inout, FILE *ff);
int fgets3(char **line_out, unsigned char **buffer_inout, unsigned int *bufferlen_inout, FILE *ff);
void unpostescape(char *str_in);
int endswith(char *str, char *end);
int case_endswith(char *str, char *end);
int getfilesize(unsigned int *fs_out, char *filename);
int forkexecwait(char *newdir, char *program, char *parameter);
int forkexecwait2(char *newdir, char *program, char *param1, char *param2);
int istrchr(char *str, char ch);
int openexecpipeb(int *readfd_out, int *writefd_out, char *cmd, char *cmds[]);
void killzombies();
#ifdef _COMMON_SPOOL_H
int openexecpipe(char *cmd, struct content_spool *spool, char *cmds[]);
int openexecpipe2(struct content_spool *out, char *cmd, struct content_spool *in, char *cmds[]);
#endif
#ifdef _STDINT_H
uint64_t slowto64(char *str);
uint64_t slownto64(char *str, uint64_t len);
uint64_t slowhexto64(char *str);
int64_t slowto64s(char *str);
int64_t slownto64s(char *str, uint64_t len);
int64_t istrnchr(unsigned char *str_in, uint64_t n, char ch);
int uistrnchr(uint64_t *count_out, unsigned char *str_in, uint64_t n, char ch);
#endif
int istrcasestr(char *haystack, char *needle);
void decodehex2(unsigned char *dest, unsigned char *src, unsigned int len);
unsigned int slowhextoi(char *str);
void dos2unix(char *str);
int shiftstring(char *str);
#define swriten(a,b,c) (writen(a,(unsigned char *)b,c))
#define writensz(a,b) (writen(a,(unsigned char *)b,sizeof(b)))
#define writenlen(a,b) (writen(a,(unsigned char *)b,strlen(b)))
int writen(int fd, unsigned char *msg, unsigned int len);
#define readnsz(a,b) (readn(a,b,sizeof(b)))
int readn(int fd, unsigned char *msg, unsigned int len);
int filename_readn(unsigned char *dest, unsigned int len, char *filename);
int fwriten(FILE *ff, unsigned char *msg, unsigned int len);
int isvalidfd(int fd);
int catfile(char *srcfilename, char *destfilename);
off_t getsizefromfd(int fd);
unsigned char *memcasemem(unsigned char *haystack, unsigned int hlen, unsigned char *needle, unsigned int nlen);
int memcasecmp(unsigned char *one, unsigned char *two, unsigned int len);
int fd_fgets3(char **line_out, unsigned int *len_out, unsigned char **buffer_inout, unsigned int *bufferlen_inout, int fd);
int fd_fgets(char **line_out, unsigned int *len_out, unsigned char *buffer, unsigned int bufferlen, int fd);
#ifdef _STDINT_H
uint64_t octaltou64(char *str);
#endif
unsigned int octaltoui(char *str);
int safe_fputs(char *str, FILE *fout);
int fdclose(int *fd_inout);
int fileclose(FILE **ff_inout);
unsigned char slow3tou8(char **str_out, char *str);
#ifdef _STDINT_H
int short_uint64tohex(char *dest, uint64_t u64);
int fwrite_uint64tohex(FILE *fout, uint64_t u64);
#endif
