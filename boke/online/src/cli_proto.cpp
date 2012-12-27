#include <fcntl.h>
#include <sys/mman.h>

#include "cli_proto.h"
#include "global.h"
#include "version.h"
#include "stat.h"
#include "Cmap_conf.h"
#include "Cactivity.h"
#include "Cgame.h"

#include "./proto/pop_online_enum.h"
#include "./proto/pop_db_enum.h"
#include "./proto/main_login_enum.h"
#include "./proto/pop_db.h"
#include "./proto/main_login.h"
//-------------------------------------------------------
//函数定义
#define PROTO_FUNC_DEF(proto_name)\
    int proto_name( Csprite *p , Cmessage* c_in ) ;
#include "./proto/pop_online_func_def.h"

//-------------------------------------------------------------
//对应的结构体
#include "./proto/pop_online.h"
//-------------------------------------------------------------

//命令绑定
typedef   int(*P_DEALFUN_T)( Csprite *p, Cmessage* p_in );


//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
	#include "./proto/pop_online_bind_for_cli_request.h"
};

void init_cli_handle_funs()
{
    g_cli_cmd_map.initlist(g_cli_cmd_list,
			sizeof(g_cli_cmd_list)/sizeof(g_cli_cmd_list[0]));
}


/**
  * @brief dispatches protocol packages from client
  * @param data package from client
  * @param fdsess fd session
  * @param first_tm true if we are processing a package from a client,
  *              false if from the pending-processing queue
  * @return value that was returned by the protocol handling function
  */
int dispatch(void* data, fdsession_t* fdsess, bool first_tm)
{
	protocol_t* pkg = (protocol_t*)data;

  //返回版本信息
    if(pkg->cmd== 1) {
        const char * version=get_version_str() ;
        return send_pkg_to_client(fdsess, version, 256);
	}

	Csprite * p = g_sprite_map->get_sprite_by_fd(fdsess->fd);
	
	// 用户登录检查(不在线只能发登录协议、在线不能发登录协议) 
	if (((pkg->cmd != cli_login_cmd) && !p) || ((pkg->cmd == cli_login_cmd) && p)) {
		ERROR_LOG("error cmdid=%d, p=%p ", pkg->cmd,p);
		return -1;
	}

	//检查注册协议，已登录且未注册不能发其他协议
	if ( p && (p->flag & user_flag_reg) ==0  &&  pkg->cmd!=cli_reg_cmd ){
		KERROR_LOG(p->id, "error reg: fd=%d cmd=%u", fdsess->fd, pkg->cmd);
		return -1;
	}

	if (pkg->cmd == cli_login_cmd ){//创建实例
		if (pkg->len==18+40 ){//HARD CODE
			uint32_t userid= *((uint32_t*) (((char*)data)+sizeof(*pkg) + 4));
			p=g_sprite_map->add_sprite(userid,fdsess );
			if (!p){//
				KDEBUG_LOG(userid, "error add sprite err " );
				return -1;
			}
		}else{
			KDEBUG_LOG( 0 , "cli_login_cmd cmdid len err ,fd=%d",fdsess->fd );
			return -1;
		}
	}


	//串行化客户端数据
	if (first_tm ){
		if (p && (p->cache_buffer_lst.size()>0 || p->get_waitcmd() != 0)){//前面还有数据或当前命令正在处理
			p->cache_buffer_lst.add_buffer((char*)data);
			KDEBUG_LOG(p->id, "ADD CACHE BUFFER\t[%u]", p->get_waitcmd());
			return 0;
		}
	}else{//是缓存的报文
		assert(p->get_waitcmd()==0);
	}

	//命令分发
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(pkg->cmd );
    if (p_cmd_item ) {
		//if (g_is_test_env)
		{
			char outbuf[13000];
			bin2hex(outbuf,(char*)data,pkg->len, g_is_test_env ? 2000 : 200);
			KDEBUG_LOG(p->id,"CI[%u][%u][%s]", pkg->cmd, pkg->len, outbuf);
		}

		bool unpack_ret = p_cmd_item->proto_pri_msg->read_from_buf_ex(   
				((char*)data)+sizeof(*pkg), pkg->len -sizeof(*pkg));
        if (!unpack_ret){
            KDEBUG_LOG(p->id, "err: pkglen error");
            return -1;
        }

		p->set_waitcmd(pkg->cmd,pkg->seq);
	    return p_cmd_item->func(p, p_cmd_item->proto_pri_msg);

    }else{
		KERROR_LOG(p->id, "cmdid nofind err\t[%u %u]", pkg->cmd, pkg->len);
		return  -1;
	}
}


/*登入*/
int cli_login(Csprite *p, Cmessage* c_in)
{
	cli_login_in * p_in = P_IN;
	main_login_check_session_in db_in;
	hex2bin( db_in.session,p_in->session, sizeof(db_in.session));
	db_in.del_session_flag=1;
	db_in.gameid=12;

 	send_msg_to_db(p,main_login_check_session_cmd,&db_in );
	return 0;
}


