/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
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
#include "Cuser_farm.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "msglog.h"



using namespace std;
//user 
Cuser_farm :: Cuser_farm(mysql_interface * db , Citem_change_log * p_log):
	CtableRoute100x10( db , "USER" , "t_user_farm" , "userid") 
{ 
	this->p_item_change_log = p_log;
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_farm::del(userid_t userid )
{
	sprintf( this->sqlstr, "delete from %s where userid=%u " ,
			this->get_table_name(userid),  userid); 
	STD_REMOVE_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}


int Cuser_farm::update_state(userid_t userid, uint32_t state)
{
	sprintf( this->sqlstr, "update %s set farm_state = %u where userid=%u " ,
			this->get_table_name(userid),
			state,
			userid
			); 
	STD_REMOVE_RETURN (this->sqlstr,userid,  USER_ID_NOFIND_ERR);
}


int Cuser_farm::get_state(userid_t userid, uint32_t *p_state)
{
	sprintf( this->sqlstr, "select farm_state from %s where userid=%u " ,
			this->get_table_name(userid),
			userid
			); 
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_state);
	STD_QUERY_ONE_END();
}



int Cuser_farm::insert(userid_t userid)
{
	char jylist[200];
	char jylist_mysql[401];
	memset(jylist,0,sizeof(jylist) );

	uint32_t now = time(0);
	//家园初始内容
	home_attirelist	*p_jyattirelist=(home_attirelist*)jylist; 
	p_jyattirelist->count=1;	
	p_jyattirelist->item[0].attireid=1280001;	
	set_mysql_string(jylist_mysql ,(char *)jylist,20);

	sprintf( this->sqlstr, "insert into %s values (\
		%u, 0x0000, %u, 0, 0, 0, 0, 0, 0, 0, 0, '%s', 0x00000000, 0x00000000, 0x00000000, 0x0000, 0x0000, 0)", 
			this->get_table_name(userid), 
			userid,
			now,
			jylist_mysql
	   	);
		STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cuser_farm::get_noused_home_attirelist(const char * type_str, userid_t userid , 
		noused_homeattirelist * p_list  )
{
	sprintf(this->sqlstr, "select noused_%sattirelist from %s where userid=%u ", 
			type_str,this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
	STD_QUERY_ONE_END();
}



int	Cuser_farm::check_existed( userid_t userid)
{
	bool existed;
	int ret=this->id_is_existed(userid,&existed);
	if (ret!=SUCC) return ret;
	if (existed ){
		return SUCC;
	}else {
		return USER_ID_NOFIND_ERR;
	}
}

int Cuser_farm::update_noused_homeattirelist(const char * type_str, userid_t userid ,
		noused_homeattirelist * p_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(noused_homeattirelist))];
	set_mysql_string(mysql_attirelist,(char*)p_list, 
			4 +sizeof(attire_noused_item  )* p_list->count);
	sprintf( this->sqlstr, " update %s set \
		noused_%sattirelist='%s' \
		where userid=%u " ,
		this->get_table_name(userid), type_str ,
		mysql_attirelist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cuser_farm::home_attire_change(const char * type_str, userid_t userid , user_home_attire_change_in *p_in )
{
	if ( p_in->changeflag==2){//减少使用中
		return this->del_home_attire_used(type_str, userid, p_in->attireid );	
	}else if(p_in->changeflag==0 ){//减少未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->del_home_attire_noused(type_str, userid, &item );		
	}else if(p_in->changeflag==1 ){//增加未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->add_home_attire(type_str, userid, &item );		
	}else 
		return  ENUM_OUT_OF_RANGE_ERR;
}


int Cuser_farm::del_home_attire_used(const char * type_str, userid_t userid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	home_attire_item item;  
	item.attireid=attireid;
	int ret=this->get_homeattirelist(type_str,userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;
	_start=used_list.item ;
	_end=_start+used_list.count;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		used_list.count--;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	
	return this->update_used_homeattirelist(type_str,userid,&used_list);
}


int Cuser_farm::home_set_attire_noused(const char * type_str, userid_t userid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	home_attire_item item;  
	item.attireid=attireid;

	int ret=this->get_homeattirelist(type_str,userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;

	_start=used_list.item ;
	_end=_start+used_list.count;	

	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		used_list.count--;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	

	attire_count_with_max  noused_item;
	noused_item.attireid=attireid;
	noused_item.count=1;
	noused_item.maxcount=0xFFFFFFFF;
	ret=this->add_home_attire(type_str, userid,&noused_item);
	if (ret!=SUCC) return ret; 
	return this->update_used_homeattirelist(type_str,userid,&used_list);
}

int Cuser_farm::del_home_attire_noused(const char * type_str, userid_t userid , attire_count_with_max  * p_item,
		uint32_t is_vip_opt_type)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	attire_noused_item *_start, *_end,*_find , *_tmp ; 

	int ret=this->get_homeattirelist(type_str, userid,&used_list,&list);
	if (ret!=SUCC) return ret;

	_start=list.item ;
	_end=_start+list.count;	
	_find=std::find( _start ,  _end ,  *((attire_noused_item*) p_item) );
	if (_find!=_end ) {
		//find  ,update
		if (_find->count>p_item->count){
			_find->count-=p_item->count;
		} else if (_find->count==p_item->count){
			//去掉这一项
			_tmp=_find+1;	
			for (;_tmp!=_end; _tmp++){
				memcpy(_tmp-1, _tmp, sizeof(* _tmp));
			}
			list.count--;
		}
		else 
			return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	
	ret =  this->update_noused_homeattirelist(type_str, userid,&list);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, -p_item->count);
	}
	return ret;
}

int Cuser_farm::add_home_attire(const char * type_str, userid_t userid , attire_count_with_max  * p_item,
		uint32_t is_vip_opt_type)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	uint32_t used_count=0;

	int ret=this->get_homeattirelist(type_str,userid,&used_list,&list);
	if (ret!=SUCC) return ret;
	//在使用列表中的个数
	used_count=0;
	for (uint32_t i=0;i<used_list.count;i++ ){
		if (used_list.item[i].attireid==p_item->attireid){
			used_count++;
		}
	}	
	if (p_item->maxcount>=used_count){
		p_item->maxcount-=used_count;		
	}else{
		return VALUE_OUT_OF_RANGE_ERR;
	}



	ret=this->add_homeattire_noused( &list, p_item );
	if (ret!=SUCC) return ret;

	ret = this->update_noused_homeattirelist( type_str, userid,&list);
	/*
	 * 114D协议,下面是道具增减统计支持
	 */
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_item->attireid, p_item->count);
	}
	return ret;
}




