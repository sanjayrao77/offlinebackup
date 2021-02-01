
/*
 * yesno.c - handle user interaction
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#define DEBUG
#include "common/conventions.h"
#include "base.h"

#if 1
int getyes(int *isyes_out) {
int isyes=0;
char buffer[8];

fgets(buffer,8,stdin);
if (!strchr(buffer,'\n')) GOTOERROR;
if (tolower(buffer[0])=='y') isyes=1;

*isyes_out=isyes;
return 0;
error:
	return -1;
}
#endif

// when did this ever work?
#if 0
int getyes(int *isyes_out) {
static struct timeval tv;
fd_set fds;
int ret=0;
char ch;

FD_ZERO(&fds);
FD_SET(STDIN_FILENO,&fds);
while (1==select(STDIN_FILENO+1,&fds,NULL,NULL,&tv)) {
	if (read(STDIN_FILENO,&ch,1)!=1) GOTOERROR;
}
fflush(stdout);
if (ferror(stdout)) GOTOERROR;
FD_SET(STDIN_FILENO,&fds);
if (1!=select(STDIN_FILENO+1,&fds,NULL,NULL,NULL)) GOTOERROR;
if (read(STDIN_FILENO,&ch,1)!=1) GOTOERROR;
if ((ch=='y')||(ch=='Y')) ret=1;
while (1==read(STDIN_FILENO,&ch,1));
*isyes_out=ret;
return 0;
error:
	return -1;
}
#endif

#if 0
int getyes(int *isyes_out) {
fd_set fds;
static struct timeval tv;
char ch;

FD_ZERO(&fds);
FD_SET(STDIN_FILENO,&fds);
while (1==select(STDIN_FILENO+1,&fds,NULL,NULL,&tv)) {
	if (read(STDIN_FILENO,&ch,1)!=1) GOTOERROR;
}

if (1!=read(STDIN_FILENO,&ch,1)) GOTOERROR;
if ((ch=='y')||(ch=='Y')) {
	*isyes_out=1;
	return 0;
}

while (1==select(STDIN_FILENO+1,&fds,NULL,NULL,&tv)) {
	if (read(STDIN_FILENO,&ch,1)!=1) GOTOERROR;
}

*isyes_out=0;
return 0;
error:
	return -1;
}
#endif
