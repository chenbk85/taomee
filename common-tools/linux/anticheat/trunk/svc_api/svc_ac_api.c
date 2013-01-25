#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

#include "svc_ac_api.h"

#ifdef __cplusplus
}
#endif 


/* 
 * 若包头不是 17B, 则需要调用这个接口, 转换成17B的包头,
 * 注意: 具体的参数 (cmd,uid,...) 以 `网络序' 提供;
 */
void send_cpkg_head_to_svr_ex(const char *ac_svc_ip, uint16_t ac_svr_port,
		uint32_t len, uint8_t ver, uint32_t cmd, uint32_t uid, uint32_t ret,
		const struct sockaddr_in *cli_addr, const struct timeval *tv_now)
{

	static struct __cli_proto_t chead;
	chead.len = len;
	chead.ver = ver;
	chead.cmd = cmd;
	chead.id = uid;
	chead.ret = ret;

	send_cpkg_head_to_svr(ac_svc_ip, ac_svr_port,
			&chead, sizeof(chead), cli_addr, tv_now);
}

/**
 * @brief 发包给外挂旁路系统
 * 1. 发送时给 'recv_time' 加上时间戳;
 * 2. 有一定的发送策略(包合并, 1000包合并成一个发送, 至少1秒发送一次);
 * 3. 只发送包头, 且不会修改任何cs包的数据;
 * 4. 用了512K栈空间作为发包的缓存 ( (17B包头 + sizeof(__ac_one_cdata_t))*1000 + sizeof(__svr_pkg_t) + sizeof(__ac_body_t))
 * 5. 非线程安全 (请交由接收线程调用该接口)
 */
