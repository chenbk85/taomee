#ifndef  GF_CROUTE_FUNC_H
#define  GF_CROUTE_FUNC_H

#include <libtaomee++/proto/proto_base.h>
#include "kf/vip_config_data_mgr.hpp"
#include "Citem_change_log.h"


#include "Cfunc_route_base.h"
#include "Ctable.h"
#include "Cgongfu.h"
#include "Cgf_attire.h"
#include "Cgf_item.h"
#include "Cgf_friend.h"
#include "Cgf_role.h"
#include "Cgf_task.h"
#include "Cgf_skill.h"
#include "Cgf_killboss.h"
#include "Cgf_summon.h"
#include "Cgf_warehouse.h"
#include "Cgf_clothes_warehouse.h"
#include "Cgf_invite_code.h"
#include "Cgf_daily_action.h"
#include "Cgf_weekly_action.h"
#include "Cgf_monthly_action.h"
#include "Cgf_amb.h"
#include "Csend_email.h"
#include "Cgf_secondary_pro.h"
#include "Cgf_mail.h"
#include "Cgf_item_log.h"
#include "Cgf_market_log.h"
#include "Cgf_buff.h"
#include "Cgf_strengthen_material.h"
#include "Cgf_hero_top.h"
#include "Cgf_get_shopitem_log.h"
#include "Cgf_shop_log.h"
#include "Cgf_hunter_top.h"
#include "Cgf_donate.h"
#include "Cgf_buy_item_limit.h"
#include "Cgf_kill_boss.h"
#include "Cgf_achievement.h"
#include "Cgf_title.h"
#include "Cgf_ap_toplist.h"
#include "Cgf_contest.h"
#include "Cgf_ring_task.h"
#include "Cgf_pvp_game.h"
#include "Cgf_swap_action.h"
#include "Cgf_plant.h"
#include "Cgf_home.h"
#include "Cgf_home_log.h"
#include "Cgf_summon_dragon_list.h"
#include "Cgf_card.h"
#include "Cgf_summon_skill.h"
#include "Cgf_team.h"
#include "Cgf_team_detail.h"
#include "Cgf_wuseng_time.h"
#include "Cgf_other_info.h"
#include "Cgf_ranker.h"
#include "Cgf_master.h"
#include "Cgf_reward.h"
#include "Cgf_decorate.h"
#include "Cgf_tmp_info.h"
#include "Cgf_numen.h"
#include "Cgf_numen_skill.h"
#include "Cgf_other_active.h"
#include "Cgf_stat_info.h"


#define  BIND_PROTO_CMD(cmdid,cmd_name,struct_in,struct_out,md5_tag,bind_bitmap ) \
	{cmdid, new struct_in(),new struct_out (), md5_tag,&Croute_func::cmd_name },	
#define  P_OUT dynamic_cast<typeof p_out>(c_out);
#define  P_IN dynamic_cast<typeof p_in>(c_in);
#define  DEAL_FUNC_ARG  userid_t userid, Cmessage * c_in, Cmessage * c_out 

class Croute_func;
//定义调用函数的指针类型
typedef   int(Croute_func::*P_DEALFUN_NEW_T)(DEAL_FUNC_ARG);
typedef   struct stru_cmd_item{
	uint16_t cmdid;
	Cmessage * msg_in; 
	Cmessage * msg_out; 
	uint32_t  md5_tag;
	P_DEALFUN_NEW_T p_deal_func; 
} stru_cmd_item_t;





#define __GF_DEBUG__


/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */

#define DEFAULT_MAX_BACKAGE 9999
#define COUPONS_MAX_LIMIT   9999999

#define VIP_YEAR_USER_FLAG 9 //binary 00001001

#define strengthen_material_base_id_1 1540000
#define strengthen_material_base_id_2 1740000

const uint32_t gold_coin_item_id = 1740029;

class Croute_func:public Cfunc_route_base 
{
  private:
	stru_cmd_item_t *p_cmd_list;//命令行列表
	byte_array_t ba_in;
	byte_array_t ba_out;
	/**
	 * 功能:从deal_fun 统一初始化
	 * count:数组中元素的个数.
	 */
	inline void set_cmd_list(stru_cmd_item_t * p_list, int count ){
		for (int i=0;i<count;i++ ){
			uint16_t cmdid=p_list[i].cmdid;
			assert( this->p_cmd_list[cmdid].cmdid==0  );
			this->p_cmd_list[cmdid]=p_list[i];
		}
	}
	inline stru_cmd_item_t * get_cmd_item(uint16_t cmdid){
		if (this->p_cmd_list[cmdid].cmdid!=0 ) return & (this->p_cmd_list[cmdid]);
		else return NULL;
	}


