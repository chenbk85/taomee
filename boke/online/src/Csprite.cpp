/*
 * =========================================================================
 *
 *        Filename: sprite.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 17:37:07
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */


#include    "Csprite.h"
#include    "svr_proto.h"
#include    "global.h"
#include    "Ctimer.h"
#include    "timer_func.h"
#include    "Cmap_conf.h"
#include    "stat.h"
#include	"Cgame.h"

#include    "./proto/pop_db.h"
#include    "./proto/pop_db_enum.h"
#include    "./proto/pop_online_enum.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <async_serv/async_serv.h>
#ifdef __cplusplus
}
#endif


bool Csprite::send_msg_ex( uint16_t cmdid,int ret, Cmessage *c_in,  bool completed )
{
	static protocol_t proto_header;
	proto_header.len=sizeof(proto_header);
	proto_header.cmd=cmdid;
	proto_header.timestamp=time(NULL) ;
	proto_header.seq=	completed? this->waitseq:0;
	proto_header.ret=ret;
	KDEBUG_LOG (this->id,"CO [%u][%d] ",cmdid, ret  );

    if (send_msg_to_client(this->fdsess, (char*)&proto_header ,c_in ) == -1) {
		KDEBUG_LOG(this->id,"error,failed to send pkg to client:  cmd=%u",  cmdid);
		return false;
	}

	if (completed && this->waitcmd != cmdid ) {
		WARN_LOG("p_waitcmd not equal to proto_cmd\t[%u %u %u]", this->id, this->waitcmd,cmdid );
	}
	if (completed) {
		//DEBUG_LOG("cmdid=%u,seq=%u",this->waitcmd,this->waitseq );
		this->set_waitcmd(0,0);
	}
	return true;	
}

void Csprite::send_succ_login_response()
{
	cli_login_out cli_out;
	cli_out.regflag			= this->flag;
	cli_out.left_xiaomee 	= this->xiaomee;
	cli_out.age				= this->age;
	cli_out.color			= this->color;
	cli_out.last_islandid	= this->last_islandid;
	cli_out.last_mapid		= this->last_mapid;
	cli_out.last_x			= this->last_x;
	cli_out.last_y			= this->last_y;
	memcpy(cli_out.nick, this->nick, sizeof(cli_out.nick));
	this->item_list.get_used_list(cli_out.use_clothes_list, cli_out.effect_list);
	this->task_list.get_nodeid_list(cli_out.last_islandid, cli_out.task_nodeid_list);
	this->find_map_list_get_mapid_list(cli_out.last_islandid, cli_out.find_map_list);
	this->send_succ(&cli_out);
	//this->send_sync_time();
}

void Csprite::send_sync_time()
{
	cli_noti_svr_time_out cli_out;
	cli_out.timestamp = time(NULL);	
	this->send_succ_server_noti(cli_noti_svr_time_cmd, &cli_out);
}

bool Csprite::find_map_list_check_existed(uint32_t islandid,uint32_t mapid )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->findmap_map.find(islandid );
	if (it== this->findmap_map.end()){//没有找到
		return false;
	}

	std::set<uint32_t > & node_set= it->second;
	return node_set.find(mapid ) !=node_set.end();
}

bool Csprite::rand_get_item(uint32_t randid, std::vector<item_t>& item_list)
{
	std::map<uint32_t, Citem_rand_group>::iterator it = g_rand_group_map.find(randid);
	if (it == g_rand_group_map.end()) {
		KERROR_LOG(this->id, "invalid\t[%u]", randid);
		return false;
	}

	const Citem_rand_group& rand_group = it->second;
	
	pop_exchange_in db_in;
	item_exchange_t item_ex;
	item_t item;
	uint32_t index = rand_index(rand_group.item_list, rand_group.randbase);
	// 随机到有效物品
	if (index < rand_group.item_list.size()) {
		const Citem_rand& item_rand = rand_group.item_list[index];	
		for (uint32_t loop = 0; loop < item_rand.item_list.size(); loop ++) {
			item_ex.itemid = item_rand.item_list[loop].itemid;
			item_ex.count = item_rand.item_list[loop].count;

			item.itemid = item_rand.item_list[loop].itemid;
			item.count = item_rand.item_list[loop].count;
			item_list.push_back(item);

			if (item_ex.itemid < spec_item_max) {
				item_ex.max_count = MAX_SPEC_ITEM_COUNT;
				item_ex.day_add_cnt = item_ex.count;

				if (item_ex.itemid == spec_item_xiaomee)
					this->change_xiaomee(item_ex.count);
				else
					this->spec_item_list.change_count(item_ex.itemid, item_ex.count, false);
			} else {
				item_ex.max_count = MAX_ITEM_COUNT;
				this->item_list.add(item_ex.itemid, item_ex.count);
			}
			db_in.add_item_list.push_back(item_ex);
		}
		send_msg_to_db(this, pop_exchange_cmd, &db_in);
	}

	
	return true;
}

bool Csprite::rand_get_item(uint32_t randid, item_t& item_get)
{
	std::map<uint32_t, Citem_rand_group>::iterator it = g_rand_group_map.find(randid);
	if (it == g_rand_group_map.end()) {
		KERROR_LOG(this->id, "invalid\t[%u]", randid);
		return false;
	}

	const Citem_rand_group& rand_group = it->second;
	
	pop_exchange_in db_in;
	item_exchange_t item_ex;
	uint32_t index = rand_index(rand_group.item_list, rand_group.randbase);
	// 随机到有效物品
	if (index < rand_group.item_list.size()) {
		const Citem_rand& item_rand = rand_group.item_list[index];	
		item_ex.itemid = item_rand.item_list[0].itemid;
		item_ex.count = item_rand.item_list[0].count;

		item_get.itemid = item_rand.item_list[0].itemid;
		item_get.count = item_rand.item_list[0].count;

		if (item_ex.itemid < spec_item_max) {
			item_ex.max_count = MAX_SPEC_ITEM_COUNT;
			item_ex.day_add_cnt = item_ex.count;

			if (item_ex.itemid == spec_item_xiaomee)
				this->change_xiaomee(item_ex.count);
			else
				this->spec_item_list.change_count(item_ex.itemid, item_ex.count, false);
		} else {
			item_ex.max_count = MAX_ITEM_COUNT;
			this->item_list.add(item_ex.itemid, item_ex.count);
		}
		db_in.add_item_list.push_back(item_ex);
		send_msg_to_db(this, pop_exchange_cmd, &db_in);
	}
	
	return true;
}


