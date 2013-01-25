/*
 * =========================================================================
 *
 *        Filename: gvg.cpp
 *
 *        Version:  1.0
 *        Created:  2011-10-18 18:59:06
 *        Description:  
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include  "gvg.h"
#include  <time.h>
#include    "proto.h"
#include    "util.h"
Cgame_item * Cgvg::get_game(mapid_t homeid)
{

	std::map<mapid_t,Cgame_item>::iterator it;
	it=this->homeid_game_map.find(homeid);	
	if (it!=this->homeid_game_map.end()){
		return &(it->second);
	}else{
		return NULL;
	}
}

void Cgvg::add_game(mapid_t homeid,Cgame_item &  game)
{
	this->homeid_game_map[homeid]=game;
	//
	this->end_time_homeid_map.insert(
			std::pair <uint32_t,mapid_t >( game.end_time,homeid) );
}

void Cgvg::add_pre_game( uint32_t userid ,uint32_t level,uint32_t onlineid)
{

	uint32_t opt_level=level/5;
	if (opt_level>=opt_level_count) return ;

	stru_pre_gvg_game_item item;	
	item.add_time=time(NULL);
	item.userid=userid;
	this->pre_gvg_game_map[opt_level].push_back(item);
	stru_pre_user_info pre_item;
	pre_item.opt_level=opt_level;
	pre_item.onlineid=onlineid;
	this->user_info_map[userid]=pre_item ;  
}


void Cgvg::check_start_game( )
{
	for (uint32_t i=0;i<opt_level_count;i++)
	{
		DEBUG_LOG("level =%u",i);
		std::list<stru_pre_gvg_game_item>*p_list=&(this->pre_gvg_game_map[i]);
		//处理人数够了
		while (p_list->size()>=game_user_count){
			DEBUG_LOG("start_new_game ");
			this->start_new_game(p_list);
		}	

		std::list<stru_pre_gvg_game_item>::iterator it;
		it=p_list->begin();
		if (it==p_list->end()){//没有人
			continue;
		}	
		//处理人数不够的情况
		uint32_t now= time_t(NULL)  ;
		
		if(now-(*it).add_time>40  ){//如果等待超过40秒，则查看+下个等级 够不够
			if(i<opt_level_count-1){//有下一个等级
				std::list<stru_pre_gvg_game_item>*p_next_list=
					&(this->pre_gvg_game_map[i+1]); 
				if (p_list->size()+p_next_list->size()>=game_user_count ) {
					while (p_list->size()<game_user_count ){//把下一个等级人移过来
						p_list->push_back(*(p_next_list->begin()));	
						p_next_list->pop_front();
						//本来要重设userid->opt_level 信息,
						//但是start_new_game,将会清空数据，在此就不用处理了
						//this->user_info_map[userid]=opt_level ;  
					}
					this->start_new_game(p_list);
				}
			}
		}

	}
}

void Cgvg::start_new_game(std::list<stru_pre_gvg_game_item>* p_list )
{
	if (p_list->size()<game_user_count )  return;
	//start game 
	Cgame_item game_item;	
	game_item.start_time= time(NULL);
	game_item.end_time=game_item.start_time+600; 
	this->next_dupid++;
	game_item.mapid = ((next_dupid & 0xFFFFFF) | 0xFE000000);
	game_item.mapid = ( game_item.mapid << 32) | 41201;

	for (uint32_t j=0;j<game_user_count;j++){
		stru_pre_gvg_game_item*  p_item=&(*p_list->begin());
		DEBUG_LOG("insert :%u",p_item->userid );
		game_item.need_userid_set.insert(p_item->userid );
		p_list->pop_front();
	}	

	this->homeid_game_map[game_item.mapid ]=game_item;
	
	DEBUG_LOG("game start XXXX");
	//通知开始
	char buff[2048] = {0};
	home_proto_t *pkg = (home_proto_t*)buff;
	//client_proto_t *cli = (client_proto_t*)pkg->body;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);
	PKG_MAP_ID(buff,game_item.mapid,len);
	PKG_UINT32(buff,0,len);
	PKG_UINT32(buff,600,len);
	init_two_head(buff,len,0,61002,0,game_item.mapid ,	1357);
	std::set<uint32_t>::iterator set_it;	

	for(set_it=game_item.need_userid_set.begin();set_it!=game_item.need_userid_set.end();++set_it){
		uint32_t userid=*set_it;
	
		DEBUG_LOG("START  SEND PKG: userid=%u ",userid);
		std::map<uint32_t ,stru_pre_user_info >::iterator it;
		it=this->user_info_map.find(userid);
		if (it!=this->user_info_map.end() ){
			stru_pre_user_info *p_pre_user_info =&(it->second);
			pkg->id=userid;
			pkg->onlineid=p_pre_user_info->onlineid;
			send_pkg_to_client(all_fds[p_pre_user_info->onlineid], pkg, pkg->len);
			DEBUG_LOG("START  SEND PKG   " );
		}
	
		 
	}	
	

}


void Cgvg::del_pre_game( uint32_t userid )
{
	std::map<uint32_t ,stru_pre_user_info >::iterator it;
	it=this->user_info_map.find(userid);
	if (it!=this->user_info_map.end() ){
		stru_pre_user_info *p_pre_user_info =&(it->second);
		std::list<stru_pre_gvg_game_item> * p_list=&this->pre_gvg_game_map[p_pre_user_info->opt_level ] ;
		std::list<stru_pre_gvg_game_item>::iterator list_it=p_list->begin(); 
		while(list_it !=p_list->end() ){
			if (list_it->userid==userid) {
				p_list->erase(list_it);				
				break;
			}
			++list_it;
		}	
		this->user_info_map.erase(it);
	}
}

void Cgvg::del_user( mapid_t homeid,uint32_t userid )
{

	this->user_info_map.erase(userid);
	std::map<mapid_t,Cgame_item>::iterator it;
	it=this->homeid_game_map.find(homeid);
	if(it!=this->homeid_game_map.end()){
		it->second.userid_set.erase(userid);
		if (it->second.userid_set.size()==1 //剩下最后一个人
				&&  time(NULL)- it->second.start_time>30 //防止进两个时，出去一个的情况
				){
			//uint32_t last_userid=*(it->second.need_userid_set.begin());
			//通知他羸了 TODO
			

			//删除游戏
			this->homeid_game_map.erase(homeid );						
		}
	}
}

void Cgvg::cd_map( mapid_t homeid,uint32_t userid )
{
	std::map<mapid_t,Cgame_item>::iterator it;
	it=this->homeid_game_map.find(homeid);
	if(it!=this->homeid_game_map.end()){
		if (it->second.need_userid_set.find(userid)!= it->second.need_userid_set.end()){
			it->second.userid_set.insert(userid );

		}
	}
}

void Cgvg::check_time_out_game()
{
	std::multimap <uint32_t,mapid_t  >::iterator it;
	uint32_t now=time(NULL);
	while( true) {
		//得到最久的游戏
		it= this->end_time_homeid_map.begin();
		if(it==this->end_time_homeid_map.end()) {
			break;
		}

		uint32_t end_time=it->first;
		if (end_time<=now){//超时了
			mapid_t	 homeid=it->second ;
			if ( this->homeid_game_map.find( homeid)
					!=this->homeid_game_map.end()){//是存在超时游戏
				this->do_time_out_game(homeid);
			}
		}else{//还没有超时
			break;
		}


	}
	

}

void Cgvg::do_time_out_game( mapid_t homeid)
{
	Cgame_item &game_item=this->homeid_game_map[homeid];
	//通知超时
	char buff[2048] = {0};
	home_proto_t *pkg = (home_proto_t*)buff;
	//client_proto_t *cli = (client_proto_t*)pkg->body;
	int len = sizeof(home_proto_t) + sizeof(client_proto_t);
	PKG_MAP_ID(buff,game_item.mapid,len);
	PKG_UINT32(buff,0,len);
	PKG_UINT32(buff,4500,len);
	init_two_head(buff,len,0,61002,0,game_item.mapid ,	1357);
	std::set<uint32_t>::iterator set_it;	

	for(set_it=game_item.userid_set.begin();set_it!=game_item.userid_set.end();++set_it){
		uint32_t userid=*set_it;
		std::map<uint32_t ,stru_pre_user_info >::iterator it;
		it=this->user_info_map.find(userid);
		if (it!=this->user_info_map.end() ){
			stru_pre_user_info *p_pre_user_info =&(it->second);
			pkg->id=userid;
			pkg->onlineid=p_pre_user_info->onlineid;
			send_pkg_to_client(all_fds[p_pre_user_info->onlineid], pkg, pkg->len);
			DEBUG_LOG("START  SEND PKG   " );
		}
	
		 
	}	
	//删除游戏
	this->homeid_game_map.erase(homeid );						

}