	Cgongfu		gongfu;
	Cgf_attire	gf_attire;
	Cgf_item    gf_item;
	Cgf_friend  gf_friend;
	Cgf_role	gf_role;
	Cgf_task	gf_task;
	Cgf_skill	gf_skill;
	Cgf_invite_code gf_invite_code;
	Cgf_killboss gf_killboss;
	Cgf_summon gf_summon;
	Cgf_daction gf_daction;
	Cgf_waction gf_waction;
	Cgf_maction gf_maction;
	Cgf_warehouse gf_warehouse;
	Cgf_clothes_warehouse gf_clothes_warehouse;		
	Cgf_amb	gf_amb;
    Cclientproto *cp;
    Csend_email send_email;
	Cgf_secondary_pro    gf_secondary_pro;
	Cgf_mail             gf_mail;
	Cgf_item_log		 gf_item_log;
	Cgf_get_shopitem_log gf_get_shopitem_log;
	Cgf_market_log		 gf_market_log;
	Cgf_buff			 gf_buff;
   	Cgf_StrengthenMaterial gf_material;
	Cgf_hero_top gf_hero_top;
	Cgf_shop_log gf_shop_log;
    Cgf_hunter_top gf_hunter_top;
	Cgf_donate    gf_donate;
	Cgf_buy_item_limit  gf_buy_item_limit;
	Cgf_kill_boss       gf_kill_boss;
	Cgf_achievement     gf_achieve;
	Cgf_stat_info       gf_stat;
	Cgf_title           gf_title;
	Cgf_ap_toplist      gf_ap_toplist;
	Cgf_ring_task       gf_ring_task;
	Cgf_ring_task_history  gf_ring_task_history;
	Cgf_contest_info    gf_contest;
    Cgf_pvp_game        gf_taotai;
	Cgf_swap_action		gf_swap_action;
	Cgf_plant			gf_plant;
	Cgf_home            gf_home;
	Cgf_home_log			gf_home_log;
	Cgf_summon_dragon_list  gf_summon_dragon_list;
	Citem_change_log	gf_item_change_log;
	Cgf_card            gf_card;
	Cgf_summon_skill    gf_summon_skill;
	Cgf_team			gf_team;
	Cgf_team_detail		gf_team_detail;

	Cgf_wuseng_info     gf_wuseng_info;
	Cgf_other_info		gf_other_info;
	Cgf_ranker_info     gf_ranker;
	Cgf_master          gf_master;
	Cgf_reward          gf_reward;
	Cgf_decorate        gf_decorate;
	Cgf_tmp_info		gf_tmp_info;
	Cgf_numen           gf_numen;
	Cgf_numen_skill     gf_numen_skill;
	Cgf_other_active    gf_other_active;
  private:
    vip_config_data_mgr vip_obj;
  	add_attire_t m_attire_elem;
	char		m_tmpinfo[PROTO_MAX_SIZE];
	char		m_tmpinfo1[PROTO_MAX_SIZE];
	int do_sync_data(uint32_t userid, uint16_t cmdid );
  public:
	Croute_func (mysql_interface * db, Cclientproto *cp); 
    ~Croute_func ();

#define PROTO_FUNC_DEF(cmd_name)\
	int cmd_name(DEAL_FUNC_ARG);
	#include "./gf_db_func_def.h"

	virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		PRI_STRU * p_pri_stru;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;
		if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
			DEBUG_LOG("I:%04X:%d", cmdid, userid );
			//检查协议频率
			if (! p_pri_stru->exec_cmd_limit.add_count() ){
				DEBUG_LOG("cmd max err:cmdid %u, max_count:%u ",
					cmdid,p_pri_stru->exec_cmd_limit.limit_max_count_per_min);
			
				return CMD_EXEC_MAX_PER_MINUTE_ERR;
			}
	
	
			//检查报文长度
			if (! p_pri_stru->check_proto_size(rcvlen - (PROTO_HEADER_SIZE)) ){
				DEBUG_LOG("len err pre [%lu] send [%d]",
				p_pri_stru->predefine_len+(PROTO_HEADER_SIZE) ,rcvlen );
				return PROTO_LEN_ERR;
			}
	
