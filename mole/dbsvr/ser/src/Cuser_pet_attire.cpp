/*
 * =====================================================================================
 *
 *       Filename:  Cuser_pet_attire_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_pet_attire.h"
#include <algorithm>
#include "msglog.h"


using namespace std;
//user_pet
Cuser_pet_attire::Cuser_pet_attire(mysql_interface * db, Citem_change_log *p_log) 
	:CtableRoute100x10( db,"USER" ,"t_user_pet_attire","userid")
{ 

	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;
	
}

int Cuser_pet_attire::get(userid_t userid, uint32_t petid, uint32_t attireid,uint32_t * usedcount,uint32_t * count  )
{
	*usedcount=0; 
	*count=0; 
	sprintf( this->sqlstr, "select usedcount,count from %s \
			where userid=%u and petid=%u and attireid=%u ", 
			this->get_table_name(userid),userid,petid, attireid); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_PET_ATTIRE_ID_NOFIND_ERR) ;
			*usedcount=atoi_safe(NEXT_FIELD ); 
			*count=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_ONE_END();
}

int Cuser_pet_attire::insert(userid_t userid, uint32_t petid , uint32_t attireid, uint32_t usedcount,uint32_t count  )
{
	this->add_attire_msglog(userid, attireid, count);

	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u)", 
		this->get_table_name(userid), userid, petid, attireid,usedcount,count);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_pet_attire::remove(userid_t userid, uint32_t petid, uint32_t attireid)
{
	sprintf( this->sqlstr, "delete from %s  where userid=%u and petid=%u and attireid=%u ", 
			this->get_table_name(userid),userid,petid, attireid);
	STD_SET_RETURN_EX (this->sqlstr, USER_PET_ATTIRE_ID_NOFIND_ERR);
}

int Cuser_pet_attire::remove_pet_attire(userid_t userid, uint32_t petid, uint32_t is_vip_opt_type)
{
	attire_count *items = 0;
	uint32_t count = 0;
	int ret = 0;
	this->get_list_by_attireid_interval(userid, petid, 1200001, 1209999, 2, &count, &items);
	for(uint32_t k = 0; k < count; ++k){
		ret = remove(userid, petid, (items+k)->attireid);
		if(ret == SUCC){
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid,
					(items+k)->attireid, -(items+k)->count);
		}
	}

	if(items != 0){
		free(items);
	}
	return ret;
	//sprintf( this->sqlstr, "delete from %s  where userid=%u and petid=%u ", 
	//this->get_table_name(userid),userid,petid);
	//STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

int  Cuser_pet_attire::update_ex( userid_t userid, uint32_t petid,uint32_t attireid,
				uint32_t  usedcount,uint32_t count, uint32_t is_vip_opt_type)
{
	uint32_t table_count = 0, table_used_cnt = 0;
	int ret = get(userid, petid, attireid, &table_used_cnt, &table_count);
	if (usedcount>count ) return VALUE_OUT_OF_RANGE_ERR;
	if ( count ==0 )		{
		ret = this->remove(userid,petid,attireid);
		if(ret == SUCC){
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid,
					attireid, -table_count);
		}
		return ret;
	}else{//更新数据
		 ret=this->update( userid, petid,attireid,usedcount,count);		
		 if (ret == USER_PET_ATTIRE_ID_NOFIND_ERR){//没有数据,插入
			return this->insert(userid,petid, attireid,usedcount,count);  	
		 }
		 if(ret == SUCC){
			 if(count > table_count){
				this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid,
					attireid, count-table_count);
			 }
		 }//if
		 return ret;
	}
}

int Cuser_pet_attire::update(userid_t userid, uint32_t petid, uint32_t attireid,uint32_t  usedcount,uint32_t  count  )
{
	sprintf( this->sqlstr, 
			"update %s set usedcount = %u,count=%u  \
			where userid=%u and petid=%u and attireid=%u ", 
			this->get_table_name(userid), usedcount,count, userid, petid, attireid );
	STD_SET_RETURN_EX (this->sqlstr, USER_PET_ATTIRE_ID_NOFIND_ERR);
}

int Cuser_pet_attire::update_count( userid_t userid, uint32_t petid,uint32_t attireid,int addcount )
{
	if (addcount>=0)
	{

		sprintf( this->sqlstr, 
			"update %s set count = count + (%d) \
			where userid=%u and  petid=%u and attireid= %u ", 
			this->get_table_name(userid), addcount, userid ,  petid,  attireid);
	}
	else
	{
		//减少
		addcount=-addcount;
		sprintf( this->sqlstr, 
			"update %s set count = count-%d, usedcount=if(count-%d>usedcount,usedcount,count-%d)\
			where userid=%u and  petid=%u and  attireid= %u ", 
			this->get_table_name(userid), addcount,addcount,addcount, userid , petid, attireid);
	}
	STD_SET_RETURN_EX (this->sqlstr, USER_PET_ATTIRE_ID_NOFIND_ERR );
}

int Cuser_pet_attire::update_count_ex( userid_t userid, uint32_t petid,uint32_t attireid,int addcount )
{
	int ret=update_count(userid, petid, attireid, addcount);
	if(SUCC!=ret && addcount>0)
	{
		return insert(userid, petid, attireid, 0, addcount);
	}
	if (addcount > 0)
	{
		this->add_attire_msglog(userid, attireid, addcount);

	}

	return SUCC!=ret ? ret : SUCC;
}

int  Cuser_pet_attire:: get_all(userid_t userid, 
		uint32_t *count, user_get_pet_attire_all_out_item ** list   )	
{
	sprintf( this->sqlstr, 
		"select  petid, attireid,usedcount,count from %s \
			where userid=%u and count>0 ",
		this->get_table_name(userid),userid ); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		(*list+i)->petid=atoi_safe(NEXT_FIELD); 
		(*list+i)->attireid=atoi_safe(NEXT_FIELD); 
		(*list+i)->usedcount=atoi_safe(NEXT_FIELD); 
		(*list+i)->allcount=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int  Cuser_pet_attire:: get_used_list(userid_t userid, 
		uint32_t *count, user_get_pet_attire_used_all_out_item  ** list   )	
{
	sprintf( this->sqlstr, 
		"select  petid, attireid,usedcount from %s \
			where userid=%u and usedcount>0 ",
		this->get_table_name(userid),userid ); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
	 	INT_CPY_NEXT_FIELD((*list+i)->petid);
		INT_CPY_NEXT_FIELD((*list+i)->attireid);
		INT_CPY_NEXT_FIELD((*list+i)->count);
	STD_QUERY_WHILE_END();
}

int Cuser_pet_attire::get_pet_items(userid_t userid, uint32_t attireid_start, uint32_t attireid_end,
	   	uint32_t *count,  attire_count **list)
{
	sprintf(this->sqlstr, "select attireid, sum(count) from %s where userid = %u and attireid >= %u \
			and attireid < %u group by attireid",
			this->get_table_name(userid),
			userid,
			attireid_start,
			attireid_end
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		INT_CPY_NEXT_FIELD( (*list+i)->attireid); 
		INT_CPY_NEXT_FIELD( (*list+i)->count); 
	STD_QUERY_WHILE_END();
}
int  Cuser_pet_attire:: get_list_by_attireid_interval(userid_t userid, uint32_t petid, 
		uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
		uint32_t *count, attire_count ** list   )	
{
	char formatstr[2000] ;

	if (usedflag==0){ //nouse count return
		strcpy(formatstr, "select attireid, count-usedcount as unusedcount \
				from %s where userid=%u and petid=%u and attireid>=%u and attireid<%u \
				having unusedcount>0 order by attireid " );
	}else if (usedflag==1){ //use count return
		strcpy(formatstr,	"select attireid,usedcount from %s \
			where userid=%u  and petid=%u and  attireid>=%u and attireid<%u and usedcount>0 order by attireid ");
	}else if (usedflag==2){ // count return
		strcpy(formatstr, "select attireid,count from %s \
		 	where userid=%u and petid=%u and  attireid>=%u and attireid<%u and count>0 order by attireid ");
	}else {
		return  ENUM_OUT_OF_RANGE_ERR ;
	}
	sprintf( this->sqlstr,formatstr , 
		this->get_table_name(userid),userid, petid, attireid_start, attireid_end); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		INT_CPY_NEXT_FIELD( (*list+i)->attireid); 
		INT_CPY_NEXT_FIELD( (*list+i)->count); 
	STD_QUERY_WHILE_END();
}

int Cuser_pet_attire::update_common(userid_t userid, uint32_t petid,user_set_attire_in *item ,
		 user_set_attire_out *p_out, uint32_t is_vip_opt_type)
{
	uint32_t value;
	int ret;
	bool addflag; //check for insert  or update 
	bool isadd=  item->operatetype &0x01;
	bool isused= item->operatetype &0x02;
	bool isused_all= item->operatetype &0x04;//

	value=item->value;
	if (value<0 || value>VALUE_MAX ) return  VALUE_OUT_OF_RANGE_ERR;
	//得到数据
	ret=this->get(userid,petid,item->attireid,&(p_out->usedcount),&(p_out->count));
	
	//记录不存在,并且是增加未使用	
	addflag= (ret==USER_PET_ATTIRE_ID_NOFIND_ERR) && (isadd && !isused);
	DEBUG_LOG(" attireid opter [%u][%u]flag [%u]value[%u] maxvalue[%u]",
			userid ,item->attireid,item->operatetype,item->value ,item->maxvalue );

	if (ret!=DB_SUCC){//记录不存在
		if(!addflag)//不是增加的
			return ret;//返回错误
	}

	if (isadd  && isused ){
		p_out->usedcount+=value;
	}else if  (isadd  && !isused ){
		p_out->count+=value;

	}else if  (!isadd  && isused ){
		if (p_out->usedcount<value)return VALUE_OUT_OF_RANGE_ERR; 
		p_out->usedcount-=value;
	}else if (!isadd  && !isused ){
		if (p_out->count<value)return VALUE_OUT_OF_RANGE_ERR; 
		p_out->count-=value;						
		if (isused_all){
			if (p_out->usedcount<value)return VALUE_OUT_OF_RANGE_ERR; 
			p_out->usedcount-=value;
		}
	}

	//check value  
	if (p_out->usedcount<0 || p_out->usedcount>VALUE_MAX ) 
		return  VALUE_OUT_OF_RANGE_ERR;
	if(p_out->count > item->maxvalue){
		return LAUM_ATTIRE_COUNT_OUT_RANGE_ERR;
	}
	if (p_out->count<0 ||  p_out->count>VALUE_MAX ) 
		return  VALUE_OUT_OF_RANGE_ERR;

	if (p_out->usedcount>p_out->count) 	return  VALUE_OUT_OF_RANGE_ERR;
	p_out->attireid=item->attireid;

	if (addflag) 
		ret = this->insert(userid,petid,p_out->attireid,p_out->usedcount,p_out->count);
	else
		ret =  this->update(userid,petid,p_out->attireid,p_out->usedcount,p_out->count);
	/*
	 * 114D协议,下面是道具增减统计支持
	 */
	if(ret == SUCC){
        if((isadd && !isused) || (!isadd && !isused)){
			int32_t val = item->value;
            if(!isadd){
                val = -item->value;
				is_vip_opt_type = 0;
            }
            return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_out->attireid, val);
        }
    }
    return ret;


}