bool Ctask_list::check_existed_node(uint32_t taskid,uint32_t task_nodeid )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->taskmap.find(taskid );
	if (it== this->taskmap.end()){//没有找到
		DEBUG_LOG("TASK NOFIND:taskid=%u,nodeid=%u ",taskid,task_nodeid);
		return false;
	}

	std::set<uint32_t > & node_set= it->second;
	return node_set.find(task_nodeid ) !=node_set.end();
}

void Csprite::find_map_list_get_mapid_list(uint32_t islandid,std::vector<uint32_t> &mapid_list  )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->findmap_map.find(islandid);
	if (it== this->findmap_map.end()){//没有找到
		return  ;
	}
	std::set<uint32_t > & node_set= it->second;
	std::set<uint32_t >::iterator node_it;
	for(node_it=node_set.begin();node_it!=node_set.end();++node_it ) {
		mapid_list.push_back(*node_it  );
	}
	return ;
}

void Ctask_list::get_nodeid_list (uint32_t taskid,std::vector<uint32_t> &nodeid_list  )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->taskmap.find(taskid );
	if (it== this->taskmap.end()){//没有找到
		return  ;
	}
	std::set<uint32_t > & node_set= it->second;
	std::set<uint32_t >::iterator node_it;
	for(node_it=node_set.begin();node_it!=node_set.end();++node_it ) {
		nodeid_list.push_back(*node_it  );
	}
	return ;
}

int Csprite::find_map_list_add(uint32_t islandid,uint32_t mapid)
{
	int map_add_ret = 0;
	if (islandid) {
		if (this->findmap_map.size() == 0 || (this->findmap_map.size() == 1 
			&& this->findmap_map.find(0) != this->findmap_map.end())) {
			map_add_ret = 2;
		}
	}

	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->findmap_map.find( islandid);
	if (it== this->findmap_map.end()){//没有找到
		std::set< uint32_t > tmp_map_set;
		std::pair< std::map<uint32_t,std::set<uint32_t> >::iterator,bool > tmp_ret;
		tmp_ret=this->findmap_map.insert(
				std::pair< uint32_t ,std::set<uint32_t > >(
					islandid ,tmp_map_set ));
		if( tmp_ret.second ){//插入成功
			it=tmp_ret.first;
		}else{
			return 0;
		}
	}


	std::set<uint32_t > & node_set= it->second;

	std::pair<std::set<uint32_t>::iterator,bool> ret = node_set.insert( mapid);	
	if (ret.second) {
		map_add_ret |= 0x1;
	} else {
		map_add_ret = 0;
	}

	return map_add_ret;
}


bool Ctask_list::del(uint32_t taskid,uint32_t task_nodeid )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->taskmap.find(taskid );
	if (it== this->taskmap.end()){//没有找到
			return false;
	}

	std::set<uint32_t > & node_set= it->second;

	std::pair<std::set<uint32_t>::iterator,bool> ret;
	node_set.erase(task_nodeid); 	
	return true;
}

bool Ctask_list::add(uint32_t taskid,uint32_t task_nodeid )
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	it=this->taskmap.find(taskid );
	if (it== this->taskmap.end()){//没有找到
		std::set< uint32_t > tmp_task_node_set;
		std::pair< std::map<uint32_t,std::set<uint32_t> >::iterator,bool > tmp_ret;
		tmp_ret=this->taskmap.insert(
				std::pair< uint32_t ,std::set<uint32_t > >(
					taskid  ,tmp_task_node_set ));
		if( tmp_ret.second ){//插入成功
			it=tmp_ret.first;
		}else{
			return false;
		}
	}


	std::set<uint32_t > & node_set= it->second;

	std::pair<std::set<uint32_t>::iterator,bool> ret;
	ret = node_set.insert(task_nodeid); 		// no new element inserted
	return ret.second;

}

void Csprite::find_map_list_init(const std::vector<find_map_t>  & find_map_list )
{
	for (uint32_t i=0;i<find_map_list.size();i++){
		const find_map_t *p_item=&(find_map_list[i]);
		this->find_map_list_add(p_item->islandid,p_item->mapid);	
	}
}

void Csprite::reset_online_timer()
{
	if (this->id == 256243833 || this->id == 258791727 || this->id == 255275187 || this->id == 50181)
		return ;

	uint32_t oltime = this->get_day_oltime();
	if (oltime >= GAME_MAX_SEC) {						// 超过5小时每30分钟提示5小时
		g_timer_map->add_timer(TimeVal(GAME_REST_SEC), n_noti_total_oltime,this->id);
	} else if (oltime >= GAME_NORMAL_SEC) {	// 超过3小时每30分钟提示3小时(如当前距5小时小于30分钟，则先提示5小时)
		uint32_t timer_seconds = GAME_MAX_SEC - oltime;
		if (timer_seconds > GAME_REST_SEC)
			timer_seconds = GAME_REST_SEC;
		g_timer_map->add_timer(TimeVal(timer_seconds), n_noti_total_oltime,this->id);
	} else {								// 不足3小时每45分钟提示(如当前距3小时不足45分钟，则先提示3小时)
		uint32_t timer_seconds = GAME_NORMAL_SEC - oltime;
		if (timer_seconds > GAME_REST_SEC_2) {
			g_timer_map->add_timer(TimeVal(GAME_REST_SEC_2), n_noti_45min_online,this->id);
		} else {
			g_timer_map->add_timer(TimeVal(timer_seconds), n_noti_total_oltime,this->id);
		}
	}

}

