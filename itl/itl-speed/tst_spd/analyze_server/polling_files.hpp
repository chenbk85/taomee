#ifndef		POLLING_FILES_H
#define		POLLING_FILES_H

#define		FILE_HEAD_SYMBOL				"TAOMEEV5"
#define		FILE_HEAD_LEN					8*sizeof(char)

typedef struct file_head{
	char			symbol[8];			//读取8字节长度标识
	uint32_t		create_time;			//文件创建时间
	uint32_t		last_modify;			//最后修改时间
	uint32_t		total_record;		//文件中的总记录数
	uint32_t		processed_record;	//文件中总共处理了多少记录
}__attribute__((packed)) idc_file_head_t;

//存储探测结果的结构体
typedef struct store_result{
	uint32_t			processed	;		//是否已经处理
	uint32_t			machine_number;	//探测机器编号
	uint32_t			probe_time;		//探测时间
	uint32_t			probed_ip;		//探测的IP
	uint32_t			reachable;		//能否Ping通，//只需要true or false即可 但是为了内存对齐定义为4B
	uint32_t			avg_latency;		//平均延时,即ping值
	uint32_t			hop;				//跳数
	uint32_t			loss_percentage;//丢包率
	uint32_t			previous_hop;	//上一跳路由IP
}__attribute__((packed)) store_result_t;


typedef struct file{
	char		filename[24];		//N0000.year-month-day-hour-min==>N0000.2011-05-25-13-14'\0'
	char		fullpath[1024];		//文件全路径名
	struct file *next;				//下一个文件地址
}__attribute__((packed)) file_t;

extern uint32_t idc_ip_packet_tag;
extern uint32_t idc_area_packet_tag;
extern "C" void* polling_files(void*);

#endif
