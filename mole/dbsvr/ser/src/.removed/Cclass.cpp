/*
 * =====================================================================================
 *
 *       Filename:  Cclass.cpp
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
 *
 * =====================================================================================
 */
#include "Cclass.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

Cclass::Cclass(mysql_interface * db ) : CtableRoute10x10(db,"CLASSINFO","t_class","classid"),\
		name_len(CLASS_NAME_LEN), slogan_len(CLASS_SLOGAN_LEN), member_len(CLASS_MEMBER_NUM * 4 + 4)
{ 

}


int Cclass:: insert(uint32_t classid, class_add_in *p_item )
{
	char classname_mysql[mysql_str_len(this->name_len)];
	char classslogan_mysql[mysql_str_len(this->slogan_len)];
	char homelist[20];
	char homelist_mysql[mysql_str_len(20)];

	set_mysql_string(classname_mysql, (char*)(p_item->name), this->name_len); 
	set_mysql_string(classslogan_mysql, (char*)(p_item->slogan), this->slogan_len); 
	memset(homelist, 0, 20);
    class_used_homeattirelist *p_attirelist=(class_used_homeattirelist*)homelist; 
    p_attirelist->count=1; 
	p_attirelist->item[0].attireid=160030;
    p_attirelist->item[0].value[6]=6;   
    p_attirelist->item[0].value[7]=3;   
    p_attirelist->item[0].value[5]=1;   
    set_mysql_string(homelist_mysql, (char *)homelist, 20);

	uint32_t now = time(NULL);
	DEBUG_LOG("classname:%s:slogan:%s", classname_mysql, classslogan_mysql);
	sprintf(this->sqlstr, "insert into %s values (%u,'%s','%s', %u, %u, %u, %u, %u,\
							0x00, '%s', 0x00, 0x00, 0, %u)", 
			this->get_table_name(classid),  
			classid,
			classname_mysql,
			classslogan_mysql,
			p_item->interest,
			p_item->log,
			p_item->word,
			p_item->color,
			p_item->monitor,
			homelist_mysql,
			now
	);
	STD_INSERT_RETURN(this->sqlstr, CLASSID_EXISTED_ERR);
}

int Cclass::get(uint32_t classid, class_get_out *p_out)
{
	sprintf( this->sqlstr, "select  classname, classslogan, log, color, word,\
			monitorid from %s where classid=%u ", 
			this->get_table_name(classid),
			classid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOT_EXISTED_ERR);
		BIN_CPY_NEXT_FIELD(p_out->name, this->name_len);
		BIN_CPY_NEXT_FIELD(p_out->slogan, this->slogan_len);
		INT_CPY_NEXT_FIELD(p_out->log);
		INT_CPY_NEXT_FIELD(p_out->color);
		INT_CPY_NEXT_FIELD(p_out->word);
		INT_CPY_NEXT_FIELD(p_out->monitor);
	STD_QUERY_ONE_END();
}


int Cclass::get_member(uint32_t classid, class_member *p_out)
{
	sprintf( this->sqlstr, "select member from %s where classid=%u ", 
			this->get_table_name(classid),
			classid
		   );

	STD_QUERY_ONE_BEGIN(this-> sqlstr, CLASSID_NOT_EXISTED_ERR);
		BIN_CPY_NEXT_FIELD(p_out, this->member_len);
	STD_QUERY_ONE_END();
}



int Cclass::set_msg(uint32_t groupid, group_msg_item*p_item )
{
	char groupmsg_mysql[mysql_str_len(sizeof (p_item->groupmsg)) ];
	char groupname_mysql[mysql_str_len(sizeof (p_item->groupname) )];

	set_mysql_string(groupmsg_mysql,(char*)(p_item->groupmsg),sizeof (p_item->groupmsg)); 
	set_mysql_string(groupname_mysql,(char*)(p_item->groupname),sizeof (p_item->groupname)); 


	sprintf( this->sqlstr, "update %s set groupname='%s' , groupmsg='%s' \
		   	where groupid=%u  ", 
			this->get_table_name(groupid),  
			groupname_mysql,
			groupmsg_mysql,
			groupid
	);
	STD_SET_RETURN(this->sqlstr,groupid , CLASSID_NOT_EXISTED_ERR );	
}

int Cclass::add_member( uint32_t classid, uint32_t addid, class_member *p_member)
{
	int ret;
	uint32_t allow_max_count=50; 

	memset(p_member, 0, sizeof(class_member));
	ret=this->get_member(classid, p_member);
	if (ret!=SUCC) {
		return ret;
	}
	DEBUG_LOG("member=%u", p_member->count);
	ret=add_id_to_list((idlist*)p_member, addid, allow_max_count );
	if (ret!=SUCC){
		if 	(ret==LIST_ID_MAX_ERR ) 
			return  GROUP_MEMBER_COUNT_MAX_ERR;
		else if (ret==LIST_ID_EXISTED_ERR) 
			return GROUP_MEMBERID_EXISTED_ERR;
		else
			return ret;
	}
	
	return this->set_memberlist(classid, p_member);
}