int Cuser_farm::get(userid_t userid, user_get_farm_out_header *p_out_header,
	        	farm_noused_attirelist *p_noused, farm_used_attirelist *p_used)
{
		sprintf( this->sqlstr, "select farm_state, water_time, insect_house, farmattirelist, noused_feedstuffattirelist\
				from %s where userid=%u",
				this->get_table_name(userid),
		        userid 
		       );

		STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out_header->farm_state);
			INT_CPY_NEXT_FIELD(p_out_header->pool_state);
			INT_CPY_NEXT_FIELD(p_out_header->insect_house);
			BIN_CPY_NEXT_FIELD(p_used, sizeof(*p_used));		
			BIN_CPY_NEXT_FIELD(p_noused, sizeof(*p_noused));		
		STD_QUERY_ONE_END();
}

int Cuser_farm :: get_user_farm_table_info(userid_t userid, user_get_farm_out_header *p_out_header,
	           	farm_noused_attirelist *p_noused, farm_used_attirelist *p_used,uint32_t& water_time)
{
	int ret = this->get(userid, p_out_header, p_noused, p_used);
	if (ret != SUCC) {
		ret = insert(userid);
		if (ret == SUCC) {
			ret = this->get(userid, p_out_header, p_noused, p_used);
		}
	}

	water_time = p_out_header->pool_state;

	if (time(NULL) - p_out_header->pool_state > 24 * 3600) {
		p_out_header->pool_state = 1;
	} else {
		p_out_header->pool_state = 0;
	}
	return ret;
}

int Cuser_farm::get_attirelist( const char * type_str, userid_t userid , home_attirelist * p_home_attirelist)
{
	sprintf( this->sqlstr, "select  %sattirelist from %s where userid=%u ", 
			type_str, this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_home_attirelist  , sizeof(*p_home_attirelist ));
	STD_QUERY_ONE_END();
}




int  Cuser_farm::get_homeattirelist(const char * type_str,  userid_t userid ,
		home_attirelist * p_usedlist ,  noused_homeattirelist* p_nousedlist)
{
	sprintf( this->sqlstr, "select %sattirelist, noused_%sattirelist \
			from %s where userid=%u ", 
			type_str,type_str ,this->get_table_name(userid),userid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_usedlist,sizeof (*p_usedlist ) );
			BIN_CPY_NEXT_FIELD (p_nousedlist, sizeof (*p_nousedlist) );
	STD_QUERY_ONE_END();
}


