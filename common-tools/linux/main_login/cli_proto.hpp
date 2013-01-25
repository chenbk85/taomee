/**
 *============================================================
 *  @file      cli_proto.hpp
 *  @brief     定义了默认的两种客户端包头结构
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_CLI_PROTO_HPP_
#define LIBTAOMEEPP_CLI_PROTO_HPP_

extern "C" {
#include <libtaomee/log.h>

#include <libtaomee/project/types.h>

#include <async_serv/net_if.h>
}

#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/inet/pdumanip.hpp>

enum {
	/*! 客户端来包最大长度限制 */
	clipkg_max_size			= 8192
};

#pragma pack(1)

/**
  * @brief 客户端包头版本1，17字节，目前摩尔、赛尔和小花仙使用
  */
class CliProto1 {
public:
	/**
	  * @brief type of command id
	  */
	typedef uint32_t CmdType;

public:
	/**
	  * @brief 初始化包头
	  * @param cmdid 协议命令号
	  * @param uid   米米号
	  * @param result 错误码
	  */
	void init(uint32_t cmdid, userid_t uid, uint32_t result);

	/**
	  * @brief 发包给客户端
	  * @param sess sess 
	  * @return 0成功，-1失败 
	  */
	int send_to(fdsession_t* sess);

	/**
	  * @brief 打包数据
	  */
	template<typename T> 
	void pack(T val)
		{ taomee::pack(m_body, val, m_len); }
	/**
	  * @brief 打包数组/字符串数据
	  */
	void pack(const void* val, std::size_t len)
		{ taomee::pack(m_body, val, len, m_len); }
	/**
	  * @brief 解包数据
	  */
	template<typename T> 
	T unpack(T val)
		{ return taomee::bswap(val); }

public:
	/*! package length */
	int			m_len;
	/*! version */
	uint8_t		m_ver;
	/*! protocol command id */
	CmdType		m_cmd;
	/*! user id */
	userid_t	m_id;
	/*! errno: 0 for OK */
	uint32_t	m_ret;
	/*! body of the package */
	uint8_t		m_body[];
};

/**
  * @brief 客户端包头版本2，18字节，目前摩尔勇士和功夫派使用
  */
class CliProto2 {
public:
	/**
	  * @brief type of command id
	  */
	typedef uint16_t CmdType;

public:
	/**
	  * @brief 初始化包头
	  * @param cmdid 协议命令号
	  * @param uid   米米号
	  * @param result 错误码
	  */
	void init(uint16_t cmdid, userid_t uid, uint32_t result);

	/**
	  * @brief 发包给客户端
	  * @param sess sess 
	  * @return 0成功，-1失败 
	  */
	int send_to(fdsession_t* sess);

	/**
	  * @brief 打包数据
	  */
	template<typename T> 
	void pack(T val)
		{ taomee::pack(m_body, val, m_len); }
	/**
	  * @brief 打包数组/字符串数据
	  */
	void pack(const void* val, std::size_t len)
		{ taomee::pack(m_body, val, len, m_len); }
	/**
	  * @brief 解包数据
	  */
	template<typename T> 
	T unpack(T val)
		{ return taomee::bswap(val); }

public:
	/*! package length */
	int			m_len;
	/*! protocol command id */
	CmdType		m_cmd;
	/*! user id */
	userid_t	m_id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t	m_seqno;
	/*! errno: 0 for OK */
	uint32_t	m_ret;
	/*! body of the package */
	uint8_t		m_body[];
};

/**
  * @brief 客户端包头版本3，18字节，按小端字节序解包, 目前seer2在使用
  */
class CliProto3 {
public:
	/**
	  * @brief type of command id
	  */
	typedef uint16_t CmdType;

public:
	/**
	  * @brief 初始化包头
	  * @param cmdid 协议命令号
	  * @param uid   米米号
	  * @param result 错误码
	  */
	void init(uint16_t cmdid, userid_t uid, uint32_t result);

	/**
	  * @brief 发包给客户端
	  * @param sess sess 
	  * @return 0成功，-1失败 
	  */
	int send_to(fdsession_t* sess);

	/**
	  * @brief 打包数据
	  */
	template<typename T> 
	void pack(T val)
		{ taomee::pack_h(m_body, val, m_len); }
	/**
	  * @brief 打包数组/字符串数据
	  */
	void pack(const void* val, std::size_t len)
		{ taomee::pack(m_body, val, len, m_len); }
	/**
	  * @brief 解包数据
	  */
	template<typename T> 
	T unpack(T val)
		{ return val; }

public:
	/*! package length */
	int			m_len;
	/*! protocol command id */
	CmdType		m_cmd;
	/*! user id */
	userid_t	m_id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t	m_seqno;
	/*! errno: 0 for OK */
	uint32_t	m_ret;
	/*! body of the package */
	uint8_t		m_body[];
};

#pragma pack()

//------------------------------------------------------------------------
// For CliProto1
//
inline void
CliProto1::init(uint32_t cmdid, userid_t uid, uint32_t result)
{
	m_len = 0;
	m_ver = 0;
	m_cmd = taomee::bswap(cmdid);
	m_id  = taomee::bswap(uid);
	m_ret = taomee::bswap(result);
}

inline int
CliProto1::send_to(fdsession_t* sess)
{
	int len = m_len + sizeof(CliProto1);
	m_len = taomee::bswap(len);
	if (send_pkg_to_client(sess, this, len) == 0) {
		return 0;
	}

	KERROR_LOG(0, "failed to send pkg to client: uid=%u cmd=%u", m_id, m_cmd);
	return -1;
}

//------------------------------------------------------------------------
// For CliProto2
//
inline void
CliProto2::init(uint16_t cmdid, userid_t uid, uint32_t result)
{
	m_len   = 0;
	m_cmd   = taomee::bswap(cmdid);
	m_id    = taomee::bswap(uid);
	m_seqno = 0;
	m_ret   = taomee::bswap(result);
}

inline int
CliProto2::send_to(fdsession_t* sess)
{
	int len = m_len + sizeof(CliProto2);
	m_len = taomee::bswap(len);
	if (send_pkg_to_client(sess, this, len) == 0) {
		return 0;
	}

	KERROR_LOG(0, "failed to send pkg to client: uid=%u cmd=%u", taomee::bswap(m_id), taomee::bswap(m_cmd));
	return -1;
}

//------------------------------------------------------------------------
// For CliProto3
//
inline void
CliProto3::init(uint16_t cmdid, userid_t uid, uint32_t result)
{
	m_len   = 0;
	m_cmd   = cmdid;
	m_id    = uid;
	m_seqno = 0;
	m_ret   = result;
}

inline int
CliProto3::send_to(fdsession_t* sess)
{
	int len = m_len + sizeof(CliProto3);
	m_len = len;
	if (send_pkg_to_client(sess, this, len) == 0) {
		return 0;
	}

	KERROR_LOG(0, "failed to send pkg to client: uid=%u cmd=%u", m_id, m_cmd);
	return -1;
}
#endif // LIBTAOMEEPP_CLI_PROTO_HPP_