int Cuser_pet_attire::set_no_vip(userid_t userid)
{
	//脱下VIP装扮
	sprintf( this->sqlstr, 
			"update %s set usedcount=0 \
			where userid=%u and attireid in( \
			12201, 12247, 12291, 12297, 12313, 12319, 12355, 12382, 12400, 12441,\
			12202, 12273, 12292, 12299, 12314, 12320, 12377, 12383, 12401, 12442,\
			12203, 12280, 12293, 12300, 12315, 12331, 12378, 12384, 12435, 12443,\
			12204, 12286, 12294, 12310, 12316, 12336, 12379, 12385, 12436, 12444,\
			12205, 12287, 12295, 12311, 12317, 12352, 12380, 12398, 12437, 12445,\
			12206, 12288, 12296, 12312, 12318, 12353, 12381, 12399\
				) ", 
			this->get_table_name(userid),
			userid
		   );
	STD_SET_LIST_RETURN(this->sqlstr);
}

int Cuser_pet_attire::get_pet_now_colth(const userid_t userid, const uint32_t petid, uint32_t* count,
		user_pet_attire_set_used_out_item** list, const bool is_colth)
{
	if(is_colth)
	{
		//选出正在穿着的衣服
		sprintf(this->sqlstr, "select attireid from %s where userid = %u and petid = %u and usedcount=1 and attireid >= 1200001 and attireid <= 1209999",
				this->get_table_name(userid),
				userid,
				petid
			);
	} else {
		sprintf(this->sqlstr, "select attireid from %s where userid = %u and petid = %u and usedcount=1 and attireid >= 1210001 and attireid <= 1219999",
				this->get_table_name(userid),
				userid,
				petid
			);
	}

	STD_QUERY_WHILE_BEGIN(this->sqlstr,list, count);
		INT_CPY_NEXT_FIELD((*list+i)->attireid); 
	STD_QUERY_WHILE_END();
}

