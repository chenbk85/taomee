/*
 * =====================================================================================
 *
 *       Filename:  Cuser_attire.cpp
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

#include "Cuser_attire.h"
#include <algorithm>
#include "msglog.h"

using namespace std;

#define MODE_BASE  160549

/**
 * @brief 这个列表内的物品都需要加入图鉴中，百宝集图鉴中
 */
static uint32_t global_pic_book_attire_id_list[]=
{
	190022,
	190027,
	190028,
	190141,
	190142,
	190166,
	190167,
	190186,
	190196,
	190201,
	190202,
	190203,
	190216,
	190228,
	190230,
	190238,
	190239,
	190241,
	190244,
	190245,
	190247,
	190249,
	190250,
	190252,
	190254,
	190256,
	190257,
	190259,
	190260,
	190261,
	190263,
	190265,
	190267,
	190350,
	190351,
	190352,
	190379,
	190380,
	190388,
	190418,
	190419,
	190425,
	190437,
	190442,
	190458,
	190269,
	190270,
	190271,
	190425,
	190519,
	190520,
	190521,
	190430,
	190522,
	190523,
	190524,
	190525,
	190526,
	190432,
	190284,
	190285,
	190575,
	190576,
	190577,
	190289,
	190287,
	190614,
	190292,
	190291,
	190293,
	190638,
	190300,
	190297,
	190298,
	190302,
	190304,
	190653,
	190654,
	190660,
	190661,
	190662,
	190306,
	190025,
	190667,
	190308,
	190395,
	190310,
	190670,
	190683,
	190312,
	190314,
	190315,
	190318,
	190320,
	190321,
	190324,
	190327,
	190328,
	190330,
	190332,
	190333,
	190334,
	190335,
	190744,
	190747,
	190807,
	190337,
	190338,
	190340,
	190810,
	190811,
	190814,
	190815,
	190816,
	190817,
	190818,
	190819,
	190812,
	190821,
	190823,
	190824,
	190825,
	190826,
	190827,
	190828,
	190829,
	190830,
	190831,
	190832,
	190833,
	190834,
	190835,
	190836,
	190342,
	190343,
	190431,
	190433,
	190903,
	190904,
	190905,
	190916,
	190919
};

bool attire_id_in_pic_list(const uint32_t id)
{
	uint32_t n=sizeof(global_pic_book_attire_id_list)/sizeof(global_pic_book_attire_id_list[0]);
	uint32_t* b=global_pic_book_attire_id_list;
	uint32_t* e=global_pic_book_attire_id_list + n;

	return e != std::find(b, e, id);
}

