/*
 * =====================================================================================
 *
 *       Filename:  dbser_macro.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月07日 10时54分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef db_macro
#define db_macro
#include "db_error_base.h" 
#include "proto_header.h" 
#include <mysql/mysql.h> 
#include <libtaomee/conf_parser/config.h>
#define GET_ROUTE(cmdid) ((cmdid&0x8000)?(70+((cmdid&0x7E00)>>6)+((cmdid &0x00E0)>>5)):(cmdid>>9))

#define set_mysql_string(dst,src,n)	mysql_real_escape_string(  \
		  &(this->db->handle), dst, src, n)

inline uint64_t atoi_safe (char *str) 
{
	return 	(str!= NULL ? atoll(str):0 );
}

inline double atof_safe (char *str) 
{
	return 	(str!= NULL ? atof(str):0 );
}

#define mysql_str_len(n)	( (n) *2 +1)


//正常的db 连接 
#define STD_OPEN_AUTOCOMMIT()	   mysql_autocommit(&(this->db->handle), T)
#define STD_ROLLBACK()  mysql_rollback(&(this->db->handle))
#define STD_CLOSE_AUTOCOMMIT(nret) {\
		if (mysql_autocommit(&(this->db->handle), F)==DB_SUCC){\
				nret = DB_SUCC;\
		}else {\
				nret = DB_ERR;\
		}\
	}
#define STD_COMMIT(nret) {\
		if (mysql_commit(&(this->db->handle))==DB_SUCC){\
				nret = DB_SUCC;\
		}else {\
				nret = DB_ERR;\
		}\
	}




//依次得到row[i]
// 在STD_QUERY_WHILE_BEGIN  和 STD_QUERY_ONE_BEGIN
#define NEXT_FIELD 	 (row[++_fi])

//变长方式copy
#define BIN_CPY_NEXT_FIELD( dst,max_len)  ++_fi; \
		mysql_fetch_lengths(res); \
		res->lengths[_fi]<max_len? \
		memcpy(dst,row[_fi],res->lengths[_fi] ): memcpy(dst,row[_fi],max_len)
//变长方式copy 用于复制
#define BIN_CPY_NEXT_FIELD_TO_MSG(item_name, msg_name)  ++_fi; \
		mysql_fetch_lengths(res); \
		item_name._##msg_name##_len=res->lengths[_fi]<sizeof(item_name.msg_name) ? res->lengths[_fi]:max_len;\
		memcpy(dst,row[_fi],item_name._##msg_name##_len );



//得到int
#define INT_CPY_NEXT_FIELD(value )  (value)=atoi_safe(NEXT_FIELD )

//得到double的值
#define DOUBLE_CPY_NEXT_FIELD(value )  (value)=atof_safe(NEXT_FIELD )



//----------------------列表模式-----------------------------
//如果
#ifdef DB_GET_LIST_NO_MALLOC   /* 没有malloc */
// use dbser_return_buf for list ,set record count to count
#define STD_QUERY_WHILE_BEGIN( sqlstr,pp_list,p_count )  \
    {   MYSQL_RES *res;\
        MYSQL_ROW  row;\
        int list_size;\
        int i;\
        this->db->id=this->id;\
        if (( this->db->exec_query_sql(sqlstr,&res))==DB_SUCC){\
            *p_count=mysql_num_rows(res);\
            *pp_list =(typeof(*pp_list))this->dbser_return_buf;\
            list_size = sizeof(typeof(**pp_list))*(*p_count);\
            if(list_size > PROTO_MAX_SIZE ){\
                DEBUG_LOG("THE SYS_ERR");\
                return SYS_ERR;\
            }\
            memset(*pp_list,0,PROTO_MAX_SIZE );\
            i=0;\
            while((row = mysql_fetch_row(res))){\
                int _fi;\
                _fi=-1;

#define STD_QUERY_WHILE_END()  \
                i++;\
            }\
            mysql_free_result(res); \
            return DB_SUCC;\
        }else {\
            return DB_ERR;\
        }\
    }

#elif defined DB_GET_LIST_NEW

