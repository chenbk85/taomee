/*
 * =========================================================================
 *
 *        Filename: Citem.h
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:29:51
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  CITEM_H
#define  CITEM_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"


class Citem:public CtableRoute100x10
{
private:
	int	update(userid_t userid,  uint32_t itemid ,uint32_t usecount,uint32_t count );
public:
	int	insert(userid_t userid,  uint32_t itemid ,uint32_t usecount,uint32_t count );
	int	del(userid_t userid,  uint32_t itemid );
	Citem (mysql_interface * db);
	//is_set_use :是否同时设置已经使用的个数
	int	change_count(userid_t userid,  uint32_t itemid ,int value,
			uint32_t max_count ,bool  is_set_use =false);
	int	set_use( userid_t userid,  uint32_t itemid , bool is_use  );
	int	get_list(userid_t userid, std::vector<item_ex_t> &task_list );
	int	get_item(userid_t userid,uint32_t itemid, item_ex_t  * p_out);

	int	set_unused_all( userid_t userid  );

	int	set_used_list( userid_t userid ,std::vector<uint32_t> & used_list , uint32_t used_count);

};

#endif  /*CITEM_H*/
