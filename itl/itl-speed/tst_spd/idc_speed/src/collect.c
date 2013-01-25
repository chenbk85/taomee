/*
 * collect.c 采集数据 多线程方式，一个线程探测一个IP，主线程等待所有线程都结束之后，统一将结果存储文件。
 *
 *  Created on:	2011-7-5
 *  Author:		Singku
 *	Paltform:	Linux Fedora Core 8 x86-32
 *	Compiler:	GCC-4.1.2
 *	Copyright:	TaoMee, Inc. ShangHai CN. All Rights Reserved
 */

// needed for io operation fprintf, sprintf etc.
#include <stdio.h>

// needed for malloc
#include <stdlib.h>

// needed for string operation
#include <string.h>

// needed for usleep
#include <unistd.h>

// needed for inet_aton(), ntohl()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// needed for thread operation
#include <pthread.h>

// needed for directory operation
#include <sys/stat.h>
#include <dirent.h>

// needed for strerror()
#include <errno.h>

// needed for log
#include "log.h"

// needed for global variables
#include "init_and_destroy.h"

// needed for common struct and global variables
#include "common.h"

#include "collect.h"

/*采用mtr工具进行探测
0.72及以下版本的mtr以报表模式输出时是不带跳数序号的,默认带序号。
 * buf中存放了awk_string后用popen执行，会得到形如
 * 0      	1305712613 	218.199.102.190 	16 	218.199.102.190 	229.4 	33 		202.112.53.14的结果
 * 机器号  	探测时间		被探测IP			跳数		终止IP		延迟		丢包率		上一跳
 * 需要检查数据的合法性：
 * 探测机器编号 目前只有3个0,1,2 理论上应该可扩展,设置为unsigned int.
 * 探测时间(由1970年01月01日00时起到现在的秒数来表示
 * 被探测的IP地址必须是一个合法的IP
 * 跳数不能超过255(数据包的TTL，最大为255)最小是1 只可能是这个区间的数值 否则是由于MTR给的参数有误导致
 * 终止IP地址是指MTR最后探测的IP，如果能ping通，则终止IP＝被探测IP，ping不同的话返回"???"，
 * 且在此跳丢包，需要记录上一跳IP
 * 平均延迟：若为0，则有可能没有延迟，此时被探测IP＝终止IP；也有可能Ping不通，此时终于IP地址是???
 * 上一跳IP作为网络不通时，网络诊断的依据。
 * 将接受到的合法数据打包发送给数据接受服务器
 * 特殊情况的结果:
 * 网络不通===>0 1305712850 61.23.25.2 16 ??? 0.0 100％ 61.25.0.150
 * 探测错误的IP地址==>0 1305712924 256.256.256 -1
 * 中间路由出现环路。导致没有丢包，但是不可达，此时设置延迟为0。此时丢报＝0 延迟＝0
 */
