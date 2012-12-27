/**
 * =====================================================================================
 *
 *	@file  db_utility.hpp
 *
 *	@brief  数据库实用工具库
 *
 *	@version:  1.0
 *	Created:  2010年01月04日 18时03分54秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra db_utility.hpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef M_serlib_db_utility_hpp
#define M_serlib_db_utility_hpp

#include<ctime>
#include<string>
#include<sstream>
#include<vector>

/**
 * @namespace db_utility 
 * @brief 这里提供所有DB程序都可以使用关键性便利函数
 */
namespace db_utility
{
	/**
	 * @brief 计算当前时间与指定时间之间相差的天数
	 * @param const time_t& t 指定时间
	 * @return 指定时间与当前时间早的天数，可以是负数，说明指定时间在当前时间之后
	 */
	inline int before_now_by_day(const time_t& t)
	{
		const size_t second_by_day=24 * 60 * 60;
		const size_t now_day=std::time(0)/second_by_day + 1;
		const size_t t_day=t/second_by_day + 1;

		return now_day - t_day;
	}

	/**
	 * @brief 判断指定时间是不是今天
	 * @param cosnt time_t& t 指定时间
	 * @return true：是今天，false：不是今天
	 */
	inline bool is_today(const time_t& t)
	{
		return 0==before_now_by_day(t);
	}

	/**
	 * @brief 把一个类型转换为字符串
	 * @param const Type& v 指定类型的值
	 * @return 一个静态分配的内存空间
	 */
	template<typename Type>
	inline const std::string to_string(const Type& v)
	{
		std::ostringstream ss;
		ss << v;
		return ss.str();
	}

	/**
	 * @brief 出于接口，提供一个重载
	 * @return 一个静态分配的内存空间
	 */
	inline const std::string to_string(const std::string& v)
	{
		return v;
	}

	/**
	 * @brief 把一个类型转换为字符串
	 * @param const Type& v 指定类型的值
	 * @return 是否成功
	 */
	template<typename Type>
	inline bool string_to(const std::string& str, Type& v)
	{
		std::istringstream ss;
		ss.str(str);

		ss >> v;

		return ss;
	}

	/**
	 * @brief 出于接口，提供一个重载
	 * @return 是否成功
	 */
	inline bool string_to(const std::string& str, std::string& v)
	{
		v=str;
		return true;
	}

	class packet_t
	{
	public:
		packet_t()
		{
			_data.reserve(1024);
		}

		void data(const std::string& d)
		{
			_data.append(d.begin(), d.end());
		}

		void data(const char* b, const char* e)
		{
			_data.append(b, e);
		}

		void data(const char* b, size_t n)
		{
			_data.append(b, n);
		}

		template<typename Type>
		void data(const Type& v)
		{
			_data.append((const char*)(&v), sizeof(v));
		}

		const char* data()
		{
			return _data.data();
		}

		size_t size()
		{
			return _data.size();
		}

		void clear()
		{
			_data.clear();
		}

	private:
		std::string _data;
	};

}

/**
 * @brief 用于定义一个值类型的宏
 * @param type_name 需要定义的类型名
 * @param filed_name 指定这个值类型在数据库中对应的字段名
 * @param value_type 指定内部存储字段值的类型，目前可以是任意内置类型
 */