/*完成任务节点*/
int cli_task_complete_node(Csprite *p, Cmessage* c_in)
{
	cli_task_complete_node_in * p_in = P_IN;
	cli_task_complete_node_out cli_out;
	KDEBUG_LOG(p->id, "COMPLETE TASK\t[id=%u node=%u]", p_in->islandid, p_in->task_nodeid);

	//检查任务是否可以完成
	Ctask_node *p_task_node=g_task_map.get_task_node(p_in->islandid,p_in->task_nodeid );	
	if (p_task_node==NULL){
		p->send_err(cli_task_node_nofind_err);
		return 0;
	}

	//检查所有前置任务
	uint32_t i;
	for (i=0; i< p_task_node->req_taskid_list.size();i++){
		if (!p->task_list.check_existed_node(
					p_in->islandid,p_task_node->req_taskid_list[i] )){
			p->send_err(cli_task_node_req_task_err);
			return 0;
		}
	}

	//检查所需的物品
	for (i=0; i< p_task_node->req_item_list.size();i++){
		Citem *p_item=&p_task_node->req_item_list[i];
		if (!p->item_list.check_enough(p_item->itemid,p_item->count )){
			p->send_err(cli_task_node_need_item_nofind_err);
			return 0;
		}
	}

	//检查钱是否ok
	if(p_in->task_nodeid == END_TASK_NODEID){//是最后任务, 加钱
		if (!p->check_change_xiaomee( TASK_XIAOMEE_VALUE)){
			p->send_err(cli_xiaomee_max_err);
			return 0;
		}
	}

	//加入到内存中		
	bool add_flag=p->task_list.add(p_in->islandid,p_in->task_nodeid);
	if( add_flag ){
		item_exchange_t item;

 		pop_exchange_in db_in;
		//加入任务
		item.itemid = spec_item_task;
		item.count=p_in->islandid;
		item.max_count=p_in->task_nodeid;
		db_in.add_item_list.push_back(item );

		item.max_count=99;
		//扣物品
		for (i=0; i< p_task_node->recycle_item_list.size();i++){
			Citem *p_item=&p_task_node->recycle_item_list[i];
			p->item_list.del(p_item->itemid,p_item->count );
			//
			item.itemid=p_item->itemid;
			item.count=p_item->count;
			db_in.del_item_list.push_back(item );
		}

		int add_exp = 0;
		bool need_add_exp = false;

		//加物品
		for (i=0; i< p_task_node->reward_item_list.size();i++){
			Citem *p_item=&p_task_node->reward_item_list[i];
			if (!p->item_list.check_enough(p_item->itemid, 1)) {
				p->item_list.add(p_item->itemid,p_item->count );
				//没有的物品要加到数据库 
				item.itemid=p_item->itemid;
				item.count=p_item->count;
				item.max_count = 1;
				db_in.add_item_list.push_back(item );
				stat_log_item(p_item->itemid);
				need_add_exp = true;
			}
		}

		// 每张卡片获得10经验
		if (need_add_exp)
			add_exp += 10;

		if(p_in->task_nodeid == END_TASK_NODEID){//是最后任务, 加钱
			p->change_xiaomee( TASK_XIAOMEE_VALUE);

			item.itemid=spec_item_xiaomee;
			item.count=TASK_XIAOMEE_VALUE ;
			db_in.add_item_list.push_back(item );
			//记录足迹
			p->user_log_list.add(2, p_in->islandid);

			// 通关加90经验
			add_exp += 90;

		
			// 计入任务结束的统计数据
			p->task_list.finish_task_count ++;
			stat_log_task_finish(p->id, p->task_list.finish_task_count);
			uint32_t online_time = p->online_time + (time(NULL) - p->login_time);
			KDEBUG_LOG(p->id,"SUCC99999X :%u %u ", p_in->islandid, online_time);
			send_sw_user_island_complete(p, p_in->islandid);
		}

		if (add_exp) {
			p->change_experience(add_exp);
			item.itemid = spec_item_experience;
			item.count = add_exp;
			item.day_add_cnt = add_exp;
			item.max_count = MAX_SPEC_ITEM_COUNT;
			db_in.add_item_list.push_back(item);
		}

		//同步db
		send_msg_to_db(p, pop_exchange_cmd ,&db_in );

		// 添加统计项
		stat_log_task_node(p, p_in->islandid, p_in->task_nodeid);
	} // else if (false) 重复完成任务，不做任何处理，返回客户端

	//通知前端
	p->task_list.get_nodeid_list (p_in->islandid, cli_out.task_nodeid_list);
	p->send_succ( &cli_out );
	return 0;
}

/*得到物品列表*/
int cli_get_item_list(Csprite *p, Cmessage* c_in)
{
	cli_get_item_list_in * p_in = P_IN;
	cli_get_item_list_out  out ;
	if (p_in->startid >p_in->endid ) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	std::map<uint32_t, Citem_ex >::iterator it,itlow,itup;	
	itlow=p->item_list.item_list.lower_bound( p_in->startid  );	
	itup=p->item_list.item_list.upper_bound( p_in->endid);	

	for ( it=itlow ; it != itup ; it++ ){
		item_t item;
		item.itemid=it->second.itemid;
		item.count=it->second.count;
		out.item_list.push_back(item);
	}
	p->send_succ(&out);
	return SUCC;
}

/*设置昵称*/
int cli_set_nick(Csprite *p, Cmessage* c_in)
{
	cli_set_nick_in * p_in = P_IN;

	p_in->nick[sizeof (p_in->nick)-1]='\0';
	if( tm_dirty_check(7,  p_in->nick) > 0) { 
		p->send_err(cli_nick_err );
		return 0; 	
	}

	memcpy (p->nick,p_in->nick,sizeof(p->nick));
	pop_set_nick_in	 db_in;
	memcpy (db_in.nick,p_in->nick,sizeof(db_in.nick));
 	send_msg_to_db(p,pop_set_nick_cmd ,&db_in );
	p->send_succ();
	return 0;
}


/*进入场景*/
int cli_walk(Csprite *p, Cmessage* c_in)
{
	cli_walk_in * p_in = P_IN;
	if (!g_map_conf_map.check_valid(p_in->islandid, p_in->mapid)) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	if (!g_map_conf_map.check_island_open(p_in->islandid)) {
		if (p->get_last_islandid() != p_in->islandid) {
			p->send_err(cli_island_no_open_err);
			return 0;
		}
	}
	
	//找到了某张地图
	int add_flag=p->find_map_list_add(p_in->islandid,p_in->mapid);
	if( add_flag ){
 		pop_find_map_add_in db_in;
		db_in.islandid=p_in->islandid;
		db_in.mapid=p_in->mapid;
 		send_msg_to_db(p,pop_find_map_add_cmd ,&db_in );
	}

	//先返回包
	p->send_succ();

	//通知
	g_sprite_map->cd_map(p->id,p_in,false);

	// 添加统计项
	stat_log_enter_map(p, p_in->islandid, p_in->mapid);

	if (add_flag & 0x2) { // 第一次进入某个岛
		// 根据是否新注册米米号送物品
		pop_exchange_in exchange_db_in;
		item_exchange_t item;

		// 都送卜卜兔子装
		if (!p->item_list.check_enough(item_bobo_rabit_suit, 1)) {
			p->item_list.add(item_bobo_rabit_suit, 1);
			item.itemid = item_bobo_rabit_suit;
			item.count = 1;
			item.max_count = 1;
			exchange_db_in.add_item_list.push_back(item);
		}

		if (p->flag & user_flag_newid) {	// 如果是新注册米米号
			// 200卜克豆
			p->change_xiaomee(200);
			item.itemid = spec_item_xiaomee;
			item.count = 200;
			exchange_db_in.add_item_list.push_back(item);

			// 卜克宝宝装
			if (!p->item_list.check_enough(item_bobo_baby_suit, 1)) {
				p->item_list.add(item_bobo_baby_suit, 1);
				item.itemid = item_bobo_baby_suit;
				item.count = 1;
				item.max_count = 1;
				exchange_db_in.add_item_list.push_back(item);
			}
		} else {							// 非新注册的米米号
			// 100卜克豆
			p->change_xiaomee(100);
			item.itemid = spec_item_xiaomee;
			item.count = 100;
			exchange_db_in.add_item_list.push_back(item);
		}
		send_msg_to_db(p, pop_exchange_cmd, &exchange_db_in);

		cli_noti_first_enter_island_out noti_island_out;
		noti_island_out.islandid = p_in->islandid;
		noti_island_out.xiaomee = 100;
		noti_island_out.is_newid = !!(p->flag & user_flag_newid);
		KDEBUG_LOG(p->id, "first enter\t[%u]", noti_island_out.is_newid);
		p->send_succ_server_noti(cli_noti_first_enter_island_cmd, &noti_island_out);
	}
	
	if (p_in->islandid == 8) {
		p->noti_game_seat();
	}
	return 0;
}

