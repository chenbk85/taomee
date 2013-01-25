#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_POINT 20

enum _proto_fcgi_cmd {
	proto_fcgi_cdn_report						= 0x2001,
	proto_fcgi_url_report						= 0x2002,
};

typedef struct fcgi_cdn{
	uint32_t	cdn_ip;			//cdn_ip
	uint32_t	speed;				//下载速率
}__attribute__((packed)) fcgi_cdn_t;

typedef struct fcgi_url{
	uint32_t	page_id;				//页面编号
	uint32_t count;					//断点数
	uint32_t	vlist[MAX_POINT];	//断点耗时
}__attribute__((packed)) fcgi_url_t;

typedef union union_result{
	fcgi_cdn_t	m_cdn;
	fcgi_url_t	m_url;
}__attribute__((packed)) union_result_t;

typedef struct fcgi_packet{
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//client_ip
	uint32_t	test_time;		//测试时间
}__attribute__((packed)) fcgi_common_t;

typedef struct fcgi_store{
	uint32_t type;				//在内存中标识是CDN还是URL
	uint32_t special_flag;	//说明这个数据是特殊数据，是没有往分析服务器发送成功的，需要另外专门存储
	uint32_t	project_number;	//项目编号
	uint32_t	user_id;			//用户编号
	uint32_t	client_ip;		//client_ip
	uint32_t	test_time;		//测试时间
	union_result_t record;	//结果
}__attribute__((packed)) fcgi_store_t;

static void print_cdn(fcgi_store_t *record, uint32_t data_len)
{
	if (data_len != sizeof(fcgi_store_t)) {
		fprintf(stderr, "cdn:incomplete record!\n");
		return ;
	}
	printf("type:%u,special:%u,proj:%u,user:%u,client:%u,time:%u,cdn:%u,speed:%u\n",
			record->type,record->special_flag,record->project_number,record->user_id,
			record->client_ip,record->test_time,record->record.m_cdn.cdn_ip,
			record->record.m_cdn.speed);
}

static void print_url(fcgi_store_t *record, uint32_t data_len)
{
	if (data_len != sizeof(fcgi_store_t)) {
		fprintf(stderr, "url:incomplete record!\n");
		return ;
	}

	printf("type:%u,special:%u,proj:%u,user:%u,client:%u,time:%u,",
			record->type,record->special_flag,record->project_number,record->user_id,
			record->client_ip,record->test_time);
	int count = record->record.m_url.count;
	int i;
	printf("page:%u,count:%u,list:", record->record.m_url.page_id, count);
	for (i = 0; i < count; i++) {
		printf("%u ", record->record.m_url.vlist[i]);
	}
	printf("\n");
}

static int process_file(char *filepath)
{
	fcgi_store_t tmp;
	char zero[1024];
	memset(zero, 0x0, sizeof(zero));
	int fd = open(filepath, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "fail to open file %s\n", filepath);
		return -1;
	}
	int read_len;
	while ((read_len = read(fd, &tmp, sizeof(fcgi_store_t))) >= -1) {
		if (read_len == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				fprintf(stderr, "Error  when read files <%s>: %s\n", filepath, strerror(errno));
				return -1;
			}
		} else if (read_len < (int)sizeof(fcgi_store_t)) {
			return 0;
		} else {
			if (memcmp(&tmp, zero, read_len) == 0) {	//读到分钟分段
				continue;
			} else {
				if (tmp.type == proto_fcgi_cdn_report) {
					print_cdn(&tmp, (uint32_t)read_len);
				} else if (tmp.type == proto_fcgi_url_report) {
					print_url(&tmp, (uint32_t)read_len);
				} else {
					fprintf(stderr, "Invalid meta file:%s\n", filepath);
				}
			}//不是分钟分段
		}//读到完整记录
	}

	close(fd);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "\tUsage: ./%s cdn_url_file_path\n", argv[0]);
		return -1;
	}

	process_file(argv[1]);

	return 0;
}