int Cclass::set_memberlist(uint32_t classid, class_member *p_item)
{
	char memberlist_mysql[mysql_str_len(sizeof (*p_item) )];
	set_mysql_string(memberlist_mysql,(char*)(&(p_item->count)), 
			p_item->count*(sizeof(p_item->memberid[0]))+4); 

	sprintf( this->sqlstr, "update %s set member='%s' where classid= %u", 
			this->get_table_name(classid),  
			memberlist_mysql,
			classid
	    	);
	STD_SET_RETURN(this->sqlstr, classid, CLASSID_NOT_EXISTED_ERR);	

}

int Cclass::del_member( uint32_t classid, uint32_t memberid, class_member * p_memberlist  )
{
	int ret;
	memset(p_memberlist, 0, sizeof(class_member));
	ret=this->get_member(classid, p_memberlist);
	if (ret!=SUCC){
		return ret;
	}

	ret=del_id_from_list((idlist*) p_memberlist,memberid );
	if (ret!=SUCC) {
		return GROUP_MEMBERID_NOFIND_ERR;
	}

	return this->set_memberlist(classid, p_memberlist);
}

int Cclass::del(uint32_t classid)
{
	
	sprintf( this->sqlstr, "delete from  %s where classid= %u", 
			this->get_table_name(classid),  
			classid 
		   );
	STD_SET_RETURN(this->sqlstr, classid , CLASSID_NOT_EXISTED_ERR );	
}


int Cclass::get_noused_attirelist(uint32_t classid, class_noused_homeattirelist *p_list)
{
    	sprintf(this->sqlstr, "select home_noused from %s where classid=%u",
				this->get_table_name(classid),
				classid
				);

    STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
            BIN_CPY_NEXT_FIELD(p_list,sizeof (*p_list));
    STD_QUERY_ONE_END();
}

int Cclass::get_used_attirelist(uint32_t classid, class_used_homeattirelist *p_list)
{
    	sprintf(this->sqlstr, "select home_used from %s where classid=%u",
				this->get_table_name(classid),
				classid
				);

    STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
            BIN_CPY_NEXT_FIELD(p_list, sizeof (*p_list));
    STD_QUERY_ONE_END();
}

int Cclass::update_noused_attirelist(uint32_t classid, class_noused_homeattirelist * p_list)
{
	char mysql_attirelist[mysql_str_len(sizeof(class_noused_homeattirelist))];
	set_mysql_string(mysql_attirelist, (char*)p_list, 
			4 +sizeof(attire_noused_item)* p_list->count);

	sprintf( this->sqlstr, " update %s set home_noused='%s'	where classid=%u " ,
			 this->get_table_name(classid),
			 mysql_attirelist,
			 classid
		   );

	STD_SET_RETURN(this->sqlstr, classid, USER_ID_NOFIND_ERR);	
}

int Cclass::update_used_attirelist(uint32_t classid, class_used_homeattirelist * p_list)
{
	char mysql_attirelist[mysql_str_len(sizeof(class_used_homeattirelist))];
	set_mysql_string(mysql_attirelist, (char*)p_list, 
			4 +sizeof(home_attire_item)* p_list->count);

	sprintf( this->sqlstr, " update %s set home_used='%s' where classid=%u " ,
			 this->get_table_name(classid),
			 mysql_attirelist,
			 classid
		   );

	STD_SET_RETURN(this->sqlstr, classid, USER_ID_NOFIND_ERR);	
}

int Cclass::add_home_attire(uint32_t classid, attire_count_with_max *p_item)
{
	noused_homeattirelist list;
	home_attirelist  used_list;
	uint32_t used_count=0;

	memset(&list, 0, sizeof(noused_homeattirelist));
	memset(&used_list, 0, sizeof(home_attirelist));
	int ret=this->get_homeattirelist(classid, &used_list, &list);
	if (ret!=SUCC) return ret;
	//在使用列表中的个数
	used_count=0;
	DEBUG_LOG("===used_list%u", used_list.count);
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

	ret=this->add_homeattire_noused(&list, p_item);
	if (ret!=SUCC) return ret;
	return this->update_noused_attirelist(classid, &list);
}


void Cclass::get_home_attire_total(ATTIRE_MAPITEM *p_itemmap,
			   	class_used_homeattirelist *p_usedlist,
				class_noused_homeattirelist *p_nousedlist)
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




int  Cclass::get_homeattirelist(uint32_t classid, class_used_homeattirelist *p_usedlist,
							  class_noused_homeattirelist *p_nousedlist)
{
	sprintf(this->sqlstr, "select home_used, home_noused from %s where classid=%u ", 
			this->get_table_name(classid),
			classid
		   );
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_usedlist,sizeof (*p_usedlist ) );
			BIN_CPY_NEXT_FIELD (p_nousedlist, sizeof (*p_nousedlist) );
	STD_QUERY_ONE_END();
}