void Ctask_list::show()
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	for( it=this->taskmap.begin() ; it!=this->taskmap.end() ; ++it ) {
		std::set<uint32_t > & node_set= it->second;
		std::set<uint32_t >::iterator set_it;
		for( set_it=node_set.begin() ; set_it!=node_set.end(); ++set_it ) {
			KDEBUG_LOG(0,"task info:%u ,%u ",it->first,*set_it  );
		}
	}
}

//在收到报文，返回报文时都会设置
void Csprite:: set_waitcmd(uint16_t cmdid, int seq )
{
	this->waitcmd=cmdid;
	this->waitseq=seq;
	if(this->waitcmd==0 && this->cache_buffer_lst.size()>0){
		//完成了,并有数据还没有分发
		g_timer_map->add_timer(TimeVal(now) ,n_deal_cahce_cmd,this->id);
	}	
}

Csprite_map::Csprite_map()
{
	this->game_map = new Cgame_map();
}

Csprite_map::~Csprite_map()
{
	delete this->game_map;
	this->game_map = NULL;
}

bool Csprite_map::del_sprite_by_fd (  uint32_t fd ) 
{
	//清空fd <->userid 映射
	uint32_t userid=this->fd_userid_map.unmap_fd_userid(fd);
	if  (userid==0 ){
		return false;
	}

	std::map<uint32_t, Csprite > ::iterator it; 
	it=this->spritemap.find(userid);

	if(it!=this->spritemap.end()){//找到了
		Csprite *p=&(it->second);

		if (p->game_map_id>0 ){//还在游戏中
			KDEBUG_LOG(p->id," p->game_map_id>0" );
			g_sprite_map->game_map->noti_map_game_stat(p->game_map_id, 2);
			g_sprite_map->game_map->game_user_left( p->game_map_id,p->id);
		}

		p->unseat_game();

		if (p->get_is_in_muti_map()){
			//通知有用户离开
			cli_noti_user_left_map_out cli_out;
			cli_out.obj_userid=p->id;
			this->noti_to_map_others(p,cli_noti_user_left_map_cmd,&cli_out  );

			//清空userid <->> mapid
			this->online_map[ p->get_onine_mapid()].erase(p->id);
		}
	
		//清空用户的定时器
		g_timer_map->del_by_key(p->id );

		if (p->is_logined()){
			//通知下线
			send_sw_report_user_onoff(p,false );
			//同步db
			pop_logout_in db_in;
			uint32_t online_time_once = p->get_once_oltime();

			db_in.last_islandid=p->get_last_islandid();
			db_in.last_mapid=p->get_last_mapid();
			db_in.last_x=p->last_x;
			db_in.last_y=p->last_y;
			db_in.last_login=p->login_time;
			db_in.online_time=p->online_time+online_time_once ;
			db_in.last_online_id = get_server_id();
	 		send_msg_to_db(p,pop_logout_cmd,&db_in );
			KDEBUG_LOG(p->id,"ONLINETIME:%u",online_time_once );

			// 更新当日在线时长
			p->spec_item_list.change_count(spec_item_day_oltime, online_time_once);

			// 添加统计项
			stat_log_online_time(p, online_time_once);
			stat_log_login_off(p, online_time_once);
		}
	}else{
		return false;
	}

	//清空map <->
	this->spritemap.erase(it );
	return true;

}
	
void Csprite_map::cd_map( uint32_t userid, cli_walk_in * p_in , bool is_login  )
{
	KDEBUG_LOG(userid,"cd map:%u, %u" ,p_in->islandid,p_in->mapid  );
	
	Csprite *p =this->get_sprite(userid);
	if (p==NULL ) {
		KDEBUG_LOG( userid ,"ERROR, userid,nofind " );
		return ;
	}

 	bool old_is_in_muti_map=p->get_is_in_muti_map(); 
	//离开原先的地图
	if (!is_login && old_is_in_muti_map ){
		uint32_t old_mapid=p->get_onine_mapid();
		this->online_map[old_mapid].erase(p->id);
		//通知有用户离开
		cli_noti_user_left_map_out cli_out;
		cli_out.obj_userid=p->id;
		this->noti_to_map_others(p,cli_noti_user_left_map_cmd,&cli_out  );
	}

	p->set_islandid_and_mapid(p_in->islandid ,p_in->mapid);
	p->last_x=p_in->x;
	p->last_y=p_in->y;

	//进入地图
	if(p->get_is_in_muti_map()){
		uint32_t online_mapid=p->get_onine_mapid();

		this->online_map[online_mapid].insert(userid);
		DEBUG_LOG(" add online_mapid=%u,userid=%u ", online_mapid,userid );
		//通知有用户进入
		cli_noti_one_user_info_out cli_out;
		p->get_user_info(&(cli_out.user_info));
		this->noti_to_map_others(p,cli_noti_one_user_info_cmd ,&cli_out );
	}

	if (p->is_logined()) {
		pop_logout_in db_in;
		db_in.last_islandid=p->get_last_islandid();
		db_in.last_mapid=p->get_last_mapid();
		db_in.last_x=p->last_x;
		db_in.last_y=p->last_y;
		db_in.last_login=p->login_time;
		db_in.online_time=p->online_time;
		db_in.last_online_id = get_server_id();
		send_msg_to_db(p,pop_logout_cmd,&db_in );
	}
}

void Csprite_map::noti_all(uint32_t cmd, Cmessage * p_msg, uint32_t except_uid)
{
	std::map<uint32_t, Csprite >::iterator it; 
	for (it = spritemap.begin(); it != spritemap.end(); it ++) {
		Csprite& sp = it->second;	
		if (sp.id == except_uid)
			continue;
		sp.send_succ_server_noti(cmd, p_msg);
	}
}