void Cuser_farm::get_home_attire_total(ATTIRE_MAPITEM * p_itemmap,
			   	home_attirelist * p_usedlist,
				noused_homeattirelist * p_nousedlist )
{
	uint32_t attireid;
	//set used count
	for (uint32_t i=0;i<p_usedlist->count;i++ )	{
		attireid=p_usedlist->item[i].attireid;
		if ( p_itemmap->find(attireid)==p_itemmap->end()) {
			//(*p_itemmap).insert(map<uint32_t,uint32_t> :: value_type(attireid,1));
			(*p_itemmap)[attireid]=1;
		}else{
			(*p_itemmap)[attireid]++;
		}
	}

	//set noused count
	for (uint32_t i=0;i<p_nousedlist->count;i++ ){
		attireid=p_nousedlist->item[i].attireid;
		if ( p_itemmap->find(attireid)==p_itemmap->end()) {
			//(*p_itemmap).insert(map<uint32_t,uint32_t> :: value_type(attireid,
			//p_nousedlist->item[i].count));
			(*p_itemmap)[attireid]= p_nousedlist->item[i].count;
			
		}else{
			(*p_itemmap)[attireid]+=p_nousedlist->item[i].count;
		}
	}
}

int Cuser_farm::get_attire_count( const char * type_str, userid_t userid, uint32_t  attireid,
		uint32_t count_flag,  uint32_t *p_count )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	int ret;	
	//get old data
	if ((ret=this->get_homeattirelist( type_str,
		userid,&old_usedlist, & old_nousedlist))!=SUCC){
		return ret;
	}
	*p_count=0;
	if ( count_flag==0 ||count_flag==2 ){//未使用
		for (uint32_t i=0;i<old_nousedlist.count;i++ )	{
			//DEBUG_LOG("attireid [%u][%u] ",i,old_nousedlist.item[i].attireid );
			if(old_nousedlist.item[i].attireid==attireid){
				(*p_count)+=old_nousedlist.item[i].count;
				break;
			}
		}
	}

	if ( count_flag==1 ||count_flag==2 ){//已使用
		for (uint32_t i=0;i<old_usedlist.count;i++ )	{
			//DEBUG_LOG("attireid [%u][%u] ",i,old_usedlist.item[i].attireid );
			if( old_usedlist.item[i].attireid==attireid){
				(*p_count)++;
			}
		}
	}

	return SUCC;
}

int Cuser_farm::update_homeattire_all(const char * type_str,  userid_t userid ,  
			home_attirelist * p_usedlist , 
			noused_homeattirelist * p_nousedlist )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	ATTIRE_MAPITEM olditemmap; 	
	ATTIRE_MAPITEM newitemmap; 	
	ATTIRE_MAPITEM::iterator oldit;
	ATTIRE_MAPITEM::iterator newit;
	int ret;	
	
	//get old data
	if ((ret=this->get_homeattirelist( type_str,
		userid,&old_usedlist, & old_nousedlist))!=SUCC){
		return ret;
	}

	//check
	this->get_home_attire_total(&olditemmap,&old_usedlist, &old_nousedlist  );

	this->get_home_attire_total(&newitemmap,p_usedlist , p_nousedlist );		
	//check size    
	if (olditemmap.size()!= newitemmap.size()){
			DEBUG_LOG("type count err db[%lu] send[%lu]",
					olditemmap.size(),newitemmap.size() );
			return USER_SET_ATTIRE_DATA_ERR;
	}		

	for( oldit=olditemmap.begin();oldit!=olditemmap.end();++oldit){
		newit=newitemmap.find(oldit->first);
		if (newit !=newitemmap.end() && newit->second==oldit->second ){
			// find  and count is equal   
			continue;	
		}else{
			if (newit ==newitemmap.end()){
				DEBUG_LOG("sendlist no find attireid[%u]", oldit->first );
			}else{
				DEBUG_LOG("sendlist attireid [%u],  count err :db[%u] send[%u]", 
					oldit->first, oldit->second,newit->second);
			}
			return USER_SET_ATTIRE_DATA_ERR;
		}
	}

	return this->update_homeattirelist(type_str, userid,p_usedlist, p_nousedlist );	

}

int Cuser_farm::update_homeattirelist(const char * type_str, userid_t userid ,   
			home_attirelist * p_usedlist,noused_homeattirelist * p_nousedlist )
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];
	char mysql_nousedlist[mysql_str_len(sizeof (noused_homeattirelist ))];

	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	set_mysql_string(mysql_nousedlist,(char*)p_nousedlist, 
			4+sizeof(attire_noused_item)* p_nousedlist->count);

	sprintf( this->sqlstr, " update %s set \
		%sattirelist='%s', \
		noused_%sattirelist ='%s' \
		where userid=%u " ,
		 this->get_table_name(userid), 
		 type_str,mysql_usedlist,
		type_str, mysql_nousedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}