void send_cpkg_head_to_svr(const char *ac_svc_ip, uint16_t ac_svr_port,
		const void *chead, uint32_t cheadlen,
		const struct sockaddr_in *cli_addr, const struct timeval *tv_now)
{
#define ac_likely(x) __builtin_expect(!!(x), 1)
#define ac_unlikely(x) __builtin_expect(!!(x), 0)


	static char svr_pkg_buf[512*1024];

	int ret, flag;
	size_t len = 0;
	static int ac_udp_fd = -1;
	struct sockaddr_in ac_addr;
	socklen_t tolen = sizeof(ac_addr);
	struct __svr_pkg_t *pkg = (struct __svr_pkg_t *)svr_pkg_buf;
	struct __ac_body_t *ac_body = (struct __ac_body_t *)(pkg->body);
	struct __ac_one_cdata_t *cdata = NULL;

	static int tot_rcd_count = 0;
	int init_tot_rcd_count = tot_rcd_count;
	const int max_batch_sent_num = MAX_BATCH_SENT_NUM;
	const time_t max_batch_sent_gap = 1;
	int need_send = 0;
	struct timeval now;
	time_t now_sec;
	static time_t last_sent_time = 0;

	size_t pad = sizeof(struct __svr_pkg_t) + sizeof(struct __ac_body_t);
	size_t tot_len = pad + tot_rcd_count * sizeof(struct __ac_one_cdata_t);
	ssize_t max_left = sizeof(svr_pkg_buf) - tot_len;


	if (ac_unlikely(cheadlen != sizeof(struct __cli_proto_t))) {
		/* 包头长度不匹配, 不发送 */
		return ;
	}

	/* 尽量给 tv_now 啊 !!! */
	if (tv_now) {
		now.tv_sec  = tv_now->tv_sec;
		now.tv_usec  = tv_now->tv_usec;
	} else {
		gettimeofday(&now, 0);
	}
	now_sec = now.tv_sec;
	if (ac_unlikely(last_sent_time == 0)) {
		last_sent_time = now_sec;
	}

	/* 剩下的缓存不够存下一个cs包头了, 先发出去 */
	if (ac_unlikely(sizeof(struct __ac_one_cdata_t) > max_left)) {
		need_send = 1;
		goto try_send;
	}

	/* 已经收集到一次发送的最大合并的数量了, 先发出去 */
	if (tot_rcd_count >= max_batch_sent_num) {
		need_send = 1;
		goto try_send;
	}

	/* 超过最长收集间隔也发 */
	if (now_sec - last_sent_time >= max_batch_sent_gap) {
		need_send = 1;
		goto try_send;
	}

try_send:
	if (need_send && tot_rcd_count) {
		/* check or create udp socket */
		if (ac_unlikely(ac_udp_fd == -1)) {
			if ((ac_udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
				last_sent_time = now_sec;
				tot_rcd_count = 0;
				return ;
			}
			/* set_socket_reuseaddr */
			flag = 1;
			setsockopt(ac_udp_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

			/* set_socket_nonblock */
			flag = fcntl(ac_udp_fd, F_GETFL, 0);
			flag |= O_NONBLOCK;
			flag |= O_NDELAY;
			fcntl(ac_udp_fd, F_SETFL, flag);
		}

		/* make ac_addr */
		ac_addr.sin_family = AF_INET;
		inet_pton(AF_INET, ac_svc_ip, &(ac_addr.sin_addr));
		ac_addr.sin_port = htons(ac_svr_port);

		/* prepare svr_pkg_head */
		pkg->len = htonl(tot_len);
		pkg->cmd = htonl(__ac_cmd_report_cpkg_head);
		ac_body->count = htonl(tot_rcd_count);

		/* sendto */
		ret = sendto(ac_udp_fd, svr_pkg_buf, tot_len, 0,
				(const struct sockaddr *)&ac_addr, tolen);
		last_sent_time = now_sec;
		tot_rcd_count = 0;

		//printf("sendto, ret=%d\n", ret);
	}

	/* add new data */
	if (init_tot_rcd_count != tot_rcd_count) {
		tot_len = pad + tot_rcd_count * sizeof(struct __ac_one_cdata_t);
		max_left = sizeof(svr_pkg_buf) - tot_len;
	}
	if (ac_unlikely(sizeof(struct __ac_one_cdata_t) > max_left)) {
		tot_rcd_count = 0;
		return ;
	}
	cdata = &(ac_body->cdata[tot_rcd_count]);
	
	if (ac_likely(cli_addr)) {
		memcpy(&(cdata->cinfo.addr), cli_addr, sizeof(cdata->cinfo.addr));
	}
	memcpy(&(cdata->cinfo.recv_time), &now, sizeof(cdata->cinfo.recv_time));
	memcpy(&(cdata->chead), chead, cheadlen);

	tot_rcd_count++;

	//printf("%d: add_tv: [%ld, %ld]\n", tot_rcd_count, now.tv_sec, now.tv_usec);

#undef ac_likely
#undef ac_unlikely
#undef MAX_BATCH_SENT_NUM
}

#ifdef __TEST_SVC_AC_API__

char buf[1024];
int main(int argc, char **argv)
{
	int i, loops = 0;
	char *ac_ip;
	uint16_t ac_port;
	if (argc != 5) {
		printf("Usage: %s <loops> <send_interval(us)> <ac_ip> <ac_port>\n", argv[0]);
		return -1;
	}
	loops = atoi(argv[1]);
	useconds_t usec = atoi(argv[2]);
	ac_ip = argv[3];
	ac_port = atoi(argv[4]);

	struct timeval now;
	struct sockaddr_in cliaddr;
	struct __cli_proto_t *p = (struct __cli_proto_t *)buf;

	struct timeval tv1, tv2, tv3;
	gettimeofday(&tv1, NULL);
	for (i = 0; i < loops; i++) {
		p->len = htonl(100);
		p->ver = 0;
		p->cmd = htonl(1000);
		p->id = htonl(50001);
		p->ret = 0;

		cliaddr.sin_family = AF_INET;
		inet_pton(AF_INET, "11.11.11.11", &(cliaddr.sin_addr));
		cliaddr.sin_port = htons(12345);

		gettimeofday(&now, 0);
		send_cpkg_head_to_svr(ac_ip, ac_port, p, sizeof(struct __cli_proto_t), &cliaddr, &now);

		usleep(usec);
	}
	gettimeofday(&tv2, NULL);
	timersub(&tv2, &tv1, &tv3);
	double sendtime = tv3.tv_sec + tv3.tv_usec / (1000 * 1000);

	printf("Finish to send, loops: %d, time: %f\n", loops, sendtime);

	size_t s = sizeof(struct __svr_pkg_t) + sizeof(struct __ac_body_t) + 1000 * sizeof(struct __ac_one_cdata_t);
	printf("1000 batch svr_pkg size: %zd\n", s);
	printf("size_info: svr_pkg_t: %zd, ac_body_t: %zd, ac_one_cdata_t: %zd, cli_proto_t: %zd\n",
			sizeof(struct __svr_pkg_t), sizeof(struct __ac_body_t), sizeof(struct __ac_one_cdata_t), sizeof(struct __cli_proto_t));

	return 0;
}
#endif /* __TEST_SVC_AC_API__ */
