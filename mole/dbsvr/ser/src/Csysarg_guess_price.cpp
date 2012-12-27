/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_guess_price.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/14/2012 10:20:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_guess_price.h"


Csysarg_guess_price::Csysarg_guess_price(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_guess_price")
{

}

int Csysarg_guess_price::insert(userid_t userid, char* nick, uint32_t itemid, uint32_t price)
{
	uint32_t table_price = 0;
	this->get_exist(userid, itemid, price);
	if(table_price < price){
		if(table_price == 0){
			char nick_mysql[mysql_str_len(NICK_LEN)];
			memset(nick_mysql, 0, sizeof(nick_mysql));
			set_mysql_string(nick_mysql,nick, NICK_LEN);

			sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s', %u, %u)",
					this->get_table_name(),
					userid,
					itemid,
					nick_mysql,
					price,
					(uint32_t)get_date(time(0))
					);

			STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
		}
		else{
			int ret = this->update(userid, itemid, price);
			return ret;
		}
	
	}
	return 0;
	
}

int Csysarg_guess_price::update(userid_t userid, uint32_t itemid, uint32_t price)
{
	sprintf(this->sqlstr, "update %s set price = %u where userid = %u and itemid = %u",
			this->get_table_name(),
			price,
			userid,
			itemid
			);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_guess_price::get_records_by_date(userid_t userid, uint32_t date, 
		sysarg_get_guess_price_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select userid, nick, itemid, price, datetime from %s where datetime = %u limit 200",
			this->get_table_name(),
			date
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list +i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->price);
		INT_CPY_NEXT_FIELD((*pp_list + i)->date);
	STD_QUERY_WHILE_END();
}

int Csysarg_guess_price::get_exist(uint32_t userid, uint32_t itemid,  uint32_t &price)
{
	sprintf(this->sqlstr, "select price from %s where userid = %u and itemid = %u",
			this->get_table_name(),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_ID_NOFIND_ERR) ;
		INT_CPY_NEXT_FIELD( price );
	STD_QUERY_ONE_END();
}

int Csysarg_guess_price::get_records_by_itemid( 
		sysarg_get_free_vip_player_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select userid, nick, price from %s where itemid = 0",
			this->get_table_name()
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list +i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->month);
	STD_QUERY_WHILE_END();
}