void  Csprite_map::noti_to_map_ex(Csprite* p, uint32_t cmdid , 
		Cmessage * p_msg , uint32_t obj_userid, bool is_all ){

	if ( obj_userid==0 ){
		uint32_t online_mapid=p->get_onine_mapid();
		std::set<uint32_t> &userid_set=this->online_map[online_mapid ];
		std::set<uint32_t>::iterator  it;
		for(it= userid_set.begin();it!=userid_set.end();++it  ){
			uint32_t userid=*it;
			Csprite *obj_p=this->get_sprite( userid);
			if(obj_p==NULL){
				KDEBUG_LOG( p->id,"ERROR, noti user cd map, objuserid,nofind :%u",
						userid  );
				continue;
			}
			if (!is_all &&  obj_p->id ==p->id ){//是自己
				continue;
			}

			obj_p->send_succ_server_noti(cmdid ,p_msg );
		}
	}else{//发给某个人
		Csprite *obj_p=this->get_user_in_same_map(p,obj_userid) ;
		if(obj_p){
			obj_p->send_succ_server_noti(cmdid ,p_msg );
		}else{
			KDEBUG_LOG( p->id,"ERROR, noti user cd map, objuserid,nofind :%u",
					obj_userid  );
			return ;
		}
	}
}

Csprite * Csprite_map::get_user_in_same_map( Csprite *p,uint32_t obj_userid )
{
	uint32_t online_mapid=p->get_onine_mapid();
	std::set<uint32_t> &userid_set=this->online_map[online_mapid ];
	if ( userid_set.find(obj_userid) != userid_set.end()){
		return  this->get_sprite(obj_userid );
	}
	return  NULL;
}

// 给用户加经验
void Csprite::change_experience(int count, bool sync_db)
{
	uint32_t old_level = this->level;
	this->spec_item_list.change_count(spec_item_experience, count, sync_db);
	this->reset_level();

	if (count > 0) {
		cli_noti_user_level_up_out cli_out;
		cli_out.src_userid = this->id;
		cli_out.level = this->level;
		cli_out.addexp = count;
		cli_out.experience = this->spec_item_list.get_count(spec_item_experience);
		KDEBUG_LOG(this->id, "ADD EXP\t[%u]", cli_out.addexp);
		if (this->level > old_level) {
			cli_out.is_level_up = 1;

			if (this->level >= 2 && old_level < 2) {
				cli_out.itemid = 300027;
			} else if (this->level >= 10 && old_level < 10) {
				cli_out.itemid = 300028;
			} else if (this->level >= 20 && old_level < 20) {
				cli_out.itemid = 300029;
			} else if (this->level >= 30 && old_level < 30) {
				cli_out.itemid = 300030;
			} else if (this->level >= 40 && old_level < 40) {
				cli_out.itemid = 300031;
			} else if (this->level >= 50 && old_level < 50) {
				cli_out.itemid = 300032;
			} else if (this->level >= 60 && old_level < 60) {
				cli_out.itemid = 300033;
			} else if (this->level >= 70 && old_level < 70) {
				cli_out.itemid = 300034;
			} else if (this->level >= 80 && old_level < 80) {
				cli_out.itemid = 300035;
			} else if (this->level >= 90 && old_level < 90) {
				cli_out.itemid = 300036;
			} else if (this->level >= 100 && old_level < 100) {
				cli_out.itemid = 300037;
			}
		}

		if (cli_out.itemid) {
			if (!this->item_list.check_enough(cli_out.itemid, 1)) {
				this->item_list.add(cli_out.itemid, 1);
				pop_exchange_in db_in;
				item_exchange_t item;
				item.itemid = cli_out.itemid;
				item.count = 1;
				item.max_count = 1;
				db_in.add_item_list.push_back(item);
				send_msg_to_db(this, pop_exchange_cmd, &db_in);
			} else {
				cli_out.itemid = 0;
			}
		}

		if (this->get_is_in_muti_map()) {
			g_sprite_map->noti_to_map_all(this, cli_noti_user_level_up_cmd, &cli_out);
		} else {
			this->send_succ_server_noti(cli_noti_user_level_up_cmd, &cli_out);
		}
	}
}

bool Csprite::check_change_xiaomee(int32_t change_value )
{
  int value=int(this->xiaomee)+ change_value;	
  return value >= 0 && value <= MAX_SPEC_ITEM_COUNT;
}

bool Csprite::change_xiaomee(int32_t change_value )
{
	if (this->check_change_xiaomee(change_value )){
	  	this->xiaomee+=change_value;
	  	return  true;
	}else{
		KERROR_LOG(this->id, "fail: xiaomee=%u, change_value=%d", this->xiaomee,change_value );
	  	return  false;
	}
}

void  Csprite::set_islandid_and_mapid(uint32_t islandid,uint32_t mapid )
{
	this->last_islandid=islandid;
	this->last_mapid=mapid;
	//设置该场景是否是多人模式
	this->is_in_muti_map=g_map_conf_map.check_multi(
			this->last_islandid,this->last_mapid );
	DEBUG_LOG("cd islandid=%u  mapid=%u  ,multi =%u ", 
		  this->last_islandid,this->last_mapid ,int( this->is_in_muti_map) 	);
}



void Csprite::get_user_info( online_user_info_t * p_item	)
{
	p_item->obj_userid=this->id;	
	memcpy(p_item->nick,this->nick,sizeof(this->nick));
	p_item->color=this->color;
	p_item->xiaomee = this->xiaomee;
	p_item->level = this->level;
	p_item->experience = this->spec_item_list.get_count(spec_item_experience);
	KDEBUG_LOG(this->id, "EXP LV\t[%u %u]", p_item->level, p_item->experience);
	p_item->x=this->last_x;
	p_item->y=this->last_y;
	this->item_list.get_used_list(p_item->use_clothes_list, p_item->effect_list, p_item->medal_list);
	//p_item->game_point = spec_item_list.get_count(spec_item_game_point);
	p_item->game_point = this->get_valid_gamepoint();
}

