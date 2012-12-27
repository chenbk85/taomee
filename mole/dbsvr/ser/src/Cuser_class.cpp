/*
 * =====================================================================================
 *
 *       Filename:  Cuser_class.cpp
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
#include "Cuser_class.h"
#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

using namespace std;
//user 
Cuser_class::Cuser_class(mysql_interface * db, Citem_change_log *p_log ):CtableRoute100x10( db , "USER" , "t_user_class" , 
		"classid") 
{ 
	this->p_item_change_log = p_log;
}

int Cuser_class::del(userid_t classid )
{
	sprintf( this->sqlstr, "delete from %s where classid=%u " ,
			this->get_table_name(classid),  classid); 
	STD_REMOVE_RETURN (this->sqlstr,classid,  CLASSID_NOFIND_ERR);
}


int Cuser_class::insert(userid_t classid, user_class_create_in  * p_in )
{
	char classname_mysql[mysql_str_len(CLASS_NAME_LEN)];
	char classslogan_mysql[mysql_str_len(CLASS_SLOGAN_LEN)];
	char homelist[20];
	char homelist_mysql[mysql_str_len(20)];

	set_mysql_string(classname_mysql, (char*)(p_in->class_name), CLASS_NAME_LEN ); 
	set_mysql_string(classslogan_mysql, (char*)(p_in->class_slogan),CLASS_SLOGAN_LEN); 
	memset(homelist, 0, 20);
    home_attirelist *p_attirelist=(home_attirelist*)homelist; 
    p_attirelist->count=1; 
	p_attirelist->item[0].attireid=1260015;
    p_attirelist->item[0].value[6]=6;   
    p_attirelist->item[0].value[7]=3;   
    p_attirelist->item[0].value[5]=1;   
    set_mysql_string(homelist_mysql, (char *)homelist, 20);

	uint32_t now = time(NULL);
	sprintf(this->sqlstr, "insert into %s values (%u,'%s','%s', %u, %u, %u, %u, %u,%u,0, %u, \
							0x00000000, '%s', 0x00000000, 0x00000000, 0x00000000 )", 
			this->get_table_name(classid),  
			classid,
			classname_mysql,
			classslogan_mysql,
			now,
			p_in->interest,
			p_in->class_logo,
			p_in->class_word,
			p_in->class_color,
			p_in->class_jion_flag,
			p_in->class_access_flag,
			homelist_mysql
	);
	STD_INSERT_RETURN(this->sqlstr, CLASSID_EXISTED_ERR);
}

int Cuser_class::get_noused_home_attirelist(const char * type_str, userid_t classid , 
		noused_homeattirelist * p_list  )
{
	sprintf( this->sqlstr, "select noused_%sattirelist from %s where classid=%u ", 
			type_str,this->get_table_name(classid),classid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
	STD_QUERY_ONE_END();
}

int Cuser_class::update_noused_homeattirelist(const char * type_str, userid_t classid ,
		noused_homeattirelist * p_list )
{
	char mysql_attirelist[mysql_str_len(sizeof(noused_homeattirelist))];
	set_mysql_string(mysql_attirelist,(char*)p_list, 
			4 +sizeof(attire_noused_item  )* p_list->count);
	sprintf( this->sqlstr, " update %s set \
		noused_%sattirelist='%s' \
		where classid=%u " ,
		this->get_table_name(classid), type_str ,
		mysql_attirelist,
		classid );
	STD_SET_RETURN(this->sqlstr,classid,CLASSID_NOFIND_ERR );	
}
int Cuser_class::home_attire_change(const char * type_str, userid_t classid , user_home_attire_change_in *p_in )
{
	if ( p_in->changeflag==2){//减少使用中
		return this->del_home_attire_used(type_str, classid, p_in->attireid );	
	}else if(p_in->changeflag==0 ){//减少未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->del_home_attire_noused(type_str, classid, &item );		
	}else if(p_in->changeflag==1 ){//增加未使用
		attire_count_with_max item;
		item.attireid=p_in->attireid;
		item.count=p_in->value;
		item.maxcount=p_in->maxvalue;
		return this->add_home_attire(type_str, classid, &item );		
	}else 
		return  ENUM_OUT_OF_RANGE_ERR;
}


int Cuser_class::del_home_attire_used(const char * type_str, userid_t classid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	home_attire_item item;  
	item.attireid=attireid;
	int ret=this->get_homeattirelist(type_str,classid,&used_list,&noused_list);
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
	return this->update_used_homeattirelist(type_str,classid,&used_list);
}


int Cuser_class::home_set_attire_noused(const char * type_str, userid_t classid , uint32_t attireid )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	home_attire_item item;  
	item.attireid=attireid;

	int ret=this->get_homeattirelist(type_str,classid,&used_list,&noused_list);
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
	ret=this->add_home_attire(type_str, classid,&noused_item);
	if (ret!=SUCC) return ret; 
	return this->update_used_homeattirelist(type_str,classid,&used_list);
}

int Cuser_class::del_home_attire_noused(const char * type_str, userid_t classid , attire_count_with_max  * p_item)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	attire_noused_item *_start, *_end,*_find , *_tmp ; 

	int ret=this->get_homeattirelist(type_str, classid,&used_list,&list);
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
		return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}	
	return this->update_noused_homeattirelist(type_str, classid,&list);
}

int Cuser_class::add_home_attire(const char * type_str, userid_t classid , attire_count_with_max  * p_item,
		uint32_t is_vip_opt_type)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	uint32_t used_count=0;

	int ret=this->get_homeattirelist(type_str,classid,&used_list,&list);
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
	
	ret = this->update_noused_homeattirelist( type_str, classid,&list);
	/*
	 * 114D协议,下面是道具增减统计支持
	 */
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, classid, p_item->attireid, p_item->count);
	}
	return ret;

}

