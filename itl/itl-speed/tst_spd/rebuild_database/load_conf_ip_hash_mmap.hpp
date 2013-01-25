#ifndef		INITIATE_H_
#define		INITIATE_H_

#include <stdint.h>
#include <glib.h>



#define		MMAP_FILE_SYMBOL		"TAOMEEV5"	//mmap文件的文件头标识，8字节
#define		MMAP_FILE_SYMBOL_LEN	8				//mmap文件的文件头标识长度，接着4字节表示记录数 4字节表示类型
#define		MMAP_FILE_RECORD_LEN	sizeof(uint32_t)
#define		MMAP_FILE_READY_LEN	sizeof(uint32_t)
#define		MMAP_FILE_TYPE_LEN		sizeof(uint32_t)
#define		MMAP_FILE_KEY_LEN		sizeof(uint32_t)
#define		MMAP_FILE_VALUE_LEN	sizeof(uint32_t)
#define		MMAP_FILE_LATEST_LEN	sizeof(uint32_t)
#define		MMAP_FILE_HEAD_LEN		sizeof(mmap_file_head_t)	//mmap文件头长度 8＋4+4+4＝20

#define		IDC_MASTER_INVALID		0xFFFFFFFF
#define		MAX_DB_SERVER			10				//数据库模块的最大个数
#define		REMOVE_IDC_FILE			1
#define		RENAME_IDC_FILE			2

#define		MAX_AREA_RECORD			8192
#define		MAX_IP_RECORD			1024
#define		MAX_PAGE_RECORD			100

#define		SHIFT_BITS				16

#define		idc_mmap_size_area		MAX_AREA_RECORD * sizeof(idc_test_area_t)
#define		idc_mmap_size_ip		MAX_IP_RECORD * sizeof(idc_test_ip_t)
#define		cdn_mmap_size_area		MAX_AREA_RECORD * sizeof(cdn_speed_area_t)
#define		cdn_mmap_size_ip		MAX_IP_RECORD * sizeof(cdn_speed_ip_t)
#define		url_mmap_size_page		MAX_AREA_RECORD * sizeof(url_test_t)

typedef enum _mmap_type{
	MMAP_TYPE_IDC_IP				= 1,
	MMAP_TYPE_IDC_AREA				= 2,
	MMAP_TYPE_CDN_IP				= 3,
	MMAP_TYPE_CDN_AREA				= 4,
	MMAP_TYPE_URL					= 5,
}mmap_type_e;

enum _const_variable{
	STR_IP_LEN				= 16,
	MAX_IDC_NUMBER			= 16,
	STR_IDCNO_AND_IP_LEN	= 21,
	MAX_PROJECT_NUMBER		= 128,
	BUF_NET_SEND_LEN		= 1024,
	DIR_PATH_LEN				= 1024,
};

typedef struct mmap_head{
	char		file_symbol[MMAP_FILE_SYMBOL_LEN];
	uint32_t	records_num;
	uint32_t	ready_num;
	uint32_t	type;			//说明该MMAP的类型，类型定义见上面
	uint32_t	key;			//其他mmap保留，url的mmap用来记录pageid
	uint32_t	value;			//其他mmap保留，url的mmap用来记录pageid对应的具体mmap索引0-99
	uint32_t	latest_time;	//其他mmap保留，url的mmap用来记录该page的最新时间.
}__attribute__((packed)) mmap_file_head_t;

typedef struct my_mmap{
	mmap_file_head_t*mmap_head;	//指向mmap的头部
	char 				*mmap_p;		//指向mmap的数据部分
	uint32_t			records_num;	//本表总记录数
	uint32_t			ready_num;	//待发记录,有的IP或cityisp记录可能占了一个位置,但是并没有实际数据
	uint32_t			type;
	uint32_t			key;
	uint32_t			value;
	uint32_t			latest_time;
}__attribute__((packed)) my_mmap_t;

typedef struct link_ip{
	uint32_t	link_no;
	uint32_t	idc_observer;
	uint32_t	ip;
	uint32_t	idc_master;
	uint8_t	hashkey[21];//idc_observer&ip组成hashkey以便idc文件用machine_number&ip查询
	struct link_ip *next;
}__attribute__((packed)) link_ip_t;

typedef struct dbser{
	int			id;
	char		ip[STR_IP_LEN];
	uint16_t	port;
	int			fd;
}__attribute__((packed)) db_server_t;

typedef struct url_page_used {
	uint32_t used_count;
	uint32_t status[MAX_PAGE_RECORD];
} url_page_used_t;
extern url_page_used_t *url_page_status;

extern char			dbserver_ip[STR_IP_LEN];
extern uint16_t		dbserver_port;
extern char			bind_ip[STR_IP_LEN];
extern uint16_t		bind_port;
extern uint16_t		max_project_number;
extern uint16_t		max_page_per_proj;
extern uint16_t		delay_upper_bound;
extern uint16_t		max_idc_number;
extern uint16_t		remove_or_rename;
extern uint32_t		max_cdn_speed;
extern char			idc_file_dirpath[DIR_PATH_LEN];
extern char			idc_rename_dirpath[DIR_PATH_LEN];
extern char			cdn_url_file_dirpath[DIR_PATH_LEN];
extern char			idc_mmap_file_dirpath[DIR_PATH_LEN];
extern char			cdn_mmap_file_dirpath[DIR_PATH_LEN];
extern char			url_mmap_file_dirpath[DIR_PATH_LEN];
extern link_ip_t	*link_ip_set;

extern GHashTable*		idc_hash_ip;
extern GHashTable*		cdn_hash_ip[128];
extern GHashTable*		link_hash_ip;
extern GHashTable*		hash_cityisp;
extern GHashTable**	url_page_index;

extern my_mmap_t		idc_test_ip_mmap;
extern my_mmap_t		idc_test_area_mmap;
extern my_mmap_t*		cdn_speed_ip_mmap;
extern my_mmap_t*		cdn_speed_area_mmap;
extern my_mmap_t*		url_page_mmap;

extern db_server_t		db_server_set[MAX_DB_SERVER];

extern pthread_t		idc_thread;
extern pthread_mutex_t	idc_mutex;
extern pthread_cond_t	idc_cond;

extern pthread_t		cdn_and_url_thread;
extern pthread_mutex_t	cdn_and_url_mutex;
extern pthread_cond_t	cdn_and_url_cond;


extern "C" int		load_conf_ip_hash_mmap_connect();
extern "C" void		free_linkip();


#endif		//INITIATE_H_