void Csprite::reset_level()
{
	double exp = this->spec_item_list.get_count(spec_item_experience);
	this->level = quadratic(5, -5, -exp);
	stat_log_user_level(this->id, this->level);
}

void Csprite_map::set_all_user_offline( ){
	std::map<uint32_t, Csprite > ::iterator it ; 
	std::vector<int > fd_list;
	for (it=this->spritemap.begin();it!=this->spritemap.end();++it ){
		KDEBUG_LOG(it->second.id, "SET USER OFFLINE MIDNIGHT\t[fd=%d]", it->second.fdsess->fd);
		fd_list.push_back( it->second.fdsess->fd);
		noti_cli_leave_out cli_out;
		cli_out.reason = 1;
		it->second.send_succ_server_noti(noti_cli_leave_cmd, &cli_out);
	}

	for (uint32_t i=0; i<fd_list.size();i++ ){
		close_client_conn(fd_list[i] );
	}
}


void Cuser_log_list::init( std::vector<user_log_t> & log_list )
{
	std::vector<user_log_t>::iterator it;
	Cuser_log user_log_item;
	for(it=log_list.begin();it!=log_list.end();++it ){
		user_log_t *p_item= &(*it);
 		user_log_item.logtime=p_item->logtime;
 		user_log_item.v1=p_item->v1;
 		user_log_item.v2=p_item->v2;
		this->user_log_list.push_back(user_log_item);
	}
}

bool Citem_list::item_can_change(uint32_t itemid)
{
	//看看是不是公共可换
	if (g_item_conf_map.check_can_change(itemid))
		return true;

	// 看看是不是卡片包含的
	std::map<uint32_t, std::set<uint32_t> >::iterator iter;
	iter = item_card_map.find(itemid);
	// 不属于任何卡片
	if (iter == item_card_map.end())
		return false;

	std::set<uint32_t>& card_list = iter->second;
	std::set<uint32_t>::iterator it = card_list.begin();
	for (; it != card_list.end(); it ++) {
		if (this->item_list.find(*it) != this->item_list.end())
			return true;
	}

	return false;
}

void Cuser_log_list::add(uint32_t v1, uint32_t v2)
{
	Cuser_log user_log;
	user_log.logtime = time(NULL);
	user_log.v1 = v1;
	user_log.v2 = v2;
	this->user_log_list.push_back(user_log);
}

void Cuser_log_list::add(Cuser_log & item )
{
	this->user_log_list.push_back(item);
}

void Cuser_log_list::get_list( std::vector<user_log_t> & log_list )
{
	std::vector<Cuser_log>::iterator it;
	user_log_t user_log_item;
	for(it=this->user_log_list.begin(); it!=this->user_log_list.end();++it ){
		Cuser_log *p_item= &(*it);
 		user_log_item.logtime=p_item->logtime;
 		user_log_item.v1=p_item->v1;
 		user_log_item.v2=p_item->v2;
		log_list.push_back(user_log_item);
		DEBUG_LOG("USER LOG\t[%u %u %u]", p_item->logtime, p_item->v1, p_item->v2);
	}
}


bool Citem_list::check_enough(uint32_t itemid, uint32_t count )
{
	std::map<uint32_t ,Citem_ex >::iterator it;
	it=this->item_list.find(itemid);
	if(it==this->item_list.end()){
		return false;
	}
	return (it->second.count>=count);
}	

bool Citem_list::add(uint32_t itemid, uint32_t count, bool is_set_use )
{
	std::map<uint32_t ,Citem_ex >::iterator it;
	it=this->item_list.find(itemid);
	if(it==this->item_list.end()){
		Citem_ex item;			
		item.itemid=itemid;
		item.count=count;
		item.use_count=0;
		std::pair< std::map<uint32_t,Citem_ex >::iterator,bool > tmp_ret;
		tmp_ret=this->item_list.insert(
				std::pair< uint32_t ,Citem_ex  >(
					itemid ,item ));
		if( tmp_ret.second ){//插入成功
			it=tmp_ret.first;
		}else{
			return false;
		}
	}else{
		it->second.count+=count;
	}

	if (is_set_use ){
		it->second.use_count=1;
	}

	return true;
}


bool Citem_list::del(uint32_t itemid, uint32_t count )
{
	std::map<uint32_t ,Citem_ex >::iterator it;
	it=this->item_list.find(itemid);
	if(it==this->item_list.end()){
		return false;
	}

	if (it->second.count>=count ){
		it->second.count-=count;
		if (it->second.count<it->second.use_count){
			it->second.use_count=it->second.count;
		}
		if (it->second.count==0){
			this->item_list.erase(it);
		}
		return true;
	}else{
		return false;
	}
}
bool Citem_list::set_cloth_used_list(std::vector<uint32_t> itemid_list, 
		std::vector<uint32_t> &del_can_change_list,
		std::vector<uint32_t> &set_noused_list,
		std::vector<uint32_t> &add_can_change_list,
		std::vector<uint32_t> &set_used_list
		)
{
	//检查一下物品是否存在	
	std::set<uint32_t> set_used_set;
	for (uint32_t i=0;i< itemid_list.size();i++ ){
		uint32_t itemid=itemid_list[i];

		std::map<uint32_t ,Citem_ex >::iterator it = this->item_list.find(itemid);
		if (it != this->item_list.end()) {			// 如果存在，则直接设置使用
			set_used_list.push_back(itemid);
			set_used_set.insert(itemid);
		} else if (this->item_can_change(itemid)) {	// 不存在，可交换获得
			add_can_change_list.push_back(itemid);
		} else {									// 否则出错
			ERROR_LOG("nofind id :%u", itemid);
			return false;
		}
		/*
		if ( !this->item_can_change(itemid)){
			//在可更换的列表中没有
			std::map<uint32_t ,Citem_ex >::iterator it = this->item_list.find(itemid);
			if(it==this->item_list.end()){//不存在列表中
				ERROR_LOG("nofind id :%u", itemid);
				return false;
			}
			set_used_list.push_back(itemid);
		}else{
			add_can_change_list.push_back(itemid);
		}
		*/
	}

	std::map<uint32_t ,Citem_ex >::iterator map_it;
	//设置装扮都未使用,这个部分代码要小心修改
	for ( map_it=this->item_list.begin(); map_it!=this->item_list.end(); ) {
		uint32_t itemid=map_it->second.itemid;
		uint32_t use_count=map_it->second.use_count;
		if (g_item_conf_map.is_cloth(itemid) && use_count>0 ){	// 只有装扮才设置未使用
			if (this->item_can_change(itemid) 
				&& set_used_set.find(itemid) == set_used_set.end()){				// 在可更换的列表
				del_can_change_list.push_back(itemid);
				this->item_list.erase(map_it++);		
			}else{												//自己的
				set_noused_list.push_back(itemid);
				map_it->second.use_count=0;
				++map_it;
			}
		}else{
			++map_it;
		}
	}

	//设置使用中,自己的物品
	for (uint32_t i=0;i< set_used_list.size();i++ ){
		std::map<uint32_t ,Citem_ex >::iterator it;
		this->item_list[set_used_list[i]].use_count=1;
	}
	//设置使用中,可更换的物品
	for (uint32_t i=0;i< add_can_change_list.size();i++ ){
		uint32_t itemid=add_can_change_list[i];
		Citem_ex *p_item=&(this->item_list[itemid ]);
		p_item->itemid=itemid;
		p_item->use_count=1;
		p_item->count=1;
	}

	return true;
}