int Cuser_class::get_simple_info( userid_t classid , user_class_get_simple_info_out  *p_out )
{
	id_list memberlist={};
	sprintf( this->sqlstr, "select \
			class_logo,\
			class_color,\
			class_word,\
			class_jion_flag,\
			class_access_flag,\
			class_name,\
			class_slogan,\
			member_list\
			from %s where classid=%u ", 
			 this->get_table_name(classid),classid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->class_logo);
		INT_CPY_NEXT_FIELD(p_out->class_color);
		INT_CPY_NEXT_FIELD(p_out->class_word);
		INT_CPY_NEXT_FIELD(p_out->class_jion_flag);
		INT_CPY_NEXT_FIELD(p_out->class_access_flag);
		BIN_CPY_NEXT_FIELD (&(p_out->class_name), sizeof(p_out->class_name));
		BIN_CPY_NEXT_FIELD (&(p_out->class_slogan), sizeof(p_out->class_slogan));
		BIN_CPY_NEXT_FIELD (&(memberlist), sizeof(memberlist));
		p_out->class_member_count=memberlist.count;
	STD_QUERY_ONE_END();


}
int Cuser_class::get_flag_info( userid_t classid , user_class_get_flag_info_out *p_out )
{
	id_list memberlist={};
	sprintf( this->sqlstr, "select \
			class_logo,\
			class_color,\
			class_word,\
			class_jion_flag,\
			class_access_flag,\
			member_list\
			from %s where classid=%u ", 
			 this->get_table_name(classid),classid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->class_logo);
		INT_CPY_NEXT_FIELD(p_out->class_color);
		INT_CPY_NEXT_FIELD(p_out->class_word);
		INT_CPY_NEXT_FIELD(p_out->class_jion_flag);
		INT_CPY_NEXT_FIELD(p_out->class_access_flag);
		BIN_CPY_NEXT_FIELD (&(memberlist), sizeof(memberlist));
		p_out->class_member_count=memberlist.count;
	STD_QUERY_ONE_END();
}


int Cuser_class::get_info( userid_t classid , stru_user_class_info *p_out )
{
	sprintf( this->sqlstr, "select \
			interest,\
			class_logo,\
			class_color,\
			class_word,\
			class_jion_flag,\
			class_access_flag,\
			class_name,\
			class_slogan,\
			member_list ,\
			homeattirelist \
			from %s where classid=%u ", 
			 this->get_table_name(classid),classid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->simple_info.interest);
		INT_CPY_NEXT_FIELD(p_out->simple_info.class_logo);
		INT_CPY_NEXT_FIELD(p_out->simple_info.class_color);
		INT_CPY_NEXT_FIELD(p_out->simple_info.class_word);
		INT_CPY_NEXT_FIELD(p_out->simple_info.class_jion_flag);
		INT_CPY_NEXT_FIELD(p_out->simple_info.class_access_flag);
		BIN_CPY_NEXT_FIELD (p_out->simple_info.class_name, 
				sizeof(p_out->simple_info.class_name));
		BIN_CPY_NEXT_FIELD (p_out->simple_info.class_slogan, 
				sizeof(p_out->simple_info.class_slogan));

		BIN_CPY_NEXT_FIELD (&(p_out->memberlist), sizeof(p_out->memberlist));
		BIN_CPY_NEXT_FIELD (&(p_out->home_used_list), sizeof(p_out->home_used_list));
	STD_QUERY_ONE_END();
}

