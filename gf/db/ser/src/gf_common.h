#ifndef  GF_GF_COMMON_H
#define  GF_GF_COMMON_H


//在有返回列表 时采用， 
//ret:返回值  
//stru_header:结构体头部
//pstru_item:列表起始指针
//itemcount: 个数
//注意事项：pstru_item所指向的空间应是molloc得到的,:free(pstru_item);
#define STD_RETURN_WITH_STRUCT_BUF(ret,stru,buf,buf_len) \
		if (ret==SUCC){ \
			SET_SNDBUF(sizeof(stru)+buf_len); \
			memcpy(SNDBUF_PRIVATE_POS, (char*)(&stru), sizeof(stru)); \
			memcpy(SNDBUF_PRIVATE_POS+sizeof(stru), (char*)(buf), buf_len); \
		}\
		return ret; 


#define STD_RETURN_REPORT_VERSION(str_version) \
	SET_SNDBUF(strlen(str_version) + 240 - 4); \
	memset(SNDBUF_PRIVATE_POS - 4, 0x0, strlen(str_version) + 240); \
	memcpy(SNDBUF_PRIVATE_POS - 4, (char*)(str_version), strlen(str_version)); \
	return SUCC;

		
#endif

