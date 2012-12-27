/*
 * =====================================================================================
 * 
 *       Filename:  Clogin.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CLOGTABLE_INC
#define  CLOGTABLE_INC
#include "Ctable.h"
#include "CtableDate.h"
#include "logproto.h"
#include "proto.h"
class Clogin :public Ctable {
	public:
		Clogin(mysql_interface * db ):Ctable( db,"LOGDB","t_login"){}
		int insert(char * logtime,TLOGIN* t);

		//tp->usertype must be define
		int getstatisticsdata(char * startdate,char * enddate, uint8_t usertype, TUSERINFO * tp );
		int getonlinetimelevelcount(char * startdate,char * enddate,
				uint8_t usertype, uint32_t start, 
				uint32_t end, uint32_t *count );
};
class Cuser_opt:public CtableDate {
	public:
		Cuser_opt(mysql_interface * db ):CtableDate( db,"LOGDB","t_user_opt","userid"  ){}

		int insert(uint32_t logtime,
		userid_t userid,uint32_t flag ,uint32_t regpost,uint32_t  ip );
};



class Cpetgrade :public Ctable {
	public:
		Cpetgrade(mysql_interface * db ):Ctable( db,"LOGDB","t_petgrade"){}
		int insert(char * logtime);
		int getpetinfor(char * startdate,char * enddate, TPETINFO *tp);
		int get_register_count( char * startdate,char * enddate, uint32_t *count,uint32_t * addcount);
		//某一时间增加的注册数据
		int get_register_addcount( char * startdate,char * enddate,uint8_t usertype ,uint32_t *addcount);
		int get_register_count_with_enddate( char * enddate,uint8_t usertype, uint32_t *count);
				
};

class Cuserinfo :public Ctable {
	public:
		Cuserinfo(mysql_interface * db ):Ctable( db,"LOGDB","t_user_info"){}
		int insert( TUSERINFO* tp);
		int remove(char *logtime, uint8_t usertype );
		int get(char *logtime,uint8_t usertype, TUSERINFO* tp );
};

class Cpetinfo :public Ctable {
	public:
		Cpetinfo(mysql_interface * db ):Ctable( db,"LOGDB","t_pet_info"){}
		int insert( TPETINFO* tp);
		int remove(char *logtime );
};



class Cfruitlevelinfo :public Ctable {
	public:
		Cfruitlevelinfo(mysql_interface * db )
			:Ctable( db,"LOGDB","t_fruit_level_info"){}
		int insert(TUSERINFO* tp);
		int remove(char *logtime, uint8_t usertype );
};

class Cserver:public Ctable {
	public:
		Cserver(mysql_interface * db ):Ctable( db,"LOGDB","t_server"){}
		int insert(char * logtime, char *ip,  uint32_t logcount ) ;
		int check(char * logtime, char * ip , bool * isloged );
};

class Conline_usercount:public Ctable {
	public:
		Conline_usercount(mysql_interface * db ):Ctable( db,"LOGDB","t_online_usercount"){}
		int insert(TONLINE_USERCOUNT *  tonline ) ;
		int ms_all(char* date  );

};
class Cxiaomee_use:public Ctable {
	public:
		Cxiaomee_use (mysql_interface * db ):Ctable( db,"LOGDB","t_xiaomee_use"){}
		int insert(uint32_t date,uint32_t hour, uint32_t usecount  ) ;
		int add_xiaomee (uint32_t date,uint32_t hour, uint32_t usecount  ) ;
};

//相关定义
//每天:活跃用户设置
//type:1003  
//type:1007 
//type:1015
//type:1030 
//
//每天小米进出
//type:2101 (进)
//type:2102 (出)

//渠道 登入 人数
//type:2200+n (0<n<100) n:渠道编号 

//订单数量
//type:3000  






class Cday_count:public Ctable {
	public:
		Cday_count (mysql_interface * db ):Ctable( db,"LOGDB","t_day_count"){}
		int insert(uint32_t type, uint32_t date, uint32_t count  ) ;
		int update(uint32_t type, uint32_t date, uint32_t count  ) ;
};

//相关定义
//时段注册(分钟)  type:0  id1=0..1439  	 id2=0
//xiaomee进出数据:type:2  id1=reason   id2=reason_ex
//注册地址类型:   type:3  id1=reg_addr_type  id2=0
//每天等级分布:   type:4  id1=等级(1..101) 		id2=0

class Cday_count_ex:public Ctable {
	public:
		Cday_count_ex (mysql_interface * db ):Ctable( db,"LOGDB","t_day_count_ex"){}
		int insert(uint32_t type, uint32_t date,uint32_t id1,uint32_t id2 , uint32_t count  ) ;
		int update(uint32_t type, uint32_t date,uint32_t id1,uint32_t id2 , uint32_t count  ) ;
		int add(uint32_t type, uint32_t date,uint32_t id1,uint32_t id2 , uint32_t count  ) ;
		int add_or_insert(uint32_t type,uint32_t date,uint32_t id1,uint32_t id2,uint32_t count);
};

#endif   /* ----- #ifndef CLOGTABLE_INC  ----- */