int Cuser_class::get_attirelist( const char * type_str, userid_t classid , home_attirelist * p_home_attirelist)
{
	sprintf( this->sqlstr, "select  %sattirelist from %s where classid=%u ", 
			type_str, this->get_table_name(classid),classid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_home_attirelist  , sizeof(*p_home_attirelist ));
	STD_QUERY_ONE_END();
}

int  Cuser_class::get_homeattirelist(const char * type_str,  userid_t classid ,
		home_attirelist * p_usedlist ,  noused_homeattirelist* p_nousedlist)
{
	sprintf( this->sqlstr, "select %sattirelist, noused_%sattirelist \
			from %s where classid=%u ", 
			type_str,type_str ,this->get_table_name(classid),classid);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_usedlist,sizeof (*p_usedlist ) );
			BIN_CPY_NEXT_FIELD (p_nousedlist, sizeof (*p_nousedlist) );
	STD_QUERY_ONE_END();
}

void Cuser_class::get_home_attire_total(ATTIRE_MAPITEM * p_itemmap,
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
			(*p_itemmap)[attireid]= p_nousedlist->item[i].count;
		}else{
			(*p_itemmap)[attireid]+=p_nousedlist->item[i].count;
		}
	}
}

int Cuser_class::get_attire_count( const char * type_str, userid_t classid, uint32_t  attireid,
		uint32_t count_flag,  uint32_t *p_count )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	int ret;	
	//get old data
	if ((ret=this->get_homeattirelist( type_str,
		classid,&old_usedlist, & old_nousedlist))!=SUCC){
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

int Cuser_class::update_homeattire_all(const char * type_str,  userid_t classid ,  
			home_attirelist * p_usedlist  )
{

	int ret;
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;

	if ((ret=this->get_homeattirelist( type_str,
		classid,&old_usedlist, & old_nousedlist))!=SUCC){
		return ret;
	}
	noused_homeattirelist  new_nousedlist; 
	ret= cal_home_attire_list(&old_usedlist,&old_nousedlist,p_usedlist,
		 HOME_NOUSE_ATTIRE_ITEM_MAX, &new_nousedlist  )	;
	if (ret!=SUCC){
		return  USER_SET_ATTIRE_DATA_ERR;
	}
	return this->update_homeattirelist(type_str, classid,p_usedlist, &new_nousedlist );	

}

int Cuser_class::update_homeattirelist(const char * type_str, userid_t classid ,   
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
		where classid=%u " ,
		 this->get_table_name(classid), 
		 type_str,mysql_usedlist,
		type_str, mysql_nousedlist,
		classid );
	STD_SET_RETURN(this->sqlstr,classid,CLASSID_NOFIND_ERR );	
}
int Cuser_class::update_used_homeattirelist(const char * type_str, userid_t classid ,   
			home_attirelist * p_usedlist )
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];

	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	sprintf( this->sqlstr, " update %s set \
		%sattirelist='%s' \
		where classid=%u " ,
		 this->get_table_name(classid), type_str,
		mysql_usedlist,
		classid );
	STD_SET_RETURN(this->sqlstr,classid,CLASSID_NOFIND_ERR );	
}

int Cuser_class::del_homeattire_noused( noused_homeattirelist *p_list, 
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

int Cuser_class::del_homeattire_used( home_attirelist *p_list , home_attire_item * p_item )
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

int Cuser_class::add_homeattire_noused(noused_homeattirelist *p_list,
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
			return VALUE_OUT_OF_RANGE_ERR;
		}
	}else{ //add 
		if ( p_list->count >= HOME_NOUSE_ATTIRE_ITEM_MAX ){
			DEBUG_LOG("maxlen>default[%u]" ,p_list->count );
			return USER_ATTIRE_MAX_ERR;
		}
		if ( p_item->count>p_item->maxcount){
			DEBUG_LOG("send count[%u] > maxcount[%u]  err",
					p_item->count,p_item->maxcount);
			return VALUE_OUT_OF_RANGE_ERR;
		}

		p_list->item[p_list->count].attireid= p_item->attireid;
		p_list->item[p_list->count].count= p_item->count;
		p_list->count++;
	}		
	return SUCC;
}