bool Citem_list::set_effect_used(uint32_t itemid, uint32_t& unset_itemid)
{
	uint32_t type = g_item_conf_map.get_type(itemid);
	if (!type) 
		return false;

	std::map<uint32_t, Citem_ex >::iterator it,itlow,itup;	

	it = this->item_list.find(itemid);
	if (it == this->item_list.end()) 
		return false;
	it->second.use_count = 1;

	itlow=this->item_list.lower_bound(200000);	
	itup= this->item_list.end();	

	for ( it=itlow ; it != itup ; it++ ){
		if (it->second.use_count && it->second.itemid != itemid) {
			if (type == g_item_conf_map.get_type(it->second.itemid)) {
				unset_itemid = it->second.itemid;
				it->second.use_count = 0;
				break;
			}
		}
	}

	return true;
}

bool Citem_list::set_effect_unused(uint32_t itemid)
{
	std::map<uint32_t, Citem_ex>::iterator it;
	it = this->item_list.find(itemid);

	if (it == this->item_list.end()) {
		ERROR_LOG("nofind id\t[%u]", itemid);
		return false;
	}

	if (it->second.use_count == 0) {
		ERROR_LOG("no use id\t[%u]", itemid);
		return false;
	}

	it->second.use_count = 0;
	return true;
}

// 得到用户正在使用中的物品 和 勋章数量
void Citem_list::get_used_list( std::vector<uint32_t> & use_cloth_list, 
								std::vector<uint32_t> & effect_list,
								std::vector<uint32_t> & medal_list)
{
	std::map<uint32_t ,Citem_ex >::iterator map_it;
	for ( map_it=this->item_list.begin(); map_it!=this->item_list.end(); ++map_it ) {
		uint32_t itemid=map_it->second.itemid;
		uint32_t use_count=map_it->second.use_count;
		if (use_count>0 ){
			if (g_item_conf_map.is_cloth(itemid)) {
				use_cloth_list.push_back(itemid);		
			} else {
				effect_list.push_back(itemid);
			}
		}

		if (g_item_conf_map.is_medal(itemid)) {
			medal_list.push_back(itemid);
		}
	}

}

// 得到用户正在使用中的物品 
void Citem_list::get_used_list( std::vector<uint32_t> & use_cloth_list, 
								std::vector<uint32_t> & effect_list)
{
	std::map<uint32_t ,Citem_ex >::iterator map_it;
	for ( map_it=this->item_list.begin(); map_it!=this->item_list.end(); ++map_it ) {
		uint32_t itemid=map_it->second.itemid;
		uint32_t use_count=map_it->second.use_count;
		if (use_count>0 ){
			if (g_item_conf_map.is_cloth(itemid)) {
				use_cloth_list.push_back(itemid);		
			} else {
				effect_list.push_back(itemid);
			}
		}
	}
}
void Citem_list::init(const std::vector<item_ex_t> & db_item_list)
{
	for (uint32_t i=0;i<db_item_list.size();i++){
		const item_ex_t *p_item=&(db_item_list[i]);
		Citem_ex  item;
		item.itemid=p_item->itemid;
		item.use_count=p_item->use_count;
		item.count=p_item->count;
		this->item_list[p_item->itemid]=item;
	}
}

void Citem_list::get_comprise_item_list(uint32_t itemid, std::vector<item_t> & comprise_list)
{
	std::map<uint32_t, std::vector<uint32_t> >::iterator it;
	it = g_item_conf_map.cardid_comprise_map.find(itemid);
	if (it == g_item_conf_map.cardid_comprise_map.end())
		return;

	for (uint32_t loop = 0; loop < it->second.size(); loop ++) {
		std::map<uint32_t, Citem_ex>::iterator it_ex;
		it_ex = this->item_list.find(it->second[loop]);
		if (it_ex != this->item_list.end()) {
			item_t item;
			DEBUG_LOG("EEEEEEEEEEE\t[%u %u]", item.itemid, item.count);
			item.itemid = it_ex->second.itemid;
			item.count = it_ex->second.count;
			comprise_list.push_back(item);
		}
	}
}

