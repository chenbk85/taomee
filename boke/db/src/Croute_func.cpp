/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:  
 *
 *		Version:  1.0
 *		Created:  2009年3月02日 16时39分06秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Croute_func.h" 
#include  <libtaomee++/utils/strings.hpp>

extern  char * version_str;

#include "./proto/pop_db.h"
stru_cmd_item_t g_cmd_map[]={
#include "./proto/pop_db_bind.h"
};
#define DEALFUN_COUNT  (int(sizeof(g_cmd_map )/sizeof(g_cmd_map[0])))

stru_cmd_db_name g_cmd_db_name_list[]={
	{ 0,{ "XXXX"	} }	,
	{ 0x1000,{ "XXXX"	} }	,
	{ 0x7400,{ "POP"	} }	,
};
/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Croute_func::Croute_func (mysql_interface * db):Cfunc_route_db_v2(db),
	user(db),task(db),item(db),find_map(db),user_log(db),day(db), game(db)
{
	DEBUG_LOG("cmdid count:%u",DEALFUN_COUNT );
	this->set_cmd_list(g_cmd_map,DEALFUN_COUNT );
	this->init_cmd_db_map(g_cmd_db_name_list,
			sizeof(g_cmd_db_name_list )/sizeof(g_cmd_db_name_list[0]) );
}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */


/*登入*/
int Croute_func::pop_login(DEAL_FUNC_ARG)
{
	pop_login_out* p_out=P_OUT;
	ret=this->user.get_all_info(userid,&(p_out->user_base_info));
	if (ret==USER_ID_NOFIND_ERR ){//不存在insert
		pop_reg_in  item;
		memset(&item,0,sizeof (item) );
		this->user.insert(userid, &item );
		ret=this->user.get_all_info(userid,&(p_out->user_base_info));
	}

	if(ret!=SUCC) return ret;

	ret=this->task.get_list(userid,p_out->task_list);
	if(ret!=SUCC) return ret;
	ret=this->item.get_list(userid,p_out->item_list);
	if(ret!=SUCC) return ret;
	ret = this->day.get_list(userid, p_out->item_day_list);
	if(ret!=SUCC) return ret;
	ret=this->find_map.get_list(userid,p_out->find_map_list);
	if(ret!=SUCC) return ret;
	ret=this->user_log.get_list(userid,p_out->user_log_list);
	if (ret!=SUCC) return ret;
	ret=this->game.get_list(userid, p_out->game_info_list);

	return ret;
}

/*注册*/
int Croute_func::pop_reg(DEAL_FUNC_ARG)
{
	pop_reg_in* p_in=P_IN;

	ret=this->user.update_reg(userid,p_in);
	if (ret!=SUCC) return ret;

	for (uint32_t i=0;i<p_in->add_item_list.size();i++) {
		item_exchange_t *p_item= &(p_in->add_item_list[i]);
		if (p_item->itemid == 2) {
			ret = this->user.change_xiaomee(userid, p_item->count);
		} else {
			ret=this->item.change_count(userid,
				p_item->itemid,p_item->count,p_item->max_count ,true );
		}
		if (ret!=SUCC)	 return ret; 
	}

	user_log_t user_log_item;
	user_log_item.logtime=time(NULL);
	user_log_item.v1=1;
	user_log_item.v2=0;
	ret=this->user_log.insert(userid,&user_log_item );

	return ret;
}
/*完成子节点任务*/
int Croute_func::pop_task_complete_node(DEAL_FUNC_ARG)
{
	pop_task_complete_node_in* p_in=P_IN;
	ret=this->task.insert(userid,p_in->taskid,p_in->task_nodeid );
	return ret;
}

/*交换物品*/
int Croute_func::pop_exchange(DEAL_FUNC_ARG)
{
	pop_exchange_in* p_in=P_IN;
	if(p_in->opt_flag!=0){
		return ENUM_OUT_OF_RANGE_ERR;
	}
	uint32_t i;
	for( i=0 ; i<p_in->del_item_list.size() ; i++ ) {
		item_exchange_t *p_item=&p_in->del_item_list[i];
		if( p_item->itemid==1 ){//任务
			ret=this->task.del(userid,p_item->count ,p_item->max_count );
		}else if( p_item->itemid==2 ){//是xiaomee
			ret=this->user.change_xiaomee(userid, -p_item->count);
		}else if (p_item->itemid < 50000) {
			ret = this->day.change_count(userid, p_item->itemid, -p_item->count);
		}else{
			ret=this->item.change_count(userid,p_item->itemid,
				-p_item->count,p_item->max_count );
		}
		if(ret!=SUCC){
			STD_ROLLBACK();
			return ret;
		}
	}
	
	for( i=0 ; i<p_in->add_item_list.size() ; i++ ) {
		item_exchange_t *p_item=&p_in->add_item_list[i];
		if( p_item->itemid==1 ){//任务
			uint32_t taskid= p_item->count ;
		   	uint32_t task_nodeid=p_item->max_count;
			ret=this->task.insert(userid,taskid,task_nodeid );
			if(ret==SUCC && task_nodeid==99999){//是该岛的最后任务
				//记录足迹
				user_log_t user_log_item;
				user_log_item.logtime=time(NULL);
				user_log_item.v1=2;
				user_log_item.v2=taskid;
				ret=this->user_log.insert(userid, &user_log_item);
			}
		}else if( p_item->itemid==2 ){//是xiaomee
			ret=this->user.change_xiaomee(userid, p_item->count);
		} else if (p_item->itemid < 50000) { // 是小游戏积分
			ret = this->day.change_count(userid, p_item->itemid,  p_item->count, p_item->day_add_cnt);
		}else{
			ret=this->item.change_count(userid,p_item->itemid,
				p_item->count,p_item->max_count );
		}
		if(ret!=SUCC){
			STD_ROLLBACK();
			return ret;
		}
	}

	return ret;
}