/*得到用户在该岛上的信息*/
int cli_get_user_island_task_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_island_task_info_in * p_in = P_IN;
	cli_get_user_island_task_info_out  cli_out ;

	p->task_list.get_nodeid_list (p_in->islandid, cli_out.task_nodeid_list);
	p->send_succ(&cli_out);

	return 0;
}

/*增加找到的地图*/
//不使用了
int cli_find_map_add( Csprite *p, Cmessage* c_in)
{
	cli_find_map_add_in * p_in = P_IN;

	bool add_flag=p->find_map_list_add(p_in->islandid,p_in->mapid);
	if( add_flag ){
 		pop_find_map_add_in db_in;
		db_in.islandid=p_in->islandid;
		db_in.mapid=p_in->mapid;

 		send_msg_to_db(p,pop_find_map_add_cmd ,&db_in );
	}
	
	return 0;
}

/*用户注册*/
int cli_reg(Csprite *p, Cmessage* c_in)
{
	cli_reg_in * p_in = P_IN;
	if (p->flag & user_flag_reg ){
		p->send_err(cli_request_arg_err);
		return 0; 	
	}
	p_in->nick[sizeof (p_in->nick)-1]='\0';
	
	if( tm_dirty_check(7,  p_in->nick) > 0) { 
		memset(p_in->nick, 0,sizeof(p_in->nick ));
		strcpy(p_in->nick,"小卜克");
	}
//9A5321 F5CC70 FFDED5 9F7442 FFEFD7
	//检查肤色
	if (!(	p_in->color == 0x9A5321
		||  p_in->color == 0xF5CC70
		||  p_in->color == 0xFFDED5
		||  p_in->color == 0x9F7442
		||  p_in->color == 0xFFEFD7
				)){
		KERROR_LOG(p->id,"error color err:%d",p_in->color );
		p->send_err(cli_request_arg_err);
		return 0; 	
	}

	if (p_in->item_list.size()!=5 ){
		KERROR_LOG(p->id,"item list count err :%u",(uint32_t) p_in->item_list.size() );
		p->send_err(cli_request_arg_err);
		return 0;
	}

	//检查物品
	for (uint32_t i=0;i<p_in->item_list.size() ;i++ ){
		item_t* p_item=&(p_in->item_list[i]);
		if (p_item->count !=1 ) {
			KERROR_LOG(p->id,"item count err :[%u] !=1", p_item->count );
			p->send_err(cli_request_arg_err);
			return 0;
		}
		if ( user_reg_config_map[i].find(p_item->itemid )
				== user_reg_config_map[i].end()){
			//没有找到
			KERROR_LOG(p->id,"index [%u] item id  err :[%u] ", i,p_item->itemid);
			p->send_err(cli_request_arg_err);
			return 0;
		}
	}

	pop_reg_in db_in;
	db_in.age=p_in->age;
	db_in.color=p_in->color;
	memcpy(db_in.nick, p_in->nick, sizeof(db_in.nick) );
	//设置已经注册
	p->flag |= user_flag_reg;
	p->age=p_in->age;
	p->color=p_in->color;
	memcpy(p->nick, p_in->nick,sizeof(p->nick) );
	db_in.flag = (p->flag & (user_flag_reg | user_flag_newid | user_flag_sample));

	for (uint32_t i=0;i<p_in->item_list.size() ;i++ ){
		item_exchange_t item_exchange;
		item_t* p_item=&(p_in->item_list[i]);
		p->item_list.add(p_item->itemid,p_item->count,true );
 		item_exchange.itemid=p_item->itemid;
 		item_exchange.max_count=1;
 		item_exchange.count=p_item->count;
		db_in.add_item_list.push_back( item_exchange);
	}
	
	//设置用户已经注册过了,－＞基础库
 	main_login_add_game_flag(p, p_in->channelid, 12);

	//设置db
 	send_msg_to_db(p,pop_reg_cmd ,&db_in );

	//记录足迹
	p->user_log_list.add(1, 0);
	
	// 添加统计项 新注册用户
	stat_log_new_user(p);
	
	//发回客户端
	cli_reg_out cli_out;
	cli_out.left_xiaomee = p->xiaomee;
	memcpy(cli_out.nick, p->nick, sizeof(cli_out.nick));
	p->send_succ(&cli_out);

	return 0;
}


/*设置使用物品列表*/
int cli_set_item_used_list(Csprite *p, Cmessage* c_in)
{
	cli_set_item_used_list_in * p_in = P_IN;
	if (p_in->itemid_list.size()<4){
		KERROR_LOG(p->id, "item size =%u",(uint32_t )(p_in->itemid_list.size()));
		p->send_err(cli_request_arg_err);
		return 0; 	
	}

	// 检查同一位置是否有两件装扮 
	std::map<uint32_t, uint32_t> type_list;
	for(uint32_t i=0;i<p_in->itemid_list.size();i++){
		uint32_t clothtype=g_item_conf_map.get_clothtype( p_in->itemid_list[i]);
		type_list[clothtype] += 1;
		if ( type_list[clothtype]>1){//同一位置有两个
			KERROR_LOG(p->id,"type_list type have 2 :itemid=%u",p_in->itemid_list[i] );
			p->send_err(cli_request_arg_err);
			return 0; 	
		}
	}

	if (type_list[5]==0 ){
		KERROR_LOG(p->id,"type_list :5 nofind");
		p->send_err(cli_request_arg_err);
		return 0; 	
	}

	pop_set_item_used_list_in   db_in;	
	bool ret=p->item_list.set_cloth_used_list(p_in->itemid_list,
			db_in.del_can_change_list,
			db_in.set_noused_list,
			db_in.add_can_change_list,
			db_in.set_used_list
		   	);
	if(!ret){
		p->send_err(cli_request_arg_err);
		return 0; 	
	}


 	send_msg_to_db(p,pop_set_item_used_list_cmd ,&db_in  );

	if (p->get_is_in_muti_map()){
		//通知其它人	
		cli_noti_user_use_clothes_list_out noti_out;		
		noti_out.obj_userid=p->id;
		noti_out.use_clothes_list=p_in->itemid_list;
		g_sprite_map->noti_to_map_others(p, cli_noti_user_use_clothes_list_cmd, &noti_out);
	}
	p->send_succ();
	
	return 0;
}