int Cuser_class::swap_homeattire(const char *   type_str, userid_t classid , 
				uint32_t oldcount,attire_count* oldlist,
				uint32_t newcount,attire_count_with_max * newlist )
{
	noused_homeattirelist noused_list;
	home_attirelist  used_list;
	attire_count * p_noused_item;
	int real_del_count;
	int ret;
	uint32_t i,j;
	ret=this->get_homeattirelist(type_str, classid,&used_list,&noused_list);
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
	return this->update_homeattirelist(type_str,classid,&used_list,&noused_list  );
}

int Cuser_class::update_used_homeattirelist_with_check(const char * type_str, userid_t classid ,   
			home_attirelist * p_new_usedlist )
{
	int ret;
	home_attirelist old_usedlist;  
	ret=this->get_attirelist( type_str , classid, &old_usedlist);
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
	return this->update_used_homeattirelist( type_str,classid,p_new_usedlist);	
}


int Cuser_class::add_id(userid_t classid ,const  char * id_flag_str , userid_t id )
{
	int ret;
	uint32_t allow_max_count=100;
	id_list idlist={};
	uint32_t * find_index, *idstart, *idend;	

	ret=this->get_idlist(classid,id_flag_str,&idlist);
	if (ret==SUCC){

		idstart=idlist.item;
		idend=idstart+idlist.count;	
		
		find_index=std::find( idstart ,  idend ,  id );
		if (find_index!=idend ) {
			//find 
			return LIST_ID_EXISTED_ERR;
		}else if ( idlist.count >= allow_max_count){
			//out of max value
			return LIST_ID_MAX_ERR;
		}else{
			idlist.item[idlist.count ]=id;
			idlist.count++;
			return this->update_idlist(classid,id_flag_str ,&idlist);
		}
	}else{
		return ret;
	}
}

int Cuser_class::get_idlist(userid_t classid ,const  char * id_flag_str , id_list * idlist )
{
	sprintf( this->sqlstr, "select  %s \
			from %s where classid=%u ", 
		id_flag_str,this->get_table_name(classid), classid);

	//copy idlist  
	STD_QUERY_ONE_BEGIN(this-> sqlstr,CLASSID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(idlist, sizeof(*idlist ));
	STD_QUERY_ONE_END();
}
int Cuser_class::set_medal_info(userid_t classid , user_class_set_medal_info_in *p_in,
		user_class_set_medal_info_out *p_out  )
{
	int ret ;
	p_out->add_attire_count=0;
	ret =this->add_medal(classid, p_in->medaltype,p_in->attireid );
	if (ret==DB_ERR) return ret;
	attire_count_with_max  noused_item;
	noused_item.count=1;
	noused_item.maxcount=1;
	DEBUG_LOG("===medal %u %u",p_in->attireid_1, p_in->attireid_2);
	noused_item.attireid=p_in->attireid_1;
	ret=this->add_home_attire(CLASS_HOME_STR, classid,&noused_item);
	if (ret==SUCC){
		p_out->add_attire_count++;
	}
	noused_item.maxcount = 1;
	noused_item.attireid=p_in->attireid_2;
	ret=this->add_home_attire(CLASS_HOME_STR, classid,&noused_item);
	if (ret==SUCC){
		if (noused_item.maxcount <= 1) {
			p_out->add_attire_count++;
		}
	}
	if (p_out->add_attire_count == 0) {
		return CLASS_HAVE_THIS_MEDAL_ERR;
	}
	return SUCC;
}



int Cuser_class::update_idlist(userid_t classid , const char * id_flag_str ,  id_list * idlist )
{
	char mysql_idlist[mysql_str_len(sizeof(*idlist ) )];
	set_mysql_string(mysql_idlist,(char*)idlist,
			ID_LIST_HEADER_LEN +sizeof (userid_t)*(idlist->count));
	sprintf( this->sqlstr, " update %s set \
		%s ='%s'\
		where classid=%u " ,
		this->get_table_name(classid), 
		id_flag_str,
		mysql_idlist,
		classid );
	STD_SET_RETURN(this->sqlstr,classid,CLASSID_NOFIND_ERR);	
}

int Cuser_class::del_id(userid_t classid ,const char * id_flag_str , userid_t id )
{
	int ret;
	id_list idlist={};
	uint32_t * new_idend, *idstart, *idend;	
	ret=this->get_idlist(classid,id_flag_str,&idlist);
	if (ret==SUCC){
		idstart=idlist.item;
		idend=idstart+idlist.count;	
		new_idend=std::remove( idstart ,  idend ,  id );
		if (new_idend != idend) {
			idlist.count=new_idend-idstart;	
			return this->update_idlist(classid,id_flag_str ,&idlist);
		}else{
			return LIST_ID_NOFIND_ERR ;
		}

	}else{
		return ret;
	}
}
int Cuser_class::change_info(userid_t classid,  user_class_change_info_in * p_in)
{	


	char mysql_class_name[mysql_str_len(sizeof(p_in->class_name))];
	char mysql_class_slogan[mysql_str_len(sizeof(p_in->class_slogan))];

	set_mysql_string(mysql_class_name ,p_in->class_name , sizeof(p_in->class_name));
	set_mysql_string(mysql_class_slogan ,p_in->class_slogan , sizeof(p_in->class_slogan));

	sprintf( this->sqlstr, " update %s set \
			class_name='%s', \
			class_slogan='%s', \
			class_jion_flag=%u, \
			class_access_flag=%u \
			where classid=%u " ,
			this->get_table_name(classid),  
			mysql_class_name,	
			mysql_class_slogan,	
			p_in->class_jion_flag,
			p_in->class_access_flag,
		   	classid); 
	STD_SET_RETURN (this->sqlstr,classid,  CLASSID_NOFIND_ERR);
}

int Cuser_class::get_benefactor_list(userid_t userid, stru_opt_log_list *p_out )
{
	memset(p_out,0,sizeof(*p_out));
	sprintf( this->sqlstr, "select benefactor_list from %s where classid=%d", 
			 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,CLASSID_NOFIND_ERR );
			BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
	STD_QUERY_ONE_END();
}

int Cuser_class::update_benefactor_list(userid_t userid ,stru_opt_log_list * p_list )
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];

	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->items[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set  benefactor_list ='%s' \
		where classid=%u " ,
		this->get_table_name(userid), 
		mysql_list,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,CLASSID_NOFIND_ERR );	
}

