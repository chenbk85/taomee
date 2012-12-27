/*
 * =====================================================================================
 *
 *       Filename:  func_comm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:30:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  FUNC_COMM_INC
#define  FUNC_COMM_INC
#include "proto.h" 
#include "common.h" 
#include <map> 
using namespace std;



typedef map<uint32_t , uint32_t>  ATTIRE_MAPITEM;
//计算小屋数据,

inline int  cal_home_attire_list (
		home_attirelist * p_usedlist, // IN :db 中的
		noused_homeattirelist * p_nousedlist,// IN : db 中的 
		home_attirelist * p_new_usedlist,//IN 设置的使用列表
		uint32_t max_type_count, //IN
		noused_homeattirelist * p_new_nousedlist //OUT :计算出的未使用的 数据
		
		)
{
	ATTIRE_MAPITEM  itemmap;
	uint32_t attireid;
	//set old used count
	for (uint32_t i=0;i<p_usedlist->count;i++ )	{
		attireid=p_usedlist->item[i].attireid;
		if ( itemmap.find(attireid)==itemmap.end()) {
			itemmap[attireid]=1;
		}else{
			itemmap[attireid]++;
		}
	}

	//set old noused count
	for (uint32_t i=0;i<p_nousedlist->count;i++ ){
		attireid=p_nousedlist->item[i].attireid;

		if ( itemmap.find(attireid)==itemmap.end()) {
			itemmap[attireid]= p_nousedlist->item[i].count;
		}else{
			itemmap[attireid]+=p_nousedlist->item[i].count;
		}
	}

	//set used count
	for (uint32_t i=0;i<p_new_usedlist->count;i++ )	{
		attireid=p_new_usedlist->item[i].attireid;
		if ( itemmap.find(attireid)==itemmap.end()) {
			return FAIL;
		}else{
			if (itemmap[attireid]>1 ) {
				itemmap[attireid]--;
			}else if  (itemmap[attireid]==1 ) {
				itemmap.erase(attireid );
			}else {
				return FAIL;
			}
		}
	}
	//复制数据
	p_new_nousedlist->count=itemmap.size();
	if (p_new_nousedlist->count > max_type_count ) {
		return FAIL;
	}

	ATTIRE_MAPITEM::iterator it;
	uint32_t i=0;
	for( it=itemmap.begin();it!=itemmap.end();++it){
		p_new_nousedlist->item[i].attireid=it->first;
		p_new_nousedlist->item[i].count=it->second;
		i++;
	}
	return SUCC;
}


inline int  get_attire_count (noused_homeattirelist * p_list, uint32_t attireid)
{
	for (uint32_t i=0;i<p_list->count;i++ )	{
		if(p_list->item[i].attireid==attireid){
			return p_list->item[i].count;
		}
	}
	return 0;
}

inline int  add_attire_count (noused_homeattirelist * p_list,
		attire_count_with_max * p_item ,uint32_t max_type_count )
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
		if ( p_list->count >= max_type_count ){
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

inline uint32_t home_get_attire_used_count(uint32_t attireid,
			stru_home_used_attirelist * p_usedlist_1,
			stru_home_used_attirelist * p_usedlist_2,
			stru_home_used_attirelist * p_usedlist_3)
{
	uint32_t count=0;
	count+=p_usedlist_1->get_attire_count(attireid);
	count+=p_usedlist_2->get_attire_count(attireid);
	count+=p_usedlist_3->get_attire_count(attireid);
	return count;
}

inline int  reduce_attire_count (noused_homeattirelist * p_list,
		attire_count_with_max * p_item )
{
	attire_noused_item *_start, *_end,*_find , *_tmp ; 
	_start=p_list->item ;
	_end=_start+p_list->count;	
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
			p_list->count--;
		}
		else 
			return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}else{
		return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}	

	return SUCC;
}
inline void home_usedlist_to_map(ATTIRE_MAPITEM  &itemmap,
		stru_home_used_attirelist &usedlist ){
		uint32_t attireid;
		for (uint32_t i=0;i<usedlist.count;i++ )	{
		attireid=usedlist.item[i].attireid;
		if ( itemmap.find(attireid)==itemmap.end()) {
			itemmap[attireid]=1;
		}else{
			itemmap[attireid]++;
		}
	}
}
inline void home_noused_list_to_map(ATTIRE_MAPITEM  &itemmap,
		stru_noused_attirelist &noused_attireidlist ){
	uint32_t attireid;
	for (uint32_t i=0;i<noused_attireidlist.count;i++ ){
		attireid=noused_attireidlist.item[i].attireid;
		if ( itemmap.find(attireid)==itemmap.end()) {
			itemmap[attireid]= noused_attireidlist.item[i].count;
		}else{
			itemmap[attireid]+= noused_attireidlist.item[i].count;
		}
	}

}

inline void home_all_to_map(ATTIRE_MAPITEM  &itemmap,
			stru_noused_attirelist 	&noused_attireidlist, 
			stru_home_used_attirelist &usedlist_1,
			stru_home_used_attirelist &usedlist_2,
			stru_home_used_attirelist &usedlist_3
		 ){
	home_noused_list_to_map(itemmap,noused_attireidlist );
	home_usedlist_to_map(itemmap,usedlist_1 );
	home_usedlist_to_map(itemmap,usedlist_2 );
	home_usedlist_to_map(itemmap,usedlist_3 );
}

inline int  reduce_used_attire (stru_home_used_attirelist * p_list,
		uint32_t attireid )
{
	home_attire_item *_start, *_end,*_find , *_tmp ; 
	home_attire_item item;  
	item.attireid=attireid;
	_start= p_list->item ;
	_end=_start+p_list->count;	
	_find=std::find( _start ,  _end , item );
	if (_find!=_end ) {
		//去掉这一项
		_tmp=_find+1;	
		for (;_tmp!=_end; _tmp++){
			memcpy(_tmp-1, _tmp, sizeof(* _tmp));
		}
		p_list->count--;
	}else{
		return USER_ATTIRE_ID_NOFIND_ERR;
	}	

	return SUCC;
}



#endif   /* ----- #ifndef FUNC_COMM ----- */