bool Cspec_item_list::init(Csprite* obj_p, const std::vector<item_day_limit_t>& item_day_list)
{
	this->p = obj_p;
	for (uint32_t loop = 0; loop < item_day_list.size(); loop ++) {
		if (item_day_list[loop].itemid < spec_item_max) {
			Cday_item item;
			item.itemid = item_day_list[loop].itemid;
			item.total = item_day_list[loop].total;
			item.daycnt = item_day_list[loop].daycnt;
			item_list.insert(std::pair<uint32_t, Cday_item>(item.itemid, item));
		}
	}

	return true;
}

void Cspec_item_list::get_list(uint32_t start, uint32_t end, std::vector<item_day_limit_t>& item_day_list)
{
	std::map<uint32_t, Cday_item >::iterator it,itlow,itup;	
	itlow=this->item_list.lower_bound(start);	
	itup= this->item_list.upper_bound(end);	

	for ( it=itlow ; it != itup ; it++ ){
		item_day_limit_t item_day;
		item_day.itemid = it->second.itemid;
		item_day.total = it->second.total;
		item_day.daycnt = it->second.daycnt;
		item_day_list.push_back(item_day);
	}

}

uint32_t Cspec_item_list::add(uint32_t itemid, uint32_t count, bool is_sync_db)
{
	// 如果没有找到对应的配置信息 则不能增加
	item_conf_t* p_item_conf = g_item_conf_map.get_item_conf(0, itemid);
	if (!p_item_conf) {
		ERROR_LOG("invalid spec item add\t[%u %u]", itemid, count);
		return 0;
	}

	// 增加物品
	std::map<uint32_t, Cday_item>::iterator it = item_list.find(itemid);
	if (it == item_list.end()) {
		Cday_item item;
		item.itemid = itemid;

		// 是否到达总上限
		if (count > p_item_conf->total_max)
			count = p_item_conf->total_max;

		// 增加每日上限
		if (count > p_item_conf->day_max)
			count = p_item_conf->day_max;
		item.daycnt = count;

		item.total = count;
		item_list.insert(std::pair<uint32_t, Cday_item>(itemid, item));
	} else {
		// 是否到达总上限
		if (it->second.total + count > p_item_conf->total_max)
			count = p_item_conf->total_max - it->second.total;

		// 增加每日上限
		if (it->second.daycnt + count > p_item_conf->day_max)
			count = p_item_conf->day_max - it->second.daycnt;
		it->second.daycnt += count;

		it->second.total += count;
	}

	// 同步到db
	if (count && is_sync_db) {
		pop_exchange_in db_in;
		item_exchange_t item;
		item.itemid = itemid;
		item.count = count;
		item.day_add_cnt = count;
		item.max_count = p_item_conf->total_max;
		db_in.add_item_list.push_back(item);
		send_msg_to_db(p, pop_exchange_cmd, &db_in);
	}

	return count;
}

bool Cspec_item_list::check_change_count(uint32_t itemid, int count)
{
	if (!count) return true;

	std::map<uint32_t, Cday_item>::iterator it = item_list.find(itemid);
	if (count < 0) {
		return it != item_list.end() ? int(it->second.total) >= -count : false;
	} else {
		item_conf_t* p_item_conf = g_item_conf_map.get_item_conf(0, itemid);
		if (!p_item_conf) return false;
		uint32_t new_cnt = (it != item_list.end() ? it->second.total : 0) + count;
		uint32_t new_day_cnt = (it != item_list.end() ? it->second.daycnt : 0) + count;
		return new_cnt <= p_item_conf->total_max && new_day_cnt <= p_item_conf->day_max;
	}
}

uint32_t Cspec_item_list::del(uint32_t itemid, uint32_t count, bool is_sync_db)
{
	if (!count) return 0;

	std::map<uint32_t, Cday_item>::iterator it = item_list.find(itemid);
	if (it != item_list.end()) {
		if (it->second.total < count)
			count = it->second.total;
		it->second.total -= count;
	} else {
		count = 0;
	}

	if (count && is_sync_db) {
		pop_exchange_in db_in;
		item_exchange_t item;
		item.itemid = itemid;
		item.count = count;
		db_in.del_item_list.push_back(item);
		send_msg_to_db(p, pop_exchange_cmd, &db_in);
	}

	return count;
}

int Cspec_item_list::change_count(uint32_t itemid, int count, bool is_sync_db)
{
	if (count > 0) {
		count = add(itemid, count, is_sync_db);
	} else if (count < 0) {
		count = del(itemid, -count, is_sync_db);
	}

	return count;
}

uint32_t Cspec_item_list::get_count(uint32_t itemid)
{
	std::map<uint32_t, Cday_item>::iterator it = item_list.find(itemid);
	return it != item_list.end() ? it->second.total : 0;
}

uint32_t Cspec_item_list::get_day_count(uint32_t itemid)
{
	std::map<uint32_t, Cday_item>::iterator it = item_list.find(itemid);
	return it != item_list.end() ? it->second.daycnt : 0;
}

void Ctask_list::init(const std::vector<task_t>  & task_list )
{
	this->finish_task_count = 0;
	for (uint32_t i=0;i<task_list.size();i++){
		const task_t *p_item=&(task_list[i]);
		this->add(p_item->taskid,p_item->task_nodeid );	
		if (p_item->task_nodeid == END_TASK_NODEID)
			this->finish_task_count ++;
	}
}

void Csprite::get_user_info( cli_get_user_info_out   &cli_out  )
{
	cli_out.color=this->color;
	cli_out.regtime=this->register_time;
	cli_out.left_xiaomee=this->xiaomee;
	cli_out.level = this->level;
	cli_out.experience = this->spec_item_list.get_count(spec_item_experience);
	//cli_out.game_point = this->spec_item_list.get_count(spec_item_game_point);
	cli_out.game_point = this->get_valid_gamepoint();
	memcpy(cli_out.nick,this->nick,sizeof(cli_out.nick));
	this->user_log_list.get_list(cli_out.user_log_list );
	this->item_list.get_used_list(cli_out.use_clothes_list, cli_out.effect_list, cli_out.complete_islandid_list);
	//this->task_list.get_completed_taskid(cli_out.complete_islandid_list);
}

