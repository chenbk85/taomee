#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define IP_NUM	10
int online_ip[IP_NUM] = {9, 29, 36, 49, 50, 58, 66,\
						 140, 142, 143};

//90, 91, 130, 134, 136, used

unsigned int on_port[3] = {1863, 1201, 1221};
unsigned int game_port[3] = {3001, 3002, 3003};
unsigned int game_ip[2] = {60, 85};
unsigned int dbpr_ip[2] = {20, 23};
unsigned int dbpr_port[3] = {21001, 21002, 21003};

char tag[3] = {'a', 'b', 'c'};

typedef struct {
	int  in_ip;
	int  out_ip;
}out_ip_itm_t;

out_ip_itm_t oiit[] = {{131, 76}, {132, 77}, {133, 78}, {135, 79}, {139, 80},
					   {140, 81}, {141, 82}, {142, 83}, {143, 84}, {144, 85}};

int get_out_ip_from(int iip)
{
	int lp;
	for (lp = 0; lp < 10; lp++) {
		if (oiit[lp].in_ip == iip)
			return oiit[lp].out_ip;
	}
	return 0;
}

void create_startid_worknum(unsigned int* pstart_id, unsigned int* pworknum, int lop, int idx)
{
	if (online_ip[lop] <= 19 || online_ip[lop] == 140) {
		if (idx == 2) {
			*pstart_id += 14;
			*pworknum = 14;
		} else {
			*pstart_id += 13;
			*pworknum = 13;
		}
	} else {
		*pstart_id += 20;
		*pworknum = 20;
	}
	return;
}

int main(int argc, char* argv[])
{
	char file_name[64];
	char buf[256];

	unsigned int online_start_id = 1;
	int loop, indx;
	for (loop = 0; loop < IP_NUM; loop++) {
		for (indx = 0; indx < 3; indx++) {
			FILE* fd = NULL;
			memset(file_name, 0, sizeof(file_name));
			snprintf(file_name, sizeof(file_name), "bench.%u.%c", online_ip[loop], tag[indx]);
			fd = fopen(file_name, "w");
			if (fd == NULL) {
				printf("cannot open file %s", file_name);
				return -1;
			}
			//online_ip
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "online_ip\t114.80.98.%u\n", online_ip[loop]);
			int wt_serv_ip = get_out_ip_from(online_ip[loop]);
			if (wt_serv_ip) {
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "online_ip\t112.65.240.%u\n", wt_serv_ip);
			}
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//online_port
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "online_port\t%u\n", on_port[indx]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//online_start_id  worker_num
			int work_num;
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "online_start_id\t%u\n", online_start_id);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			printf("%s", buf);
			create_startid_worknum(&online_start_id, &work_num, loop, indx);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "worker_num\t%u\n", work_num);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			printf("%s", buf);
			//gameserv_ip
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "gameserv_ip\t114.80.98.%u\n", game_ip[loop%2]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//gameserv_port
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "gameserv_port\t%u\n", game_port[indx]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//gameserv_test_ip
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "gameserv_test_ip\t114.80.98.%u\n", game_ip[loop%2]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//gameserv_test_port
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "gameserv_test_port\t%u\n", game_port[indx]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}
			//local_ip
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "local_ip\t192.168.0.%u\n", online_ip[loop]);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}

			//domain
			memset(buf, 0, sizeof(buf));
			int dm = 0;
			if (wt_serv_ip)
				dm = 1;
			snprintf(buf, sizeof(buf), "domain\t%u\n", dm);
			if(EOF == fputs(buf, fd)) {
				printf("write to file %s error:%s", file_name, buf);
				return -1;
			}

			if (fclose(fd) == -1) {
				printf("close file %s error", file_name);
				return -1;
			}

		}
	}
}
