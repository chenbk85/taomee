#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "benchapi.h"
#include "tsc.h"
#include "util.h"

static int ht, ncpus;
int tscmsec;
long long tscsec; 

void tsc_init () 
{
        char buf[512], *p, *q, *info[3];
        int fd;

	__asm__("cpuid":"=a"(ht), "=b"(fd), "=c"(ht), "=d"(ht) : "0"(1));
	ht = (fd & 0xff0000) >> 16;
	if(ht < 1) ht = 1;

	tscmsec = 0;
	fd = open("/proc/stat", O_RDONLY);
	if(fd >= 0) {
		read(fd, buf, 512);
		close(fd);
		buf[511] = '\0';
		for(p=buf; (p=strchr(p, '\n')); ) {
			p++;
			if(strncmp(p, "cpu", 3)==0 && isdigit(p[3]))
				ncpus = strtol(p+3, NULL, 10) + 1;
		}
		if(ncpus <= 0) ncpus = 1;
	}
	if((ncpus % ht) != 0) ht = 1;

        fd = open("/proc/cpuinfo", O_RDONLY);
        if(fd==-1) goto fail;
        read(fd, buf, 512);
        close(fd);
        buf[511] = '\0';
        p = strstr(buf, "cpu MHz");
        if(p==NULL) goto fail;
        q = strchr(p, '\n');
        if(q==NULL) goto fail;
        *q = '\0';
        switch (str_explode(" :.\t\r\n", p+7, info, 3)) {
        case 1: info[1] = "000";
        case 2:
        case 3: tscmsec = atoi(info[0]) * 1000;
		if(isdigit(info[1][0])) {
			tscmsec += (info[1][0]-'0')*100;
			if(isdigit(info[1][1])) {
				tscmsec += (info[1][1]-'0')*10;
				if(isdigit(info[1][2]))
					tscmsec += info[1][2]-'0';
			}
		}
                break;
        }
fail:
        if(tscmsec == 0) {
		long long tsc0, tsc1, tsc2;
		struct timespec tv;

		rdtscll(tsc0);
		tv.tv_sec = 1;
		tv.tv_nsec = 1000000;
		while(nanosleep(&tv,&tv)!=0)/*NULL LOOP*/;
		rdtscll(tsc1);
		tv.tv_sec = 0;
		tv.tv_nsec = 1000000;
		while(nanosleep(&tv,&tv)!=0)/*NULL LOOP*/;
		rdtscll(tsc2);
		tscmsec = (2*tsc1-tsc0-tsc2)/1000;
	}
	
	tscsec = tscmsec * 1000LL;
}
