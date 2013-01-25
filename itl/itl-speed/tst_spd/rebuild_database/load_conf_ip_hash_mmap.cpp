#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <glib.h>
#include <pthread.h>
#include <error.h>

extern "C" {
#include "net_if.h"
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
#include "common.h"
}

#include "load_conf_ip_hash_mmap.hpp"
#include "ip_dict.hpp"
#include "polling_cdn_url_files.hpp"

char 			dbserver_ip[STR_IP_LEN] = "10.1.1.141";	//数据库模块所在的IP地址－也即MMAP中的数据发送的目的地
uint16_t		dbserver_port = 13380;			//数据库模块所在的IP绑定的端口
char			bind_ip[STR_IP_LEN] = "10.1.8.244";		//本机就是分析机,此IP就是本程序所在机器的IP——接受FCGI的数据
uint16_t		bind_port = 8888;					//本机就是分析机，此PORT就是本程序绑定的PORT

uint16_t		max_project_number	= MAX_PROJECT_NUMBER;	//默认最大项目数,cdn and url的mmap数目根据项目数来决定 最大为1023
uint16_t		max_page_per_proj	= MAX_PAGE_RECORD;	//单个项目中所有URL的总断点数.
uint16_t		accepted_project_number = 1;	//URL测速中可接受的项目编号
uint16_t		delay_upper_bound = 1000;		//IDC延时过大告警
uint16_t		max_idc_number = MAX_IDC_NUMBER;//最大缓存的IDC机房数
uint16_t		remove_or_rename = REMOVE_IDC_FILE;//默认remove
uint32_t		max_cdn_speed = 0xFFFFFFFF;		//

char			idc_file_dirpath[DIR_PATH_LEN] = "/home/probe_ip_save";
char			idc_rename_dirpath[DIR_PATH_LEN] = "/home/probe_ip_save_renamed";
char			cdn_url_file_dirpath[DIR_PATH_LEN] = "/home/analyze_server/cdn_url";
char			idc_mmap_file_dirpath[DIR_PATH_LEN] = "/home/analyze_server/mmap_file/idc";
char			cdn_mmap_file_dirpath[DIR_PATH_LEN] = "/home/analyze_server/mmap_file/cdn";
char			url_mmap_file_dirpath[DIR_PATH_LEN] = "/home/analyze_server/mmap_file/url";
char			link_ip_config_file[DIR_PATH_LEN] = "/home/analyze_server/link_ip.config";

link_ip_t		*link_ip_set;		//链路的IP地址，每个IDC可能有多条链路

GHashTable*	idc_hash_ip;			//hash表记录每个ip在mmap中的偏移地址
GHashTable*	cdn_hash_ip[128];	//全网探测和cdn测速的ip集是不同的,且因ip是32位，
										//无法在初始时刻完全建立ip的hash表,需要在运行时逐步建立
										//且不同的任务需要同时查询和修改
										//所以idc和cdn采用不同的ip hash表
GHashTable*	link_hash_ip;		//记录link对应的IP地址，用以快速查询某个IP是否属于link
GHashTable*	hash_cityisp;		//城市运营商的集合是相同并已知的，且数目有限(3000-4000)
										//可以在初始化时完全建立，运行时只需查询，
										//无须更新，故采用相同的HASH表，多个任务共享

GHashTable	**url_page_index;	//为每个项目建立一个url的page索引
url_page_used_t *url_page_status = NULL;

my_mmap_t		idc_test_ip_mmap;
my_mmap_t		idc_test_area_mmap;
my_mmap_t*	cdn_speed_ip_mmap;
my_mmap_t*	cdn_speed_area_mmap;
my_mmap_t*	url_page_mmap;

mmap_file_head_t mmap_file_head;//标识合法的mmap文件

db_server_t		db_server_set[MAX_DB_SERVER];//dbserver 集

pthread_t		idc_thread;	//发送idc文件的线程
pthread_mutex_t	idc_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	idc_cond = PTHREAD_COND_INITIALIZER;

pthread_t		cdn_and_url_thread;//处理cdn和url备份mmap文件的线程
pthread_mutex_t	cdn_and_url_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	cdn_and_url_cond = PTHREAD_COND_INITIALIZER;