#define	AWK_STRING	"awk \'BEGIN\
{\
	ver='\"$(mtr -v)\"';\
	date='\"$(date +%%s)\"';\
	while(\"mtr -r -c 6 -n %d.%d.%d.%d\"|getline>0){\
		line_count++;\
		pre_ip=cur_ip;\
		if(ver>0.72)\
			cur_ip=$2;\
		else \
			cur_ip=$1;\
	}\
	if(ver>0.72){\
		loss=$3;\
		latency=$6;\
	}\
	else{\
		loss=$2;\
		latency=$4;\
	} \
	hop=line_count-1;\
	print %d,date,\"%d.%d.%d.%d\",hop,cur_ip,latency,loss,pre_ip;\
}'",ip->ip[0], ip->ip[1], ip->ip[2], ip->ip[3], machine_number, \
	ip->ip[0], ip->ip[1], ip->ip[2], ip->ip[3]

#define	PING_STRING	"awk \'BEGIN\
{\
	while(\"ping -c 6 -i 0.2 -W 1 %d.%d.%d.%d 2>&1 |grep received\"|getline>0){\
		loss=$6;\
	}\
	print loss;\
}'",ip->ip[0], ip->ip[1], ip->ip[2], ip->ip[3]


thread_queue_t		thread_queue;			//线程队列
ip_addr_t			*probe_ip_link = NULL;	//待探测IP链表
store_result_t		*result_link = NULL;	//探测结果链表
uint16_t			thread_count = 0;		//不能超过MAX_THREAD
pthread_t			thread;					//pthread_create中用到

pthread_spinlock_t lock_thread_count;		//各线程更改thread_count时必须互斥
pthread_spinlock_t lock_result_link;		//各线程结果插入结果链表必须互斥
pthread_spinlock_t lock_thread_queue;		//退出线程回到线程队列时必须互斥

uint32_t			result_count = 0;		//结果总数，如果不出错应该等于探测的IP数

/*
 * @brief 探测单个ip，并将结果挂在结果链表中
 * @param void *arg, 一个ip结构
 */
static void mtr_handler(void *arg)
{
	my_thread_t		*tmp_thread;			//指向线程队列中的一个线程管理结构
	ip_addr_t		*ip;					//待探测的IP
	store_result_t	*tmp_result;			//待存储的结果指针
	probe_result_t	aresult;				//探测结果
	struct in_addr	inip;					//用于IP地址转换 主机序到点分格式
	char			cmd_buf[TEXT_LINE_LEN];			//存放popen调用的子进程命令字符串
	char			result_buf[TEXT_LINE_LEN];		//存放popen得到的结果串
	char			*buf;					//用于strtok_r,指向分割符
	char			*cr;					//指向result_buf中回车换行字符
	char			*pointer[10];			//8个返回值 while里面会调用9次 如果只有9个 最后第一次调用会访问越界
	int				ret_count;				//strtok_r中用来计数
	int 			hop;					//记录探测某一个IP的跳数
	char			*save_ptr;				//strtok_r函数的保存指针
	FILE			*result;				//popen的结果串以管道文件的方式传递

	//===================线程ID入队列====
	tmp_thread = (my_thread_t *)malloc(sizeof(my_thread_t));
	if (tmp_thread == NULL) {
		write_log("[Error]:Create thread:malloc failed!!!in function:%s\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	tmp_thread->athread = pthread_self();
	tmp_thread->next = NULL;

	pthread_spin_lock(&lock_thread_queue);
	if (thread_queue.head == NULL) {
		thread_queue.tail = tmp_thread;
		thread_queue.head = thread_queue.tail;
	}
	else {
		thread_queue.tail->next = tmp_thread;
		thread_queue.tail = tmp_thread;
	}
	pthread_spin_unlock(&lock_thread_queue);
	//=======================

	//========增加线程个数=========
	pthread_spin_lock(&lock_thread_count);
	thread_count++;
	pthread_spin_unlock(&lock_thread_count);

	int redo_count = 0;	//当发现某IP探测结果的延迟大于1秒时，重复测试一下，最多测3次。
	int i;
	char original_result[TEXT_LINE_LEN];	//存储原始结果串，以写日志
redo:
	//准备工作，清除各缓冲 各标志置0
	ip = (ip_addr_t*)arg;
	tmp_result = NULL;

	//added by singku 2011-08-08 for ping loss------start
	//为popen准备命令.
	uint32_t ping_loss;
	sprintf(cmd_buf, PING_STRING);
	result = popen(cmd_buf, "r");
	fread(result_buf, sizeof(char), sizeof(result_buf), result);
	pclose(result);
	if (result_buf[0] != 0)
		ping_loss = strtoul(result_buf, NULL, 10);
	//added by singku 2011-08-08 for ping loss-------end

	//为popen准备命令.
	sprintf(cmd_buf, AWK_STRING);

	result = popen(cmd_buf, "r");
	fread(result_buf, sizeof(char), sizeof(result_buf), result);
	pclose(result);
	//保存原始结果串
	memcpy(original_result, result_buf, sizeof(original_result));

	cr = strchr(result_buf, (int)'\n');//找到换行符
	if (cr != NULL)//有换行符
		*cr = 0;//换行符改成终结符。

	//分割结果串
	for(i = 0; i < 9; i++)
		pointer[i] = 0;
	save_ptr = NULL;
	buf = result_buf;
	ret_count = 0;
	while ((pointer[ret_count] = strtok_r(buf, " ", &save_ptr)) != NULL) {
		buf = NULL;
		ret_count++;
	}

	//合法性检查
	aresult.machine_number = strtoul(pointer[0], NULL, 10);
	aresult.probe_time = strtoul(pointer[1], NULL, 10);
	if (inet_aton(pointer[2], &inip) == 0) {
		//探测了一个非法IP 记录错误日志并直接返回
		write_log("[Warning]:probed a illegal ip address %s\n", pointer[2]);
		goto ret;
	}
	aresult.probed_ip = ntohl(inip.s_addr);

	//探测合法IP hop>=0; 如果hop==0表示本机网卡没启动,因为即使探测本地环回地址也有一跳
	hop = atoi(pointer[3]);
	if (hop == 0) {
		write_log("[Error]:network is unusable!!!\n");
		goto ret;
	}
	aresult.hop = hop;
	//探测IP和跳数都正常 则只需要考虑是否ping通
	if (inet_aton(pointer[4],&inip) == 0)
		//终止IP错误 表示没有ping通 ,终止IP地址设置为0
		aresult.end_ip = 0;
	else
		aresult.end_ip = ntohl(inip.s_addr);

	aresult.avg_latency = (uint32_t)strtoul(pointer[5], NULL, 10);
	aresult.loss_percentage = (uint32_t)strtoul(pointer[6], NULL, 10);
	if (ping_loss < 100)
		aresult.loss_percentage = ping_loss;
	if (aresult.avg_latency > 1000 && ++redo_count <= 3)
		goto redo;
	else if (redo_count > 3)
		write_log("[Error]:ip:%u,delay:%u, original:%s\n",
				aresult.probed_ip, aresult.avg_latency, original_result);

	if (inet_aton(pointer[7], &inip) == 0)
		aresult.previous_hop = 0;//没有上一跳结果，则有可能是上一跳丢包，或者只有1跳就通，或者1跳就不通
	else
		aresult.previous_hop = ntohl(inip.s_addr);

	if (aresult.end_ip == aresult.probed_ip)//ping通
		aresult.reachable = 1;
	else {
		aresult.reachable = 0;
		aresult.avg_latency = 0;//既然不能ping通，延迟必然是0，但不管得到的延迟是多少 都置0。防止意外
		aresult.loss_percentage = 100;
	}

	if((tmp_result = (store_result_t*)malloc(sizeof(store_result_t))) == NULL) {
		write_log("[Error]:Get Result:malloc failed!!!in function:%s\n", __FUNCTION__);
		goto ret;
	}
	tmp_result->processed = 0;
	//probe_result比store_result少了第一个4字节，多了最后一个4字节
	memcpy((char*)tmp_result + sizeof(uint32_t),
			&aresult, sizeof(store_result_t) - sizeof(uint32_t));
	tmp_result->next = NULL;

	//结果入结果队列，由于有多个线程写这个队列，故要互斥
	pthread_spin_lock(&lock_result_link);
	if (result_link == NULL)
		result_link = tmp_result;
	else {
		tmp_result->next = result_link;
		result_link = tmp_result;
	}
	result_count++;
	pthread_spin_unlock(&lock_result_link);

ret:
	pthread_spin_lock(&lock_thread_count);
	thread_count--;
	pthread_spin_unlock(&lock_thread_count);
}

/*
 * @brief探测所有IP
 */
static int probe_ip()
{
	ip_addr_t *pnext = probe_ip_link;//指向带探测IP链表头
	int wait_count = 0;	//主分配逻辑分配的线程在启动后会将自己的ID加入线程队列,
						//主逻辑如果等待wait_count次都没有发现有线程入队列，则说明线程分配存在问题
	int	 head_processed = 0;//记录线程队列头部的节点是否已经判断过(因始终保持一个队头)。
	my_thread_t *tmp_head;
	my_thread_t *tmp_tail;
	pthread_spin_init(&lock_thread_count, 0);
	pthread_spin_init(&lock_result_link, 0);
	pthread_spin_init(&lock_thread_queue, 0);

	thread_queue.head = thread_queue.tail = NULL;

	while (pnext != NULL) {
		while (thread_count >= MAX_THREAD)//若达到最大线程数目，则等待10ms
			usleep(10000);

		if ((pthread_create(&thread, NULL, (void*)mtr_handler, (void*)pnext)) != 0)
			write_log("Create thread failed when probe ip!\n");
		pnext = pnext->next;
	}

	//等待所有线程退出
	//所有IP都开始探测后，线程队列中会加入节点，如果等了3秒都没有节点加入，说明线程没有启动。程序存在问题
	while (thread_queue.head == NULL && wait_count < 300) {
		wait_count++;//3秒钟都没有建立队列，说明程序有问题，则返回
		usleep(10000);//test queue head until not null;
	}

	if (wait_count >= 300) {
		write_log("[Error]:time's up when waitting queue!!!in function:%s\n", __FUNCTION__);
		return -1;
	}

	wait_count = 0;	//现在表示head和tmp_thread的相等次数 如果一直都相等，则说明所有线程都已经退出。
	//也可采用环形队列的方式，判定read = write时即所有线程结束，但相对麻烦。
	tmp_head = thread_queue.head;
	tmp_tail = thread_queue.tail;
	while (thread_queue.head != NULL && wait_count < 300) {
		if (thread_queue.head == thread_queue.tail){
			//若相等，则判定是否结束，但不删除节点，防止同时在头部出，在尾部入出错。若没有结束则等待其结束。
			if (! head_processed){
				wait_count = 0;
				head_processed = 1;
				//process
				if (thread_queue.head->athread != 0)
					pthread_join(thread_queue.head->athread, NULL);//等待队列头节点的线程退出
			}
			else {
				wait_count++;
				usleep(10000);
			}
		}
		else {
			if (! head_processed) {	//不相等，且头部线程还没结束(未处理),则等待其结束并删除节点。
				wait_count = 0;
				head_processed = 0;
				tmp_head = thread_queue.head;
				thread_queue.head = thread_queue.head->next;
				if (tmp_head->athread != 0)
					pthread_join(tmp_head->athread, NULL);
				free(tmp_head);
			}
			else {	//不相等，且头部线程已经结束(处理过),则直接删除头部节点
				wait_count = 0;
				head_processed = 0;
				tmp_head = thread_queue.head;
				thread_queue.head = thread_queue.head->next;
				free(tmp_head);
			}
		}
	}
	return 0;
}

/*
 * @brief 递归创建目录
 * @param path,数据存储路径
 */
static int my_mkdir(const char *path)
{
	struct stat buf;
	mode_t mode = 0x00fff;
	char tmp_path[FULL_PATH_LEN];
	const char *slash = path;//必然最开始是'/'
	const char *cur;
	uint16_t len;
	if (!*( slash+1 ))
		return 0;	//整个path就是'/'

	while ((slash = strchr(slash+1,(int)'/')) != NULL ) {
		cur = slash;		//保存当前/所在位置
		len = slash-path+1;	//包括了“\0”
		snprintf(tmp_path, len, "%s", path);
		//判断存储目录是否存在
		if (stat(tmp_path, &buf) == -1)
			if (errno == ENOENT) {
				if (mkdir(tmp_path,mode) == -1) {
					write_log("Can't create dir %s :%s\n", tmp_path, strerror(errno));
					return -1;
				}
			}

		//若存在但不是目录
		if ((buf.st_mode & S_IFDIR) == 0) {
			remove(tmp_path);	//不是一个目录则删除之
			if (mkdir(tmp_path,mode) == -1) { //并创建一个目录mode的设置使得所有人对其有所有权限
				write_log("Can't create dir %s :%s\n", tmp_path, strerror(errno));
				return -1;
			}
		}
	}

	if (*(cur+1)) {//‘/’后面还有字符表示是最后的字符，则创建全名目录 path 否则是最后的/ 不需要创建
		//判断存储目录是否存在
		if (stat(path,&buf) == -1)
			if (errno == ENOENT)
				if (mkdir(path,mode) == -1) {
					write_log("Can't create dir %s :%s\n", path, strerror(errno));
					return -1;
				}

		//若存在但不是目录
		if (( buf.st_mode & S_IFDIR ) == 0) {
			remove(path);	//不是一个目录则删除之
			if (mkdir(path,mode) == -1) {//并创建一个目录mode的设置使得所有人对其有所有权限
				write_log("Can't create dir %s :%s\n", path, strerror(errno));
				return -1;
			}
		}
	}
	return 0;
}

/*
 * @brief 探测结果链表中的数据存入硬盘文件
 */
static int store_result_to_file()
{
	FILE *resultfile;
	store_result_t *tmpnext = result_link;
	uint32_t seconds;
	uint32_t zero = 0;
	char data_store_subpath_day[FULL_PATH_LEN];	//存储路径下的日期子目录名.
	char tmp_file_full_path_name[FULL_PATH_LEN];//临时文件全名
	char file_full_path_name[FULL_PATH_LEN];//文件全名－临时文件写完毕后改名

	sprintf(data_store_subpath_day, "%s/%s-%s-%s", data_store_path,
			timestring.year, timestring.month, timestring.day);
	if (my_mkdir((const char *)data_store_subpath_day) == -1)//递归创建目录
		return -1;

	sprintf(tmp_file_full_path_name, "%s/tmp.%s",
			data_store_subpath_day, timestring.result_string);//以路径和时间命名
	sprintf(file_full_path_name, "%s/N%04d.%s",
			data_store_subpath_day, machine_number, timestring.result_string);//以路径和时间命名

	resultfile = fopen(file_full_path_name, "wb+");	//以二进制方式写
	if (resultfile == NULL) {
		write_log("Can not open result file %s!!!,please check dir and privilleges\n",
				file_full_path_name);
		return -1; //直接返回即可 不用结束程序
	}
	seconds = time((time_t*)NULL);
	//写文件头
	fwrite(FILE_HEAD_SYMBOL, 1, F_SYMBOL_LEN, resultfile);//文件标识
	fwrite(&seconds, sizeof(seconds), 1, resultfile);//创建时间
	fwrite(&seconds, sizeof(seconds), 1, resultfile);//最后读取时间
	fwrite(&result_count, sizeof(result_count), 1, resultfile);//总记录数
	fwrite(&zero, sizeof(zero), 1, resultfile);//已处理记录数
	//写数据
	while( tmpnext != NULL ) {
		fwrite(tmpnext,( sizeof(store_result_t)-sizeof((store_result_t*)0) ), 1, resultfile );
		tmpnext=tmpnext->next;
	}

	fclose(resultfile);
	//临时文件更名
	rename(tmp_file_full_path_name,file_full_path_name);

	return 0;
}

/*
 * @brief 释放每次循环中为探测IP分配的内存 因为每分钟执行一次，一次执行大概5秒,
 *  即使每轮都分配和释放，也不会有频繁的内存分配与释放
 */
static void free_probed_ip()
{
	ip_addr_t *tmpip, *ipnext;
	//probe_ip_link;
	ipnext = probe_ip_link;
	while (ipnext != NULL) {
		tmpip = ipnext;
		ipnext = ipnext->next;
		free(tmpip);
	}
	probe_ip_link = NULL;
}

/*
 * @brief 释放每次循环中为探测结果分配的内存 因为每分钟执行一次，
 *  即使每轮都分配和释放，也不会有频繁的内存分配与释放
 */
static void free_result()
{
	store_result_t *tmpresult, *resultnext;
	//store_result_link;
	resultnext = result_link;
	while (resultnext != NULL) {
		tmpresult = resultnext;
		resultnext = resultnext->next;
		free(tmpresult);
	}
	result_link = NULL;
	result_count = 0;
}

/*
 * @brief 数据采集线程控制逻辑
 */
//#include <time.h>
void* data_collect()
{
	//pthread_detach(pthread_self());
	while (1) {
		//等待分钟的整点开始
		while( time((time_t*)NULL)%60 > 2 ) //整点2秒以内
			usleep(1000);//休眠1ms usleep时间是微秒
		//printf("start:%d\n",(int)time(0));
		//更新打印日志的时间.
		get_time_string();
		if (collect_load_config() == -1)
			pthread_exit(NULL);
		if (probe_ip() == -1)
			pthread_exit(NULL);
		if (store_result_to_file() == -1)
			pthread_exit(NULL);
		free_probed_ip();
		free_result();
		//printf("end:%d\n",(int)time(0));
		//休眠到整点
		usleep( (60-time((time_t*)NULL)%60)*1000000 );
	}

	return 0;
}
