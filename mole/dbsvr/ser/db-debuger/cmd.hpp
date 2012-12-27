/**
 * =====================================================================================
 *
 *	@file  cmd.hpp
 *
 *	@brief  组装一个命令的数据与头
 *
 *	@version:  1.0
 *	Created:  2009年12月24日 20时01分34秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra cmd.hpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef M_dbser_cmd_hpp
#define M_dbser_cmd_hpp

#include<cstring>
#include<string>
#include<vector>

#include <boost/spirit/core.hpp>
#include"proto_header.h"
#include"dll.hpp"

namespace db_debuger
{
typedef char Byte;

class Cmd
{
public:
	typedef std::string DataType;
public:
	Cmd()
	{
		std::memset(&_header, 0, PROTO_HEADER_SIZE);
	}

	Cmd(const uint16_t cid, const userid_t uid)
	{
		std::memset(&_header, 0, PROTO_HEADER_SIZE);
		_header.cmd_id=cid;
		_header.id=uid;
	}

	void cmd_id(const uint16_t cid)
	{
		_header.cmd_id=cid;
	}

	uint16_t cmd_id() const
	{
		return _header.cmd_id;
	}

	void user_id(const userid_t uid)
	{
		_header.id=uid;
	}

	userid_t user_id() const
	{
		return _header.id;
	}

	int32_t result() const
	{
		return _header.result;
	}

	void data(const DataType& d)
	{
		_send_data.insert(_send_data.end(), d.begin(), d.end());
	}

	void data(const Byte* b, const Byte* e)
	{
		_send_data.insert(_send_data.end(), b, e);
	}

	template<typename Type>
	void data(const Type& v)
	{
		_send_data.insert(_send_data.end(), (const char*)(&v), (const char*)(&v) + sizeof(v));
	}

	const DataType& data()
	{
		return _receive_data;
	}

	void clear()
	{
		_send_data.clear();
		_receive_data.clear();
	}

	void send()
	{
		_header.proto_length=PROTO_HEADER_SIZE+_send_data.size();
		const Byte* h=(Byte*)(&_header);
		DataType d(h, h+PROTO_HEADER_SIZE);
		d.insert(d.end(), _send_data.begin(), _send_data.end());
		_dll.data_on(d.data(), d.size());

		//处理返回的数据
		_header=*((PROTO_HEADER*)_dll.receive_buf());
		if(_dll.good())
		{
			_receive_data.assign(_dll.receive_buf()+PROTO_HEADER_SIZE, _dll.receive_buf()+_dll.receive_length());
			_dll.clear();
		}
	}

	bool good()
	{
		return 0==_header.result;
	}

private:
	PROTO_HEADER _header; ///< 协议头
	DataType _send_data; ///< 命令发送私用数据
	DataType _receive_data; ///< 命令接收私用数据

	DllFun _dll;
};


template<typename Type>
struct value_to_buffer
{
value_to_buffer(std::string& buf)
:_buf(buf)
{}

value_to_buffer(std::string* buf)
:_buf(*buf)
{}

void operator()(const Type& v) const
{
	const char* p=(char*)(&v);
	const_cast<std::string&>(_buf).append(p,sizeof(v));
}

private:
	std::string& _buf;
};

namespace rule_space
{

	namespace cmd_value_express
	{
		namespace impl
		{
			using namespace boost::spirit;
			std::string buf;
			rule<> rule_u32=str_p("u32") >> ch_p(':') >> uint_parser<uint32_t>()[value_to_buffer<uint32_t>(buf)];
			rule<> rule_u16=str_p("u16") >> ch_p(':') >> uint_parser<uint16_t>()[value_to_buffer<uint16_t>(buf)];
			rule<> rule_u8=str_p("u8") >> ch_p(':') >> uint_parser<uint8_t>()[value_to_buffer<uint8_t>(buf)];
			rule<> rule_i32=str_p("i32") >> ch_p(':') >> int_parser<int32_t>()[value_to_buffer<int32_t>(buf)];
			rule<> rule_i16=str_p("i16") >> ch_p(':') >> int_parser<int16_t>()[value_to_buffer<int16_t>(buf)];
			rule<> rule_i8=str_p("i8") >> ch_p(':') >> int_parser<int8_t>()[value_to_buffer<int8_t>(buf)];
			rule<> rule_num= rule_u32 | rule_u16 | rule_u8 | rule_i32 | rule_i16 | rule_i8;
			rule<> express = rule_num % ch_p(',');
		}

		struct Agent//系统中不同时存在两个Agent对象
		{
			boost::spirit::rule<>& express() const
			{
				return impl::express;
			}

			const std::string& data() const
			{
				return impl::buf;
			}

			~Agent()
			{
				impl::buf.clear();
			}
		};


	}
}

// 解析一个这样的串 u32:234,u32:123,
bool value_string_parse(std::string& dest_buf, const std::string& value_string)
{
	using boost::spirit::rule;
	using boost::spirit::space_p;
	rule_space::cmd_value_express::Agent agent;
	if(parse(value_string.c_str(), agent.express(), space_p).full)
	{
		dest_buf.append(agent.data());
		return true;
	}

	return false;
}

struct CmdValue
{
	uint16_t cmd_id;
	uint32_t user_id;
	std::string cmd_value;
};

bool cmd_config_string_parse(CmdValue& value, const std::string& cmd_string)
{

}

}

#endif //M_dbser_cmd_hpp