int Croute_func::pop_find_map_add(DEAL_FUNC_ARG){
	pop_find_map_add_in* p_in=P_IN;
	ret=this->find_map.insert(userid,p_in->islandid,p_in->mapid );
	return ret;
}

/*退出*/
int Croute_func::pop_logout(DEAL_FUNC_ARG){
	pop_logout_in* p_in=P_IN;
	ret=this->user.update_logout_info(userid,p_in );
	return ret;
}

/*设置昵称*/
int Croute_func::pop_set_nick(DEAL_FUNC_ARG){
	pop_set_nick_in* p_in=P_IN;
	ret=this->user.set_nick(userid,p_in );	

	return ret;
}

/*设置使用列表，会将原有使用的设置为未使用*/
int Croute_func::pop_set_item_used_list(DEAL_FUNC_ARG){
	pop_set_item_used_list_in* p_in=P_IN;

	uint32_t i;
	//移除物品
	for (i=0;i<p_in->del_can_change_list.size();i++ ){
		ret=this->item.del(userid,p_in->del_can_change_list[i]);
		if(ret!=SUCC){
			STD_ROLLBACK();
			return ret;
		}	
	}
	//设置use_count=0
	ret=this->item.set_used_list(userid,p_in->set_noused_list ,0);
	if(ret!=SUCC){
		STD_ROLLBACK();
		return ret;
	}	

	//增加物品
	for (i=0;i<p_in->add_can_change_list.size();i++ ){
		ret=this->item.insert(userid,p_in->add_can_change_list[i],1,1);
		if(ret!=SUCC){
			STD_ROLLBACK();
			return ret;
		}	
	}
		
	//设置use_count=1
	ret=this->item.set_used_list(userid,p_in->set_used_list,1);
	if(ret!=SUCC){
		STD_ROLLBACK();
		return ret;
	}	


	return ret;
}

/*删除任务*/
int Croute_func::pop_task_del(DEAL_FUNC_ARG){
	pop_task_del_in* p_in=P_IN;

	ret=this->task.del(userid,p_in->taskid,p_in->task_nodeid );

	return SUCC;
}

/*得到用户的所有信息*/
int Croute_func::pop_get_all_info(DEAL_FUNC_ARG){
	pop_get_all_info_out* p_out=P_OUT;

	ret=this->user.get_all_info(userid,&(p_out->user_base_info));
	if(ret!=SUCC) return ret;

	ret=this->task.get_list(userid,p_out->task_list);
	if(ret!=SUCC) return ret;
	ret=this->item.get_list(userid,p_out->item_list);
	if(ret!=SUCC) return ret;
	ret=this->day.get_list(userid,p_out->item_day_list);
	if(ret!=SUCC) return ret;
	ret=this->find_map.get_list(userid,p_out->find_map_list);
	if(ret!=SUCC) return ret;
	ret=this->user_log.get_list(userid,p_out->user_log_list);
	if(ret!=SUCC) return ret;
	ret=this->game.get_list(userid, p_out->game_info_list);




	return ret;
}

