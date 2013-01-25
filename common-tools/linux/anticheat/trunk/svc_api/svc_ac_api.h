#ifndef __SVC_AC_API_H__
#define __SVC_AC_API_H__


/* 业务根据需要修改这个值: 原则是:
 * (1) 根据客户端包量尽量使得全系统发包的包量在1秒钟几百个;
 * (2) 同时必须保证这个值不能超过1000
 */
#define MAX_BATCH_SENT_NUM		(500)


#pragma pack(1)

/**
  * @brief cs包头 (各字段均是网络序)
  */
struct __cli_proto_t {
    /*! package length */
    uint32_t    len;
    /*! protocol version */
    uint8_t     ver;
    /*! protocol command id */
    uint32_t    cmd;
    /*! user id */
    uint32_t	id;
    /*! used as an auto-increment sequence number if from client */
    uint32_t    ret;
    /*! body of the package */
    uint8_t     body[]; 
};

struct __client_info_t {
	/* 玩家的IP地址 (由cs包的接收者赋值) */
	struct sockaddr_in		addr;
	/* cs包被recv的时间 (由cs包的接收者赋值) */
	struct timeval			recv_time;
};

struct __ac_one_cdata_t {
	/* client 信息 */
	struct __client_info_t		cinfo;
	/* client pkg head 原始数据 */
	struct __cli_proto_t		chead;
};

struct __ac_body_t {
	uint32_t				count;
	struct __ac_one_cdata_t	cdata[];
};

struct __svr_pkg_t {
	/*! package length */
	uint32_t	len;
	/*! protocol version */
	uint32_t	ver;
	/*! protocol command id */
	uint32_t	cmd;
	/*! user id */
	uint32_t	uid;
	/*! seqno in server's pkg */
	uint32_t	seqno;
	/*! body of the package */
	uint8_t     body[];
};

#pragma pack()

enum __ac_cmd_e {
	/* 上报cs包头 */
	__ac_cmd_report_cpkg_head		= 1001,
};



/* 
 * 若包头不是 17B, 则需要调用这个接口, 转换成17B的包头,
 * 注意: 具体的参数 (cmd,uid,...) 以 `网络序' 提供;
 */
void send_cpkg_head_to_svr_ex(const char *ac_svc_ip, uint16_t ac_svr_port,
		uint32_t len, uint8_t ver, uint32_t cmd, uint32_t uid, uint32_t ret,
		const struct sockaddr_in *cli_addr, const struct timeval *tv_now);

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
		const struct sockaddr_in *cli_addr, const struct timeval *tv_now);


#endif /* __SVC_AC_API_H__ */