#define DB_MACRO_define_value_type(type_name, filed_name, value_type)\
	struct type_name\
	{\
		static const std::string& name()\
		{\
			static std::string n(#filed_name);\
			return n;\
		}\
\
		value_type& value()\
		{\
			return _value;\
		}\
\
		value_type value() const\
		{\
			return _value;\
		}\
\
		type_name& value(const value_type& v)\
		{\
			_value=v;\
			return *this;\
		}\
\
		type_name& value(const char* s)\
		{\
			db_utility::string_to(s, _value);\
			return *this;\
		}\
\
	private:\
		value_type _value;\
	};

/**
 * @brief 用于定义一个值类型的宏，它使用一个缓冲区，而不是一个特定的类型
 * @param type_name 需要定义的类型名
 * @param filed_name 指定这个值类型在数据库中对应的字段名
 * @param buf_size 指定使用的缓冲区的大小
 */
#define DB_MACRO_define_buf_value_type(type_name, filed_name, buf_size)\
	struct type_name\
	{\
		static const std::string& name()\
		{\
			static std::string n(#filed_name);\
			return n;\
		}\
\
		std::string& value()\
		{\
			return _value;\
		}\
\
		std::string value() const\
		{\
			return _value;\
		}\
\
		type_name& value(const std::string& v)\
		{\
			_value=v;\
			return *this;\
		}\
\
		type_name& value(const char* s)\
		{\
			_value.assign(s, buf_size);\
			return *this;\
		}\
\
		enum\
		{\
			nbyte=buf_size,\
		};\
\
	private:\
		std::string _value;\
	};

//**************************************************************************************************
//by_userid

/**
 * @brief 定义一个函数模板，它接受 \b 1 个值类型的参数，并把相应数据库的字段更新为指定值
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * 			宏的名字中存在一个“1”，表示宏定义的函数模板接受一个值参数
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 * @see DB_MACRO_define_template_update_2_by_userid
 */
#define DB_MACRO_define_template_update_1_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_update_by_userid(const FiledType& v)\
	{\
		sprintf(this->sqlstr, "update %s set"\
			" %s=%s "\
			" where "#id_in_sql"=%u",\
			this->get_table_name(id_in_cpp), \
			v.name().c_str(),\
			db_utility::to_string(v.value()).c_str(),\
			id_in_cpp\
		);\
\
		STD_SET_RETURN_EX(this->sqlstr,err_code);\
	}\

/**
 * @brief 定义一个函数模板，它接受 \b 2 个值类型的参数，并把相应数据库的字段更新为指定值
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 * @see DB_MACRO_define_template_update_1_by_userid
 */
#define DB_MACRO_define_template_update_2_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType_1, typename FiledType_2>\
	int template_update_by_userid(const FiledType_1& v1, const FiledType_2& v2)\
	{\
		sprintf(this->sqlstr, "update %s set "\
			"%s=%s, "\
			"%s=%s "\
			" where "#id_in_sql"=%u",\
			this->get_table_name(id_in_cpp), \
			v1.name().c_str(),\
			db_utility::to_string(v1.value()).c_str(),\
			v2.name().c_str(),\
			db_utility::to_string(v2.value()).c_str(),\
			id_in_cpp\
		);\
\
		STD_SET_RETURN_EX(this->sqlstr,err_code);\
	}

/**
 * @brief 定义一个函数模板，它接受 \b 1 个值类型引用，把数据库中相应的字段取出放于其中
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 * @see DB_MACRO_define_template_select_2_by_userid
 * @see DB_MACRO_define_template_select_3_by_userid
 */
#define DB_MACRO_define_template_select_1_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_select_by_userid(FiledType& v)\
	{\
		sprintf(this->sqlstr, "select %s from %s "\
				" where "#id_in_sql"=%u", \
				v.name().c_str(),\
				this->get_table_name(id_in_cpp),\
				id_in_cpp\
			); \
\
		STD_QUERY_ONE_BEGIN(this->sqlstr,err_code);\
				v.value(NEXT_FIELD);\
		STD_QUERY_ONE_END();\
	}

/**
 * @brief 定义一个函数模板，它接受 \b 2 个值类型引用，把数据库中相应的字段取出放于其中
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 * @see DB_MACRO_define_template_select_1_by_userid
 * @see DB_MACRO_define_template_select_3_by_userid
 */
#define DB_MACRO_define_template_select_2_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType_1, typename FiledType_2>\
	int template_select_by_userid(FiledType_1& v1, FiledType_2& v2)\
	{\
		sprintf(this->sqlstr, "select %s, %s from %s "\
				" where "#id_in_sql"=%u", \
				v1.name().c_str(),\
				v2.name().c_str(),\
				this->get_table_name(id_in_cpp),\
				id_in_cpp\
			); \
\
		STD_QUERY_ONE_BEGIN(this->sqlstr,err_code);\
				v1.value(NEXT_FIELD);\
				v2.value(NEXT_FIELD);\
		STD_QUERY_ONE_END();\
	}

/**
 * @brief 定义一个函数模板，它接受 \b 3 个值类型引用，把数据库中相应的字段取出放于其中
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 * @see DB_MACRO_define_template_select_1_by_userid
 * @see DB_MACRO_define_template_select_2_by_userid
 */
#define DB_MACRO_define_template_select_3_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType_1, typename FiledType_2, typename FiledType_3>\
	int template_select_by_userid(FiledType_1& v1, FiledType_2& v2, FiledType_3& v3)\
	{\
		sprintf(this->sqlstr, "select %s, %s, %s from %s "\
				" where "#id_in_sql"=%u", \
				v1.name().c_str(),\
				v2.name().c_str(),\
				v3.name().c_str(),\
				this->get_table_name(id_in_cpp),\
				id_in_cpp\
			); \
\
		STD_QUERY_ONE_BEGIN(this->sqlstr,err_code);\
				v1.value(NEXT_FIELD);\
				v2.value(NEXT_FIELD);\
				v3.value(NEXT_FIELD);\
		STD_QUERY_ONE_END();\
	}

//**************************************************************************************************
//select_list
/**
 * @brief 定义一个函数模板，它接受 \b 1 个值类型引用，把数据库中相应的字段取出放于其中，这里不假设记录只有一条
 * @details 这个宏使用一个ID来确定更新表中的哪一行，并且考虑到表中的字段名字与在C++的名字不统一
 * @param id_in_sql ID在SQL语句的名字
 * @param id_in_cpp ID在C++的中名字，此处实际上一个变量名，我们假设这个变量已经有合适的值
 * @param err_code 如果出现某种错误，期待函数模板返回一个错误码
 */
#define DB_MACRO_define_template_select_list_1_by_userid(id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_select_list_by_userid(std::vector<FiledType>& vect)\
	{\
		FiledType  v;\
		sprintf(this->sqlstr, "select %s from %s "\
			" where "#id_in_sql"=%u", \
			v.name().c_str(),\
			this->get_table_name(id_in_cpp),\
			id_in_cpp\
		); \
\
\
		MYSQL_RES *res;\
		MYSQL_ROW  row;\
		if((this->db->exec_query_sql(sqlstr,&res))==DB_SUCC)\
		{\
			while((row=mysql_fetch_row(res)))\
			{\
				v.value(row[0]);\
				vect.push_back(v);\
			}\
\
			mysql_free_result(res);	\
			return DB_SUCC;\
		} else {\
			return DB_ERR;\
		}\
	}

//**************************************************************************************************
//by_userid_id

#define DB_MACRO_define_template_update_1_by_userid_id(userid_in_sql, userid_in_cpp, id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_update_by_userid_id(const FiledType& v)\
	{\
		sprintf(this->sqlstr, "update %s set"\
			" %s=%s "\
			" where "#userid_in_sql"=%u "\
			" and "#id_in_sql"=%u",\
			this->get_table_name(userid_in_cpp), \
			v.name().c_str(),\
			db_utility::to_string(v.value()).c_str(),\
			userid_in_cpp,\
			id_in_cpp\
		);\
\
		STD_SET_RETURN_EX(this->sqlstr,err_code);\
	}

#define DB_MACRO_define_template_buf_update_1_by_userid_id(userid_in_sql, userid_in_cpp, id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_buf_update_by_userid_id(const FiledType& v)\
	{\
		const std::string& s=db_utility::to_string(v.value());\
\
		size_t len=mysql_str_len(s.size());\
		char mysql_list[len];\
		set_mysql_string(mysql_list, s.data(), s.size());\
\
		sprintf(this->sqlstr, "update %s set"\
			" %s='%s' "\
			" where "#userid_in_sql"=%u "\
			" and "#id_in_sql"=%u",\
			this->get_table_name(userid_in_cpp), \
			v.name().c_str(),\
			mysql_list,\
			userid_in_cpp,\
			id_in_cpp\
		);\
\
		STD_SET_RETURN_EX(this->sqlstr,err_code);\
	}

#define DB_MACRO_define_template_select_1_by_userid_id(userid_in_sql, userid_in_cpp, id_in_sql, id_in_cpp, err_code)\
	template<typename FiledType>\
	int template_select_by_userid_id(FiledType& v)\
	{\
		sprintf(this->sqlstr, "select %s from %s "\
				" where "#userid_in_sql"=%u " \
				" and "#id_in_sql"=%u", \
				v.name().c_str(),\
				this->get_table_name(userid_in_cpp),\
				userid_in_cpp,\
				id_in_cpp\
			); \
\
		STD_QUERY_ONE_BEGIN(this->sqlstr,err_code);\
				v.value(NEXT_FIELD);\
		STD_QUERY_ONE_END();\
	}

#define DB_MACRO_define_template_insert_by_userid_id(userid_in_sql, userid_in_cpp, id_in_sql, id_in_cpp, err_code)\
	int template_insert_by_userid_id()\
	{\
		sprintf(this->sqlstr,\
			"insert into %s("#userid_in_sql", "#id_in_sql")"\
			" values(%u, %u)", \
			this->get_table_name(userid_in_cpp),\
			userid_in_cpp,\
			id_in_cpp\
		);\
\
		STD_SET_RETURN_EX(this->sqlstr,err_code);\
	}

#endif //M_serlib_db_utility_hpp