#define STD_QUERY_WHILE_BEGIN( sqlstr,item_list )  \
	{ \
		typeof(item_list)&_item_list=item_list;\
		MYSQL_RES *res;\
		MYSQL_ROW  row;\
        this->db->id=this->id;\
		if (( this->db->exec_query_sql(sqlstr,&res))==DB_SUCC){\
			typeof(item_list[0]) item ;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	

#define STD_QUERY_WHILE_END()  \
			_item_list.push_back(item);\
			}\
			mysql_free_result(res);	\
			return DB_SUCC;\
		}else {\
			return DB_ERR;\
		}\
	}

#else 

// malloc for list ,set record count to count
#define STD_QUERY_WHILE_BEGIN( sqlstr,pp_list,p_count )  \
	{ 	MYSQL_RES *res;\
		MYSQL_ROW  row;\
		int i;\
        this->db->id=this->id;\
		if (( this->db->exec_query_sql(sqlstr,&res))==DB_SUCC){\
			*p_count=mysql_num_rows(res);\
			if ((*pp_list =( typeof(*pp_list))malloc(\
				sizeof(typeof(**pp_list) ) *(*p_count))) ==NULL){\
				return SYS_ERR;\
			}\
			memset(*pp_list,0, sizeof(typeof(**pp_list) ) *(*p_count) );\
			i=0;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	

#define STD_QUERY_WHILE_END()  \
				i++;\
			}\
			mysql_free_result(res);	\
			return DB_SUCC;\
		}else {\
			return DB_ERR;\
		}\
	}


#endif

#define STD_QUERY_WHILE_BEGIN_NEW( sqlstr,item_list ) \
	{ \
		typeof(item_list)&_item_list=item_list;\
		MYSQL_RES *res;\
		MYSQL_ROW  row;\
        this->db->id=this->id;\
		if (( this->db->exec_query_sql(sqlstr,&res))==DB_SUCC){\
			typeof(item_list[0]) item ;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	
#define STD_QUERY_WHILE_END_NEW()  \
            _item_list.push_back(item);\
            }\
            mysql_free_result(res); \
            return DB_SUCC;\
        }else {\
            return DB_ERR;\
        }\
    }


#define STD_QUERY_ONE_BEGIN( sqlstr, no_finderr ) {\
		uint32_t ret;\
		MYSQL_RES *res;\
		MYSQL_ROW row;\
		int rowcount;\
        this->db->id=this->id;\
		ret =this->db->exec_query_sql(sqlstr,&res);\
		if (ret==DB_SUCC){\
			rowcount=mysql_num_rows(res);\
			if (rowcount!=1) { \
	 			mysql_free_result(res);		 \
				DEBUG_LOG("no select a record [%u]",no_finderr );\
				return no_finderr;	 \
			}else { \
				row= mysql_fetch_row(res); \
				int _fi	 ; _fi=-1;
	


#define STD_QUERY_ONE_END()\
				mysql_free_result(res);\
				return DB_SUCC;\
			}\
		}else { \
			return DB_ERR;	 \
		}\
	}

#define STD_QUERY_ONE_END_WITHOUT_RETURN()  \
				mysql_free_result(res);		 \
			} \
		}else { \
			return DB_ERR;	 \
		}\
	}



#include <time.h>
inline  int  mysql_date( char*date_str, time_t t , int len  )
{
	  return strftime(date_str ,len, "%Y/%m/%d %H:%M:%S", localtime(&t));
}
inline int get_date(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}

inline int get_year_month(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*100+tm_tmp.tm_mon+1;
}


inline uint32_t hash_str(const char * key )
{
	register unsigned int h;
	register unsigned char *p; 
	for(h=0, p = (unsigned char *)key; *p ; p++)
		h = 31 * h + *p; 
	return h;
}


inline const char * config_get_strval_with_def(const char* key,const char* def ="" ){
    const char *  v= config_get_strval(key)  ;
    if ( v!=NULL){
        return  v;
    }else{
        return def;
    }
}

inline char * set_space_end(char * src, int len  )
{
    int i=0;
    while (i<len &&  src[i] !='\0' ) i++;

    for (;i<len;i++) src[i]=' ';
    return src ;

}



#endif
