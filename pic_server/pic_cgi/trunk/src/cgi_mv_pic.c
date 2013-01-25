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
gdImagePtr mv_pic_create_image(int pic_type, int size, void* data)
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
	if(len != pkglen || pkglen  < 18){
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
	for(i =0 ;i < pkg_recv->cnt && i < CNT_MAX ;i++){
		CGI_DEBUG_LOG("THUMB%u:%x", i, pkg_recv->Thumbid[i]);
	}	
	return pkg_recv;
}
RECV_FILESERVER* connect_with_fileserver(const char* buffer,Ctcp* conn_fileserver,struct recv_webclient *web_pkg, int filelen,uint32_t pic_type,uint32_t branchtype)
{
	CGI_DEBUG_LOG("SEND TO FILE SERVER");    	
	char tmp[3*1024*1024]; 
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
	PKG_H_UINT32(tmp, web_pkg->width_limit,j);		//width_limit
	PKG_H_UINT32(tmp, web_pkg->height_limit,j);		//height_limit
	PKG_H_UINT32(tmp, web_pkg->cnt, j);             //cnt
	CGI_DEBUG_LOG("[%u %u %u %u %u %u %u]",web_pkg->key,pic_type,branchtype,web_pkg->upload_time,web_pkg->width_limit,web_pkg->height_limit,web_pkg->cnt);
	for(uint32_t k=0;k < web_pkg->cnt && k < CNT_MAX;k++){
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].W, j);
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].H, j);
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].start_x, j);
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].start_y, j);
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].thumb_x, j);
	PKG_H_UINT32(tmp, web_pkg->thumb_arg[k].thumb_y, j);
	CGI_DEBUG_LOG("W(%u):%u",k,web_pkg->thumb_arg[k].W);
	CGI_DEBUG_LOG("H(%u):%u",k,web_pkg->thumb_arg[k].H);
	CGI_DEBUG_LOG("start_x(%u):%u",k,web_pkg->thumb_arg[k].start_x);
	CGI_DEBUG_LOG("start_y(%u):%u",k,web_pkg->thumb_arg[k].start_y);
	CGI_DEBUG_LOG("thumb_x(%u):%u",k,web_pkg->thumb_arg[k].thumb_x);
	CGI_DEBUG_LOG("thumb_y(%u):%u",k,web_pkg->thumb_arg[k].thumb_y);
	}
	PKG_H_UINT32(tmp, filelen, j);        			//len
	PKG_STR(tmp,buffer,j,filelen);					//Content
	CGI_DEBUG_LOG("len:%u",filelen);
	init_proto_head(tmp, web_pkg->userid, 2000, j);
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

