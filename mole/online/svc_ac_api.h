#ifndef SVC_AC_API
#define SVC_AC_API

#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>


/**
 * @brief 发包给外挂旁路系统
 * 1. 发送时给 'recv_time' 加上时间戳;
 * 2. 有一定的发送策略(包合并, 1000包合并成一个发送, 至少1秒发送一次);
 * 3. 只发送包头, 且不会修改任何cs包的数据;
 * 4. 用了512K栈空间作为发包的缓存 ( (17B包头 + sizeof(ac_one_data_t))*1000 + sizeof(svr_pkg_t) + sizeof(ac_body_t))
 * 5. 非线程安全 (请交由接收线程调用该接口)
 */
void send_cpkg_head_to_svr(const char *ac_svc_ip, uint16_t ac_svr_port,
		const void *chead, uint32_t cheadlen,
		const struct sockaddr_in *cli_addr, const struct timeval *tv_now);

#endif /* __SVC_AC_API__ */