static inline int is_legal_ip(const char *str_ip)
{
	int a[4] = {-1, -1, -1, -1};
	sscanf(str_ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	int i;
	for (i =0; i < 4; i++) {
		if (a[i] < 0 || a[i] > 255)
			return 0;
	}
	return 1;
}

static inline int is_legal_port(const int port)
{
	return ( port > 0 && port < 65536 ) ?1 :0;
}

static int my_mkdir(const char *path)
{
	struct stat buf;
	mode_t mode = 0x00fff;
	char tmp_path[DIR_PATH_LEN];
	const char *slash=path;//必然最开始是'/'
	const char *cur;
	uint16_t len;
	if( !*( slash+1 ) )
		return 0;	//整个path就是'/'

	while ( (slash = strchr(slash+1,(int)'/') ) != NULL ) {
		cur=slash;//保存当前/所在位置
		len=slash-path+1;	//包括了“\0”
		snprintf(tmp_path,len,"%s",path);
		//判断存储目录是否存在
		if ( stat(tmp_path,&buf) == -1 ) {
			if(errno == ENOENT) {
				if (mkdir(tmp_path,mode) != 0)
					ERROR_RETURN(("mkdir %s,failed:%s\t", tmp_path, strerror(errno)), -1);
			}
		}
		//若存在但不是目录
		if( ( buf.st_mode & S_IFDIR ) == 0 ) {
			ERROR_RETURN(("mkdir:%s is an existed regular file.\t", tmp_path), -1);
			remove(tmp_path);	//不是一个目录则删除之
			mkdir(tmp_path,mode);//并创建一个目录mode的设置使得所有人对其有所有权限
		}
	}
	if(*(cur+1)) {//‘/’后面还有字符表示是最后的字符，则创建全名目录 path 否则是最后的/ 不需要创建
		//判断存储目录是否存在
		if( stat(path,&buf) == -1 ) {
			if(errno == ENOENT) {
				if (mkdir(path,mode) != 0)
					ERROR_RETURN(("mkdir %s,failed:%s\t", path, strerror(errno)), -1);
			}
		}
		//若存在但不是目录
		if( ( buf.st_mode & S_IFDIR ) == 0 ) {
			ERROR_RETURN(("mkdir:%s is an existed regular file.\t", path), -1);
			remove(path);	//不是一个目录则删除之
			mkdir(path,mode);//并创建一个目录mode的设置使得所有人对其有所有权限
		}
	}
	return 0;
}

static int load_conf()
{
	char *str;
	int	 port;
	int tmp_max_project_number;
	int tmp_max_page;
	//int tmp_accepted_proj;
	int tmp_upper_bound;
	int tmp_max_idc;

	str = config_get_strval("dbserver_ip");
	if (str != NULL && is_legal_ip( (const char *)str ))
		strncpy(dbserver_ip, str, sizeof(dbserver_ip));
	else
		ERROR_RETURN(("can't find legal dbserver ip\t"), -1);

	strncpy((char*)db_server_set[0].ip, dbserver_ip, sizeof(dbserver_ip));

	str = config_get_strval("bind_ip");
	if (str != NULL && is_legal_ip( (const char *)str ))
		strncpy(bind_ip, str, sizeof(bind_ip));
	else
		ERROR_RETURN(("can't find legal bind ip\t"), -1);

	port = config_get_intval("dbserver_port", dbserver_port);
	if (is_legal_port(port))
		dbserver_port = port;
	else
		ERROR_RETURN(("can't find legal dbserver port\t"), -1);

	db_server_set[0].port = dbserver_port;

	port = config_get_intval("bind_port", bind_port);
	if (is_legal_port(port))
		bind_port = port;
	else
		ERROR_RETURN(("can't find legal bind port\t"), -1);

	tmp_max_project_number = config_get_intval("max_project_number", max_project_number);
	if (tmp_max_project_number > 0 && tmp_max_project_number <= max_project_number)
		max_project_number = tmp_max_project_number;
	else
		ERROR_RETURN(("max_project_number is illegal, max value is:%u\t",max_project_number), -1);

	tmp_max_page = config_get_intval("max_page_per_proj", max_page_per_proj);
	if (tmp_max_page > 0 && tmp_max_page <= max_page_per_proj)
		max_page_per_proj = tmp_max_page;
	else
		ERROR_RETURN(("max_page_per_proj is illegal, max value is:%u\t",max_page_per_proj), -1);

	tmp_upper_bound = config_get_intval("delay_upper_bound", delay_upper_bound);
	if (tmp_upper_bound > 0 && tmp_upper_bound <= delay_upper_bound)
		delay_upper_bound = tmp_upper_bound;
	else
		ERROR_RETURN(("delay_upper_bound is illegal, max value is:%u\t",delay_upper_bound), -1);

	tmp_max_idc = config_get_intval("max_idc_number", max_idc_number);
	if (tmp_max_idc > 0 && tmp_max_idc <= max_idc_number)
		max_idc_number = tmp_max_idc;

	uint32_t tmp_max_cdn_speed;
	tmp_max_cdn_speed = config_get_intval("max_cdn_speed", max_cdn_speed);
	if (tmp_max_cdn_speed > 1024 && tmp_max_cdn_speed <= max_cdn_speed)
		max_cdn_speed = tmp_max_cdn_speed;


	str = config_get_strval("link_ip_config_file");
	if (str != NULL )
		strncpy(link_ip_config_file, str, sizeof(link_ip_config_file));
	else
		ERROR_RETURN(("can't find link_ip_config_file\t"), -1);

	int tmp_remove_or_rename = config_get_intval("remove_or_rename", remove_or_rename);
	if (tmp_remove_or_rename == REMOVE_IDC_FILE || tmp_remove_or_rename == RENAME_IDC_FILE)
		remove_or_rename = tmp_remove_or_rename;

	if (remove_or_rename == RENAME_IDC_FILE) {
		str = config_get_strval("idc_rename_dirpath");
		if (str != NULL)
			strncpy(idc_rename_dirpath, str, sizeof(idc_rename_dirpath));
		else
			ERROR_RETURN(("can't find idc_file_dirpath\t"), -1);
	}

	str = config_get_strval("idc_file_dirpath");
	if (str != NULL)
		strncpy(idc_file_dirpath, str, sizeof(idc_file_dirpath));
	else
		ERROR_RETURN(("can't find idc_file_dirpath\t"), -1);

	str = config_get_strval("cdn_url_file_dirpath");
	if (str != NULL)
		strncpy(cdn_url_file_dirpath, str, sizeof(cdn_url_file_dirpath));
	else
		ERROR_RETURN(("can't find mmap_tmpfile_dirpath\t"), -1);


	str = config_get_strval("idc_mmap_file_dirpath");
	if (str != NULL)
		strncpy(idc_mmap_file_dirpath, str, sizeof(idc_mmap_file_dirpath));
	else
		ERROR_RETURN(("can't find idc_mmap_file_dirpath\t"), -1);

	str = config_get_strval("cdn_mmap_file_dirpath");
	if (str != NULL)
		strncpy(cdn_mmap_file_dirpath, str, sizeof(cdn_mmap_file_dirpath));
	else
		ERROR_RETURN(("can't find cdn_mmap_file_dirpath\t"), -1);

	str = config_get_strval("url_mmap_file_dirpath");
	if (str != NULL)
		strncpy(url_mmap_file_dirpath, str, sizeof(url_mmap_file_dirpath));
	else
		ERROR_RETURN(("can't find url_mmap_file_dirpath\t"), -1);

	return 0;
}

static int build_dir()
{
	if (remove_or_rename == RENAME_IDC_FILE)
		return (	my_mkdir(cdn_url_file_dirpath)
				|	my_mkdir(idc_mmap_file_dirpath)
				|	my_mkdir(cdn_mmap_file_dirpath)
				|	my_mkdir(url_mmap_file_dirpath)
				|	my_mkdir(idc_rename_dirpath));
	else
		return (	my_mkdir(cdn_url_file_dirpath)
				|	my_mkdir(idc_mmap_file_dirpath)
				|	my_mkdir(cdn_mmap_file_dirpath)
				|	my_mkdir(url_mmap_file_dirpath));
}

static int load_ip()
{
	if (!g_ipDict.init())	//加载IP库到g_ipDict的私有成员m_ipmap中
		ERROR_RETURN(("can not open \"ipdict.dat\""), -1);
	return 0;
}

static int load_hash()
{
	idc_hash_ip = g_hash_table_new(g_int_hash, g_int_equal);
	if (idc_hash_ip == NULL)
		return -1;

	int k;
	for (k = 0; k < max_project_number; k++) {
		cdn_hash_ip[k] = g_hash_table_new(g_int_hash, g_int_equal);
		if (cdn_hash_ip[k] == NULL)
			return -1;
	}

	url_page_index = (GHashTable**)malloc(sizeof(GHashTable*) * max_project_number);
	if (url_page_index == NULL)
		return -1;
	for (k = 0; k < max_project_number; k++) {
		url_page_index[k] = g_hash_table_new(g_int_hash, g_int_equal);
		if (url_page_index[k] == NULL)
			return -1;
	}

	link_hash_ip = g_hash_table_new(g_str_hash, g_str_equal);
	if (link_hash_ip == NULL)
		return -1;
	hash_cityisp = g_hash_table_new(g_str_hash, g_str_equal);
	if (hash_cityisp == NULL)
		return -1;
	return 0;

}

static int create_mmap(char *path, uint32_t mmap_size, my_mmap_t *a_mmap)
{
	//若mmap文件是新建立的，则flag置1，如为0，则上层函数根据mmap中的数据初始化hash表
	int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		ERROR_RETURN(("mmap: fail to open file %s", path), -1);
	}
	long fsize = lseek(fd, 0, SEEK_END);
	char zero[1024];
	memset(zero, 0x0,1024);
	if (fsize == 0) {
		//DEBUG_LOG("NEW MMAPFILE");
		ftruncate(fd, mmap_size + MMAP_FILE_HEAD_LEN);
		write(fd, MMAP_FILE_SYMBOL, MMAP_FILE_SYMBOL_LEN);
		write(fd, &zero, MMAP_FILE_RECORD_LEN + MMAP_FILE_READY_LEN);
		write(fd, &a_mmap->type, MMAP_FILE_TYPE_LEN);
		write(fd, &zero, MMAP_FILE_KEY_LEN + MMAP_FILE_VALUE_LEN + MMAP_FILE_LATEST_LEN);
		a_mmap->records_num = 0;
		a_mmap->ready_num = 0;
		a_mmap->key = 0;
		a_mmap->value = 0;
		a_mmap->latest_time = 0;
	}
	else {
		//DEBUG_LOG("VALID MMAPFILE");
		lseek(fd, 0, SEEK_SET );//如果不是新文件，则回到文件头并读取头部，以判断文件的正确性
		read(fd, &mmap_file_head, sizeof(mmap_file_head_t));
		if ( memcmp(mmap_file_head.file_symbol, MMAP_FILE_SYMBOL, MMAP_FILE_SYMBOL_LEN) == 0
				&& mmap_file_head.type == a_mmap->type) {
			a_mmap->records_num = mmap_file_head.records_num;
			a_mmap->ready_num = mmap_file_head.ready_num;
			a_mmap->key = mmap_file_head.key;
			a_mmap->value = mmap_file_head.value;
			a_mmap->latest_time = mmap_file_head.latest_time;
		}
		else {
			//DEBUG_LOG("INVALID MMAPFILE");
			lseek(fd, 0, SEEK_SET );//如果不是正确的mmap文件，则重置这个文件.
			ftruncate(fd, mmap_size + MMAP_FILE_HEAD_LEN);
			write(fd, MMAP_FILE_SYMBOL, MMAP_FILE_SYMBOL_LEN);
			write(fd, &zero, MMAP_FILE_RECORD_LEN + MMAP_FILE_READY_LEN);
			write(fd, &a_mmap->type, sizeof(a_mmap->type));
			write(fd, &zero, MMAP_FILE_KEY_LEN + MMAP_FILE_VALUE_LEN + MMAP_FILE_LATEST_LEN);
			a_mmap->records_num	= 0;
			a_mmap->ready_num = 0;
			a_mmap->key = 0;
			a_mmap->value = 0;
			a_mmap->latest_time = 0;
		}
	}
	char* pmap = (char *)mmap(0, MMAP_FILE_HEAD_LEN + mmap_size,
								PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//msync(pmap, MMAP_FILE_HEAD_LEN + mmap_size, MS_SYNC);
	if(pmap == (char *)-1) {
            close (fd);
            ERROR_RETURN(("mmap: fail to mmap %s", path), -1);
	}
	a_mmap->mmap_head = (mmap_file_head_t*)pmap;
	a_mmap->mmap_p = pmap + MMAP_FILE_HEAD_LEN;
	close(fd);

	return 0;
}

static void set_hash_for_ip_mmap(GHashTable* a_hashtable, my_mmap_t* a_mmap, int type)
{
	uint32_t i;
	idc_test_ip_t *idc_ip_p;
	cdn_speed_ip_t *cdn_ip_p;

	idc_ip_p = (idc_test_ip_t*)a_mmap->mmap_p;
	cdn_ip_p = (cdn_speed_ip_t*)a_mmap->mmap_p;
	code_t code;
	uint32_t isp_key;
	switch (type){
	case MMAP_TYPE_IDC_IP:
		DEBUG_LOG("INITIATE:IDC_IP:%u",a_mmap->records_num);
		for (i = 0; i < a_mmap->records_num; i++) {
			if((idc_ip_p + i)->ip == 0)
				continue;
			//有可能因为ip地址库变化，需要重启，则重启需要重新设定这些ip的isp_key
			if(g_ipDict.find((idc_ip_p + i)->ip, code, &isp_key) == false)
				continue;
			(idc_ip_p + i)->isp_key = isp_key;
			if ( g_hash_table_lookup(a_hashtable, &((idc_ip_p + i)->ip) ) == NULL)
				g_hash_table_insert(a_hashtable, &(idc_ip_p + i)->ip, &((idc_ip_p + i)->offset));
			}
		break;
	case MMAP_TYPE_CDN_IP:
		DEBUG_LOG("INITIATE:CDN_IP:%u",a_mmap->records_num);
		for (i = 0; i < a_mmap->records_num; i++) {
			if((cdn_ip_p + i)->ip == 0)
				continue;
			if(g_ipDict.find((cdn_ip_p + i)->ip, code, &isp_key) == false)
				continue;
			(cdn_ip_p + i)->isp_key = isp_key;
			if ( g_hash_table_lookup(a_hashtable, &((cdn_ip_p + i)->ip) ) == NULL)
				g_hash_table_insert(a_hashtable, &((cdn_ip_p + i)->ip), &((cdn_ip_p + i)->offset));
		}
		break;
	default:
		break;
	}
}

static void set_hash_for_area_mmap()
{
	std::map<uint32_t, code_t>::iterator it;
	uint32_t count = 0;
	idc_test_area_t *idc_area_p;
	cdn_speed_area_t *cdn_area_p;
	url_test_t *url_page_p;

	for (it = g_ipDict.m_ipmap.begin(), count = 0; it != g_ipDict.m_ipmap.end(); it++) {
		if (it->second.ipspan == 0)
			continue;

		if (g_hash_table_lookup(hash_cityisp, it->second.key) == NULL) {
			it->second.offset = count;
			g_hash_table_insert(hash_cityisp, it->second.key, &(it->second.offset));
			idc_area_p = (idc_test_area_t*)(idc_test_area_mmap.mmap_p) + count;
			//写idc_test的area mmap管理结构
			//考虑到刚读入记录就断电的情况，时间都未更新，而记录数已经增加，下次重启时，会重复判断到时间为0
			//导致记录数错误，故这里不考虑时间，都直接写入
			idc_area_p->isp_key = it->first;
			idc_test_area_mmap.records_num++;
			idc_test_area_mmap.mmap_head->records_num++;//真正写入mmap
			//写cdn_speed的area mmap
			int i;
			for (i = 0; i < max_project_number; i++) {
				cdn_area_p = (cdn_speed_area_t*)(cdn_speed_area_mmap[i].mmap_p) + count;
				cdn_speed_area_mmap[i].records_num++;
				cdn_speed_area_mmap[i].mmap_head->records_num++;
				cdn_area_p->isp_key = it->first;
			}
			//写url的mmap
			for (i = 0; i < max_page_per_proj * max_project_number; i++) {
				url_page_p = (url_test_t*)(url_page_mmap[i].mmap_p) + count;
				url_page_mmap[i].records_num++;
				url_page_mmap[i].mmap_head->records_num++;
				url_page_p->isp_key = it->first;
			}
			count++;
		}
		else
			continue;
	}
}

static int load_mmap()
{
	char full_path[DIR_PATH_LEN] = {0};
	int i = 0;

	cdn_speed_ip_mmap = (my_mmap_t*)malloc( max_project_number * sizeof(my_mmap_t) );
	cdn_speed_area_mmap = (my_mmap_t*)malloc( max_project_number * sizeof(my_mmap_t) );
	url_page_mmap = (my_mmap_t*)malloc(max_page_per_proj * max_project_number * sizeof(my_mmap_t));

	snprintf(full_path, sizeof(full_path), "%s/%s", idc_mmap_file_dirpath,"idc_test_ip_mmap");

	idc_test_ip_mmap.type = MMAP_TYPE_IDC_IP;
	if (create_mmap(full_path, idc_mmap_size_ip, &idc_test_ip_mmap) != 0)
		return -1;
	if(idc_test_ip_mmap.mmap_head->records_num != 0)//mmap中有数据，则根据mmap更新hash表
		set_hash_for_ip_mmap(idc_hash_ip, &idc_test_ip_mmap, idc_test_ip_mmap.type);

	idc_test_area_mmap.type = MMAP_TYPE_IDC_AREA;
	snprintf(full_path, sizeof(full_path), "%s/%s", idc_mmap_file_dirpath,"idc_test_area_mmap");
	if (create_mmap(full_path, idc_mmap_size_area, &idc_test_area_mmap) != 0)
		return -1;
	//重启时重设records
	idc_test_area_mmap.records_num = idc_test_area_mmap.mmap_head->records_num = 0;

	for(i = 1; i <= max_project_number; i++) {
		snprintf(full_path, sizeof(full_path), "%s/%s%03d", cdn_mmap_file_dirpath,"cdn_speed_ip_mmap",i);

		cdn_speed_ip_mmap[ i-1 ].type = MMAP_TYPE_CDN_IP;
		if (create_mmap(full_path, cdn_mmap_size_ip, &cdn_speed_ip_mmap[ i-1 ]) != 0)
			return -1;
		if(cdn_speed_ip_mmap[ i-1 ].mmap_head->records_num != 0)
			set_hash_for_ip_mmap(cdn_hash_ip[i-1], &cdn_speed_ip_mmap[ i-1 ], cdn_speed_ip_mmap[ i-1 ].type);

		cdn_speed_area_mmap[ i-1 ].type = MMAP_TYPE_CDN_AREA;
		snprintf(full_path, sizeof(full_path), "%s/%s%03d", cdn_mmap_file_dirpath,"cdn_speed_area_mmap",i);
		if (create_mmap(full_path, cdn_mmap_size_area, &cdn_speed_area_mmap[ i-1 ]) != 0)
			return -1;
		cdn_speed_area_mmap[ i-1 ].records_num = cdn_speed_area_mmap[ i-1 ].mmap_head->records_num = 0;
	}

	//url_page's mmap
	int j, pos;

	url_page_status = (url_page_used_t*)malloc(sizeof(url_page_used_t) * max_project_number);
	if(url_page_status == NULL)
		ERROR_RETURN(("URL PAGE STATUS CREATE FAILED!") ,-1);
	for(i = 0; i < max_project_number; i++) {
		url_page_status[i].used_count = 0;
		for (j = 0; j < max_page_per_proj; j++)
			url_page_status[i].status[j] = 0;
	}
	for(i = 1; i <= max_project_number; i++) {
		for(j = 1; j <= max_page_per_proj; j++) {
			snprintf(full_path, sizeof(full_path), "%s/%s%03d_%03d",
					url_mmap_file_dirpath,"url_page_mmap", i, j);
			pos = (i-1) * max_page_per_proj + (j-1);
			url_page_mmap[pos].type = MMAP_TYPE_URL;
			if (create_mmap(full_path, url_mmap_size_page, &url_page_mmap[pos]) == -1)
				return -1;
			//加入hash表 url_page_index[i]
			if(url_page_mmap[pos].key > 0
				&& url_page_mmap[pos].value > 0
				&& url_page_mmap[pos].value < max_page_per_proj) {
				g_hash_table_insert(url_page_index[i-1], &(url_page_mmap[pos].key),
						&(url_page_mmap[pos].value));
				url_page_status[i-1].used_count ++;
				url_page_status[i-1].status[url_page_mmap[pos].value] = 1;
			}
			url_page_mmap[pos].records_num = url_page_mmap[pos].mmap_head->records_num = 0;
		}
	}
	return 0;
}
int connect_2_dbserver()
{
	int i;
	for(i = 0; i< MAX_DB_SERVER; i++) {
		db_server_set[i].id = 0;
		db_server_set[i].fd = -1;
	}
	db_server_set[0].id = 1;

	db_server_set[0].fd =
			connect_to_svr((char*)db_server_set[0].ip, db_server_set[0].port, 8*1024, 1);

	if(db_server_set[0].fd == -1)
		return -1;
	return 0;
}

int load_linkip(char *filepath)
{
	FILE *fp;			//打开配置文件指针
	int a[4];			//临时存储点分IP的4个段
	int flag;			//标识IP是否合法
	char s[1024];	//存储从配置文件中读取的一行数据
	uint32_t i;		//for循环计数
	char c,c1;		//连续从文件的一行中读取的两个字符，用以标识变量、IP、注释行无用行
	char *eq = NULL;//字符串中表示"="的指针
	int len = 0;		//读取的字符串长度，如果是一个变量，其标识符的长度要大于0
	int idc;			//临时存储idc 编号
	int link;			//临时存储link编号
	int master;		//临时存储master编号
	link_ip_t *tmpip = NULL;//指向待探测ip

	fp = fopen(filepath,"rt");
	if (fp == NULL)
		ERROR_RETURN(("can not open config file:link_ip.congfig"), -1);

	while (!feof(fp)) {
		c = fgetc(fp);
		if ((int)c == -1) {	//EOF==-1
			break;
		}
		if (c != '$') {
			while (c != '\n' && !feof(fp)) {
				c = fgetc(fp);
			}
			continue;
		}
		if ((c1 = fgetc(fp)) != ':') {
			while (c != '\n' && !feof(fp)) {
				c = fgetc(fp);
			}
			continue;
		}

		if ('$' == c && ':' ==c1) {
			//memset(s,0,1024);
			len = 0;
			fgets(s,1024,fp);
			if ((eq = strchr(s,'=')) == NULL)
				continue;
			len = eq - s;
			if ( len <= 0 )
				continue;

			if (bcmp(s,"link_ip",len) == 0) {
				flag = 0;
				idc = link = master = a[0] = a[1] = a[2] = a[3] = -1;
				sscanf(eq + 1,"%d,%d,%d.%d.%d.%d,%d",&idc,&link,&a[0],&a[1],&a[2],&a[3],&master);
				if(idc <= 0 || link <= 0)
					continue;
				for (i = 0;i <= 3;i++) {
					if ( a[i] > 255 || a[i] < 0 ) {
						ERROR_LOG("[warning]:illegal link ip in file\"%s\"[%s],ignored",filepath,s);
						flag = 1;	//IP有问题！则跳出并置位错误标记为真
						break;
					}
				}//for
				if (!flag) {
					if ((tmpip = (link_ip_t*)malloc(sizeof(link_ip_t))) == NULL)
						ERROR_RETURN(("Load_Link_Config:read ip.malloc failed!"), -1);

					tmpip->ip = a[0]*256*256*256 + a[1]*256*256 + a[2]*256 + a[3];
					tmpip->idc_observer = idc;
					tmpip->idc_master = master;
					tmpip->link_no = link;
					snprintf((char*)tmpip->hashkey, sizeof(tmpip->hashkey),
								"%d%d", tmpip->idc_observer, tmpip->ip);
					tmpip->next = NULL;
					if (link_ip_set == NULL)
						link_ip_set = tmpip;
					else {
						tmpip->next = link_ip_set;
						link_ip_set = tmpip;
					}
					if (g_hash_table_lookup(link_hash_ip, tmpip->hashkey ) == NULL)
						g_hash_table_insert(link_hash_ip, tmpip->hashkey, tmpip);
				}//if legal ip
			}//if linkip
		}//if $:
	}//while
	fclose(fp);
	return 0;
}
void free_linkip()
{
	link_ip_t *tmpnext,*tmp;
	tmpnext = link_ip_set;
	while(tmpnext != NULL) {
		tmp = tmpnext;
		tmpnext = tmpnext->next;
		free(tmp);
	}
}

int load_conf_ip_hash_mmap_connect()
{
	if (load_conf() == -1)		//读取配置
		return -1;
	if (build_dir() == -1)		//创建cdn测速的待发文件目录，idc和cdn的mmap文件所在目录
		return -1;
	if (load_ip() == -1)			//读入ip库
		return -1;
	if (load_hash() == -1)		//创建hash表，用以记录mmap中每一条记录在mmap中的偏移
		return -1;
	if (load_mmap() == -1)		//创建mmap文件
		return -1;
	if (load_linkip(link_ip_config_file) == -1)
		return -1;

	DEBUG_LOG("GET from ip:ip_ready\t:[%u]",idc_test_ip_mmap.ready_num);
	DEBUG_LOG("GET from area:area_ready\t:[%u]",idc_test_area_mmap.ready_num);
	set_hash_for_area_mmap();//从ip库中读出每一条"省市运营商"记录,写入mmap，并相应建立area的hash表

	if (connect_2_dbserver() == -1)
		return 0;
	return 0;
}
