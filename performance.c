/*
 * performance.c - basic process stats
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
#include <stdint.h>
#include <ctype.h>
#include <sys/types.h>
#include "common/conventions.h"
#include "performance.h"

int printstats_performance(FILE *fout) {
FILE *fin=NULL;
char oneline[80];

fputs("memorystats: { ",fout);

sprintf(oneline,"/proc/%u/status",getpid());
fin=fopen(oneline,"rb");
if (!fin) GOTOERROR;
while (1) {
	char *temp;
	if (!fgets(oneline,80,fin)) break;
	if (memcmp(oneline,"Vm",2)) continue;
	temp=oneline+2;
	if ((!memcmp(temp,"Peak",4))
		|| (!memcmp(temp,"HWM",3))
		|| (!memcmp(temp,"Data",4))
		|| (!memcmp(temp,"Stk",3))
		|| (!memcmp(temp,"Exe",3))
		|| (!memcmp(temp,"Lib",3))
		|| (!memcmp(temp,"PTE",3))
		|| (!memcmp(temp,"PMD",3)) ) {
			temp=oneline;
			while (1) { fputc(*temp,fout); if (*temp==':') break; temp++; }
			temp++;
			while (isspace(*temp)) temp++;
			fputs(" \"",fout);
			while (*temp!='\n') { fputc(*temp,fout); temp++; }
			fputs("\", ",fout);
	}
}

fprintf(fout,"pid: %u },\n",getpid());

fclose(fin);
return 0;
error:
	iffclose(fin);
	return -1;
}