/*
 * 设置特效卡片 */
int cli_set_effect_used(Csprite *p, Cmessage* c_in)
{
	cli_set_effect_used_in *p_in = P_IN;

	// 缓存中设置
	uint32_t unset_itemid = 0;
	pop_set_item_used_list_in   db_in;	

	if (p_in->type) {	// 特效设置或替换
		if (!p->item_list.set_effect_used(p_in->itemid, unset_itemid)) {
			KERROR_LOG(p->id, "invalid itemid\t[%u]", p_in->itemid);
			p->send_err(cli_request_arg_err);
			return 0;
		}
	} else {			// 特效取消
		if (!p->item_list.set_effect_unused(p_in->itemid)) {
			p->send_succ();
			//p->send_err(cli_request_arg_err);
			return 0;
		}
		unset_itemid = p_in->itemid;
		p_in->itemid = 0;
	}

	// 设置DB
	if (p_in->itemid) {
		db_in.set_used_list.push_back(p_in->itemid);
	}

	if (unset_itemid) {
		db_in.set_noused_list.push_back(unset_itemid);
	}
	send_msg_to_db(p, pop_set_item_used_list_cmd, &db_in);

	// 通知客户端
	cli_noti_effect_used_out noti_out;
	noti_out.src_userid = p->id;
	noti_out.itemid = p_in->itemid;
	noti_out.unset_itemid = unset_itemid;
	if (p->get_is_in_muti_map()) {
		//g_sprite_map->noti_to_map_others(p, cli_noti_effect_used_cmd, &noti_out);
		g_sprite_map->noti_to_map_all(p, cli_noti_effect_used_cmd, &noti_out);
	} else {
		p->send_succ_server_noti(cli_noti_effect_used_cmd, &noti_out);
	}

	p->send_succ();
	return 0;
}

/*删除任务*/
int cli_task_del_node(Csprite *p, Cmessage* c_in)
{
	cli_task_del_node_in * p_in = P_IN;
	uint32_t i;

	//检查任务是否可以完成
	Ctask_node *p_task_node=g_task_map.get_task_node(p_in->islandid,p_in->task_nodeid );	
	if (p_task_node==NULL){
		p->send_err(cli_task_node_nofind_err);
		return 0;
	}
	//检查任务是否存在
	bool ret= p->task_list.check_existed_node(p_in->islandid,p_in->task_nodeid);	
	if (!ret){
		p->send_succ();
		return 0;
	}

	int del_exp = 0;

	//检查所需的物品
	for (i=0; i< p_task_node->reward_item_list.size();i++){
		Citem *p_item=&p_task_node->reward_item_list[i];
		del_exp += 10;
		if (!p->item_list.check_enough(p_item->itemid,p_item->count )){
			p->send_err(cli_task_node_need_item_nofind_err);
			return 0;
		}
	}
	//检查钱是否ok
	if(p_in->task_nodeid == END_TASK_NODEID){//是最后任务, 加钱
		del_exp += 90;
		if (!p->check_change_xiaomee( -TASK_XIAOMEE_VALUE)){
			p->send_err(cli_xiaomee_no_enough_err);
			return 0;
		}
	}

	if (uint32_t(del_exp) > p->spec_item_list.get_count(spec_item_experience)) {
		p->send_err(cli_xiaomee_no_enough_err);
		return 0;
	}

	p->task_list.del(p_in->islandid,p_in->task_nodeid);
	item_exchange_t item;
	pop_exchange_in db_in;
	//加入任务
	item.itemid=1;
	item.count=p_in->islandid;
	item.max_count=p_in->task_nodeid;
	db_in.del_item_list.push_back(item );


	item.max_count=99;
	//扣物品
	for (i=0; i< p_task_node->reward_item_list.size();i++){
		Citem *p_item=&p_task_node->reward_item_list[i];
		p->item_list.del(p_item->itemid,p_item->count );
		//
		item.itemid=p_item->itemid;
		item.count=p_item->count;
		db_in.del_item_list.push_back(item );
	}
	//加物品
	for (i=0; i< p_task_node->recycle_item_list.size();i++){
		Citem *p_item=&p_task_node->recycle_item_list[i];
		p->item_list.add(p_item->itemid,p_item->count );

		//
		item.itemid=p_item->itemid;
		item.count=p_item->count;
		db_in.add_item_list.push_back(item );
	}
	//检查钱是否ok
	if(p_in->task_nodeid == END_TASK_NODEID){//是最后任务, 加钱
		p->change_xiaomee( -TASK_XIAOMEE_VALUE);
		item.itemid=spec_item_xiaomee;
		item.count=TASK_XIAOMEE_VALUE ;
		db_in.del_item_list.push_back(item );
	}

	// 减少经验
	if (del_exp) {
		p->change_experience(-del_exp);
		item.itemid = spec_item_experience;
		item.count = del_exp;
		db_in.del_item_list.push_back(item);
	}

	//同步db
	send_msg_to_db(p, pop_exchange_cmd ,&db_in );

	//通知前端
	p->send_succ( );

	return 0;
}

/*得到岛上的找到地图的信息*/
int cli_get_user_island_find_map_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_island_find_map_info_in * p_in = P_IN;
	cli_get_user_island_find_map_info_out  cli_out ;

	p->find_map_list_get_mapid_list(p_in->islandid, cli_out.find_map_list);
	p->send_succ(&cli_out);

	return 0;
}

/* ----------------CODE FOR cli_get_card_list_by_islandid  ---------*/
/**/
int cli_get_card_list_by_islandid(Csprite *p, Cmessage* c_in)
{
	cli_get_card_list_by_islandid_in * p_in = P_IN;
	cli_get_card_list_by_islandid_out cli_out ;

	
	std::map<uint32_t, Citem_ex >::iterator it,itlow,itup;	
	itlow=p->item_list.item_list.lower_bound(200001 );	
	itup= p->item_list.item_list.upper_bound(399999 );	

	for ( it=itlow ; it != itup ; it++ ){
		item_t item;
		item.itemid = it->second.itemid;
		item.count = it->second.count;
		if (g_item_conf_map.is_on_island(item.itemid, p_in->islandid)){
			cli_out.cardid_list.push_back(item);
			p->item_list.get_comprise_item_list(item.itemid, cli_out.cardid_list);
		}
	}
	p->send_succ(&cli_out);


	return 0;
}