int Cuser_farm::update_used_homeattirelist(const char * type_str, userid_t userid ,   
			home_attirelist * p_usedlist )
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];

	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	sprintf( this->sqlstr, " update %s set \
		%sattirelist='%s' \
		where userid=%u " ,
		 this->get_table_name(userid), type_str,
		mysql_usedlist,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cuser_farm::del_homeattire_noused( noused_homeattirelist *p_list, 
				attire_noused_item *p_item )
{
	attire_noused_item *_start, *_end,*_find , *_tmp ; 

	_start=p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end ,  * p_item );
	if (_find!=_end ) {
		//find  ,update
			
		if (_find->count>p_item->count){
			_find->count-=p_item->count;
			DEBUG_LOG("p_list del item id:%d count:%d",p_item->attireid,p_item->count );
			return p_item->count;
		}else{
			int ret_count=_find->count;
			//去掉这一项
			_tmp=_find+1;	
			for (;_tmp!=_end; _tmp++){
				memcpy(_tmp-1, _tmp, sizeof(* _tmp));
			}
			(p_list->count)--;
			return ret_count;  
		}
	}else{
		return 0;
	}	
}

int Cuser_farm::del_homeattire_used( home_attirelist *p_list , home_attire_item * p_item )
{
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	_start=p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end , *p_item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		(p_list->count)--;
		return 1;
	}else{
		return 0;
	}	
}

int Cuser_farm::add_homeattire_noused(noused_homeattirelist *p_list,
				attire_count_with_max * p_item)
{
	attire_noused_item *_start, *_end,*_find  ; 

	_start=p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end ,  *((attire_noused_item*) p_item) );

	if (_find!=_end ) {
		//find  ,update
		_find->count+=p_item->count;

		if (_find->count>p_item->maxcount){
			DEBUG_LOG("allcount[%u]> maxcount [%u]",
					_find->count,p_item->maxcount);
			return FEEDSTUFF_THAN_MAX_ERR;
		}
	}else{ //add 
		if ( p_list->count >= HOME_NOUSE_ATTIRE_ITEM_MAX ){
			DEBUG_LOG("maxlen>default[%u]" ,p_list->count );
			return FEEDSTUFF_THAN_MAX_ERR;
		}
		if ( p_item->count>p_item->maxcount){
			DEBUG_LOG("send count[%u] > maxcount[%u]  err",
					p_item->count,p_item->maxcount);
			return FEEDSTUFF_THAN_MAX_ERR;
		}

		p_list->item[p_list->count].attireid= p_item->attireid;
		p_list->item[p_list->count].count= p_item->count;
		p_list->count++;
	}		
	return SUCC;
}

int Cuser_farm::add_feedstuff(attire_count_with_max *p_list, userid_t userid, noused_homeattirelist *p_noused)
{
	int ret = this->get_noused_home_attirelist(FEEDSTUFF_STR, userid, p_noused);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->add_homeattire_noused(p_noused, p_list);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->update_noused_homeattirelist(FEEDSTUFF_STR, userid, p_noused);
	if (ret != SUCC) {
		return ret;
	}
	return SUCC;
}

int Cuser_farm::set_water_time(userid_t userid)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "update %s set water_time = %u where userid = %u",
			this->get_table_name(userid),
			now,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );
}


int Cuser_farm::set_water_time(userid_t userid, uint32_t end_time)
{
	sprintf(this->sqlstr, "update %s set water_time = %u where userid = %u",
			this->get_table_name(userid),
			end_time,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );

}


int Cuser_farm::swap_homeattire(const char *   type_str, userid_t userid , 
				uint32_t oldcount,attire_count* oldlist,
				uint32_t newcount,attire_count_with_max * newlist )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	attire_count * p_noused_item;
	int real_del_count;
	int ret;
	uint32_t i,j;
	ret=this->get_homeattirelist(type_str, userid,&used_list,&noused_list);
	if (ret!=SUCC) return ret;
	//删除旧有的
	for (i=0;i<oldcount;i++){
 		p_noused_item=oldlist+i;
		real_del_count=this->del_homeattire_noused(&noused_list, p_noused_item );
		//从已使用中删除
		for(j=0;j<p_noused_item ->count-real_del_count;j++){
			if( this->del_homeattire_used(&used_list, 
						(home_attire_item* )p_noused_item  )!=1 ) {
				return   USER_ATTIRE_ID_NOFIND_ERR;
			}
		}
	}

	for (i=0;i<newcount;i++){
		ret=this->add_homeattire_noused(&noused_list,  (newlist+i));
		if ( ret!=SUCC) return ret;
	}
	return this->update_homeattirelist(type_str,userid,&used_list,&noused_list  );
}