//user_pet
Cuser_attire::Cuser_attire(mysql_interface * db, Citem_change_log * p_log ) 
	:CtableRoute( db,"USER" ,"t_user_attire","userid")
{ 
	p_item_change_log = p_log;	
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_attire:: get(userid_t userid, uint32_t attireid,uint32_t *usedcount,uint32_t *count,
		               uint32_t *p_chest, uint32_t *p_mode)
{

	*usedcount=0; 
	*count=0; 
	sprintf( this->sqlstr, "select usedcount,count, chest, mode from %s \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),userid, attireid); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, ATTIRE_COUNT_NO_ENOUGH_ERR) ;
			*usedcount=atoi_safe(NEXT_FIELD ); 
			*count=atoi_safe(NEXT_FIELD ); 
			*p_chest=atoi_safe(NEXT_FIELD ); 
			*p_mode=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_ONE_END();
}

int Cuser_attire::get_count(userid_t userid, uint32_t attireid,uint32_t &count)
{
	count=0; 
	sprintf(this->sqlstr, "select count from %s \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),userid, attireid); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, ATTIRE_COUNT_NO_ENOUGH_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_attire::get_used_count(userid_t userid, uint32_t attireid,uint32_t &usedcount)
{
	usedcount=0; 
	sprintf(this->sqlstr, "select usedcount from %s \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),userid, attireid); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, ATTIRE_COUNT_NO_ENOUGH_ERR) ;
		usedcount=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_attire::add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count)
{
	uint32_t log_type = 0;
	if (attireid >= 12001 && attireid <=15999)//服饰
	{
		log_type = 0x02010000+attireid-12000;

	}
	else if(attireid >= 16001 && attireid <= 16999)//变色道具
	{
		log_type = 0x0200a000+attireid-16000;
	}
	else if(attireid >= 150001 && attireid <= 159999)//小屋道具
	{
		log_type = 0x02007000+attireid-150000;
	}
	else if(attireid >= 160001 && attireid <= 169999)//小屋道具
	{
		log_type = 0x02011000+attireid-160000;
	}
	else if(attireid >= 180001 && attireid <= 189999)//喂养道具
	{
		log_type = 0x02004000+attireid-180000;
	}
	else if(attireid >= 190001 && attireid <= 199999)//收集品
	{
		log_type = 0x02014000+attireid-190000;
	}
	else if(attireid >= 1200001 && attireid <= 1209999)//拉姆装扮
	{
		log_type = 0x02001000+attireid-1200000;
	}
	else if(attireid >= 1220001 && attireid <= 1229999)//家园装扮
	{
		log_type = 0x02017000+attireid-1220000;
	}
	else if(attireid >= 1230001 && attireid <= 1239999)//种子
	{
		log_type = 0x0201a000+attireid-1230000;
	}
	else if(attireid >= 1270001 && attireid <= 1279999)//动物
	{
		log_type = 0x0201d000+attireid-1270000;
	}
	else if(attireid >= 1300001 && attireid <= 1300999)//交通工具
	{
		log_type = 0x02000000+attireid-1300000;
	}
	else if(attireid >= 1353000  && attireid <= 1353400 )//天使种子,道具
	{
		log_type = 0x04043401 + attireid -  1353000;
	}
	else if(attireid >= 1453000  && attireid <= 1552999 )//地下城物品
	{
		log_type = 0x0406FC84 + attireid -  1453000;
	}
	else if(attireid >= 1623000  && attireid <= 1632999 )//海底世界物品
	{
		log_type = 0x0409E5F0 + attireid -  1623000;
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


int Cuser_attire::insert(userid_t userid , uint32_t attireid, uint32_t usedcount,uint32_t count  )
{
	sprintf( this->sqlstr, "insert into %s values (%u, %u, %u, 0, 0, 0, %u)", 
		this->get_table_name(userid), userid, attireid,usedcount,count);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_attire::remove(userid_t userid, uint32_t attireid)
{
	sprintf( this->sqlstr, "delete from %s  where userid=%u and attireid=%u ", 
			this->get_table_name(userid),userid, attireid);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int  Cuser_attire::update_ex( userid_t userid,uint32_t attireid,
				uint32_t  usedcount,uint32_t chestcount ,uint32_t count,
				uint32_t is_vip_opt_type)
{
	uint32_t table_cnt = 0;
	int ret = this->get_count(userid, attireid, table_cnt);
	if (count==0){
		ret = this->remove(userid, attireid);  	
		if(ret == SUCC){
			if(table_cnt != 0){
				if(attireid == 190890){
					DEBUG_LOG("delete== itemid: %u, count: %u", attireid, table_cnt);
				}
				ret = this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, -table_cnt);	
			}
		}
	}else{
		this->insert(userid, attireid,usedcount,count);  	
		this->add_attire_msglog(userid,attireid, count);
		ret = this->update_ex_db( userid, attireid,usedcount,chestcount, count);  	
		if(ret == SUCC){
			if(count > table_cnt){
				if(attireid == 190890){
					DEBUG_LOG("add== itemid: %u, count: %u", attireid, count-table_cnt);
				}	
				ret = this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, count-table_cnt);	
			}
			else if(count < table_cnt){
				if(attireid == 190890){
					DEBUG_LOG("delete== itemid: %u, count: %u", attireid, table_cnt-count);
				}
				ret = this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, -(table_cnt-count));	
			}
		}//if(ret==SUCC)
	}

	return ret;
	
}

int Cuser_attire::update_ex_db(userid_t userid, uint32_t attireid,uint32_t  usedcount,
	 	uint32_t chestcount , uint32_t  count  )
{
	sprintf( this->sqlstr, 
			"update %s set usedcount = %u, chest=%u, count=%u  \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),usedcount,
			chestcount, count, userid,attireid );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_attire::update(userid_t userid, uint32_t attireid,uint32_t  usedcount,uint32_t  count  )
{
	sprintf( this->sqlstr, 
			"update %s set usedcount = %u,count=%u  \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),usedcount,count, userid,attireid );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_attire::update_used_count_inc(userid_t userid, uint32_t attireid, uint32_t count)
{
	sprintf( this->sqlstr, 
			"update %s set usedcount = usedcount + %u where userid=%u and attireid=%u ", 
			this->get_table_name(userid), count, userid,attireid );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_attire::update_used_count(userid_t userid, uint32_t attireid, uint32_t usedcount)
{
	sprintf( this->sqlstr, 
			"update %s set usedcount = %u where userid=%u and attireid=%u ", 
			this->get_table_name(userid), usedcount, userid,attireid );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_attire::update_count( userid_t userid,uint32_t attireid,int addcount)
{
	if (addcount>=0){
		this->add_attire_msglog(userid,attireid, addcount);

		sprintf( this->sqlstr, 
			"update %s set count = count + (%d) \
			where userid=%u and attireid= %u ", 
			this->get_table_name(userid), addcount, userid , attireid);
	}else{
		//减少
		addcount=-addcount;
		sprintf( this->sqlstr, 
			"update %s set count = count-%d, usedcount=if(count-%d-chest-mode>usedcount,usedcount,count-%d-chest-mode)\
			where userid=%u and attireid= %u ", 
			this->get_table_name(userid), addcount,addcount,addcount, userid , attireid);
	}

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cuser_attire::set_no_vip( userid_t userid )
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
			this->get_table_name(userid), userid );
	STD_SET_LIST_RETURN(this->sqlstr);
}

int Cuser_attire::check_list( userid_t userid,
				uint32_t oldcount,attire_count* p_oldlist,
				uint32_t newcount,attire_count_with_max * p_newlist, 
				user_change_user_value_in *p_uci)
{
	int ret;
	uint32_t db_count;
	uint32_t i;
	attire_count * p_dblist;
	attire_count * find_index, *idstart, *idend;	
	pair< attire_count *  , attire_count * > range;

	ret=this->get_all_list_by_attireid_interval(userid,0,
			0xFFFFFFFF,2,&db_count,&p_dblist  );	
	if (ret!=SUCC) return ret;

	//二分查找

	//check DB数据中是否有足够个数
	idstart=p_dblist;
	idend=idstart+db_count;	
		
	for (i=0;i<oldcount;i++ ){
		//0，代表摩尔豆，1代表经验值，2 力量值，3 智力值，4 魅力值
		switch (p_oldlist[i].attireid  ){
			case 0:p_uci->addxiaomee-=p_oldlist[i].count;
				break;
			case 1:p_uci->addexp-=p_oldlist[i].count;
				break;
			case 2:p_uci->addstrong-=p_oldlist[i].count;
				break;
			case 3:p_uci->addiq-=p_oldlist[i].count;
				break;
			case 4:p_uci->addcharm-=p_oldlist[i].count;
				break;
			default : //交换成物品
				range=equal_range(idstart,idend,p_oldlist[i]);
				find_index=range.first;		
		
				if (find_index==idend || 
						find_index->attireid!=p_oldlist[i].attireid) {//没有找到
					free(p_dblist);
					return  USER_ATTIRE_ID_NOFIND_ERR;
				}
		
				if (p_oldlist[i].count>find_index->count) {//数目不足
					free(p_dblist);
					return USER_SWAP_ATTIRE_NOENOUGH_ERR; 
				}else{
					find_index->count-=p_oldlist[i].count;//防止 oldlist 中有 attireid 重复
				}
				break;
		}

	}

	//new list max count check
	
	for (i=0;i<newcount;i++ ){
		switch (p_newlist[i].attireid  ){
			case 0:p_uci->addxiaomee+=p_newlist[i].count;
				break;
			case 1:p_uci->addexp+=p_newlist[i].count;
				break;
			case 2:p_uci->addstrong+=p_newlist[i].count;
				break;
			case 3:p_uci->addiq+=p_newlist[i].count;
				break;
			case 4:p_uci->addcharm+=p_newlist[i].count;
				break;
			default : //交换成物品
				range=equal_range(idstart,idend, *((attire_count*)(&(p_newlist[i]))) );
				find_index=range.first;		
		
				if (find_index==idend || 
						find_index->attireid!=p_newlist[i].attireid) {//没有找到
					if (p_newlist[i].count>p_newlist[i].maxcount){
						free(p_dblist);
						return  USER_SWAP_ATTIRE_MAX_ERR ;
					}else{
						continue;
					}
				}
				//找到了。。
				if (p_newlist[i].count+find_index->count>p_newlist[i].maxcount) {
					//数目过多
					free(p_dblist);
					return USER_SWAP_ATTIRE_MAX_ERR ; 
				}
				break;
		}
	}
	free(p_dblist);
	return SUCC; 
}

int Cuser_attire::swaplist( userid_t userid,
				uint32_t oldcount,attire_count* p_oldlist,
				uint32_t newcount,attire_count_with_max * p_newlist, uint32_t is_vip_opt_type )
{
	uint32_t i;
	//更新数据
	//attridid: 0，代表增加摩尔豆，1代表经验值，2 力量值，3 智力值，4 魅力值
	int ret = -1;
	for (i=0;i<oldcount;i++ ){
		if (p_oldlist[i].attireid >5 ){
			ret = this->update_count(userid,p_oldlist[i].attireid , -(p_oldlist[i].count) );
			if(ret == SUCC){
				if(p_oldlist[i].attireid == 190890){
					DEBUG_LOG("delete== itemid: %u, count: %u", p_oldlist[i].attireid, (p_oldlist[i].count));
				}
				this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_oldlist[i].attireid,
				   	-(p_oldlist[i].count));	
			}//if
		}

	}
	for (i=0;i<newcount;i++ ){
		if (p_newlist[i].attireid >5 ){
			ret=this->update_count(userid,p_newlist[i].attireid , p_newlist[i].count);
            if (ret!=SUCC){//没有现成记录，insert
            	ret = this->insert(userid , p_newlist[i].attireid, 0, p_newlist[i].count );
            }
			DEBUG_LOG("20121215outer---ret: %d, userid: %u, itemid: %u, count: %u", ret, userid, p_newlist[i].attireid, (p_newlist[i].count));
			if(ret == SUCC){
				DEBUG_LOG("20121215inner---ret: %d, userid: %u,  itemid: %u, count: %u", ret, userid, p_newlist[i].attireid, (p_newlist[i].count));
				 this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_newlist[i].attireid,
				   	(p_newlist[i].count));	
			}//if
		}
	}
	return SUCC;
}
	
int  Cuser_attire:: get_list_by_attireid_interval(userid_t userid, 
		uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
		uint32_t *count, attire_count ** list   )	
{
	char formatstr[2000] ;
	if (usedflag==0){ //nouse count return
		strcpy(formatstr, "select attireid, count-usedcount-chest-mode as unusedcount \
				from %s where userid=%u and attireid>=%u and attireid<%u \
				having unusedcount>0  order by attireid " );
	}else if (usedflag==1){ //use count return
		strcpy(formatstr,	"select attireid,usedcount from %s \
			where userid=%u and attireid>=%u and attireid<%u and usedcount>0 order by attireid ");
	}else if (usedflag==2){ // count return
		strcpy(formatstr, "select attireid,count-chest-mode from %s \
		 	where userid=%u and attireid>=%u and attireid<%u and count>chest+mode order by attireid ");
	}else {
		return  ENUM_OUT_OF_RANGE_ERR ;
	}
	sprintf( this->sqlstr,formatstr , 
		this->get_table_name(userid),userid, attireid_start, attireid_end); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		(*list+i)->attireid=atoi_safe(NEXT_FIELD); 
		(*list+i)->count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int  Cuser_attire:: get_list_by_attireid_for_bus(userid_t userid, 
		uint32_t attireid_start, uint32_t attireid_end,uint32_t *count, uint32_t** list)	
{
	char formatstr[2000] ;
	strcpy(formatstr,	"select attireid from %s \
		where userid=%u and attireid>=%u and attireid<%u and usedcount>0 order by attireid ");

	sprintf( this->sqlstr,formatstr , 
		this->get_table_name(userid),userid, attireid_start, attireid_end); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		*(*list+i) = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
	
/**
 * @brief 返回背包和衣柜里的物品 
 * @userid 米米号 
 * @attireid_start 开始的物品ID号
 * @attireid_end 结束的物品ID号
 * @usedflag 返回哪种类型的数据，0表示返回未使用，1表示返回使用的总数，2表示返回总数
 * @count 返回物品种类的数目
 * @list 返回物品的信息
 */
int  Cuser_attire:: get_all_list_by_attireid_interval(userid_t userid, 
		uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
		uint32_t *count, attire_count ** list   )	
{
	char formatstr[2000] ;
	if (usedflag==0){ //nouse count return
		strcpy(formatstr, "select attireid, count-usedcount as unusedcount \
				from %s where userid=%u and attireid>=%u and attireid<%u \
				having unusedcount>0  order by attireid " );
	}else if (usedflag==1){ //use count return
		strcpy(formatstr,	"select attireid,usedcount from %s \
			where userid=%u and attireid>=%u and attireid<%u and usedcount>0 order by attireid ");
	}else if (usedflag==2){ // count return
		strcpy(formatstr, "select attireid,count from %s \
		 	where userid=%u and attireid>=%u and attireid<%u and count>0 order by attireid ");
	}else {
		return  ENUM_OUT_OF_RANGE_ERR ;
	}
	sprintf( this->sqlstr,formatstr , 
		this->get_table_name(userid),userid, attireid_start, attireid_end); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		(*list+i)->attireid=atoi_safe(NEXT_FIELD); 
		(*list+i)->count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}


int Cuser_attire::update_common(userid_t userid,user_set_attire_in *item , user_set_attire_out *p_out,
		uint32_t is_vip_opt_type)
{
	uint32_t value;
	int ret;
	bool addflag; //check for insert  or update 
	bool isadd=  item->operatetype &0x01;
	bool isused= item->operatetype &0x02;
	bool isused_all= item->operatetype &0x04;//

	value=item->value;
	if (value<0 || value>VALUE_MAX ) {
		DEBUG_LOG("VALUE=%d,MAX=%d",value,VALUE_MAX);
		return  VALUE_OUT_OF_RANGE_ERR;
	}
	//得到数据
	uint32_t chest = 0;
	uint32_t mode = 0;
	ret=this->get(userid,item->attireid,&(p_out->usedcount),&(p_out->count), &chest, &mode);
	
	//记录不存在,并且是增加未使用	
	addflag= (ret==ATTIRE_COUNT_NO_ENOUGH_ERR) && (isadd && !isused);
	DEBUG_LOG(" attireid opter [%u][%u]flag [%u]value[%u] maxvalue[%u]",
			userid ,item->attireid,item->operatetype,item->value ,item->maxvalue );

	if (ret!=DB_SUCC){//记录不存在
		if(!addflag )//不是增加的
			return ret;//返回错误
	}
	if (isadd  && isused ){
		p_out->usedcount+=value;
		
		this->add_attire_msglog(userid,item->attireid, value);

	}else if  (isadd  && !isused ){
		p_out->count+=value;	

		this->add_attire_msglog(userid,item->attireid, value);

	}else if  (!isadd  && isused ){
		if (p_out->usedcount<value){
			DEBUG_LOG("usedcount = %d,[<] value=%d",p_out->usedcount,value);
			return VALUE_OUT_OF_RANGE_ERR;
		}
		p_out->usedcount-=value;
	}else if (!isadd  && !isused ){
		//减少总数
		if (p_out->count-chest-mode<value)return ATTIRE_COUNT_NO_ENOUGH_ERR; 
		p_out->count-=value;						
		if (isused_all){
			//同时减少已使用的
			//if (p_out->usedcount<value) {
			//	DEBUG_LOG("usedcount = %d,[<] value=%d",p_out->usedcount,value);
			//	return VALUE_OUT_OF_RANGE_ERR;
			//} 
			if (p_out->count < p_out->usedcount) { //减少后的总数小于正使用的
				p_out->usedcount = p_out->count;
			}
		}
	}

	//chack value  
	if (p_out->usedcount<0 || p_out->usedcount>VALUE_MAX ) 
		return  VALUE_OUT_OF_RANGE_ERR;
	if (p_out->count<0 || ( p_out->count>item-> maxvalue && isadd )) 
		return  VALUE_OUT_OF_RANGE_ERR;

	if (p_out->usedcount>p_out->count-chest-mode){
		DEBUG_LOG("usedcount = %d,[>] p_out->count=%d - chest=%d - mode=%d",p_out->usedcount,p_out->count,chest,mode);
		return  VALUE_OUT_OF_RANGE_ERR;
	}
	p_out->attireid=item->attireid;

	if (p_out->attireid == 1351056) {//如果是商城积分
		uint32_t msgbuff[2] = {userid, p_out->count};
		if (p_out->count > 1000) {
			msglog(this->msglog_file, 0x0408A349, time(NULL), &msgbuff, sizeof(msgbuff));
		}
	}

	if (addflag) 
		ret =  this->insert(userid,p_out->attireid,p_out->usedcount,p_out->count);
	else
		ret =  this->update(userid,p_out->attireid,p_out->usedcount,p_out->count);

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
			if(p_out->attireid == 190890){
				if(val < 0){
						DEBUG_LOG("delete== itemid: %u, count: %u", p_out->attireid, val);
					}
				else{
						DEBUG_LOG("add== itemid: %u, count: %u", p_out->attireid, val);
				}
			}
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_out->attireid, val);	
		}
	}
	return ret;

}

int  Cuser_attire:: get_list_by_idlist(userid_t userid, 
		id_list * p_attireidlist, uint32_t *count, attire_count ** list   )	
{
	char formatstr[3000]={};
	char valuestr[100]={};
	if (p_attireidlist->count>100 || p_attireidlist->count<1 ) return VALUE_OUT_OF_RANGE_ERR;  
	for(uint32_t i=0; i<p_attireidlist->count;i++) {
		sprintf (valuestr,"%d,",p_attireidlist->item[i] );
		strcat(formatstr,valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';

	sprintf( this->sqlstr,  "select attireid,count from %s \
		 	where userid=%u and attireid in (%s)" , 
		this->get_table_name(userid),userid,formatstr); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		INT_CPY_NEXT_FIELD(	(*list+i)->attireid); 
		INT_CPY_NEXT_FIELD((*list+i)->count); 
	STD_QUERY_WHILE_END();
}

int Cuser_attire:: get_nousedcount (userid_t userid, uint32_t attireid,uint32_t * p_noused_count  )
{
	sprintf( this->sqlstr, "select count-usedcount-chest-mode from %s \
			where userid=%u and attireid=%u ", 
			this->get_table_name(userid),userid, attireid); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_ID_NOFIND_ERR) ;
			INT_CPY_NEXT_FIELD( *p_noused_count );
	STD_QUERY_ONE_END();
}

/* 把背包里的物品放到衣柜里 */
int Cuser_attire::put_attire_chest(userid_t userid, uint32_t attireid)
{
	uint32_t count = 0;
	uint32_t ret = this->get_chestcount(userid, &count);
	if (ret != SUCC) {
		return ret;
	}
	if (count >= 200) {
		return CHEST_OUT_OF_RANGER_ERR;
	}
	ret = this->put_chest(userid, attireid);
	return ret;
}

/*使衣柜的物品数加一*/
int Cuser_attire::put_chest(userid_t userid, uint32_t attireid) 
{
	sprintf(this->sqlstr, "update %s set chest = chest + 1 where userid = %u and attireid = %u\
			and (count > (usedcount+chest+mode))",
			this->get_table_name(userid),
			userid,
			attireid
			);
	STD_SET_RETURN_EX (this->sqlstr, VALUE_OUT_OF_RANGE_ERR);
}

/* 从衣柜里减去一件衣服 */
int Cuser_attire::get_chest(userid_t userid, uint32_t attireid)
{
	sprintf(this->sqlstr, "update %s set chest = chest - 1 where userid = %u and chest >0 and attireid = %u",
			this->get_table_name(userid),
			userid,
			attireid
			);
	STD_SET_RETURN_EX (this->sqlstr, CHEST_ATTIRE_NOT_ENOUGH_ERR);
}

/* 得到衣柜里衣服的列表 */
int Cuser_attire::get_chest_list(userid_t userid, uint32_t *p_count, user_attire_get_chest_list_out_item **pp_list)
{
	sprintf(this->sqlstr, "select attireid, chest from %s where userid = %u and chest > 0",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->attireid); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->count); 
	STD_QUERY_WHILE_END();
}

/* 得到衣柜里衣服的总数 */
int Cuser_attire:: get_chestcount(userid_t userid, uint32_t * p_chest_count)
{
	sprintf( this->sqlstr, "select sum(chest) from %s where userid=%u", 
			this->get_table_name(userid),
			userid
		   ); 
			
	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_ID_NOFIND_ERR) ;
			INT_CPY_NEXT_FIELD(*p_chest_count);
	STD_QUERY_ONE_END();
}

int  Cuser_attire:: get_list_ex(userid_t userid, 
		uint32_t start, uint32_t count,
		uint32_t *p_count,  user_attire_get_list_ex_out_item ** pp_list   )	
{
	sprintf( this->sqlstr,"select attireid, usedcount, chest ,count  from %s  where userid=%u order by attireid limit %u ,%u  " , 
		this->get_table_name(userid),userid, start,  count); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->usedcount);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->chest);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->count);
	STD_QUERY_WHILE_END();
}

/**
 * @brief 得到各个不连续物品ID的数目
 * @param userid 米米号
 * @param p_in 输入的要查询的ID号
 * @param count 输入的ID号的个数
 * @param pp_out 输出查询的结构(物品的ID号和对应的个数)
 * @param p_out_count 查询到的物品的个数
 */
int Cuser_attire::get_discontinuous_attire(userid_t userid, user_attire_get_some_attire_count_in_item *p_in, uint32_t count,
		user_attire_get_some_attire_count_out_item **pp_out, uint32_t *p_out_count)
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if (count>200 || count<1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	for(uint32_t i = 0; i < count; i++) {
		sprintf (valuestr,"%d,", (p_in + i)->attireid);
		strcat(formatstr, valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';
	
	sprintf( this->sqlstr,  "select attireid, count from %s where attireid in (%s) and count > 0 and userid = %u", 
			this->get_table_name(userid),
			formatstr,
			userid
		   ); 
	    STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_out, p_out_count) ;
		    INT_CPY_NEXT_FIELD( (*pp_out + i)->attireid); 
		    INT_CPY_NEXT_FIELD((*pp_out + i)->count); 
		STD_QUERY_WHILE_END();
}

/**
 * @brief 得到各个不连续物品ID的数目, 除了衣柜里的
 * @param userid 米米号
 * @param p_in 输入的要查询的ID号
 * @param count 输入的ID号的个数
 * @param pp_out 输出查询的结构(物品的ID号和对应的个数)
 * @param p_out_count 查询到的物品的个数
 */
int Cuser_attire::get_discontinuous_attire_except_chest(userid_t userid, user_attire_get_attire_except_chest_in_item *p_in,
	            	uint32_t count, user_attire_get_attire_except_chest_out_item **pp_out, uint32_t *p_out_count)
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if (count>200 || count<1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	for(uint32_t i = 0; i < count; i++) {
		sprintf (valuestr,"%d,", (p_in + i)->attireid);
		strcat(formatstr, valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';
	
	sprintf( this->sqlstr,  "select attireid, count - chest from %s where attireid in (%s) and count > 0 and userid = %u", 
			this->get_table_name(userid),
			formatstr,
			userid
		   ); 
	    STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_out, p_out_count) ;
		    INT_CPY_NEXT_FIELD( (*pp_out + i)->attireid); 
		    INT_CPY_NEXT_FIELD((*pp_out + i)->count); 
		STD_QUERY_WHILE_END();
}

int Cuser_attire::update_set_all_noused(userid_t userid)
{
	const uint32_t attire_max = 15999;
	sprintf(this->sqlstr, "update %s set usedcount = 0 where userid = %u and attireid <= %u",
			this->get_table_name(userid),
			userid,
			attire_max
			);
	STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

int Cuser_attire::set_used_attire(userid_t userid, user_attire_set_used_in_item *p_in, uint32_t count)
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if (count>20) {
		return VALUE_OUT_OF_RANGE_ERR;
	}

	uint32_t ret = this->update_set_all_noused(userid);
	if (ret != SUCC) {
		return ret;
	}
	if (count == 0) {
		return SUCC;
	}
	for(uint32_t i = 0; i < count; i++) {
		sprintf (valuestr,"%d,", (p_in + i)->attireid);
		strcat(formatstr, valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';
	
	sprintf( this->sqlstr,  "update %s set usedcount = 1 where userid = %u and attireid in (%s) and count-chest-mode > 0" , 
			this->get_table_name(userid),
			userid,
			formatstr
		   ); 
	STD_SET_LIST_RETURN_WITH_CHECK(this->sqlstr, (int)count, ATTIRE_COUNT_NO_ENOUGH_ERR);
}

/* @brief 除掉模特身上穿的衣服 
 * @param userid 米米号
 * @param index mode的标号
 */
int Cuser_attire::update_set_mode_noused(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, "update %s set mode = 0 where userid = %u and mode_index = %u",
			this->get_table_name(userid),
			userid,
			index
			);
	STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

/* @brief 设置mode身上穿的衣服 
 * @param userid 米米号
 * @param p_in 输入的服装ID号 
 * @param count 输入服装的总数
 * @param index mode的 标号
 */
int Cuser_attire::set_mode_attire(userid_t userid, user_attire_set_mode_attire_in_item *p_in, uint32_t count, uint32_t index)
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if (count>20) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	/* 先去除模特身上穿的衣服 */
	uint32_t ret = this->update_set_mode_noused(userid, index);
	if (ret != SUCC) {
		return ret;
	}
	if (count == 0) {
		return SUCC;
	}
	for(uint32_t i = 0; i < count; i++) {
		sprintf (valuestr,"%d,", (p_in + i)->attireid);
		strcat(formatstr, valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';
	
	sprintf( this->sqlstr,  "update %s set mode = 1, mode_index = %u where\
			attireid in (%s) and (count- chest - usedcount) > 0" , 
			this->get_table_name(userid),
			index,
			formatstr
		   ); 
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

/* 对衣服进行归类,每个模特身上衣服的种类 */
int Cuser_attire::get_mode_attire(userid_t userid, user_attire_all_mode_clothe *p_out)
{
	user_attire_mode_clothe *p_mode_list = NULL;
	uint32_t count = 0;
	uint32_t ret = this->get_mode_clothe(userid, &p_mode_list, &count);
	if (ret != SUCC) {
		return ret;
	}
	for(uint32_t i = 0; i < count; i++) {
		uint32_t index = (p_mode_list + i)->mode_index - MODE_BASE; 
		if (index >= 50) {
			return VALUE_OUT_OF_RANGE_ERR;
		}
		if (p_out->item[index].count == 0) {
			p_out->item[index].id[p_out->item[index].count] = (p_mode_list + i)->attireid;
			p_out->item[index].mode_index = (p_mode_list + i)->mode_index;
			p_out->item[index].count++;
			p_out->count++;
		} else {
			p_out->item[index].id[p_out->item[index].count] = (p_mode_list + i)->attireid;
			p_out->item[index].count++;
		}
	}
	free(p_mode_list);
	return SUCC;
} 

/* 得到一个用户所有模特衣服的列表 */
int Cuser_attire::get_mode_clothe(userid_t userid, user_attire_mode_clothe	**pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select attireid, mode_index from %s where userid = %u and mode > 0 order by mode_index",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->attireid); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->mode_index); 
	STD_QUERY_WHILE_END();
}

/* @brief 除掉模特身上穿的衣服 
 * @param userid 米米号
 * @param index mode的标号
 */
int Cuser_attire::update_set_mode_mole(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, "update %s set mode = 0, usedcount = 1 where userid = %u\
			and mode_index = %u and mode > 0",
			this->get_table_name(userid),
			userid,
			index
			);
	STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

/* 得到某个模特衣服的列表 */
int Cuser_attire::get_one_mode_clothe(userid_t userid, uint32_t index,
		user_attire_set_mode_to_mole_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select attireid from %s where userid = %u and mode_index = %u and mode > 0",
			this->get_table_name(userid),
			userid,
			index
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->attireid); 
	STD_QUERY_WHILE_END();
}

/**
 * @brief 使mole身上的衣服穿到模特身上 
 * @param userid 米米号
 * @param index mode的标号
 */
int Cuser_attire::update_set_clothe_mole_mode(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, "update %s set mode = 1, usedcount = 0, mode_index = %u where userid = %u\
			and usedcount > 0",
			this->get_table_name(userid),
			index,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, SUCC);
}

/**
 * @brief 得到mole身上穿的衣服
 * @param userid 米米号
 * @param pp_list 返回mole的衣服列表
 * @param p_count  mole拥有的衣服的数量
 */
int Cuser_attire::get_mole_clothe(userid_t userid, user_attire_set_used_out_item **pp_list, uint32_t *p_count)
{
	const uint32_t attire_max = 15999;
	sprintf(this->sqlstr, "select attireid from %s where userid = %u and usedcount > 0 and attireid <= %u",
			this->get_table_name(userid),
			userid,
			attire_max
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->attireid); 
	STD_QUERY_WHILE_END();
}

/**
 * @brief 删除某个范围的物品
 * @param userid 米米号
 * @param index mode的标号
 */
int Cuser_attire::delete_attirelist(userid_t userid, uint32_t start_index, uint32_t end_index, 
		uint32_t is_vip_opt_type)
{
	uint32_t db_cnt = 0;
	attire_count *list = 0;
	this->get_all_list_by_attireid_interval(userid, start_index, end_index+1, 2, &db_cnt, &list);
	for(uint32_t k = 0; k < db_cnt; ++k){
		if((list+k)->count != 0){
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, (list+k)->attireid, -(list+k)->count);	
		}
	}//for
	if(list != 0){
		free(list);
	}

	sprintf(this->sqlstr, "delete from %s where userid = %u and (attireid >= %u and attireid <= %u) ",
			this->get_table_name(userid),
			userid,
			start_index,
			end_index
			);
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Cuser_attire::reset_usedcount(userid_t userid, uint32_t start_attireid, uint32_t end_attireid)
{
	sprintf(this->sqlstr, "update  %s set usedcount = 0 where userid = %u and (attireid >= %u and attireid <= %u) ",
			this->get_table_name(userid),
			userid,
			start_attireid,
			end_attireid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 更新attireid的数量，如果数量大于最大值，设置为最大值
 */
int Cuser_attire::update_max_limit(userid_t userid, user_set_attire_max_limit_in *item, 
		user_set_attire_max_limit_out *p_out, uint32_t is_vip_opt_type)
{
	uint32_t value = item->value;
	if (value < 0 || value > VALUE_MAX ) {
		DEBUG_LOG("VALUE=%d,MAX=%d", value, VALUE_MAX);
		return  VALUE_OUT_OF_RANGE_ERR;
	}

	//得到数据
	uint32_t add_value = 0;
	uint32_t chest = 0;
	uint32_t mode = 0;
	int ret = this->get(userid, item->attireid, &(p_out->usedcount), &(p_out->count), &chest, &mode);

	//如果已有的总数量加上要增加的数量大于了max_value，则总拥有量要等于max_value
	if (p_out->count + p_out->usedcount + chest + mode + item->value > item->max_value) {
		add_value = item->max_value - (p_out->usedcount + chest + mode + p_out->count);
	} else {
		add_value = item->value;
	}
	if(add_value != 0){
		this->add_attire_msglog(userid, item->attireid, add_value);
		p_out->count = p_out->count + add_value;
		p_out->attireid = item->attireid;
		if (ret != DB_SUCC) {
			ret = this->insert(userid, p_out->attireid, p_out->usedcount, p_out->count);
		} else {
			ret = this->update(userid, p_out->attireid, p_out->usedcount, p_out->count);
		}
		if(ret == DB_SUCC){
			if(item->attireid == 190890){
				DEBUG_LOG("add== itemid: %u, count: %u", item->attireid, add_value);
			}
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, item->attireid, add_value);	
		}
		return ret;
	}
	else{
		return 0;
	}
	
}

int Cuser_attire::get_paradise_noused_attirelist(userid_t userid, user_get_paradise_attirelist_out_item **pp_list,
		uint32_t *count)
{
	const uint32_t  start_attireid = 1353000;
	const uint32_t  end_attireid = 1353380;
	uint32_t ret = get_paradise_attire(userid, pp_list, count, start_attireid, end_attireid);
	return ret;
}

int Cuser_attire::get_paradise_attire(userid_t userid, user_get_paradise_attirelist_out_item **pp_list,
		uint32_t *p_count, const uint32_t start_id, const uint32_t end_id)
{
	 sprintf(this->sqlstr, "select attireid, count from %s where userid = %u and attireid >= %u and attireid <= %u and \
			 count > 0",
            this->get_table_name(userid),
            userid,
            start_id,
            end_id
           );
    STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list+i)->attireid);
        INT_CPY_NEXT_FIELD((*pp_list+i)->count);
    STD_QUERY_WHILE_END();	
}
int Cuser_attire::get_expedition_articles(userid_t userid, user_get_expedition_bag_out_item **pp_list, 
		uint32_t *p_count, uint32_t start_id, uint32_t end_id, uint32_t flag)
{
	uint32_t tmp_count = 0, tmp_usedcount = 0;
	if(flag == 0){
		sprintf(this->sqlstr, "select attireid, count, usedcount from %s where userid = %u and \
				attireid >= %u and attireid <= %u and count > usedcount",
            this->get_table_name(userid),
            userid,
            start_id,
            end_id
           );
	}
	else{
		sprintf(this->sqlstr, "select attireid, count  from %s where userid = %u and attireid >= %u \
				and attireid <= %u",
            this->get_table_name(userid),
            userid,
            start_id,
            end_id
           );
	}
   	
    STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
        INT_CPY_NEXT_FIELD((*pp_list+i)->attireid);
		if(flag == 0){
			INT_CPY_NEXT_FIELD(tmp_count);
			INT_CPY_NEXT_FIELD(tmp_usedcount);
			(*pp_list+i)->count = tmp_count - tmp_usedcount;
			tmp_usedcount = 0;
		}
		else{
			INT_CPY_NEXT_FIELD((*pp_list+i)->count);
		}
    STD_QUERY_WHILE_END();	


}

int Cuser_attire::update_new_treasue_age(userid_t userid, uint32_t attireid, uint32_t used_count,
		uint32_t count)
{
	sprintf(this->sqlstr, "update %s set usedcount = %u, chest = 0, mode = 0, mode_index = 0, \
		count = %u where userid = %u and attireid = %u",
		this->get_table_name(userid),
		used_count,
		count,
		userid,
		attireid
		);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Cuser_attire::update_assign_count(userid_t userid, uint32_t attireid, uint32_t used_count, 
		uint32_t count, uint32_t is_vip_opt_type)
{
	uint32_t table_cnt = 0;
	this->get_count(userid, attireid, table_cnt);
	int ret = update_new_treasue_age(userid, attireid, used_count, count);
	if(ret == SUCC){
		if(table_cnt > count){
			if(attireid == 190890){
				DEBUG_LOG("delete== itemid: %u, count: %u", attireid, (table_cnt - count));
			}
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, -(table_cnt - count));	
		}
		else if(count > table_cnt){
			if(attireid == 190890){
				DEBUG_LOG("add== itemid: %u, count: %u", attireid, (count-table_cnt));
			}
			this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, count-table_cnt);	
		}
	}
	return ret;
	
}

int Cuser_attire::update_inc_count_and_used(userid_t userid, uint32_t attireid, uint32_t inc_cnt,
		uint32_t inc_used)
{

	sprintf(this->sqlstr, "update %s set count = count + %u, usedcount = usedcount + %u where userid = %u \
			and attireid = %u",
			this->get_table_name(userid),
			inc_cnt,
			inc_used,
			userid,
			attireid
			);

	STD_SET_RETURN_EX (this->sqlstr, SUCC);

}

int Cuser_attire::update_usedcount_count(userid_t userid, uint32_t attireid, uint32_t inc_cnt, 
		uint32_t inc_used, uint32_t is_vip_opt_type)
{

	int ret = update_inc_count_and_used(userid, attireid, inc_cnt, inc_used);
	if(ret == SUCC){
		if(attireid == 190890){
			DEBUG_LOG("add== itemid: %u, count: %u", attireid, inc_cnt);
		}
		this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, attireid, inc_cnt);	
	}
	return ret;

}


int Cuser_attire::take_off(userid_t userid, uint32_t itemid, uint32_t cnt)
{
	sprintf(this->sqlstr, "update %s set usedcount = if(usedcount >= %u, usedcount-%u, 0) where \
			userid = %u and attireid = %u",
			this->get_table_name(userid),
			cnt,
			cnt,
			userid,
			itemid
			);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_attire::put_on(userid_t userid, uint32_t itemid, uint32_t cnt)
{
	sprintf(this->sqlstr, "update %s set usedcount = usedcount + %u where userid = %u and attireid = %u",
            this->get_table_name(userid),
			cnt,
            userid,
            itemid
            );
    
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_attire::get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count - usedcount from %s where userid = %u and attireid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