/*设置某一字段的值*/
int Croute_func::pop_user_set_field_value(DEAL_FUNC_ARG){
	pop_user_set_field_value_in* p_in=P_IN;

	ret=this->user.set_field_value(userid,p_in);

	return ret;
}
/*设置相关的值*/
int Croute_func::pop_opt(DEAL_FUNC_ARG){
	pop_opt_in* p_in=P_IN;
	if (p_in->opt_groupid==1){				//物品
		if(p_in->opt_type == 1) {
			ret=this->item.change_count(userid,p_in->optid,p_in->count, 0xFFFFF);
		} else if (p_in->opt_type == 3) {
			ret = this->item.set_use(userid, p_in->optid, bool(p_in->count));
		}
	}else if (p_in->opt_groupid==2){		//任务
		//设置
		if (p_in->count==1 ){
			ret=this->task.insert(userid,p_in->optid ,p_in->v1);
		}else{
			ret=this->task.del(userid,p_in->optid ,p_in->v1);
		}
	}else if (p_in->opt_groupid==3){		//找到地图
		//设置
		if (p_in->count==1 ){
			ret=this->find_map.insert(userid,p_in->optid ,p_in->v1);
		}else{
			ret=this->find_map.del(userid,p_in->optid ,p_in->v1);
		}
	} else if (p_in->opt_groupid == 4) {	// 每日物品
		if (p_in->opt_type == 1) {
			if (p_in->count > 0) {
				ret = this->day.change_count(userid, p_in->optid, p_in->count, p_in->count);
			} else {
				ret = this->day.del(userid, p_in->optid);
				//ret = this->day.change_count(userid, p_in->optid, p_in->count, 0);
			}
		} else if (p_in->opt_type == 2) {
			ret = this->day.back_day(userid, p_in->optid);
		}
	} else if (p_in->opt_groupid == 5) {
		ret = this->game.add_game_info(userid, p_in->optid, p_in->count, p_in->v1, p_in->v2);
	} else if (p_in->opt_groupid == 997) {
		char str[100] = {0};
		sprintf(str,"cd ~/boke/online/bin&&./startup.sh");
		DEBUG_LOG("RESTART\t[%s]", str);
		system(str);
		ret = 0;	
	} else if (p_in->opt_groupid == 998) {
		char str[100] = {0};
		sprintf(str,"cd ~/boke/online/src&&./getclixml.sh");
		DEBUG_LOG("UPDATE XML\t[%s]", str);
		system(str);
		ret = 0;	
	} else if (p_in->opt_groupid == 999) {	// 一键初始化
		this->user.delete_all_by_userid(userid);
		this->user_log.delete_all_by_userid(userid);
		this->day.delete_all_by_userid(userid);
		this->game.delete_all_by_userid(userid);
		this->find_map.delete_all_by_userid(userid);
		this->item.delete_all_by_userid(userid);
		this->task.delete_all_by_userid(userid);
		ret = SUCC;
	} else {
		ret=ENUM_OUT_OF_RANGE_ERR;
	}


	return ret;
}

/*得到版本信息*/
int Croute_func::get_server_version(DEAL_FUNC_ARG){
//	sleep(10);
	get_server_version_out* p_out=P_OUT;
	memset(p_out->version,0,sizeof(p_out->version));
    std::vector<std::string> file_name_list;
    strncpy(p_out->version,get_version_str("2011-08-11",file_name_list ).c_str(),sizeof ( p_out->version)  );
	ret=SUCC;
	return ret;
}

/*复制用户*/
int Croute_func::pop_copy_user(DEAL_FUNC_ARG){
	pop_copy_user_in* p_in=P_IN;
	uint32_t dsc_userid=p_in->dsc_userid;
	std::string tmp_sql_str , sql_str;		
	this->user.get_insert_sql_by_userid(userid,tmp_sql_str,"userid",dsc_userid );
	sql_str+=tmp_sql_str;
	this->find_map.get_insert_sql_by_userid(userid,tmp_sql_str,"userid",dsc_userid );
	sql_str+=tmp_sql_str;
	this->item.get_insert_sql_by_userid(userid,tmp_sql_str,"userid",dsc_userid );
	sql_str+=tmp_sql_str;
	this->task.get_insert_sql_by_userid(userid,tmp_sql_str,"userid",dsc_userid );
	sql_str+=tmp_sql_str;

	this->user_log.get_insert_sql_by_userid(userid,tmp_sql_str,"userid",dsc_userid );
	sql_str+=tmp_sql_str;
	
	
    //导入数据
    int db_ret=0;
    mysql_set_server_option(&this->db->handle, MYSQL_OPTION_MULTI_STATEMENTS_ON);
    db_ret=mysql_query( &this->db->handle,sql_str.c_str()  );
    MYSQL_RES *result;
    do {
        if ((result= mysql_store_result(&this->db->handle ))!=NULL ){
            mysql_free_result(result);
        }
    } while (!mysql_next_result(&this->db->handle  ));
    mysql_set_server_option(&this->db->handle, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	
	ret=SUCC;
	return ret;
}


/*用户足迹*/
int Croute_func::pop_user_log_add(DEAL_FUNC_ARG){
	pop_user_log_add_in* p_in=P_IN;
	ret=this->user_log.insert(userid,&(p_in->user_log) );
	return ret;
}

int Croute_func::udp_post_msg(DEAL_FUNC_ARG)
{
	return 0;
}

int Croute_func::pop_add_game_info(DEAL_FUNC_ARG)
{
	pop_add_game_info_in* p_in = P_IN;
	ret = this->game.add_game_info(userid, p_in->gameid, p_in->win_flag);
	return ret;
}

int Croute_func::pop_get_base_info(DEAL_FUNC_ARG)
{
	pop_get_base_info_out* p_out = P_OUT;
	ret = this->user.get_last_online_id(userid, p_out->last_online_id);
	return ret;
}
int Croute_func::pop_set_flag(DEAL_FUNC_ARG)
{
	pop_set_flag_in* p_in = P_IN;
	ret = this->user.set_flag(userid, p_in);
	return ret;
}

int Croute_func::pop_set_color(DEAL_FUNC_ARG)
{
	pop_set_color_in* p_in = P_IN;
	ret = this->user.set_color(userid, p_in);
	return ret;
}
// ./proto/pop_db_db_src.cpp
