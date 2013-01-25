/*
 * =====================================================================================
 *
 *       Filename:  Cactivation.cpp
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


#include "Cvip_item_log.h"
#include "proto/mole2_db.h"

Cudp_sender* Cvip_item_log::p_sender = NULL;

Cvip_item_log::Cvip_item_log(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "vip_item_log")
{
	this->p_sender = new Cudp_sender(config_get_strval("LOG_VIP_ITEM_ADDR"));
}

int Cvip_item_log::add(uint32_t logtype,uint32_t itemid, int32_t count)
{
	int ret;
	GEN_SQLSTR(this->sqlstr, "update %s set count=count+%d where logday=%u and logtype=%u and itemid=%u",
			this->get_table_name(), count, today(), logtype, itemid);
	ret = this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
	if(ret == MOLE2_KEY_NOT_EXIST_ERR) {
		GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,%d)",
				this->get_table_name(),today(),logtype,itemid,count);
		ret = this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
	}
	return ret;
}

void Cvip_item_log::log(userid_t userid, uint32_t itemid, int32_t count)
{
	log_vip_item_in log_item;
	if(count < 0 && itemid > 350000 && itemid < 360000)  {
		log_item.logtype = 0x44455355;
		log_item.itemid = itemid;
		log_item.count = count;
		p_sender->send_db_msg(userid, 0xD430, 0, &log_item);
	}
}