uint32_t Csprite::get_valid_gamepoint()
{
	uint32_t got_point = this->spec_item_list.get_count(spec_item_game_point);
	uint32_t used_point = this->spec_item_list.get_count(spec_item_game_point_used);
	if (got_point > used_point)
		return got_point - used_point;
	return 0;
}
	
void Csprite::noti_seat_changed(uint32_t side, uint32_t userid)
{
	cli_noti_game_seat_out cli_out;
	game_seat_t seat;
	seat.gameid = this->wait_gameid;
	seat.side = side;
	seat.userid = userid;
	cli_out.gamelist.push_back(seat);
	g_sprite_map->noti_to_map_all(this, cli_noti_game_seat_cmd, &cli_out);
}

void Csprite::unseat_game()
{
	if (this->wait_gameid) {
		std::map<uint32_t, game_wait_t>::iterator it;
		it = g_sprite_map->game_map->game_wait_map.find(this->wait_gameid);
		if (it != g_sprite_map->game_map->game_wait_map.end() && it->second.userid == this->id) {
			g_sprite_map->game_map->game_wait_map.erase(this->wait_gameid);
			this->noti_seat_changed(2, this->id);
		}
		this->wait_gameid = 0;
	}
}

void Csprite::noti_game_seat()
{
	cli_noti_game_seat_out cli_out;
	std::map<uint32_t, game_wait_t>::iterator it;
	for (it = g_sprite_map->game_map->game_wait_map.begin();
			it != g_sprite_map->game_map->game_wait_map.end(); it ++) {
		game_seat_t seat;
		seat.gameid = it->first;
		seat.userid = it->second.userid;
		seat.side = it->second.side;
		cli_out.gamelist.push_back(seat);
	}
	this->send_succ_server_noti(cli_noti_game_seat_cmd, &cli_out);
}

uint32_t Csprite::get_day_oltime()
{
	if (this->id == 256243833 || this->id == 258791727 || this->id == 255275187 || this->id == 50181)
		return 0;
	return spec_item_list.get_day_count(spec_item_day_oltime) + get_once_oltime();
}

void Csprite::add_login_count()
{
	if (this->spec_item_list.get_day_count(spec_item_login_count) == 0) {
		this->spec_item_list.add(spec_item_login_count, 1, true);
	}
}

uint32_t Csprite::get_once_oltime() 
{
	uint32_t online_time_once = time(NULL);
	// 如果登录时间不是同一天，或者登录时间比当前晚（改过时间）,统一作为0点登录
	if (!is_same_day(online_time_once, login_time) || online_time_once < login_time) {
		KERROR_LOG(this->id, "time err\t[now=%u, login=%u]", online_time_once, login_time);
		online_time_once -= get_day_start_seconds(online_time_once);
	} else {
		online_time_once -= login_time;
	}
	return online_time_once;
}

void Citem_list::get_itemid_list(uint32_t startid,uint32_t endid,
		std::vector<uint32_t> & out_list )
{
	std::map<uint32_t, Citem_ex >::iterator it,itlow,itup;	
	itlow=this->item_list.lower_bound(startid );	
	itup= this->item_list.upper_bound(endid );	

	for ( it=itlow ; it != itup ; it++ ){
		out_list.push_back(it->second.itemid );
	}
}

void Ctask_list::get_completed_taskid(std::vector<uint32_t> &taskid_list)
{
	std::map<uint32_t ,std::set<uint32_t > >::iterator it;
	for (it=this->taskmap.begin();it!=this->taskmap.end(); ++it ){
		if(it->second.find(END_TASK_NODEID ) != it->second.end()  ){
			//存在最后的任务节点
			taskid_list.push_back(it->first );
		}		
	}
}

void Cgame_stat_map::init(Csprite* obj_p, std::vector<game_info_t>& game_stat_list)
{
	this->p = obj_p;
	for (uint32_t loop = 0; loop < game_stat_list.size(); loop ++) {
		Cgame_stat tmp_stat;
		tmp_stat.win_time = game_stat_list[loop].win_time;
		tmp_stat.lose_time = game_stat_list[loop].lose_time;
		tmp_stat.draw_time = game_stat_list[loop].draw_time;
		this->game_stat_map.insert(std::pair<uint32_t, Cgame_stat>(
					game_stat_list[loop].gameid, tmp_stat));
	}
}

void Cgame_stat_map::add_game_stat(uint32_t gameid, uint32_t win_flag, bool is_sync_db)
{
	std::map<uint32_t, Cgame_stat>::iterator it = this->game_stat_map.find(gameid);
	if (it == this->game_stat_map.end()) {
		Cgame_stat tmp_stat;
		tmp_stat.win_time = 0;
		tmp_stat.lose_time = 0;
		tmp_stat.draw_time = 0;
		it = this->game_stat_map.insert(std::pair<uint32_t, Cgame_stat>(gameid, tmp_stat)).first;
	}

	switch (win_flag) {
	case 0:
		it->second.lose_time ++;
		break;
	case 1:
		it->second.win_time ++;
		break;
	case 2:
		it->second.draw_time ++;
		break;
	}

	if (is_sync_db) {
		pop_add_game_info_in db_in;
		db_in.gameid = gameid;
		db_in.win_flag = win_flag;
		send_msg_to_db(p, pop_add_game_info_cmd, &db_in);
	}
}

void Cgame_stat_map::get_game_state_list(std::vector<game_info_t>& game_stat_list)
{
	for (std::map<uint32_t, Cgame_stat>::iterator it = this->game_stat_map.begin();
			it != this->game_stat_map.end(); it ++) {
		game_info_t tmp_game_info;
		tmp_game_info.gameid = it->first;
		tmp_game_info.win_time = it->second.win_time;
		tmp_game_info.lose_time = it->second.lose_time;
		tmp_game_info.draw_time = it->second.draw_time;
		game_stat_list.push_back(tmp_game_info);
	}
}
