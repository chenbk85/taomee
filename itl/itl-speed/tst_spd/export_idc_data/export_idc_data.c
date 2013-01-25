#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

typedef struct file_head{
	char			symbol[8];			//读取8字节长度标识
	uint32_t		create_time;			//文件创建时间
	uint32_t		last_modify;			//最后修改时间
	uint32_t		total_record;		//文件中的总记录数
	uint32_t		processed_record;	//文件中总共处理了多少记录
}__attribute__((packed)) idc_file_head_t;

static void print_record(store_result_t *record)
{
	printf("process:%u,m_num:%u,time:%u,ip:%u,reachable:%u,delay:%u,loss:%u,hop:%u,prev:%u\n",
			record->processed,record->machine_number,record->probe_time,
			record->probed_ip,record->reachable,record->avg_latency,
			record->loss_percentage,record->hop,record->previous_hop);
}

static int process_file(const char *filename)
{
	FILE *fp;
	store_result_t	result;
	idc_file_head_t f_head;

	fp = fopen(filename,"rb");
	if (fp == NULL) {
		fprintf(stderr, "idc:can not open file %s!\n",filename);
		return -1;
	}

	//读取文件头，判定是否非法文件
	int read_len = fread(&f_head, sizeof(idc_file_head_t), 1, fp);
	if (read_len < 1
		|| memcmp(f_head.symbol, "TAOMEEV5", 8) != 0
		|| f_head.create_time > f_head.last_modify
		|| f_head.total_record < f_head.processed_record) {
		fclose(fp);
		fprintf(stderr, "idc: bad file %s!\n",filename);
		return -1;
	}
	//读取每条记录，更新hash表和mmap.
	while (!feof(fp)) {
		read_len=fread(&result, sizeof(store_result_t), 1, fp);
		if (read_len != 1)
			break;
		if (result.machine_number == 0) {
			fclose(fp);
			fprintf(stderr, "idc file:idc_number == 0 in file %s\n", filename);
			return -1;
		}
		print_record(&result);
	}//while
	fclose(fp);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "\tUsage: ./%s idc_file_path\n", argv[0]);
		return -1;
	}

	process_file(argv[1]);

	return 0;
}
