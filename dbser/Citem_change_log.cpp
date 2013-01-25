/*
 * =====================================================================================
 *
 *       Filename:  Cpresent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *  common.h
 * =====================================================================================
 */

#include "Citem_change_log.h"
#include "db_error_base.h" 
#include "db_macro.h" 

Citem_change_log::Citem_change_log(mysql_interface * db,const char * db_name, const char * table_name	)
	:CtableRoute(db , db_name , table_name,"" )
{

}


int Citem_change_log::insert(uint32_t logtime, uint32_t opt_type, uint32_t userid,
			uint32_t  itemid, uint32_t count ) 
{
   sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%d )",
        this->get_table_name(userid,logtime ),
		logtime, 
		opt_type,
        userid,
        itemid,
        count
        );
    return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);

}

int Citem_change_log::add(uint32_t logtime, item_change_log_opt_type opt_type, uint32_t userid,
			uint32_t  itemid, uint32_t count ) 
{
	if   (opt_type >2 ) return ENUM_OUT_OF_RANGE_ERR;
     int ret;
     ret=this->insert(logtime,(uint32_t)opt_type,userid,itemid,count );
     if ( ret!=SUCC ) {
        this->create_table(userid,logtime);
     	ret=this->insert(logtime,(uint32_t)opt_type,userid,itemid,count );
     }
    return ret;


}
char *  Citem_change_log::get_table_name(uint32_t userid ,uint32_t logtime)
{

	this->id=id;
	sprintf (this->db_name,"%s_%02d", this->db_name_pre,userid%100 );
	this->db->select_db(this->db_name);


	sprintf (this->db_table_name,"%s.%s_%8.8d",
		this->db_name, 
		this->table_name_pre,
		get_date(logtime) 
		);
	return this->db_table_name;
}

int Citem_change_log::create_table( uint32_t userid,uint32_t logtime)
{
    sprintf( this->sqlstr, "create table if not exists %s (\
	  `logtime` int(10) unsigned NOT NULL,\
	  `opttype` int(2) unsigned NOT NULL default '0',\
	  `userid` int(10) unsigned NOT NULL,\
	  `itemid` int(10) unsigned NOT NULL default '0',\
	  `count` int(10)  NOT NULL default '0'\
        )ENGINE=innodb",
        this->get_table_name(userid ,logtime ));
    return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Citem_change_log::add(uint32_t logtime, bool is_vip_opt, uint32_t userid,
			uint32_t  itemid, int count ) 
{
    enum item_change_log_opt_type opt_type;
        
    if (is_vip_opt && count>0  ){//vip 增加
        opt_type=item_change_log_opt_type_vip_add;
    }else if  ( !is_vip_opt && count>0  ){//正常增加
        opt_type=item_change_log_opt_type_add;
    }else if  ( !is_vip_opt && count<=0  ){//正常减少
        opt_type=item_change_log_opt_type_del;
        count=-count;
    }else{
		DEBUG_LOG("Citem_change_log::add ERROR:is_vip_opt==true,count>0:=%d ",count );
	}
    return this->add(logtime ,opt_type,userid, itemid,(uint32_t)count );
}