int Cuser_pet_attire::set_pet_used_attire(const userid_t userid, const uint32_t petid, const uint32_t count,
		const user_pet_attire_set_used_in_item* list, const bool is_colth)
{
	//把当前的衣服脱下
	if(is_colth)
	{
		sprintf(this->sqlstr, "update %s set usedcount=0  where userid = %u and petid = %u and usedcount=1 and attireid >= 1200001 and attireid <= 1209999",
			this->get_table_name(userid),
			userid,
			petid
			);
	} else {
		sprintf(this->sqlstr, "update %s set usedcount=0  where userid = %u and petid = %u and usedcount=1 and attireid >= 1210001 and attireid <= 1219999",
			this->get_table_name(userid),
			userid,
			petid
			);
	
	}
	STD_INSERT_WITHOUT_RETURN(this->sqlstr, SUCC);

	if (count == 0)
	{
		return SUCC;
	}
	if(0==count)//只脱衣服，不穿衣服
	{
		return SUCC;
	}

	//准备sql语句，穿上新衣服
	std::ostringstream sql_in_string;
	for(int i=0; i<(int)count; ++i, ++list)
	{
		sql_in_string << list->attireid;
		if(i < (int)count-1)
		{
			sql_in_string << ',';
		}
	}

	sprintf(this->sqlstr, "update %s set usedcount=1  where userid = %u and petid = %u and attireid in (%s)",
			this->get_table_name(userid),
			userid,
			petid,
			sql_in_string.str().c_str()
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_pet_attire::add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count)
{
	uint32_t log_type = 0;

	if(attireid >= 1200001 && attireid <= 1209999)//拉姆装扮
	{
		log_type = 0x02001000+attireid-1200000;
	}

	if (log_type > 0)
	{
		struct USERID_NUM{
			uint32_t id;
			uint32_t num;
		};

		USERID_NUM s_userid_num = {};
		s_userid_num.id = userid ;
		s_userid_num.num = count;

		msglog(this->msglog_file, log_type, time(NULL), &s_userid_num, sizeof(s_userid_num));

	}
	
	return SUCC;
}