int Cclass::update_homeattire_all(uint32_t classid, class_used_homeattirelist *p_usedlist, 
								class_noused_homeattirelist * p_nousedlist )
{
	home_attirelist 	old_usedlist;
	noused_homeattirelist old_nousedlist;
	ATTIRE_MAPITEM olditemmap; 	
	ATTIRE_MAPITEM newitemmap; 	
	ATTIRE_MAPITEM::iterator oldit;
	ATTIRE_MAPITEM::iterator newit;
	int ret;	
	
	//get old data
	if ((ret=this->get_homeattirelist(classid, &old_usedlist, &old_nousedlist))!=SUCC) {
		return ret;
	}

	//check
	this->get_home_attire_total(&olditemmap, &old_usedlist, &old_nousedlist);

	this->get_home_attire_total(&newitemmap, p_usedlist, p_nousedlist);		
	//check size    
	if (olditemmap.size()!= newitemmap.size()){
			DEBUG_LOG("type count err db[%u] send[%u]",
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

	return this->update_homeattirelist(classid, p_usedlist, p_nousedlist );	

}

int Cclass::update_homeattirelist(uint32_t classid, home_attirelist *p_usedlist,
		                        noused_homeattirelist * p_nousedlist)
{
	char mysql_usedlist[mysql_str_len(sizeof (home_attirelist))];
	char mysql_nousedlist[mysql_str_len(sizeof (noused_homeattirelist ))];

	set_mysql_string(mysql_usedlist,(char*)p_usedlist, 
			4+sizeof(home_attire_item)* p_usedlist->count);

	set_mysql_string(mysql_nousedlist,(char*)p_nousedlist, 
			4+sizeof(attire_noused_item)* p_nousedlist->count);

	sprintf(this->sqlstr, " update %s set home_used='%s', \
		   home_noused ='%s' where classid=%u " ,
		   this->get_table_name(classid), 
		   mysql_usedlist,
		   mysql_nousedlist,
		   classid
		  );
	STD_SET_RETURN(this->sqlstr, classid,USER_ID_NOFIND_ERR );	
}


int Cclass::add_homeattire_noused(class_noused_homeattirelist *p_list, attire_count_with_max * p_item)
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
			return USER_NOUSED_ATTIRE_MAX_ERR;
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


int Cclass::class_change_info(int classid, class_change_info_in *p_item)
{
	char classname_mysql[mysql_str_len(this->name_len)];
	char classslogan_mysql[mysql_str_len(this->slogan_len)];

	set_mysql_string(classname_mysql, (char*)(p_item->name), this->name_len); 
	set_mysql_string(classslogan_mysql, (char*)(p_item->slogan), this->slogan_len); 

	sprintf(this->sqlstr, "update %s set classname='%s', classslogan='%s', interest=%u,\
			               log = %u, color = %u, word = %u where classid = %u", 
			this->get_table_name(classid),  
			classname_mysql,
			classslogan_mysql,
			p_item->interest,
			p_item->log,
			p_item->word,
			p_item->color,
			classid
	);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cclass::class_get_benefactor(uint32_t classid, class_benefactor_info *p_list)
{
	sprintf(this->sqlstr, "select buyitem from %s where classid = %u",
			 this->get_table_name(classid),
			 classid
	       );

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
            BIN_CPY_NEXT_FIELD (p_list, sizeof(class_benefactor_info));
    STD_QUERY_ONE_END();
}

int Cclass::class_benefactor_attire(uint32_t classid, userid_t userid, attire_count_with_max *item)
{
	class_benefactor_info person_info;
	char temp_buffer[2000];
	memset(&person_info, 0, sizeof(class_benefactor_info));
	this->class_get_benefactor(classid, &person_info);
	if (person_info.count  < 100) {
		person_info.count++;
	}
	memcpy(temp_buffer, &person_info.item[0], sizeof(person_info.item[0]) * 99);
	memcpy(&person_info.item[1], temp_buffer, sizeof(person_info.item[0]) * 99);
	person_info.item[0].userid = userid;
	person_info.item[0].attireid = item->attireid;
	person_info.item[0].time = time(0);
	person_info.item[0].count = item->count;
	char personinfo_mysql[mysql_str_len(sizeof(class_benefactor_info))];
	set_mysql_string(personinfo_mysql, (char*)(&person_info), sizeof(class_benefactor_info));
	sprintf(this->sqlstr, "update %s set buyitem = '%s' where classid = %u",
					       this->get_table_name(classid),
						   personinfo_mysql,
						   classid
		   );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cclass::class_get_state(uint32_t classid, uint32_t *state)
{
	sprintf(this->sqlstr, "select state from  %s where userid = %u",
			this->get_table_name(classid),
			classid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
         INT_CPY_NEXT_FIELD(*state);
	 STD_QUERY_ONE_END();
}


int Cclass::class_set_state(uint32_t classid, uint32_t state)
{
	sprintf(this->sqlstr, "update %s set state = %u where userid = %u",
			this->get_table_name(classid),
			state,
			classid
		   );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_NOFIND_ERR);
}