/*和npc聊天*/
int cli_talk_npc(Csprite *p, Cmessage* c_in)
{
	cli_talk_npc_in * p_in = P_IN;
	KDEBUG_LOG(p->id, "TALK NPC\t[%u]", p_in->npc_id);

	std::map<uint32_t, uint32_t>::iterator it = npc_trans_map.find(p_in->npc_id);
	if (it != npc_trans_map.end()) {
		if (p->spec_item_list.get_count(it->second) == 0) {
			p->change_experience(10, true);	
		}

		p->spec_item_list.change_count(it->second, 1);
	}


	//记录
	stat_log_talk_npc(p, p_in->npc_id);
	p->send_succ();
	return 0;
}

/*用户移动 多人模式下*/
int cli_user_move(Csprite *p, Cmessage* c_in)
{
	cli_user_move_in * p_in = P_IN;
	p->last_x=p_in->mouse_x;
	p->last_y=p_in->mouse_y;

	if(p->get_is_in_muti_map()){
		//通知其它人	
		cli_noti_user_move_out noti_out;
		noti_out.obj_userid= p->id;
		noti_out.start_x= p_in->start_x;
		noti_out.start_y= p_in->start_y;

		noti_out.mouse_x= p_in->mouse_x;
		noti_out.mouse_y= p_in->mouse_y;
		noti_out.type = p_in->type;
		g_sprite_map->noti_to_map_others(p, cli_noti_user_move_cmd, &noti_out);
	}

	p->send_succ();
	return 0;
}

/*聊天*/
int cli_talk(Csprite *p, Cmessage* c_in)
{
	cli_talk_in * p_in = P_IN;
	//通知
	cli_noti_talk_out noti_out;		
	noti_out.src_userid=p->id;
	noti_out.obj_userid = p_in->obj_userid;
	//检查报文长度
	if (p_in->_msg_len>sizeof( noti_out.msg )){
		p->send_err(cli_request_arg_err);
		return 0;
	}

	if (p_in->_msg_len < 9999) {
		p_in->msg[p_in->_msg_len] = '\0';
	} else {
		p_in->msg[9998] = '\0';
	}

	if (tm_dirty_check(7,  p_in->msg) > 0) {
		p->send_err(cli_dirty_word_err);
		return 0;
	}
	
	memcpy(noti_out.src_nick,p->nick,sizeof(noti_out.src_nick) );
	//设置变长字符串
	memcpy(noti_out.msg,p_in->msg,p_in->_msg_len );
	noti_out._msg_len=p_in->_msg_len;

	if (p_in->obj_userid == 1) {
		g_sprite_map->game_map->noti_all(p->game_map_id, cli_noti_talk_cmd, &noti_out, p->id);	
	} else if( p->get_is_in_muti_map() ){
		g_sprite_map->noti_to_map_all(p, cli_noti_talk_cmd, &noti_out);
	}

	p->send_succ();
	return 0;
}

/*得到当前地图的所有人信息*/
int cli_get_all_user_info_from_cur_map(Csprite *p, Cmessage* c_in)
{
	cli_get_all_user_info_from_cur_map_out	cli_out ;

	if(p->get_is_in_muti_map()){
		uint32_t online_mapid=p->get_onine_mapid();
		std::set<uint32_t> &userid_set=g_sprite_map ->online_map[online_mapid ];
		std::set<uint32_t>::iterator  it;
		for(it= userid_set.begin();it!=userid_set.end();++it  ){
			uint32_t obj_userid=*it;
			Csprite *obj_p=g_sprite_map ->get_sprite( obj_userid);
			if(obj_p==NULL){
				KDEBUG_LOG(p->id ,"ERROR, noti user cd map, objuserid,nofind :%u",obj_userid  );
				continue;
			}

			online_user_info_t		tmp_user_info;

			obj_p->get_user_info(&tmp_user_info );
  			cli_out.online_user_info_list.push_back(tmp_user_info );
		}
		//通知自己:地图里有什么人
	}

	p->send_succ(&cli_out);
	return 0;
}

/*得到当前地图的所有人信息*/
int cli_get_game_user(Csprite *p, Cmessage* c_in)
{
	cli_get_game_user_out cli_out ;

	if(p->get_is_in_muti_map()){
		uint32_t online_mapid=p->get_onine_mapid();
		std::set<uint32_t> &userid_set=g_sprite_map ->online_map[online_mapid ];
		std::set<uint32_t>::iterator  it;
		for(it= userid_set.begin();it!=userid_set.end();++it  ){
			uint32_t obj_userid=*it;
			Csprite *obj_p=g_sprite_map ->get_sprite( obj_userid);
			if(obj_p==NULL || !obj_p->game_map_id)
				continue;

			game_user_t game_user;
			if (g_sprite_map->game_map->get_game_user(obj_p->game_map_id, game_user, obj_p->id))
				cli_out.game_user_list.push_back(game_user);
		}
		//通知自己:地图里有什么人
	}

	p->send_succ(&cli_out);
	return 0;
}



enum {
	game_response_succ,				// 成功发送请求
	game_response_not_same_map,		// 对方不在同一个地图中
	game_response_in_other_game,	// 对方已经参加其他游戏
};