int Cuser_farm::update_used_homeattirelist_with_check(const char * type_str, userid_t userid ,   
			home_attirelist * p_new_usedlist )
{
	int ret;
	home_attirelist old_usedlist;  
	ret=this->get_attirelist( type_str , userid, &old_usedlist);
	if(ret!=SUCC) return ret;
	if(old_usedlist.count!=p_new_usedlist->count)		{
		DEBUG_LOG("count:err %u %u",old_usedlist.count,p_new_usedlist->count  );
		return USER_SET_ATTIRE_DATA_ERR;
	}	
	for (uint32_t i=0;i<old_usedlist.count;i++ ){
		if (p_new_usedlist->item[i]!=old_usedlist.item[i]){
			DEBUG_LOG("item:err %u %u",p_new_usedlist->item[i].attireid,
				   old_usedlist.item[i].attireid	);
			return  USER_SET_ATTIRE_DATA_ERR;
		}
	}
	return this->update_used_homeattirelist( type_str,userid,p_new_usedlist);	
}




int Cuser_farm::get_jyaccess_list(userid_t userid, access_jylist *p_out )
{
	sprintf( this->sqlstr, "select farm_accesslist from %s where  userid=%d ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}


int Cuser_farm::access_jy (userid_t userid, user_access_jy_in *p_in )
{
	int ret;
	access_jylist jylist;	
	ret=this->get_jyaccess_list(userid,&jylist );
	if(ret!=SUCC) return ret;
	DEBUG_LOG("=opt type %u",p_in->opt_type);
	mms_opt_item * find_index, *idstart, *idend;

	//用于从指示从那里开始复制
	mms_opt_item 	 *cpy_index;	
	idstart=&(jylist.item[0]) ;
	idend=idstart+jylist.count;	
	find_index=std::find(idstart,idend, (*p_in));
	if (find_index!=idend ) {
		//找到,保留原有的浇水,除虫标志
		p_in->opt_type=find_index->opt_type;			
		cpy_index=find_index;
	}else{
		//没有找到
		p_in->opt_type=0;			
		jylist.count++;
		if (jylist.count>JY_ACCESS_MAX){
			jylist.count=JY_ACCESS_MAX;
			//指向最后一项数据
			cpy_index=find_index-1;
		}else{
			cpy_index=find_index;
		}
	}
	//复制数据
	while( cpy_index!=idstart ){
		memcpy(cpy_index,cpy_index-1,sizeof(*cpy_index));
		cpy_index--;	
	}
	DEBUG_LOG("opt type %u", p_in->opt_type);
	//第一个数据填上
	memcpy(idstart, p_in ,sizeof(*cpy_index));

	//更新
	return this->update_jy_accesslist(userid,&jylist);

}

int Cuser_farm::update_jy_accesslist( userid_t userid, access_jylist *p_list ) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->item[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set farm_accesslist ='%s' \
		where  userid=%u " ,
		this->get_table_name(userid), 
		mysql_list, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cuser_farm::jy_accesslist_set_opt (userid_t userid, uint32_t opt_userid ,uint32_t opt_type )
{
	int ret;
	access_jylist jylist;	

	mms_opt_item item;
	item.userid=opt_userid;

	ret=this->get_jyaccess_list(userid,&jylist );
	if(ret!=SUCC) return ret;

	mms_opt_item * find_index, *idstart, *idend;
	//用于从指示从那里开始复制
	mms_opt_item 	 *cpy_index;	

	idstart=&(jylist.item[0]) ;
	idend=idstart+jylist.count;	
	find_index=std::find( idstart,idend, item  );
	if (find_index==idend ) {
		//没有找到就算了
		return SUCC ;
	}

	//找到 + 加入相关操作标志
	find_index->opt_type |= opt_type;			
	memcpy(&item ,find_index,sizeof(item) );	

	cpy_index=find_index;


	//偏移复制数据
	while( cpy_index!=idstart ){
		memcpy(cpy_index,cpy_index-1,sizeof(*cpy_index));
		cpy_index--;	
	}

	//第一个数据填上
	memcpy(idstart, &item ,sizeof(*idstart));

	//更新
	return this->update_jy_accesslist(userid,&jylist);
}



int Cuser_farm::get_thiever_info(userid_t userid, stru_thiever *p_out)
{
	sprintf( this->sqlstr, "select thiever_time, thiever_list from %s where userid=%d", 
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->thiever_time);
		BIN_CPY_NEXT_FIELD (&(p_out->member_list), sizeof (p_out->member_list ));
	STD_QUERY_ONE_END();
}


int Cuser_farm::update_thiever(userid_t userid, stru_thiever *p_list)
{
	char mysql_list[mysql_str_len(sizeof (p_list->member_list))];

	set_mysql_string(mysql_list,(char*)(&(p_list->member_list)) , 
			4+sizeof(p_list->member_list.id[0])* p_list->member_list.count);

	sprintf( this->sqlstr, " update %s set thiever_time = %u, thiever_list='%s' \
		     where userid=%u " ,
			this->get_table_name(userid), 
			p_list->thiever_time,
			mysql_list,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid , USER_ID_NOFIND_ERR );	
}


int Cuser_farm :: day_between(const time_t pre_time)
{

    struct tm *yymmdd;
    time_t old_time;
    time_t now_time;

    yymmdd = localtime(&pre_time);
    yymmdd->tm_sec = 0;
    yymmdd->tm_min = 0;
    yymmdd->tm_hour = 0;
    old_time = mktime(yymmdd);

    now_time = time(NULL);
    yymmdd = localtime(&now_time);
    yymmdd->tm_sec = 0;
    yymmdd->tm_min = 0;
    yymmdd->tm_hour = 0;
    now_time = mktime(yymmdd);
    return ((now_time - old_time) / (60 * 60 * 24));

}


int Cuser_farm::thiever_animal(userid_t userid, userid_t fish_userid, uint32_t id, uint32_t type)
{
	uint32_t state = 0;

	int ret = this->get_state(userid, &state);
	if (ret != SUCC) {
		return ret;
	}
	if (type == 1) {//陆地动物
		if (state == 1 || state == 3) {//1表示陆地动物不能偷取
			return FARM_HAS_BEEN_LOCKED_ERR;
		}
	}

	if (type == 0) {
		if (state == 2 || state == 3) {//2表示水生动物不能偷取
			return FARM_HAS_BEEN_LOCKED_ERR;
		}
	}
	stru_thiever temp;
	ret = this->get_thiever_info(userid, &temp);
	if (ret != SUCC) {
		return ret;
	}
	int day = day_between(temp.thiever_time);
	if (day > 0) {
		temp.thiever_time = time(0);
		temp.member_list.count = 1;
		temp.member_list.id[0] = fish_userid;
		ret = this->update_thiever(userid, &temp);
		return ret;
	}
	if (temp.member_list.count > 30) {
		return VALUE_OUT_OF_RANGE_ERR;
	}

	for (uint32_t i = 0; i < temp.member_list.count; i++) {
		if (temp.member_list.id[i] == fish_userid) {
			return FARM_HAVE_THIEVER_TODAY_ERR;
		}
	}

	temp.member_list.id[temp.member_list.count] = fish_userid;
	temp.member_list.count++;
	ret = this->update_thiever(userid, &temp);
	return ret;
}

/* brief  返回种植手和饲养手的数值 
 * @param userid 米米号
 * @param p_plant 返回种植手的数值
 * @param p_breed 返回饲养手的数值 
 */
int Cuser_farm::get_plant_breed(userid_t userid, uint32_t *p_plant, uint32_t *p_breed)
{
	sprintf( this->sqlstr, "select plant, breed from %s where  userid=%d ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_plant);
		INT_CPY_NEXT_FIELD (*p_breed);
	STD_QUERY_ONE_END();
}

/* breif 得到种植手和饲养手有关的字段的值
 * @param userid 米米号
 * @param p_time 返回时间值
 * @param p_plant_limit 返回种植上限
 * @param p_breed_limit 返回饲养的上限
 * @param p_plant 返回种植手的数值
 * @param p_breed 返回饲养手的数值
 */
int Cuser_farm::time_limit(userid_t userid, uint32_t *p_time, uint32_t *p_plant_limit,
		uint32_t *p_breed_limit, uint32_t *p_plant, uint32_t *p_breed)
{
	sprintf( this->sqlstr, "select time, plant_limit, breed_limit, plant, breed from %s where  userid=%d ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_time);
		INT_CPY_NEXT_FIELD(*p_plant_limit);
		INT_CPY_NEXT_FIELD (*p_breed_limit);
		INT_CPY_NEXT_FIELD (*p_plant);
		INT_CPY_NEXT_FIELD (*p_breed);
	STD_QUERY_ONE_END();
}

/* @brief 更新种植和养殖的经验值，并同时更新当天的上限
 * @param userid 用户的米米号
 * @param plant 要增加的种植手的值
 * @param breed 要增加的种植手的值
 * @param time 更新时间，更新为当天的时间 
 * @param plant_limit 更新种植的上限
 * @param breed_limit 更新饲养的上限 
 */
int Cuser_farm::update_time_limit(userid_t userid, uint32_t plant, uint32_t breed, uint32_t time,
		        uint32_t plant_limit, uint32_t breed_limit)
{
	sprintf( this->sqlstr, " update %s set plant = plant + %u, breed = breed + %u, time = %u,\
			plant_limit = %u, breed_limit = %u where userid=%u " ,
			this->get_table_name(userid), 
			plant,
			breed,
			time,
			plant_limit,
			breed_limit,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid , USER_ID_NOFIND_ERR );	
}

/* @brief 更新种植手和养殖手的值
 * @param userid 用户的米米号
 * @param plant 更增加的种植手字段的值
 * @param breed 更增加的养殖手的字段值
 */
int Cuser_farm::update_plant_breed(userid_t userid, uint32_t plant, uint32_t breed) 
{
	sprintf( this->sqlstr, " update %s set plant = plant + %u, breed = breed + %u where userid=%u",
			this->get_table_name(userid), 
			plant,
			breed,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid , USER_ID_NOFIND_ERR );	
}

/* @breif 收获是加经验值
 * @param userid 米米号 
 * @param type 1表示家园加，2表示牧场加
 * @param add  表示增量的多少
 * @param p_ex  返回目前的数值
 */
int Cuser_farm::catch_get_exp(userid_t userid, uint32_t type, uint32_t add, uint32_t *p_ex)
{
	uint32_t ret = 0;
	uint32_t temp = 0;
	//家园
	if (type == 1) {
		ret = this->update_plant_breed(userid, add, 0);
		if (ret != SUCC) {
			if (ret == USER_ID_NOFIND_ERR) {
				this->insert(userid);
				ret = this->update_plant_breed(userid, add, 0);
				if (ret != SUCC) {
					return ret;
				}
			}
		}
		ret = this->get_plant_breed(userid, p_ex, &temp);
		return ret;
	} else if (type == 2) {
		ret = this->update_plant_breed(userid, 0, add);
		if (ret != SUCC) {
			if (ret == USER_ID_NOFIND_ERR) {
				this->insert(userid);
				ret = this->update_plant_breed(userid, 0, add);
				if (ret != SUCC) {
					return ret;
				}
			}
		}
		ret = this->get_plant_breed(userid, &temp, p_ex);
		return ret;
	} else {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/* @breif 给家园和牧场的种植手，增加经验值 
 * @param userid 用户的米米号
 * @param add 增加的值
 * @param type 1表示给家园增加，2表示给牧场增加
 * @param p_new 返回增加后的最新值
 */
int Cuser_farm::add_other(userid_t userid, uint32_t add, uint32_t type, int32_t *p_new)
{
	uint32_t old_time = 0;
	uint32_t plant_limit = 0;
	uint32_t breed_limit = 0;
	uint32_t plant = 0;
	uint32_t breed = 0;
	uint32_t ret = this->time_limit(userid, &old_time, &plant_limit, &breed_limit, &plant, &breed);
	if (ret != SUCC) {
		if (ret == 1105) {
			ret = SUCC;
		} else {
			return ret;
		}
	}
	uint32_t today = get_date(time(NULL));
	if (old_time != today) {
		old_time = today;
		plant_limit = 0;
		breed_limit = 0;
	}
	//家园
	if (type == 1) {
		//不能超过上限20
		if (plant_limit < 20) {
			if (plant_limit + add > 20) {
				add = 20 - plant_limit;
				plant_limit = 20;
			} else {
				plant_limit += add;
			}
			ret = this->update_time_limit(userid, add, 0, old_time, plant_limit, breed_limit);
			if (ret != SUCC) {
				if (ret == 1105) {
					ret = this->insert(userid);
					if (ret != SUCC) {
						return ret;
					}
					ret = this->update_time_limit(userid, add, 0, old_time, plant_limit, breed_limit);
					if (ret != SUCC) {
						return ret;
					}
				}
			} else {
				return ret;
			}
			*p_new = plant + add;
		}
	//牧场 
	} else if (type == 2) {
		if (breed_limit < 20) {
			if (breed_limit + add > 20) {
				add = 20 - breed_limit;
				breed_limit = 20;
			} else {
				breed_limit += add;
			}
			ret = this->update_time_limit(userid, 0, add, old_time, plant_limit, breed_limit);
			if (ret != SUCC) {
				if (ret == 1105) {
					this->insert(userid);
					if (ret != SUCC) {
						return ret;
					}
				 	ret = this->update_time_limit(userid, 0, add, old_time, plant_limit, breed_limit);
					if (ret != SUCC) {
						return ret;
					}
				} else {
					return ret;
				}
			}
			*p_new = breed + add;
		}
	} else {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/* 得到t_user_farm表中的某些字段 */
int Cuser_farm::get_farm_web(userid_t userid, user_farm_get_web_out *p_out)
{
	sprintf( this->sqlstr, "select farm_state, water_time, net, plant, breed, plant_limit, breed_limit,\
			time from %s where  userid=%d ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->farm_state);
		INT_CPY_NEXT_FIELD(p_out->water_time);
		INT_CPY_NEXT_FIELD(p_out->net);
		INT_CPY_NEXT_FIELD(p_out->plant);
		INT_CPY_NEXT_FIELD(p_out->breed);
		INT_CPY_NEXT_FIELD(p_out->plant_limit);
		INT_CPY_NEXT_FIELD(p_out->breed_limit);
		INT_CPY_NEXT_FIELD(p_out->time);
	STD_QUERY_ONE_END();
}

/* 设置t_user_farm表中的某些字段 */
int Cuser_farm::set_farm_web(userid_t userid, user_farm_set_web_in *p_in) 
{
	sprintf( this->sqlstr, " update %s set farm_state = %u, water_time = %u, net = %u, plant = %u, breed = %u,\
			plant_limit = %u, breed_limit = %u, time = %u where userid=%u",
			this->get_table_name(userid), 
			p_in->farm_state,
			p_in->water_time,
			p_in->net,
			p_in->plant,
			p_in->breed,
			p_in->plant_limit,
			p_in->breed_limit,
			p_in->time,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid , USER_ID_NOFIND_ERR );	
}

/* @breif 得到rabbit_list字段
 * @param userid 用户的米米号
 * @param p_out 保存返回的字段
 */
int Cuser_farm::get_rabbit_list(userid_t userid, rabbit_foster_list *p_out)
{
	sprintf( this->sqlstr, "select rabbit_list from %s where userid=%d", 
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out, sizeof (rabbit_foster_list));
	STD_QUERY_ONE_END();
}

/* @breif 更新rabbit_list字段
 * @param userid 用户的米米号
 * @param p_list 要保存的信息
 */
int Cuser_farm::update_rabbit_list(userid_t userid, rabbit_foster_list *p_list)
{
	char mysql_list[mysql_str_len(sizeof (rabbit_foster_list))];

	set_mysql_string(mysql_list,(char*)(p_list), sizeof(rabbit_foster_list));

	sprintf( this->sqlstr, " update %s set rabbit_list='%s' where userid=%u " ,
			this->get_table_name(userid), 
			mysql_list,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid , USER_ID_NOFIND_ERR );	
}

/* @brief 放兔子到youdi那去,并记录ID号和时间
 * @param userid 用户米米号 
 * @param id兔子的ID号
 */
int Cuser_farm :: foster_rabbit(userid_t userid, uint32_t id)
{
	rabbit_foster_list rabbit_list = { };
	uint32_t ret = this->get_rabbit_list(userid, &rabbit_list);
	if (ret != SUCC) {
		return ret;
	}
	if (rabbit_list.count >= 15) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	rabbit_list.item[rabbit_list.count].id = id;
	rabbit_list.item[rabbit_list.count].time = time(NULL);
	rabbit_list.count++;
	ret = this->update_rabbit_list(userid, &rabbit_list);
	return ret;
}

/* @breif 判断是否有兔子到期，如果到期使其返回牧场
 * @param userid 用户的米米号
 * @param p_array 保存达到期限的米米号
 * @param p_count 保存还有多少兔子没达到期限
 */
int Cuser_farm :: del_foster_rabbit(userid_t userid, uint32_t *p_array, uint32_t *p_count)
{
	rabbit_foster_list rabbit_list = { };
	uint32_t ret = this->get_rabbit_list(userid, &rabbit_list);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t index = 0;
	uint32_t now = time(NULL);
	for (uint32_t i = 0; i < rabbit_list.count; i++) {
		if (now - rabbit_list.item[i].time > 3600 * 24) {
			p_array[index++] = rabbit_list.item[i].id;
		} else {
			break;
		}
	}
	if (index > 0) {
		rabbit_list.count -= index;
		memmove((char *)(&rabbit_list) + 4, (char *)(&rabbit_list) + 4 + sizeof(rabbit_foster_info) * index,
				sizeof(rabbit_foster_info) * (rabbit_list.count));
	}
	*p_count = rabbit_list.count;
	return SUCC;
}

/* @brief 设置USER_FARM表的一个字段的值
 * @param usrid 米米号
 * @param type 更改的那个字段值，1表示更改昆虫房
 * @param value 要改变的数值
 */
int Cuser_farm :: update_col(userid_t userid, uint32_t type, uint32_t value)
{
	char col_name[100] = { };
	switch(type) {
	case 1:
		strcpy(col_name, "insect_house");
		break;
	default:
		return VALUE_OUT_OF_RANGE_ERR;
		break;
	}
	uint32_t ret = this->set_int_value(userid, col_name, value);
	return ret;
}

int Cuser_farm::add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count)
{
	uint32_t log_type = 0;
	
	if(attireid >= 1270001 && attireid <= 1279999)//动物
	{
		log_type = 0x0201d000+attireid-1270000;
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

int Cuser_farm::update_count(userid_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where userid = %u",
			this->get_table_name(userid),
			value,
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Cuser_farm::get_count(userid_t userid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);        
        INT_CPY_NEXT_FIELD(count);
    STD_QUERY_ONE_END();       	
}