int parse_web_data(const char* s_data,struct recv_webclient *d_data)
{
	CGI_DEBUG_LOG("PARSE WEB DATA");
	char upload_s_tmp[256];
	char d_tmp[256];
	uint32_t j =0,i =0;
	int s_len = strlen(s_data);
    str2hex(s_data,s_len,upload_s_tmp);
	upload_s_tmp[s_len/2] = '\0';
	des_decrypt_n(KEY,upload_s_tmp,d_tmp,s_len/16);		
	UNPKG_H_UINT32(d_tmp,d_data->userid,j);
	UNPKG_H_UINT32(d_tmp,d_data->key,j);
	UNPKG_H_UINT32(d_tmp,d_data->width_limit,j);
	UNPKG_H_UINT32(d_tmp,d_data->height_limit,j);
	UNPKG_H_UINT32(d_tmp,d_data->cnt,j);
	CGI_DEBUG_LOG("RECV WEB[%u %u %u %u %u]",d_data->userid,d_data->key,d_data->width_limit,d_data->height_limit,d_data->cnt);
	for(i = 0;i < d_data->cnt && i < CNT_MAX;i++){
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].W,j);
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].H,j);
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].start_x,j);
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].start_y,j);
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].thumb_x,j);
		UNPKG_H_UINT32(d_tmp,d_data->thumb_arg[i].thumb_y,j);
		CGI_DEBUG_LOG("[%u] [%u %u %u %u %u %u]",i,d_data->thumb_arg[i].W,d_data->thumb_arg[i].H,d_data->thumb_arg[i].start_x,d_data->thumb_arg[i].start_y,d_data->thumb_arg[i].thumb_x,d_data->thumb_arg[i].thumb_y);
	}
	return SUCC;
}
int cgiMain(void){
    cgiFilePtr file;
    char fileNameOnServer[64];
    char buffer[pic_max_len];
    int got = 0;
	int filelen = 0;
	Ctcp *conn_fileserver = NULL; 	
	struct recv_webclient data;
	uint32_t pic_type = 0;
	uint32_t branchtype = 0;
	RECV_FILESERVER *recv_data = NULL;
	log_init("/cgi/log/",(log_lvl_t)8,1000000,0,"");
	CGI_DEBUG_LOG("------------------CGI_UPLOAD_PIC-----------------------");
	cgiFormFileSize((char *)"file", &filelen);
	CGI_DEBUG_LOG("filelen %u",filelen);
	if(filelen > pic_max_len){
		CGI_ERROR_LOG("pic size error");
		return FAIL;
	}
	if (cgiFormFileName((char *)"file",fileNameOnServer, sizeof(fileNameOnServer)) !=cgiFormSuccess) {   
		CGI_ERROR_LOG("could not retrieve filename, file: %s\n", fileNameOnServer);
		return FAIL;
    }

	data.width_limit = 800;
	data.height_limit = 600;
	data.cnt = 2;
	data.thumb_arg[0].W = 0;
	data.thumb_arg[0].H = 0;
	data.thumb_arg[0].start_x = 0;
	data.thumb_arg[0].start_y = 0;
	data.thumb_arg[0].thumb_x = 140;
	data.thumb_arg[0].thumb_y = 140;
	
	data.thumb_arg[1].W = 0;
	data.thumb_arg[1].H = 0;
	data.thumb_arg[1].start_x = 0;
	data.thumb_arg[1].start_y = 0;
	data.thumb_arg[1].thumb_x = 70;
	data.thumb_arg[1].thumb_y = 70;
	char bus_path[128];
	uint32_t photo_id = 0;
	cgiFormInteger((char*)"userid",(int*)&data.userid,0);
	cgiFormInteger((char*)"key",(int*)&data.key,0);
	cgiFormInteger((char*)"upload_time",(int*)&data.upload_time,0);
	cgiFormString((char*)"bus_path",bus_path,128);
	cgiFormInteger((char*)"photo_id",(int*)&photo_id,0);


	CGI_DEBUG_LOG("userid key upload_time[%u %u %u]",data.userid,data.key,data.upload_time);

	 //目前文件存在于系统临时文件夹中，通常为/tmp，通过该命令打开临时文件。临时文件的名字与用户文件的名字不同，所以不能通过路径/tmp/userfilename的方式获得文件
    if (cgiFormFileOpen((char *)"file", &file) != cgiFormSuccess) {
		CGI_ERROR_LOG("could not open the file");
        return FAIL;
    }
	if(-1 == config_init("/opt/taomee/cgi_conf/bench.conf")){
		CGI_ERROR_LOG("read conf_file error");
		return FAIL;
	}
	conn_fileserver = new Ctcp(config_get_strval("bind_fileserver_ip"),config_get_intval("bind_fileserver_port", 0),0,10);
	if(cgiFormFileRead(file, buffer, pic_max_len, &got) ==cgiFormSuccess){
		if(got > 0){
			uint8_t filedate[8];
			memcpy(filedate,(uint8_t*)buffer,8);
			parse_uploadfile_type(&pic_type ,&branchtype,filedate);
			CGI_DEBUG_LOG("pic_type branchtype [%u %u]",pic_type,branchtype);
			if(pic_type == 2 ){
				CGI_ERROR_LOG("image type error.");
				delete conn_fileserver;
				cgiFormFileClose(file);
				config_exit();
				return FAIL;
			}
			if(NULL == mv_pic_create_image(branchtype, got, buffer)){
				CGI_ERROR_LOG("invalid file");
				delete conn_fileserver;
				cgiFormFileClose(file);				
				config_exit();
				return FAIL;
			}

			recv_data = connect_with_fileserver(buffer,conn_fileserver,&data,filelen,pic_type,branchtype);
      		if(recv_data == NULL){
				CGI_ERROR_LOG("cgi connect fileserver error");
				delete conn_fileserver;
				cgiFormFileClose(file);
				config_exit();
				return FAIL;
			}
			delete conn_fileserver;
			cgiFormFileClose(file);
			CGI_DEBUG_LOG("[RESULT:]%u  %u %u  %u  %s  %u  %u %s \n",
						photo_id,
						data.userid,
						data.key,
						branchtype,
						recv_data->lloccode,
						recv_data->Thumbid[0],
						recv_data->Thumbid[1],
						bus_path);
			config_exit();
			return SUCC;
		}

	}
	CGI_ERROR_LOG("cgi read file error:got=%d",got);
	delete conn_fileserver;
	cgiFormFileClose(file);
	config_exit();
	return FAIL;	
}
    