/*请求和某人玩游戏*/
int cli_game_request(Csprite *p, Cmessage* c_in)
{
	cli_game_request_in * p_in = P_IN;
	cli_game_request_out cli_out;
	cli_out.obj_user_stat = game_response_not_same_map;
	if (p->game_map_id) {
		p->send_succ(&cli_out);
		return 0;
	}
	
	if(p->get_is_in_muti_map()){
		if (p_in->obj_userid == 0 || p_in->obj_userid == 1) {	// 坐位式游戏
			p->unseat_game();
			std::map<uint32_t, game_wait_t>::iterator it;
			it = g_sprite_map->game_map->game_wait_map.find(p_in->gameid);
			game_wait_t gwait;
			gwait.userid = p->id;
			gwait.side = p_in->obj_userid;
			if (it == g_sprite_map->game_map->game_wait_map.end()) {
				g_sprite_map->game_map->game_wait_map[p_in->gameid] = gwait;
				p->wait_gameid = p_in->gameid;
				p->noti_seat_changed(p_in->obj_userid, p->id);
			} else {
				uint32_t obj_uid = it->second.userid;
				Csprite *obj_p = g_sprite_map->get_user_in_same_map(p, obj_uid);
				if (!obj_p || obj_p->game_map_id) {
					g_sprite_map->game_map->game_wait_map[p_in->gameid] = gwait;
					p->wait_gameid = p_in->gameid;
					p->noti_seat_changed(p_in->obj_userid, p->id);
				} else {
					obj_p->unseat_game();
					g_sprite_map->game_map->add(p_in->gameid, obj_uid, p->id );
					cli_noti_game_is_start_out noti_out; 
					noti_out.gameid=p_in->gameid;
					noti_out.is_start=1;
					noti_out.obj_userid=p->id;
					noti_out.why_not_start_flag=0;
					obj_p->send_succ_server_noti(cli_noti_game_is_start_cmd,&noti_out);

					noti_out.obj_userid = obj_uid;
					p->send_succ_server_noti(cli_noti_game_is_start_cmd, &noti_out);
					g_sprite_map->game_map->noti_map_game_stat(p->game_map_id, 1);
				}
			}
		} else if (p_in->obj_userid == 2) {
			p->unseat_game();
			p->send_succ(&cli_out);
			return 0;
		} else {
			Csprite *obj_p= g_sprite_map->get_user_in_same_map(p,p_in->obj_userid );
			if (obj_p ){// 用户存在
				if(obj_p->game_map_id>0 ){
					KDEBUG_LOG(obj_p->id,"obj_p->game_map_id:%u ",obj_p->game_map_id);
					cli_out.obj_user_stat=game_response_in_other_game;
				} else if (obj_p->busy_state) {
					cli_out.obj_user_stat = obj_p->busy_state;
				} else {
					cli_out.obj_user_stat = game_response_succ;
					cli_noti_game_request_out noti_out; 
					noti_out.gameid=p_in->gameid;
					noti_out.src_userid=p->id;
					Cgame_session::make_session(noti_out.request_session, p->id, p_in->gameid, obj_p->id);
					obj_p->send_succ_server_noti(cli_noti_game_request_cmd,&noti_out);
				}
			}
		}
	}

	p->send_succ(&cli_out);
	return 0;
}

/*确认请求*/
int cli_game_play_with_other(Csprite *p, Cmessage* c_in)
{
	cli_game_play_with_other_in * p_in = P_IN;
	cli_game_play_with_other_out cli_out;

	if(p->get_is_in_muti_map()){
		if (!Cgame_session::check_session(
			p_in->request_session, p_in->obj_userid, p_in->gameid, p->id)){
			p->send_err(cli_request_arg_err);
			return 0;
		}

		if (p->game_map_id) {
			cli_out.obj_user_stat = 2;
			p->send_succ(&cli_out);
			return 0;
		}
		
		Csprite *obj_p=g_sprite_map->get_user_in_same_map(p,p_in->obj_userid); 
		if (obj_p ){//用户存在
			if(obj_p->game_map_id>0 ){
				cli_out.obj_user_stat = p_in->is_start ? 2 : 0;
				p->send_succ(&cli_out);
				return 0;
			}
	
			//加入列表
			if (p_in->is_start ){//开启游戏
				g_sprite_map->game_map->add(p_in->gameid,p_in->obj_userid,p->id );
				stat_log_game(p->id, p_in->gameid);
				stat_log_game(p_in->obj_userid, p_in->gameid);
			}	

			//通知对方
			cli_noti_game_is_start_out noti_out; 
			noti_out.gameid=p_in->gameid;
			noti_out.is_start=p_in->is_start;
			noti_out.obj_userid=p->id;
			noti_out.why_not_start_flag=p_in->why_not_start_flag;
			obj_p->send_succ_server_noti(cli_noti_game_is_start_cmd,&noti_out);

			// is_start==1  则也发给自己
			if ( p_in->is_start ){
				noti_out.obj_userid=p_in->obj_userid;
				p->send_succ_server_noti(cli_noti_game_is_start_cmd ,&noti_out);
			
				g_sprite_map->game_map->noti_map_game_stat(p->game_map_id, 1);
			}

		}else{
			cli_out.obj_user_stat = p_in->is_start == 1 ? 1 : 0;
			p->send_succ(&cli_out);
			return 0;
		}
	}else{
		p->send_err(cli_request_arg_err);
		return 0;
	}

	p->send_succ(&cli_out);
	return 0;
}


/*客户端确认游戏可以开始了*/
int cli_game_can_start(Csprite *p, Cmessage* c_in)
{
	KDEBUG_LOG(p->id, "SET GAME START\t[%u]",p->game_map_id);
	g_sprite_map->game_map->set_user_can_start(p->game_map_id,p->id);

	p->send_succ();
	return 0;
}


/*用户游戏操作*/
int cli_game_opt(Csprite *p, Cmessage* c_in)
{
	cli_game_opt_in * p_in = P_IN;
	g_sprite_map->game_map->user_opt(p->game_map_id,p->id,p_in );

	p->send_succ();
	return 0;
}

/*游戏结束*/
int cli_cur_game_end(Csprite *p, Cmessage* c_in)
{
	cli_cur_game_end_in * p_in = P_IN;
	g_sprite_map->game_map->game_end(p->game_map_id,p->id,p_in->win_flag ,false);
	p->send_succ();
	return 0;
}

/*离开游戏*/
int cli_game_user_left_game(Csprite *p, Cmessage* c_in)
{
	g_sprite_map->game_map->noti_map_game_stat(p->game_map_id, 2);
	g_sprite_map->game_map->game_user_left(p->game_map_id,p->id);
	p->send_succ();
	return 0;
}

/*购买物品*/
int cli_buy_item(Csprite *p, Cmessage* c_in)
{
	cli_buy_item_in * p_in = P_IN;
	cli_buy_item_out cli_out;
	
	source_conf_t *p_source_conf=g_item_conf_map.get_buy_info(p_in->itemid);
	if (p_source_conf==NULL) {
		p->send_err(cli_request_arg_err);
		return 0;
	}
	
	//看看物品是不是已经存在了
	if (p->item_list.check_enough(p_in->itemid,1 )){
		p->send_err(p_source_conf->source == 1 ? cli_itemid_existed_err : cli_item_ever_get_err);
		return 0;
	}

	//检查钱是不是够
	if (!p->check_change_xiaomee( -p_source_conf->pay )){
		p->send_err(cli_xiaomee_no_enough_err );
		return 0;
	}

	
	//成功，开始处理
	item_exchange_t item;

	pop_exchange_in db_in;

	if (p_source_conf->pay) {
		//扣钱
		p->change_xiaomee( -p_source_conf->pay );

		item.itemid=spec_item_xiaomee;
		item.count=p_source_conf->pay;
		db_in.del_item_list.push_back(item );
	}

	//加物品
	
	p->item_list.add(p_in->itemid,1);

	//
	item.itemid=p_in->itemid;
	item.count=1;
	item.max_count=1;
	db_in.add_item_list.push_back(item );

	stat_log_item(p_in->itemid);

	//同步db
	send_msg_to_db(p, pop_exchange_cmd ,&db_in );

	cli_out.left_xiaomee=p->xiaomee;
	p->send_succ( &cli_out);
	return 0;
}

