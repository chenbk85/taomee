#include <stdio.h>
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
#include <map>
#include <string>
#include "proto.h"
#include <gd.h>
extern "C"{
#include "libtaomee/crypt/qdes.h"
#include "libtaomee/dataformatter/bin_str.h"
#include "libtaomee/conf_parser/config.h"
#include "log.h"
#include "cgi_proto.h"
}
#include "tcpip.h"
#include "cgic.h"

int parse_uploadfile_type(uint32_t* pic_type ,uint32_t* branchtype,uint8_t filedate[9])
{

	uint8_t png_type[8] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
	uint8_t gif_type_1[6] = {0x47,0x49,0x46,0x38,0x39,0x61};
	uint8_t gif_type_2[6] = {0x47,0x49,0x46,0x38,0x37,0x61};
	switch (filedate[0]){
		case 0xff:
			if (filedate[1]==0xd8){
				*pic_type = 1;
				*branchtype = jpg;	//jpg
				return SUCC;
			}
			break;
		case 0x89:
			if(0 == memcmp(filedate,png_type,8)){
				*pic_type = 1;
				*branchtype = png;	//png  
				return SUCC;
			}
		break;
		case 0x47:
			if(0 == memcmp(filedate,gif_type_1,6) || 0 == memcmp(filedate,gif_type_2,6)){
				*pic_type = 1;
				*branchtype = gif;	//gif
				return SUCC;
			}
		break;
		default:
			*pic_type = 2;
			*branchtype = 0;
		break;
		}
	*pic_type = 2;
	*branchtype = 0;
	return SUCC;
}

gdImagePtr mv_logo_create_image(int pic_type, int size, void* data)
{
	switch (pic_type) {
	case JPG:
		return gdImageCreateFromJpegPtr(size, data);
	case PNG:
		return gdImageCreateFromPngPtr(size, data);
	case GIF:
		return gdImageCreateFromGifPtr(size, data);
	default:
		CGI_ERROR_LOG("bad pic type\t[%u]", pic_type);
		return NULL;
	}
}


RECV_FILESERVER* received_from_fileserver(char* pkg,int pkglen)
{
	CGI_DEBUG_LOG("RECEIVE FROM FILE SERVER,len :%d",pkglen);
	int len = *(uint32_t*)pkg;
	if(len != pkglen  || pkglen < 18){
		CGI_ERROR_LOG("cgi receive from fileserver error len[%u,%u]",len,pkglen);
		return NULL;							
	}
	protocol_t* recv = (protocol_t*)pkg;
	if(recv->ret){
		CGI_ERROR_LOG("upload error.ret:[%u]",recv->ret);
		return NULL;			    
	}

	struct recv_fileserver* pkg_recv = (struct recv_fileserver*)(pkg + PROTO_H_SIZE);
	CGI_DEBUG_LOG("KEY:%u", pkg_recv->key);	
	CGI_DEBUG_LOG("LLOCCODE:%s,strlen:%d", pkg_recv->lloccode,strlen(pkg_recv->lloccode));
	CGI_DEBUG_LOG("THUMB CNT:%u", pkg_recv->cnt);
	uint32_t i = 0;
	for(i =0 ;i < pkg_recv->cnt ;i++){
		CGI_DEBUG_LOG("THUMB%u:%x", i, pkg_recv->Thumbid[i]);
	}	
	return pkg_recv;
}
RECV_FILESERVER* connect_with_fileserver(const char* buffer,struct logo_thumb_arg logo_thumb_arg[],Ctcp* conn_fileserver,struct logo_recv_webclient *web_pkg, int filelen,uint32_t pic_type,uint32_t branchtype)
{

	CGI_DEBUG_LOG("SEND TO FILE SERVER");    
	char tmp[3*1024*1024]; 
	uint32_t i =0;
	if(web_pkg == NULL){
		CGI_ERROR_LOG("web_pkg == NULL");
		return NULL;
	}	
	memset(tmp, 0, sizeof tmp);  	
	int j = PROTO_H_SIZE;
	CGI_DEBUG_LOG("-----key:%u",web_pkg->key);
	PKG_H_UINT32(tmp, web_pkg->key, j);				//key
	PKG_H_UINT32(tmp, pic_type, j);            		//type	
	PKG_H_UINT32(tmp, branchtype, j);				//branchtype
	PKG_H_UINT32(tmp, web_pkg->upload_time,j);	//upload_time
	PKG_H_UINT32(tmp, web_pkg->cnt, j);             //cnt
	CGI_DEBUG_LOG("[%u %u %u %u %u]",web_pkg->key,pic_type,branchtype,web_pkg->upload_time,web_pkg->cnt);
	for(i = 0; i < web_pkg->cnt;i++){
		PKG_H_UINT32(tmp, logo_thumb_arg[i].thumb_len, j);
		PKG_STR(tmp,logo_thumb_arg[i].thumb_data,j,logo_thumb_arg[i].thumb_len);
		CGI_DEBUG_LOG("thumb_len[%u] :%u",i,logo_thumb_arg[i].thumb_len);
	}
	PKG_H_UINT32(tmp, filelen, j);        			//len
	PKG_STR(tmp,buffer,j,filelen);					//Content
	CGI_DEBUG_LOG("len:%u",filelen);
	init_proto_head(tmp, web_pkg->userid, 2001, j);
	char *recv= NULL;
	int len = 0;
	CGI_DEBUG_LOG("~~~~~~~~~~~~~begin:branchtype[%u]",branchtype);
	int ret = conn_fileserver->do_net_io((const char *)tmp,j,&recv,&len);
	CGI_DEBUG_LOG("~~~~~~~~~~~~~end,ret:%d",ret);
    if(ret != SUCC){
		CGI_ERROR_LOG("cgi connect fileserver error :errorid:%d,ip[%s],port[%d]",ret,conn_fileserver->get_ip(),conn_fileserver->get_port());
		return NULL;		            
	}
	return (received_from_fileserver(recv,len));
}


