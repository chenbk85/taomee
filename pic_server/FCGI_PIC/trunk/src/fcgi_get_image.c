#ifndef _FASTCGI
#include <stdio.h>
#else
#include "fcgi_stdio.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "proto.h"
extern "C"{
#include "log.h"
#include "fcgi_proto.h"
#include "libtaomee/crypt/qdes.h"
#include "libtaomee/dataformatter/bin_str.h"
#include "libtaomee/conf_parser/config.h"
}
#include "tcpip.h"
uint16_t GET_IMAG_lloccode_checksum(uint8_t* buf, int len)
{
	uint16_t	sum = 0;
	for ( ; len > 0; len--) {
		sum += *buf++;
	}
	sum = (sum >> 8) + (sum && 0xff);
	sum += (sum >> 8);
	return ~sum;
}

int
Fcgi_get_pic_suffix(int branch_type, char asuffix[16])
{
	memset(asuffix, 0, sizeof(asuffix));
	switch(branch_type){
	case '1':		
		memcpy(asuffix, ".JPEG", 5);
		return 0;
	case '2':
		memcpy(asuffix, ".PNG", 4);
		return 0;
	case '3':
		memcpy(asuffix, ".GIF", 4);
		return 0;
	default:
		memcpy(asuffix, ".JPEG", 5);
		return 0;
	}
}
int
Fcgi_analyse_lloccode(char alloccode[Lloccode_Len],char path[PATH_MAX_LEN], int* path_len,uint8_t* type ,uint8_t* branchtype,char filename[16])
{
	int hostid;
	uint32_t uid = 0;
	uint32_t thumbids[CNT_MAX];
	char date[9] = {0}, dir[4] = {0}, pic[4] = {0};
	sscanf(alloccode, "%02X%c%c%8s%3s%3s%010u%08X%08X%08X%08X",
			&hostid, type, branchtype, date, dir, pic, &uid, 
			&thumbids[0], &thumbids[1], &thumbids[2], &thumbids[3]);
	if (*type != '1') { // only for picture now
		CGI_ERROR_LOG("invalid lloccode type\t[%u %s]", type, alloccode);
		return -1;
	}
	char suffix[16] = {0};
	Fcgi_get_pic_suffix(*branchtype, suffix);
	*path_len = snprintf(path, PATH_MAX_LEN, "%s%c/%s/%s/%s%s", FILE_ROOT_PATH,*type, 
		date, dir, pic, suffix);
	snprintf(filename,16,"%s%s",pic,suffix);
//	CGI_DEBUG_LOG("filename:[%s] path:[%s]",filename,path);
	return 0;
}

int FCGI_GET_IMAG_Main(const char*lloccode,const uint32_t ip,const uint32_t web_time){
#ifndef _FASTCGI	
	FILE *fp = NULL;
#else
	FCGI_FILE *fp = NULL;
#endif	
	int f_num = 0;
	char pic_buf[pic_max_len];	
	CGI_DEBUG_LOG("CGI_VIEW_PIC:ip[%u] time[%u] len [%u] lloccode[%s]",ip,web_time,strlen(lloccode),lloccode); 
	time_t now_time;
	struct tm *ptr;

	now_time = time(NULL);
	ptr=gmtime(&now_time);
	char tmpbuf[128];
	strftime(tmpbuf, 128, "%a, %d %b %Y %T GMT",ptr);
	
	if(strlen(lloccode) != 64){
		CGI_ERROR_LOG("lloccode error,len:%d",strlen(lloccode));
		HeaderContentType("image/jpeg");
		HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}
	uint16_t sum =0;
	uint16_t checksum = 0;
	sum = GET_IMAG_lloccode_checksum((uint8_t*)lloccode, 60);
	sscanf(lloccode+60,"%2x",(unsigned int*)&checksum);
	if(checksum != (sum & 0xff)){
		CGI_ERROR_LOG("lloccode checksum error,checksum[%x] sum&0xff[%x]",checksum,(sum & 0xff));
		HeaderContentType("image/jpeg");
	    HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}

	uint8_t type;
	uint8_t branchtype;
	char filedir[PATH_MAX_LEN] = {0};
	char filename[16] = {0};
	int filedir_len = 0;
	Fcgi_analyse_lloccode((char*)lloccode,filedir,&filedir_len,&type,&branchtype,filename);

	if(type != '1'){
		CGI_ERROR_LOG("pic_type error :%c",type);
		HeaderContentType("image/jpeg");
		HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}
	
	switch(branchtype){
		case '1':
			HeaderContentType("image/jpeg");
			HeaderLastModified(tmpbuf);
			HeaderContentDisposition(filename);
		break;
		case '2':
			HeaderContentType("image/png");
			HeaderLastModified(tmpbuf);
			HeaderContentDisposition(filename);
		break;
		case '3':
			HeaderContentType("image/gif");
			HeaderLastModified(tmpbuf);
			HeaderContentDisposition(filename);
		break;
		default:
			CGI_ERROR_LOG("pic_branchtype error :%c",branchtype);
		    HeaderContentType("image/jpeg");
			HeaderStatus(404,"Not Found");
			HeaderEnd;
			return FAIL;
	}
 
#ifndef _FASTCGI	
	if(!(fp = fopen(filedir,"r"))){
		CGI_ERROR_LOG("can not read file %s",filedir);
		HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}
	HeaderEnd;
	f_num = fread(pic_buf,sizeof (char),pic_max_len,fp);
	CGI_DEBUG_LOG("Number of items read = %d",f_num);
	while(k < f_num){
		fprintf(stdout,"%c",pic_buf[k]);
		k++;
	}
	fclose(fp);
	fwrite(pic_buf, 1, f_num, stdout);
#else
	if(!(fp = FCGI_fopen(filedir,"r"))){
		CGI_ERROR_LOG("can not read file %s",filedir);
		HeaderStatus(404,"Not Found");
		HeaderEnd;
		return FAIL;
	}
	HeaderEnd;
	f_num = FCGI_fread(pic_buf,sizeof (char),pic_max_len,fp);
	FCGI_fclose(fp);
	CGI_DEBUG_LOG("------------Number of items read = %d",f_num);
	FCGI_fwrite(pic_buf, 1, f_num, FCGI_stdout);
#endif	
	return SUCC;
}