int Cuser_class::benefactor_list_add(userid_t userid , stru_opt_log_item * p_item )
{
	int ret;
	stru_opt_log_list list;
	ret=this->get_benefactor_list(userid,&list );
	if(ret!=SUCC) return ret;
	list.insert(p_item );
	return this->update_benefactor_list(userid,&list ); 
}


int Cuser_class::set_medal(userid_t classid, class_medal *p_list)
{
	char mysql_list[mysql_str_len(sizeof (class_medal))];

    set_mysql_string(mysql_list, (char*)p_list,
				            4+sizeof(p_list->items[0])* p_list->count);
	sprintf(this->sqlstr, "update %s set  class_medal ='%s' \
	        where classid=%u " ,
	        this->get_table_name(classid),
	        mysql_list,
	        classid
			);
	STD_SET_RETURN_EX(this->sqlstr, CLASSID_NOFIND_ERR);
}

int Cuser_class :: get_medal(userid_t classid, class_medal *p_list)
{
	sprintf(this->sqlstr, "select class_medal from %s where classid = %u",
			this->get_table_name(classid),
			classid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,CLASSID_NOFIND_ERR );
		BIN_CPY_NEXT_FIELD (p_list, sizeof (class_medal));
	STD_QUERY_ONE_END();
}

int Cuser_class :: add_medal(userid_t classid, uint32_t type, uint32_t attire_id)
{
	class_medal temp;
	memset(&temp, 0, sizeof(class_medal));
	int ret = this->get_medal(classid, &temp);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t i = 0;
	for (i = 0; i < temp.count; i++) {
		if (temp.items[i].type == type) {
			return CLASS_HAVE_THIS_MEDAL_ERR;
		}
	}
	temp.items[temp.count].type = type;
	temp.items[temp.count].attire_id = attire_id;
	temp.count++;
	if (temp.count > 200) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	ret = this->set_medal(classid, &temp);
	return ret;
}



int Cuser_class :: get_attire_list_count(uint32_t classid, uint32_t attireid, uint32_t count_flag, uint32_t *p_count)
{
	home_attirelist usedlist;
	noused_homeattirelist nousedlist;
	uint32_t ret = this->get_homeattirelist("home", classid, &usedlist, &nousedlist);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->get_one_attire_count(usedlist, nousedlist, attireid, count_flag, p_count);
	if (ret != SUCC) {
		return ret;
	}
	return SUCC;
}


int Cuser_class :: get_one_attire_count(home_attirelist old_usedlist, noused_homeattirelist old_nousedlist,
		         uint32_t attireid, uint32_t count_flag, uint32_t *p_count)
{
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