int cgiMain(void){
	char fileNameOnServer[64];
    char buffer[pic_max_len];
  	char **thumb_buf = (char**)malloc(CNT_MAX*sizeof(char*));
	thumb_buf[0] = (char*)malloc(CNT_MAX*thumb_max_len);
	thumb_buf[1] = thumb_buf[0] + thumb_max_len;

	int got = 0,got_1 = 0,got_2 = 0;
	int filelen = 0,thumb_len[CNT_MAX];
	Ctcp *conn_fileserver = NULL; 	
	struct logo_recv_webclient data;
	uint32_t pic_type = 0;
	uint32_t branchtype = 0;
	RECV_FILESERVER *recv_data = NULL;
	char bus_path[128];
	log_init("/cgi/log/",(log_lvl_t)8,1000000,0,"");	

	CGI_DEBUG_LOG("------------------CGI_UPLOAD_LOGO-----------------------");
	cgiFormFileSize((char *)"file", &filelen);
	cgiFormFileSize((char *)"thumb1", &thumb_len[0]);
	cgiFormFileSize((char *)"thumb2", &thumb_len[1]);

	if(thumb_len[0] > 32768 || thumb_len[1] >32768){
		CGI_ERROR_LOG("filelen[%u] thumb1_len[%u] thumb2_len[%u]",filelen,thumb_len[0],thumb_len[1]);
		if(thumb_len[0] > 65536 || thumb_len[1] >65536){
			CGI_ERROR_LOG("filelen[%u] thumb1_len[%u] thumb2_len[%u]",filelen,thumb_len[0],thumb_len[1]);

		}
		if(thumb_len[0] > thumb_max_len || thumb_len[1] >thumb_max_len){
			CGI_ERROR_LOG("thumb_len too big thumb1_len[%u] thumb2_len[%u]",thumb_len[0],thumb_len[1]);
			free(thumb_buf);
			return FAIL;
		}

	}
	if(filelen > pic_max_len){
		free(thumb_buf);
		return FAIL;
	}
	
	if (cgiFormFileName((char *)"file",fileNameOnServer, sizeof(fileNameOnServer)) !=cgiFormSuccess) {   
		CGI_ERROR_LOG("could not retrieve filename, file: %s\n", fileNameOnServer);
		free(thumb_buf);
		return FAIL;
    	}
	uint32_t photo_id = 0;
	memset(&data,0,sizeof(struct logo_recv_webclient));
	cgiFormInteger((char*)"userid",(int*)&data.userid,0);
	cgiFormInteger((char*)"key",(int*)&data.key,0);
	cgiFormInteger((char*)"upload_time",(int*)&data.upload_time,0);
	cgiFormString((char*)"bus_path",bus_path,128);
	cgiFormInteger((char*)"photo_id",(int*)&photo_id,0);
	CGI_DEBUG_LOG("[%u %u %u %s %u]",data.userid,
									data.key,
									data.upload_time,
									bus_path,
									photo_id);
	 //目前文件存在于系统临时文件夹中，通常为/tmp，通过该命令打开临时文件。临时文件的名字与用户文件的名字不同，所以不能通过路径/tmp/userfilename的方式获得文件
	cgiFilePtr file, thumbfile_1,thumbfile_2;
	file = thumbfile_1 = thumbfile_2 = 0;
	if (cgiFormFileOpen((char *)"file", &file) != cgiFormSuccess) {
		CGI_ERROR_LOG("could not open the file");
       	free(thumb_buf);
		return FAIL;
      }
	if(cgiFormFileOpen((char *)"thumb1", &thumbfile_1) == cgiFormSuccess){
		CGI_DEBUG_LOG("open thumb_1 Success");
			data.cnt ++;
	}
	if(cgiFormFileOpen((char *)"thumb2", &thumbfile_2) == cgiFormSuccess){
		CGI_DEBUG_LOG("open thumb_2 Success");
		data.cnt ++;
	}


	CGI_DEBUG_LOG("cnt userid key upload_time[%u %u %u %u]",data.cnt,data.userid,data.key,data.upload_time);
	if(-1 == config_init("/opt/taomee/cgi_conf/bench.conf"))	
	{
		CGI_ERROR_LOG("read conf_file error");
		return FAIL;
	}
	conn_fileserver = new Ctcp(config_get_strval("bind_fileserver_ip"),config_get_intval("bind_fileserver_port", 0),0,10);	
	if(cgiFormFileRead(file, buffer, pic_max_len, &got) ==cgiFormSuccess &&
	   cgiFormFileRead(thumbfile_1, thumb_buf[0], thumb_max_len, &got_1) ==cgiFormSuccess &&
	   cgiFormFileRead(thumbfile_2, thumb_buf[1], thumb_max_len, &got_2) ==cgiFormSuccess){
		if(got > 0){
			uint8_t filedate[8];
			uint32_t i =0;
			struct logo_thumb_arg logo_thumb_arg[CNT_MAX];
			memcpy(filedate,(uint8_t*)buffer,8);
			parse_uploadfile_type(&pic_type ,&branchtype,filedate);
			CGI_DEBUG_LOG("pic_type branchtype [%u %u]",pic_type,branchtype);
			if(pic_type == 2){
				CGI_ERROR_LOG("image type error.");
				delete conn_fileserver;
			    cgiFormFileClose(file);
				cgiFormFileClose(thumbfile_1);
				cgiFormFileClose(thumbfile_2);
				free(thumb_buf);
				config_exit();
				return FAIL;																						
			}
			if(NULL == mv_logo_create_image(branchtype, got, buffer)){
				CGI_ERROR_LOG("invalid file");
				delete conn_fileserver;
			    	cgiFormFileClose(file);
				cgiFormFileClose(thumbfile_1);
				cgiFormFileClose(thumbfile_2);
				free(thumb_buf);
				config_exit();
				return FAIL;
			}

			for(i = 0; i < data.cnt && i<CNT_MAX;i++){
				logo_thumb_arg[i].thumb_len = thumb_len[i];	
				memcpy(logo_thumb_arg[i].thumb_data,thumb_buf[i],thumb_len[i]);
				CGI_DEBUG_LOG("logo_thumb_arg[%u].thumb_len [%u]",i,logo_thumb_arg[i].thumb_len);
			}
			recv_data = connect_with_fileserver(buffer,logo_thumb_arg,conn_fileserver,&data,filelen,pic_type,branchtype);
			if(recv_data == NULL){
				CGI_ERROR_LOG("cgi connect fileserver error");
				delete conn_fileserver;
			    cgiFormFileClose(file);
				cgiFormFileClose(thumbfile_1);
				cgiFormFileClose(thumbfile_2);
				free(thumb_buf);
				config_exit();
				return FAIL;
			}
			CGI_DEBUG_LOG("[RESULT:]	%u  %u %u  %u  %s  %u  %u %s \n",
							photo_id,
							data.userid,
							data.key,
							branchtype,
							recv_data->lloccode,
							recv_data->Thumbid[0],
							recv_data->Thumbid[1],
							bus_path);
			delete conn_fileserver;
			cgiFormFileClose(file);
			cgiFormFileClose(thumbfile_1);
			cgiFormFileClose(thumbfile_2);
			free(thumb_buf);
			config_exit();
			return SUCC;
		}

	}
	CGI_ERROR_LOG("cgi read file error:got got1 got2 [%u %u %u]",got,got_1,got_2);
	delete conn_fileserver;
	cgiFormFileClose(file);
	cgiFormFileClose(thumbfile_1);
	cgiFormFileClose(thumbfile_2);
	free(thumb_buf);
	config_exit();
	return FAIL;	
}
    