/* 使用小游戏积分购买物品*/
int cli_buy_item_use_gamept(Csprite *p, Cmessage* c_in)
{
	cli_buy_item_use_gamept_in * p_in = P_IN;
	cli_buy_item_use_gamept_out cli_out;
	
	source_conf_t *p_source_conf=g_item_conf_map.get_item_info(p_in->itemid);
	if (p_source_conf==NULL) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	//看看物品是不是已经存在了
	if (p->item_list.check_enough(p_in->itemid,1 )){
		p->send_err(p_source_conf->source == 1 ? cli_itemid_existed_err : cli_item_ever_get_err);
		return 0;
	}

	//检查钱是不是够
	uint32_t valid_point = p->get_valid_gamepoint();
	if (valid_point < p_source_conf->pay) {
		p->send_err(cli_xiaomee_no_enough_err );
		return 0;
	}
	
	//成功，开始处理
	item_exchange_t item;
	pop_exchange_in db_in;
	if (p_source_conf->gamept) {
		p->spec_item_list.change_count(spec_item_game_point_used, p_source_conf->gamept, false);

		item.itemid=spec_item_game_point_used;
		item.count=p_source_conf->gamept;
		item.max_count = MAX_SPEC_ITEM_COUNT;
		item.day_add_cnt = p_source_conf->gamept;
		db_in.add_item_list.push_back(item );
	}

	//加物品
	
	p->item_list.add(p_in->itemid,1);
	item.itemid=p_in->itemid;
	item.count=1;
	item.max_count=1;
	db_in.add_item_list.push_back(item );

	stat_log_item(p_in->itemid);

	//同步db
	send_msg_to_db(p, pop_exchange_cmd ,&db_in );
	cli_out.left_gamept = p->get_valid_gamepoint();

	p->send_succ( &cli_out);
	return 0;
}

int cli_get_valid_gamept(Csprite *p, Cmessage* c_in)
{
	cli_get_valid_gamept_out cli_out;
	cli_out.left_gamept = p->get_valid_gamepoint();
	p->send_succ(&cli_out);
	return 0;
}
/**/
int cli_get_user_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_info_in * p_in = P_IN;
	cli_get_user_info_out	cli_out ;
	Csprite* obj_p = (p_in->obj_userid == 0 || p_in->obj_userid == p->id) // 是自己
		? p : g_sprite_map->get_sprite(p_in->obj_userid);
	if(obj_p){
		obj_p->get_user_info(cli_out);
		p->send_succ( &cli_out);
	}else{
		p->send_err( cli_user_no_in_map_err );
	}

	return 0;
}

int cli_get_user_game_stat(Csprite *p, Cmessage* c_in)
{
	cli_get_user_game_stat_in* p_in = P_IN;
	cli_get_user_game_stat_out cli_out;
	Csprite* obj_p = (p_in->obj_userid == 0 || p_in->obj_userid == p->id)
		? p : g_sprite_map->get_sprite(p_in->obj_userid);
	if (obj_p) {
		obj_p->game_stat_map.get_game_state_list(cli_out.game_stat_list);
		cli_out.obj_userid = obj_p->id;
	} else {
		cli_out.obj_userid = p_in->obj_userid;
	}

	p->send_succ(&cli_out);
	return 0;
}

/*做表情*/
int cli_show(Csprite *p, Cmessage* c_in)
{
	cli_show_in * p_in = P_IN;
	cli_noti_show_out noti_out;		
	noti_out.src_userid=p->id;
	noti_out.expression_id=p_in->expression_id;
	noti_out.obj_userid = p_in->obj_userid;

	// 检查是否要扣除一些物品
	uint32_t itemid = g_item_movement_map.get_movement_item(p_in->expression_id);
	if (itemid) {
		if (!p->item_list.check_enough(itemid, 1)) {
			p->send_err(cli_request_arg_err);
			return 0;
		}
		p->item_list.del(itemid, 1);
		pop_exchange_in db_in;
		item_exchange_t item_ex;
		item_ex.itemid = itemid;
		item_ex.count = 1;
		item_ex.max_count = MAX_ITEM_COUNT;
		db_in.del_item_list.push_back(item_ex);
		send_msg_to_db(p, pop_exchange_cmd, &db_in);
	}

	if(p_in->obj_userid == 0 && p->get_is_in_muti_map() ){
		//通知
		g_sprite_map->noti_to_map_all(p, cli_noti_show_cmd, &noti_out);
	} else if (p_in->obj_userid == 1) {
		g_sprite_map->game_map->noti_all(p->game_map_id, cli_noti_show_cmd, &noti_out);	
	}

	p->send_succ();
	return 0;
}

// 投诉建议
int cli_post_msg(Csprite *p, Cmessage* c_in)
{
	cli_post_msg_in* p_in = P_IN;
	udp_post_msg_in db_in;	
	db_in.gameid = 12;
	db_in.type = p_in->type;
	memcpy(db_in.nick, p->nick, sizeof(p->nick));
	memcpy(db_in.title, p_in->title, sizeof(db_in.title));
	db_in._msg_len = p_in->_msg_len;
	memcpy(db_in.msg, p_in->msg, db_in._msg_len);
	send_udp_post_msg_to_db(p->id, udp_post_msg_cmd, &db_in);
	p->send_succ();
	return 0;
}

// 设置忙状态
int cli_set_busy_state(Csprite* p, Cmessage* c_in)
{
	cli_set_busy_state_in* p_in = P_IN;
	p->busy_state = p_in->state;
	p->send_succ();
	return 0;
}

// 打工获得抽奖次数, 已下架
int cli_work_get_lottery(Csprite* p, Cmessage* c_in)
{
	cli_work_get_lottery_out cli_out;
	p->send_succ(&cli_out);
	return 0;	
}

// 抽奖，已下架
int cli_draw_lottery(Csprite* p, Cmessage* c_in)
{
	p->send_succ();
	return 0;
}

// 已下架
int cli_get_lottery_count(Csprite* p, Cmessage* c_in)
{
	cli_get_lottery_count_out cli_out;
	p->send_succ(&cli_out);
	return 0;
}

// 获取特殊物品的数量
int cli_get_spec_item(Csprite* p, Cmessage* c_in)
{
	cli_get_spec_item_in* p_in = P_IN;
	cli_get_spec_item_out cli_out;
	cli_out.item.itemid = p_in->itemid;
	cli_out.item.total = p->spec_item_list.get_count(p_in->itemid);
	cli_out.item.daycnt = p->spec_item_list.get_day_count(p_in->itemid);
	p->send_succ(&cli_out);
	return 0;
}

