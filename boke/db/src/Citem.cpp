/*
 * =========================================================================
 *
 *        Filename: Citem.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "Citem.h"
#include <time.h>
Citem::Citem(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_item" , "userid", "itemid")
{

}

int	Citem::insert(userid_t userid,  uint32_t itemid ,uint32_t usecount,uint32_t count )
{
	sprintf( this->sqlstr, "insert into %s values( %u,%u, %u,%u )",
		this->get_table_name(userid), userid,itemid,usecount,count);
	return this->exec_insert_sql( this->sqlstr, ITEM_IS_EXISTED_ERR );
}

int	Citem::get_list(userid_t userid, std::vector<item_ex_t> &task_list )
{
    GEN_SQLSTR(this->sqlstr, "select itemid,usecount ,count from %s where userid=%u ",
            this->get_table_name(userid),userid);
    STD_QUERY_WHILE_BEGIN(this->sqlstr,task_list);
        INT_CPY_NEXT_FIELD(item.itemid);
        INT_CPY_NEXT_FIELD(item.use_count);
        INT_CPY_NEXT_FIELD(item.count);
    STD_QUERY_WHILE_END();
}

int	Citem::change_count(userid_t userid,  uint32_t itemid ,int value ,uint32_t max_count,bool  is_set_use  )
{
	if (value==0){
	   	return  ENUM_OUT_OF_RANGE_ERR;
	}

	item_ex_t item;
	bool insert_flag=false;
	int ret=this->get_item(userid,itemid, &item );
	if(ret==ITEM_NOFIND_ERR ){
		insert_flag=true;
		item.itemid=itemid;
		item.use_count=0;
		item.count=0;
	}else if (ret!=SUCC ){
		return ret;
	}

	if( value>0 ){//增加
		item.count+=value;		
	}else{//减少
		if(int(item.count)+value >=0 ){
			item.count+=value;		
		}else{
			return  ITEM_NOENOUGH_ERR;
		}
	}
	if (item.count>max_count){
		return  ITEM_MAX_ERR;
	}

	//调整使用中的个数
	if (item.use_count>item.count ){
		item.use_count=item.count;
	}

	if (is_set_use){
		item.use_count=1;
	}
	

	if (insert_flag ){
		return this->insert(userid,item.itemid,item.use_count,item.count );
	}else{
		return this->update(userid,item.itemid,item.use_count,item.count  );
	}

}


int	Citem::get_item(userid_t userid,uint32_t itemid, item_ex_t  * p_out)
{
	GEN_SQLSTR( this->sqlstr, "select usecount,count  from %s where userid=%u and itemid=%u", 
				this->get_table_name(userid),userid , itemid );
	p_out->itemid=itemid;
	STD_QUERY_ONE_BEGIN(this->sqlstr , ITEM_NOFIND_ERR  );
		INT_CPY_NEXT_FIELD(p_out->use_count);
		INT_CPY_NEXT_FIELD(p_out->count);
	STD_QUERY_ONE_END();
}

int	Citem::update(userid_t userid,  uint32_t itemid ,uint32_t usecount,uint32_t count )
{
	if ( count==0 ){
		return this->del(userid,itemid);
	}

	sprintf( this->sqlstr, "update %s set usecount= %u,count=%u where userid=%u and itemid=%u ",
		this->get_table_name(userid), usecount,count, userid,itemid);
	return this->exec_update_sql( this->sqlstr, ITEM_NOFIND_ERR );
}

int	Citem::set_use( userid_t userid,  uint32_t itemid ,bool is_use )
{	
	uint32_t usecount=is_use?1:0;
	sprintf( this->sqlstr, "update %s set usecount=%u where userid=%u and itemid=%u ",
		this->get_table_name(userid), usecount, userid,itemid);
	return this->exec_update_sql( this->sqlstr, ITEM_NOFIND_ERR );
}

int	Citem::del(userid_t userid,  uint32_t itemid )
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and itemid=%u ",
		this->get_table_name(userid),  userid,itemid);
	return this->exec_delete_sql( this->sqlstr, ITEM_NOFIND_ERR );
}


int	Citem::set_unused_all( userid_t userid  )
{
	sprintf( this->sqlstr, "update %s set usecount=0 where userid=%u ",
		this->get_table_name(userid),  userid);
	return this->exec_update_list_sql( this->sqlstr,SUCC );
}


#include  <sstream>
int	Citem::set_used_list( userid_t userid ,std::vector<uint32_t> & used_list , uint32_t used_count)
{
	if (used_list.size()==0 ) {
		return SUCC;
	}

	std::stringstream item_list_stream ;
	for (uint32_t i=0;i<used_list.size();i++ ){
		item_list_stream << used_list[i];
		if (i< used_list.size()-1){
			item_list_stream << ",";
		}
	}
	
	sprintf( this->sqlstr, "update %s set usecount=%u \
			where userid=%u  and  itemid in (%s)",
		this->get_table_name(userid),  used_count,userid,
			item_list_stream.str().c_str() );
	return this->exec_update_list_sql( this->sqlstr,SUCC );
}

