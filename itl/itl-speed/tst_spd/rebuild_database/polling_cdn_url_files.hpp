#ifndef		PROCESS_PACKET_H_
#define		PROCESS_PACKET_H_

#define FILE_NAME_LEN 256
#define	FULL_PATH_LEN 1024

/*
 * @typedef file_t
 * @brief 当目标目录存在多个文件时，将这些文件名全部读入，循环处理
 */
typedef struct file{
	char		filename[FILE_NAME_LEN];		//META-year-month-day-hour-min-sec==>META-2011-05-25-13-14-00'\0'
	char		fullpath[FULL_PATH_LEN];		//文件全路径名
	struct file *next;							//下一个文件地址
}__attribute__((packed)) file_t;


extern pthread_spinlock_t lock_cdn_ip;

extern uint32_t cdn_url_packet_tag;
extern "C" void* send_file(void*);
extern "C" int set_cdn_mmap(fcgi_store_t *buf, uint32_t data_len);
extern "C" int set_url_mmap(fcgi_store_t *buf, uint32_t data_len);

#endif