// 获取特殊物品的数量
int cli_get_spec_item_list(Csprite* p, Cmessage* c_in)
{
	cli_get_spec_item_list_in* p_in = P_IN;
	cli_get_spec_item_list_out cli_out;
	p->spec_item_list.get_list(p_in->startid, p_in->endid, cli_out.spec_item_list);
	p->send_succ(&cli_out);
	return 0;
}
// 挂铃铛获得奖品，已下架
int cli_hang_bell_get_item(Csprite* p, Cmessage* c_in)
{
	cli_hang_bell_get_item_out cli_out;
	p->send_succ(&cli_out);
	return 0;
}

// 用户客户端删除指定数量的物品 
int cli_del_item(Csprite* p, Cmessage* c_in)
{
	cli_del_item_in* p_in = P_IN;
	for (uint32_t loop = 0; loop < p_in->itemlist.size(); loop ++) {
		item_t& tmp_item = p_in->itemlist[loop];
		if (!p->item_list.check_enough(tmp_item.itemid, tmp_item.count)) {
			p->send_err(cli_request_arg_err);
			return 0;
		}
	}

	pop_exchange_in db_in;
	item_exchange_t item_ex;
	for (uint32_t loop = 0; loop < p_in->itemlist.size(); loop ++) {
		item_t& tmp_item = p_in->itemlist[loop];
		p->item_list.del(tmp_item.itemid, tmp_item.count);
		item_ex.itemid = tmp_item.itemid;
		item_ex.count = tmp_item.count;
		db_in.del_item_list.push_back(item_ex);
	}

	send_msg_to_db(p, pop_exchange_cmd, &db_in);
	p->send_succ();

	return 0;
}

// 圣诞树点击统计
int cli_click_chris_tree(Csprite* p, Cmessage* c_in)
{
	stat_log_click_chris_tree(p->id);
	p->send_succ();
	return 0;
}

// 点击新手指引统计
int cli_click_guide(Csprite* p, Cmessage* c_in)
{
	stat_log_click_guide(p->id);
	if (!(p->flag & user_flag_guide)) {
		p->flag |= user_flag_guide;
		pop_set_flag_in db_in;
		db_in.flag = p->flag;
		db_in.mask = user_flag_guide;
		send_msg_to_db(p, pop_set_flag_cmd, &db_in);
	}

	p->send_succ();
	return 0;
}

// 点击博士的统计
int cli_click_stat(Csprite* p, Cmessage* c_in)
{
	cli_click_stat_in* p_in = P_IN;
	stat_log_click(p->id, p_in->type);
	p->send_succ();
	return 0;
}

// 点击游戏王公告统计
int cli_click_game_notice(Csprite* p, Cmessage* c_in)
{
	stat_log_click_game_notice(p->id);
	p->send_succ();
	return 0;
}

int cli_click_pet_egg(Csprite* p, Cmessage* c_in)
{
	p->spec_item_list.change_count(spec_item_pet_egg_count, 1);
	stat_log_click_egg(p->id, p->spec_item_list.get_count(spec_item_pet_egg_count));
	p->send_succ();
	return 0;
}

int cli_get_island_time(Csprite* p, Cmessage* c_in)
{
	cli_get_island_time_out cli_out;
	cli_out.svr_time = time(NULL);
	std::map<uint32_t, Cisland_time>::iterator it;
	for (it = g_map_conf_map.island_time_map.begin(); 
			it != g_map_conf_map.island_time_map.end(); it ++) {
		Cisland_time& island_time = it->second;
		island_time_t tmp_island_time;
		tmp_island_time.islandid = it->first;
		tmp_island_time.is_open = island_time.get_is_open();
		tmp_island_time.next_state_time = island_time.get_next_state_time();
		cli_out.islandlist.push_back(tmp_island_time);
	}
	p->send_succ(&cli_out);
	return 0;
}

int cli_get_login_chest(Csprite* p, Cmessage* c_in)
{
	cli_get_login_chest_out cli_out;
	if (p->spec_item_list.get_day_count(spec_item_chest_get) > 0) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	if (p->rand_get_item(rand_id_login_chest, cli_out.item_get)) {
		p->spec_item_list.change_count(spec_item_chest_get, 1);
		stat_log_login_chest(p->id);
	}
	p->send_succ(&cli_out);

	return 0;
}

int cli_get_login_reward(Csprite* p, Cmessage* c_in)
{
	uint32_t day[8] = {1, 2, 3, 5, 10, 15, 20, 0};
	Citem rewardlist[8] = {
		{spec_item_xiaomee, 10},
		{spec_item_xiaomee, 20},
		{spec_item_xiaomee, 30},
		{item_eye_blue, 1},
		{item_eye_red, 1},
		{item_eye_green, 1},
		{item_pet, 1},
		{spec_item_xiaomee, 50},
	};
	cli_get_login_reward_in *p_in = P_IN;
	if (p_in->index > 7) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	if (p->spec_item_list.get_count(spec_item_login_count) < day[p_in->index]) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	if (p->spec_item_list.get_count(spec_item_login_reward_1 + p_in->index) > 0) {
		p->send_err(cli_request_arg_err);
		return 0;
	}

	stat_log_login_reward(p->id, p_in->index);	
	p->spec_item_list.change_count(spec_item_login_reward_1 + p_in->index, 1);
	pop_exchange_in db_in;
	item_exchange_t item;
	item.itemid = rewardlist[p_in->index].itemid;
	item.count = rewardlist[p_in->index].count;
	if (item.itemid == spec_item_xiaomee) {
		item.max_count = MAX_SPEC_ITEM_COUNT;
		p->change_xiaomee(item.count);
	} else {
		item.max_count = 1;
		p->item_list.add(item.itemid, item.count);
	}
	db_in.add_item_list.push_back(item);
	send_msg_to_db(p, pop_exchange_cmd, &db_in);

	cli_get_login_reward_out cli_out;
	cli_out.item_get.itemid = item.itemid;
	cli_out.item_get.count = item.count;
	p->send_succ(&cli_out);
	return 0;
}

int cli_set_color(Csprite* p, Cmessage* c_in)
{
	cli_set_color_in* p_in = P_IN;
	p->color = p_in->color;
	pop_set_color_in db_in;
	db_in.color = p_in->color;
	send_msg_to_db(p, pop_set_color_cmd, &db_in);
	p->send_succ();

	if (p->get_is_in_muti_map()) {
		cli_noti_set_color_out noti_out;
		noti_out.userid = p->id;
		noti_out.color = p->color;
		g_sprite_map->noti_to_map_all(p, cli_noti_set_color_cmd, &noti_out);
	}
	return 0;
}