			//处理在线分裂数据
			if (this->sync_user_data_flag ){//是否打开同步标志
				ret=this->do_sync_data(userid,cmdid);
				if (ret!=SUCC) return ret;
			}
	
			this->ret=9999;
			//调用相关DB处理函数
			*sendbuf=NULL;
			*sndlen=0;
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
			/*
			if ( this->ret==SUCC && *sendbuf==NULL ){
				DEBUG_LOG("ERROR:this->ret==SUCC && *sendbuf==NULL, use STD_RETURN() ");
                STD_ROLLBACK();
                return SYS_ERR;
			}
			*/

			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
	
			return this->ret;
		}else{
			//调用新的分
			return  this->deal_new(recvbuf, rcvlen, sendbuf, sndlen );
		}
	}

	virtual int deal_func(uint16_t cmdid, userid_t userid, Cmessage * c_in, Cmessage * c_out ,P_DEALFUN_NEW_T p_func  )
	{
		//处理在线分裂数据
		if (this->sync_user_data_flag ){//是否打开同步标志
			ret=this->do_sync_data(userid,cmdid);
			if (ret!=SUCC) return ret;
		}

		//调用相关DB处理函数
		this->ret=9999;
		this->ret=(((Croute_func*)this)->*(p_func))(
				userid,c_in ,c_out);	

		//提交数据
		if (mysql_commit(&(this->db->handle))!=DB_SUCC){
			this->db->show_error_log("db:COMMIT:err");
		}
		return this->ret;


	}



	int deal_new(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		stru_cmd_item_t * p_cmd_item;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;

		if((p_cmd_item =this->get_cmd_item(cmdid))!=NULL){
			KDEBUG_LOG ( userid ,"I:%04X", cmdid );
			//检查md5值,md5_tags 放在上送报文的result中
			uint32_t md5_value=((PROTO_HEADER*)recvbuf)->result;
			if (md5_value!=0 ){//==0,表示不用md5_tags检查
				if (p_cmd_item->md5_tag!=md5_value){
					KDEBUG_LOG(userid,"CMD_MD5_TAG_ERR:DB:%u,client:%u",
							p_cmd_item->md5_tag ,md5_value);
					return  CMD_MD5_TAG_ERR;
				}
			}


			//私有报文处理
			this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS-4,
						RECVLEN_FROM_RECVBUF-(PROTO_HEADER_SIZE-4) );
            
		
			p_cmd_item->msg_in->init();
			p_cmd_item->msg_out->init();
			//还原对象失败
			if (!p_cmd_item->msg_in->read_from_buf( this->ba_in)) {
				DEBUG_LOG("还原对象失败");
				return PROTO_LEN_ERR;	
			}
			//数据没有取完
			if (!this->ba_in.is_end()) {
				DEBUG_LOG("client 过多报文");
				return  PROTO_LEN_ERR;
			}

			//请求报文还原完毕,调用相关函数。

			this->ret=this->deal_func(cmdid,userid,
					p_cmd_item->msg_in,p_cmd_item->msg_out ,
					p_cmd_item->p_deal_func );

			if (this->ret==SUCC){//需要构造返回报文
				this->ba_out.init_postion();
				if (!p_cmd_item->msg_out->write_to_buf(this->ba_out)) {
					DEBUG_LOG("p_cmd_item->msg_out->write_to_buf :SYS_ERR" );
					return SYS_ERR;
				}

				if (!set_std_return( sendbuf,  sndlen,(PROTO_HEADER*)recvbuf , 
							SUCC,this->ba_out.get_postion() , PROTO_HEADER_SIZE-4 )) 
				{
					DEBUG_LOG("set_std_return:SYS_ERR: %u",
							this->ba_out.get_postion() );
					return SYS_ERR; 
				}
				memcpy(SNDBUF_PRIVATE_POS-4,this->ba_out.get_buf(),
						this->ba_out.get_postion()  );
				return SUCC;
			}else{//返回错误
				return this->ret;
			}
		}else{
			DEBUG_LOG("cmd no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}





//Cgongfu:

	int  gf_get_buy_item_limit_list(DEAL_FUN_ARG);
	int  gf_set_buy_item_limit_data(DEAL_FUN_ARG);

	int  get_warehouse_item_list(DEAL_FUN_ARG);
	int  gf_move_item_warehouse_to_bag(DEAL_FUN_ARG);
	int  gf_move_item_bag_to_warehouse(DEAL_FUN_ARG);

	int  get_warehouse_clothes_item_list(DEAL_FUN_ARG);
	int  gf_move_clothes_item_bag_to_warehouse(DEAL_FUN_ARG);
	int  gf_move_clothes_item_warehouse_to_bag(DEAL_FUN_ARG);

	int  gf_learn_secondary_pro(DEAL_FUN_ARG);
	int  gf_secondary_pro_fuse(DEAL_FUN_ARG);
	int  gf_get_secondary_pro_list(DEAL_FUN_ARG);

    int  gf_mail_head_list(DEAL_FUN_ARG);
	int  gf_mail_body(DEAL_FUN_ARG);
	int  gf_delete_mail(DEAL_FUN_ARG);
	int  gf_take_mail_enclosure(DEAL_FUN_ARG);
	int  gf_send_mail(DEAL_FUN_ARG);
	int  gf_reduce_money(DEAL_FUN_ARG);
	int  gf_send_system_mail(DEAL_FUN_ARG);

	int  gf_set_role_vitality_point(DEAL_FUN_ARG);
	int  gf_set_second_pro_exp(DEAL_FUN_ARG);

	int  gf_get_kill_boss_list(DEAL_FUN_ARG);
    int  gf_replace_kill_boss(DEAL_FUN_ARG);
	/* home */
	int  gf_get_home_data(DEAL_FUN_ARG);
	int  gf_set_home_data(DEAL_FUN_ARG);
	int  gf_set_home_update_tm(DEAL_FUN_ARG);
//   int  gf_replace_home_exp_level(DEAL_FUN_ARG);
//    int  gf_update_home_active_point(DEAL_FUN_ARG);	
	
	/* achieve */
	int  gf_get_achievement_data_list(DEAL_FUN_ARG);
	int  gf_replace_achievement_data(DEAL_FUN_ARG);
    int  gf_get_achieve_title(DEAL_FUN_ARG);
    int  gf_add_achieve_title(DEAL_FUN_ARG);
    int  gf_set_cur_achieve_title(DEAL_FUN_ARG);
	int  gf_get_ap_toplist(DEAL_FUN_ARG);	
	int gf_get_stat_info(DEAL_FUN_ARG);

	int gf_set_stat_info(DEAL_FUN_ARG);


	int gf_logout(DEAL_FUN_ARG);

	int gf_get_user_info(DEAL_FUN_ARG);
	int gf_get_info_for_login(DEAL_FUN_ARG);
	int gf_set_nick(DEAL_FUN_ARG);
	int gf_get_nick(DEAL_FUN_ARG);

	int gf_add_friend(DEAL_FUN_ARG);
	int gf_del_friend(DEAL_FUN_ARG);
	int gf_del_friend_whatever(DEAL_FUN_ARG);
	int gf_add_black(DEAL_FUN_ARG);
	int gf_del_black(DEAL_FUN_ARG);

	int gf_forbiden_add_friend_flag(DEAL_FUN_ARG);
	int gf_query_forbiden_friend_flag(DEAL_FUN_ARG);

	int gf_get_blacklist(DEAL_FUN_ARG);
	int gf_get_friendlist(DEAL_FUN_ARG);
	int gf_get_friendlist_type(DEAL_FUN_ARG);

	int gf_get_clothes_list(DEAL_FUN_ARG);
    int gf_get_strengthen_material_list(DEAL_FUN_ARG);
	int gf_wear_clothes(DEAL_FUN_ARG);
    int gf_take_off_clothes(DEAL_FUN_ARG);

	int gf_get_user_item_list(DEAL_FUN_ARG);
	int gf_set_item_int_value(DEAL_FUN_ARG);
	int gf_add_item(DEAL_FUN_ARG);
	int gf_add_item_attire(DEAL_FUN_ARG);
	int gf_del_item(DEAL_FUN_ARG);
	int gf_del_items(DEAL_FUN_ARG);
	int gf_bitch_sell_items(DEAL_FUN_ARG);
	int gf_buy_item(DEAL_FUN_ARG);
	int gf_sell_item(DEAL_FUN_ARG);
	int gf_battle_use_item(DEAL_FUN_ARG);
	int gf_pick_up_item(DEAL_FUN_ARG);
	int gf_pick_up_roll_item(DEAL_FUN_ARG);

	int gf_get_user_partial_info(DEAL_FUN_ARG);
	int gf_get_role_detail_info(DEAL_FUN_ARG);

	int gf_list_simple_role_info(DEAL_FUN_ARG);

	int gf_add_offline_msg(DEAL_FUN_ARG);

	int gf_role_login(DEAL_FUN_ARG);
    int gf_get_xiaomee(DEAL_FUN_ARG);

    int gf_set_role_uinque_itembit(DEAL_FUN_ARG);
	int gf_get_user_base_info(DEAL_FUN_ARG);
	uint32_t gf_get_player_vip_level(userid_t userid);
	int gf_set_user_base_info(DEAL_FUN_ARG);

	int gf_get_role_list(DEAL_FUN_ARG);

	int gf_get_role_list_ex(DEAL_FUN_ARG);
	
	int gf_set_role_itembind(DEAL_FUN_ARG);
	int gf_get_role_itembind(DEAL_FUN_ARG);
    int gf_get_double_exp_data(DEAL_FUN_ARG);
    int gf_set_double_exp_data(DEAL_FUN_ARG);
    int gf_get_max_times_chapter(DEAL_FUN_ARG);
    int gf_set_max_times_chapter(DEAL_FUN_ARG);

    int gf_get_package_iclist(DEAL_FUN_ARG);

	int gf_del_attire(DEAL_FUN_ARG);
	int gf_add_attire(DEAL_FUN_ARG);
	int gf_sell_attire(DEAL_FUN_ARG);
	int gf_repair_attire(DEAL_FUN_ARG);
	int gf_repair_all_attire(DEAL_FUN_ARG);
	int gf_get_clothes_list_ex(DEAL_FUN_ARG);
    	int gf_attire_set_int_value(DEAL_FUN_ARG);
    	int gf_compose_attire(DEAL_FUN_ARG);
   	 int gf_decompose_attire(DEAL_FUN_ARG);
    	int gf_strengthen_attire(DEAL_FUN_ARG);
	//added by cws 0620
	int gf_strengthen_attire_without_material(DEAL_FUN_ARG);
    	int gf_buy_goods(DEAL_FUN_ARG);
    	int gf_sell_goods(DEAL_FUN_ARG);

	int gf_add_role(DEAL_FUN_ARG);

//	int gf_set_role_delflg(DEAL_FUN_ARG);

	int gf_user_login(DEAL_FUN_ARG);

	int gf_set_role_int_value(DEAL_FUN_ARG);

	int gf_exchange_coins_fight_value(DEAL_FUN_ARG);
	int gf_buy_attire(DEAL_FUN_ARG);

	int gf_set_role_base_info(DEAL_FUN_ARG);
	int gf_set_role_base_info_2(DEAL_FUN_ARG);
	int gf_set_role_pvp_info(DEAL_FUN_ARG);

	int gf_set_role_stage_info(DEAL_FUN_ARG);

	int gf_del_role(DEAL_FUN_ARG);
	int gf_gray_delete_role(DEAL_FUN_ARG);
	int gf_logic_delete_role(DEAL_FUN_ARG);
	int gf_resume_gray_role(DEAL_FUN_ARG);
	
	int gf_set_role_state(DEAL_FUN_ARG);
	int gf_set_open_box_times(DEAL_FUN_ARG);
	int gf_get_donate_count(DEAL_FUN_ARG);

	int gf_add_buff(DEAL_FUN_ARG);
	int gf_update_buff_list(DEAL_FUN_ARG);
	int gf_notify_use_item(DEAL_FUN_ARG);
	int gf_notify_user_get_shop_item(DEAL_FUN_ARG);

	int gf_del_buff_on_player(DEAL_FUN_ARG); 


// task handler
	int gf_get_task_finished(DEAL_FUN_ARG);
    int gf_del_outmoded_task(DEAL_FUN_ARG);
	
	int gf_get_task_in_hand(DEAL_FUN_ARG);

	int gf_get_all_task(DEAL_FUN_ARG);

	int gf_get_task_list(DEAL_FUN_ARG);
	
	int gf_set_task_flg(DEAL_FUN_ARG);
	
	int gf_set_task_value(DEAL_FUN_ARG);

	int gf_task_swap_item(DEAL_FUN_ARG);
//ring task
	int gf_get_ring_task_list(DEAL_FUN_ARG);
	int gf_get_ring_task_history_list(DEAL_FUN_ARG);
	int gf_replace_ring_task(DEAL_FUN_ARG);
	int gf_delete_ring_task(DEAL_FUN_ARG);
	int gf_replace_ring_task_history(DEAL_FUN_ARG);

// skill handler
	int gf_add_skill_points(DEAL_FUN_ARG);

	int gf_add_skill_with_no_book(DEAL_FUN_ARG);

	int gf_del_skill(DEAL_FUN_ARG);

	int gf_learn_new_skill(DEAL_FUN_ARG);

	int gf_upgrade_skill(DEAL_FUN_ARG);

	int gf_reset_skill(DEAL_FUN_ARG);

	int gf_get_skill_list(DEAL_FUN_ARG);

	int gf_set_skill_bind_key(DEAL_FUN_ARG);

	int gf_get_skill_bind_key(DEAL_FUN_ARG);

	int gf_add_killed_boss(DEAL_FUN_ARG);

	int gf_get_killed_boss(DEAL_FUN_ARG);
//begin summon
    int gf_get_summon_list(DEAL_FUN_ARG);
    int gf_hatch_summon(DEAL_FUN_ARG);
	int gf_get_last_summon_nick(DEAL_FUN_ARG);
    int gf_feed_summon(DEAL_FUN_ARG);
    int gf_set_summon_nick(DEAL_FUN_ARG);
    int gf_change_summon_fight(DEAL_FUN_ARG);
    int gf_set_summon_property(DEAL_FUN_ARG);
    int gf_set_summon_skills(DEAL_FUN_ARG);
    int gf_use_summon_skills_scroll(DEAL_FUN_ARG);
    int gf_set_summon_mutate(DEAL_FUN_ARG);
    int gf_set_summon_type(DEAL_FUN_ARG);
    int gf_allocate_exp2summon(DEAL_FUN_ARG);

	int gf_fresh_summon_attr(DEAL_FUN_ARG);
//end summon
// ---- Begin numen
    int gf_get_numen_list(DEAL_FUN_ARG);
    int gf_invite_numen(DEAL_FUN_ARG);
    int gf_change_numen_status(DEAL_FUN_ARG);
    int gf_make_numen_sex(DEAL_FUN_ARG);
    int gf_change_numen_nick(DEAL_FUN_ARG);
// ---- End numen
//game

	int gf_game_input(DEAL_FUN_ARG);

//GF_OTHER  DB
	int gf_check_invite_code(DEAL_FUN_ARG);
	int gf_check_user_invited(DEAL_FUN_ARG);
	int gf_get_invit_code(DEAL_FUN_ARG);
//daily action
    int gf_get_daily_action(DEAL_FUN_ARG);
    int gf_set_daily_action(DEAL_FUN_ARG);
    int gf_set_child_count(DEAL_FUN_ARG);
    int gf_set_achieve_count(DEAL_FUN_ARG);
    int gf_set_amb_status(DEAL_FUN_ARG);
    int gf_set_amb_reward_flag(DEAL_FUN_ARG);
    int gf_set_account_forbid(DEAL_FUN_ARG);
//vip deal
    int gf_get_user_vip(DEAL_FUN_ARG);
    int gf_set_vip(DEAL_FUN_ARG);
    int gf_set_base_svalue(DEAL_FUN_ARG);
    int gf_set_gold(DEAL_FUN_ARG);
    int gf_set_vip_exinfo(DEAL_FUN_ARG);
	int gf_get_amb_info(DEAL_FUN_ARG);
	int gf_set_amb_info(DEAL_FUN_ARG);
    int other_gf_sync_vip(DEAL_FUN_ARG);
    int other_gf_sync_base_svalue(DEAL_FUN_ARG);
    int other_gf_sync_gold(DEAL_FUN_ARG);
    int other_gf_vip_sys_msg(DEAL_FUN_ARG);
    int gf_set_cryptogram_gift(DEAL_FUN_ARG);
    int gf_set_magic_gift(DEAL_FUN_ARG);
    int gf_set_magic_box(DEAL_FUN_ARG);

    int gf_prepare_add_product(DEAL_FUN_ARG);
    int gf_add_store_product(DEAL_FUN_ARG);
    int gf_prepare_repair_product(DEAL_FUN_ARG);
    int gf_repair_store_product(DEAL_FUN_ARG);
    int gf_get_outdated_product(DEAL_FUN_ARG);
    int gf_synchro_user_gfcoin(DEAL_FUN_ARG);
    int gf_query_user_gfcoin(DEAL_FUN_ARG);

    int gf_coupons_exchange(DEAL_FUN_ARG);
	
	int gf_get_hero_top_info(DEAL_FUN_ARG);
	int gf_set_hero_top_info(DEAL_FUN_ARG);
    int gf_get_hunter_top_info(DEAL_FUN_ARG);
	int gf_get_hunter_top_info_for_web(DEAL_FUN_ARG);
	int gf_get_clothes_info_for_web(DEAL_FUN_ARG);
	int gf_set_hunter_top_info(DEAL_FUN_ARG);
	int gf_del_player_place_in_old_top(DEAL_FUN_ARG);
	int gf_get_hunter_info(DEAL_FUN_ARG);
	int gf_add_shop_log(DEAL_FUN_ARG);
	int gf_shop_last_log(DEAL_FUN_ARG);
	
	int gf_add_donate_count(DEAL_FUN_ARG);
	int gf_get_role_base_info_for_boss(DEAL_FUN_ARG);
	int gf_get_player_community_info(DEAL_FUN_ARG);

	//for 天下第一比武大会
	//加入一个队伍
	int gf_join_contest_team(DEAL_FUN_ARG);
	//拉去所有队伍的信息
	int gf_get_contest_team_info(DEAL_FUN_ARG);

	//拉去自己的捐献信息
	int gf_get_contest_donate(DEAL_FUN_ARG);

	//捐献复活草
	int gf_contest_donate_plant(DEAL_FUN_ARG);

	int gf_donate_item(DEAL_FUN_ARG);

	int gf_get_self_contest_team(DEAL_FUN_ARG);

	int gf_upgrade_item(DEAL_FUN_ARG);
	int gf_get_swap_action(DEAL_FUN_ARG);
	//added by cws 0608
	int gf_add_swap(DEAL_FUN_ARG);
	//
	int gf_swap_action_func(DEAL_FUN_ARG);
    int gf_clear_swap_action(DEAL_FUN_ARG);
	int gf_set_client_buf(DEAL_FUN_ARG);
	int gf_get_client_buf(DEAL_FUN_ARG);
	int gf_get_plant_list(DEAL_FUN_ARG);
	int gf_set_plant_status(DEAL_FUN_ARG);
	int gf_add_effect_to_all_plants(DEAL_FUN_ARG);
	int gf_add_home_log(DEAL_FUN_ARG);
	int gf_get_home_log(DEAL_FUN_ARG);
	int gf_pick_fruit(DEAL_FUN_ARG);
    int gf_get_pvp_game_data(DEAL_FUN_ARG);
    int gf_set_pvp_game_data(DEAL_FUN_ARG);
    int gf_set_pvp_game_flower(DEAL_FUN_ARG);
    int gf_get_pvp_game_rank(DEAL_FUN_ARG);
    int gf_team_member_reward(DEAL_FUN_ARG);
	int gf_get_summon_dragon_list(DEAL_FUN_ARG);
	int gf_add_summon_dragon(DEAL_FUN_ARG);

	//for card
	int gf_get_card_list(DEAL_FUN_ARG);

	int gf_insert_card(DEAL_FUN_ARG);

	int gf_erase_card(DEAL_FUN_ARG);

	int gf_get_offline_info(DEAL_FUN_ARG);
	int gf_set_offline_info(DEAL_FUN_ARG);

	int gf_create_fight_team(DEAL_FUN_ARG);

	int gf_add_fight_team_member(DEAL_FUN_ARG);

	int gf_set_player_team_flg(DEAL_FUN_ARG);

	int gf_set_team_member_level(DEAL_FUN_ARG);

	int gf_change_team_name(DEAL_FUN_ARG);

	int gf_change_team_mcast(DEAL_FUN_ARG);

	int gf_get_team_info(DEAL_FUN_ARG);

	int gf_del_team_member(DEAL_FUN_ARG);

	int gf_del_team(DEAL_FUN_ARG);

	int gf_team_info_changed(DEAL_FUN_ARG);
    int gf_team_active_score_change(DEAL_FUN_ARG);

	int gf_get_team_top10(DEAL_FUN_ARG);

	int gf_search_team_info(DEAL_FUN_ARG);
	int gf_gain_team_exp(DEAL_FUN_ARG);


    int gf_contribute_team(DEAL_FUN_ARG);
    int gf_fetch_team_coin(DEAL_FUN_ARG);
    int gf_team_active_enter(DEAL_FUN_ARG);
    int gf_get_team_active_top100(DEAL_FUN_ARG); 
	int gf_team_reduce_tax(DEAL_FUN_ARG);

	//for wuseng
	int gf_list_true_wusheng_info(DEAL_FUN_ARG);
	int gf_set_true_wusheng_info(DEAL_FUN_ARG);

	int gf_get_other_info_list(DEAL_FUN_ARG);

	int gf_set_other_info(DEAL_FUN_ARG);

	//for ranker
	int gf_list_ranker_info(DEAL_FUN_ARG);
	int gf_set_ranker_info(DEAL_FUN_ARG);


	int gf_get_other_active(DEAL_FUN_ARG);
	int gf_set_other_active(DEAL_FUN_ARG);
    int gf_get_other_active_list(DEAL_FUN_ARG);
    int gf_get_single_other_active(DEAL_FUN_ARG);

    int gf_set_power_user(DEAL_FUN_ARG);

    //////  师徒  ////////
    int gf_get_master(DEAL_FUN_ARG);
    int gf_get_apprentice(DEAL_FUN_ARG);
    int gf_master_add_apprentice(DEAL_FUN_ARG);
    int gf_master_del_apprentice(DEAL_FUN_ARG);
    int gf_set_prentice_grade(DEAL_FUN_ARG);
    int gf_get_prentice_cnt(DEAL_FUN_ARG);

    int gf_set_reward_player(DEAL_FUN_ARG);
    int gf_get_reward_player(DEAL_FUN_ARG);
    int gf_set_player_reward_flag(DEAL_FUN_ARG);

    int gf_get_decorate_list(DEAL_FUN_ARG);
    int gf_set_decorate(DEAL_FUN_ARG);
    int gf_add_decorate_lv(DEAL_FUN_ARG);

	int gf_check_update_trade_info(DEAL_FUN_ARG);
    int gf_insert_tmp_info(DEAL_FUN_ARG);

    int gf_safe_trade_item(DEAL_FUN_ARG);

    int gf_set_player_fate(DEAL_FUN_ARG);

    int gf_set_role_god_guard(DEAL_FUN_ARG);

	int gf_fuck_version_check(DEAL_FUN_ARG);

	int gf_distract_clothes_strength(DEAL_FUN_ARG);
private:
	int del_daily_loop(userid_t userid, uint32_t role_regtime);
	int other_process(userid_t userid, uint32_t role_regtime);
	int gf_get_skill_bind_private(uint32_t userid, uint32_t role_tm, uint32_t* p_count, 
		gf_get_skill_bind_key_out_item* p_out_item);
	int gf_get_all_skills_info(uint32_t userid, uint32_t role_tm, 
		uint32_t* p_count, gf_get_skill_bind_key_out_item* p_out_item);
    int gf_get_role_package_list(uint32_t userid, uint32_t roletm, uint32_t* cnt1, 
        uint32_t* cnt2, char* outbuf, int* outlen);
    int gf_get_task_classify_list(uint32_t userid, uint32_t roletm,uint32_t* done_task_num,
        uint32_t* doing_task_num, uint32_t* cancel_task_num,char* outbuf, int* outlen);
	int del_role_phy(userid_t userid, uint32_t role_regtime);
    inline bool is_strengthen_material(uint32_t id) {
        if ((id>=strengthen_material_base_id_1 && id<strengthen_material_base_id_1+10000)
          ||(id>=strengthen_material_base_id_2 && id<strengthen_material_base_id_2+10000)){
            return true;
        } 
        return false;
    }
    uint32_t set_item_limit(uint32_t id) {
        //return id == 1700061 ? COUPONS_MAX_LIMIT : DEFAULT_MAX_BACKAGE;
        if (id == 1700061 || id == gold_coin_item_id) {
            return COUPONS_MAX_LIMIT;
        }
        return DEFAULT_MAX_BACKAGE;
    }
    int gf_reduce_item_count(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t cnt);
    int gf_increase_item(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t cnt, 
		uint32_t pile, uint32_t max_bag, item_change_log_opt_type is_vip_opt = item_change_log_opt_type_add, bool log_flg = true);
    int gf_get_item_count(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t* cnt);
	int gf_special_set(userid_t userid, uint32_t role_regtime);
	

	/* Use compiler-generated copy constructor, assignment operator and destructor */
}; /* -----  end of class  Croute_func  ----- */

/**
 * @brief convert str to uint32_t for summon skills
 * @example: "10|12|133" => "10 12 133"
 */
int my_decide_s2u(uint8_t* srcstr, uint32_t* array);


#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

