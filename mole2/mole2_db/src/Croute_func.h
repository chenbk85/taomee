/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *        PRIu64
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_FUNC_INC
#define  CROUTE_FUNC_INC


#include "Citem_change_log.h"
#include "Cargs.h"
#include "Cuser.h"
#include "Cpet.h"
#include "Cattire.h"
#include "Cskill.h"
#include "Cpet_skill.h"
#include "Citem.h"
#include "Cfunc_route_base.h"
#include "Csync_user_data.h"
#include "Crand_item.h"
#include "Cuser_note.h"
#include "Cuser_map.h"
#include "Citem_handbook.h"
#include "Cmonster_handbook.h"
#include "Cuser_title.h"
#include "Ctask_handbook.h"
#include "Cuser_battle.h"
#include "Chome.h"
#include "Cmail.h"
#include "Ctask.h"
#include "Cpresent.h"
#include "Cactivation.h"
#include "Cinvited.h"
#include "Cmax.h"
#include "Cday.h"
#include "Ccli_buff.h"
#include "Cuser2.h"
#include "Cvip_buff.h"
#include "Cvip_item_log.h"
#include "Csurvey.h"
#include "Crelation.h"
#include "Chero_cup.h"
#include "Cpet_fight.h"
#include "Cval.h"
#include "Crank.h"
#include <time.h>
/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */

#include <libtaomee++/proto/proto_base.h>
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


	Citem_change_log item_change_log;
  	Cargs		args;
	Cuser		user;
	Cpet		pet;
	Cattire		attire;
	Cskill		skill;
	Citem		item;
	Cpet_skill	pet_skill;
	Crand_item	rand_item;
	Cuser_note	user_note;
	Cuser_map	user_map;
	Citem_handbook		item_handbook;
	Cmonster_handbook	monster_handbook;
	Cuser_title			user_title;
	Ctask_handbook		task_handbook;
	Cbattle				battle;
	Chome				home;
	Cmail				mail;
	Ctask				task;
	Cpresent			present;
	Cactivation			activation;
	Cinvited			invited;
	Cmax				max;
	Cday				day;
	Ccli_buff			cli_buff;
	Cuser2				user2;
	Cvip_buff			vip_buff;
	Cvip_item_log		vip_item_log;
	Csurvey				survey;
	Crelation			relation;
	Chero_cup			hero_cup;
	Cpet_fight 			pet_fight;
	Cval                val;
	Crank               rank;
	int do_sync_data(uint32_t userid, uint16_t cmdid );



  public:

#define PROTO_FUNC_DEF(cmd_name)\
	int cmd_name(DEAL_FUNC_ARG);
	#include "./proto/mole2_db_func_def.h"

	virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		PRI_STRU * p_pri_stru;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;
		if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
			//检查协议频率
			if (! p_pri_stru->exec_cmd_limit.add_count() ){
				DEBUG_LOG("cmd max err:cmdid %u, max_count:%u ",
					cmdid,p_pri_stru->exec_cmd_limit.limit_max_count_per_min);
			
				return CMD_EXEC_MAX_PER_MINUTE_ERR;
			}
	
	
			//检查报文长度
			if (! p_pri_stru->check_proto_size(rcvlen - PROTO_HEADER_SIZE) ){
				DEBUG_LOG("len err pre [%lu] send [%d]",
				p_pri_stru->predefine_len+PROTO_HEADER_SIZE ,rcvlen );
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
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
			if ( this->ret==SUCC && *sendbuf==NULL ){
				DEBUG_LOG("ERROR:this->ret==SUCC && *sendbuf==NULL, use STD_RETURN() ");
                STD_ROLLBACK();
                return SYS_ERR;
			}

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
			this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS,
						RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE );
		
			p_cmd_item->msg_in->init();
			p_cmd_item->msg_out->init();
			//还原对象失败
			if (!p_cmd_item->msg_in->read_from_buf( this->ba_in)) {
				DEBUG_LOG("ERR: p_cmd_item->msg_in->read_from_buf ");
				return PROTO_LEN_ERR;	
			}
			//数据没有取完
			if (!this->ba_in.is_end()) {
				DEBUG_LOG("ERR: !this->ba_in.is_end()");
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
							SUCC,this->ba_out.get_postion() )) 
				{
					DEBUG_LOG("set_std_return:SYS_ERR: %u",
							this->ba_out.get_postion() );
					return SYS_ERR; 
				}
				memcpy(SNDBUF_PRIVATE_POS,this->ba_out.get_buf(),
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


	Croute_func (mysql_interface * db); 

	int get_server_info(DEAL_FUN_ARG);
	int get_args(DEAL_FUN_ARG);
	int set_args(DEAL_FUN_ARG);

	int mole2_login(DEAL_FUN_ARG);
	int mole2_user_user_update_nick(DEAL_FUN_ARG);
	int mole2_user_user_update_signature(DEAL_FUN_ARG);
	int mole2_user_user_update_color(DEAL_FUN_ARG);
	int mole2_user_user_update_professtion(DEAL_FUN_ARG);
	int mole2_user_user_add_xiaomee(DEAL_FUN_ARG);
	int mole2_user_user_reduce_xiaomee(DEAL_FUN_ARG);
	int mole2_user_user_reduce_honor(DEAL_FUN_ARG);
	int mole2_user_user_add_exp(DEAL_FUN_ARG);
	int mole2_user_get_nick(DEAL_FUN_ARG);
	int mole2_user_user_add_base_attr(DEAL_FUN_ARG);
	int mole2_user_user_add_attr_addition(DEAL_FUN_ARG);
	int mole2_user_get_day_list(DEAL_FUN_ARG);
	int mole2_user_pet_add(DEAL_FUN_ARG);
	int mole2_user_pet_get_info(DEAL_FUN_ARG);
	int mole2_user_pet_get_list(DEAL_FUN_ARG);
	int mole2_user_pet_cure(DEAL_FUN_ARG);
	int mole2_user_pet_update_nick(DEAL_FUN_ARG);
	int mole2_user_pet_get_base_attr_initial(DEAL_FUN_ARG);
	int mole2_user_pet_get_base_attr_param(DEAL_FUN_ARG);
	int mole2_user_pet_add_base_attr(DEAL_FUN_ARG);
	int mole2_user_pet_add_exp(DEAL_FUN_ARG);
	int mole2_user_pet_add_attr_addition(DEAL_FUN_ARG);
	int mole2_user_pet_get_four_attr(DEAL_FUN_ARG);
	int mole2_user_pet_update_carry_flag(DEAL_FUN_ARG);
	int mole2_user_pet_injury_cure(DEAL_FUN_ARG);
	int mole2_user_user_create_role(DEAL_FUN_ARG);
	int mole2_user_user_set_pos(DEAL_FUN_ARG);
	int mole2_user_user_login(DEAL_FUN_ARG);
	int mole2_user_pet_add_base_attr_initial(DEAL_FUN_ARG);
	int mole2_user_pet_get_flag(DEAL_FUN_ARG);
	int mole2_user_user_add_friend(DEAL_FUN_ARG);
	int mole2_user_user_del_friend(DEAL_FUN_ARG);
	int mole2_user_user_add_blackuser(DEAL_FUN_ARG);
	int mole2_user_user_del_blackuser(DEAL_FUN_ARG);
	int mole2_user_user_add_offline_msg(DEAL_FUN_ARG);
	int mole2_user_user_get_offline_msg(DEAL_FUN_ARG);
	int mole2_user_user_get_friend_list(DEAL_FUN_ARG);
	int mole2_user_user_get_black_list(DEAL_FUN_ARG);
	int mole2_user_user_check_is_friend(DEAL_FUN_ARG);
	int mole2_user_pet_get_attr_add(DEAL_FUN_ARG);
	int mole2_user_pet_add_attr_add(DEAL_FUN_ARG);
	int mole2_user_attire_add(DEAL_FUN_ARG);
	int mole2_user_attire_get_list(DEAL_FUN_ARG);
	int mole2_user_attire_get(DEAL_FUN_ARG);
	int mole2_user_attire_update_gridid(DEAL_FUN_ARG);
	int mole2_user_attire_del(DEAL_FUN_ARG);
	int mole2_user_attire_get_list_ex(DEAL_FUN_ARG);

	int mole2_user_attire_update_gridid_list(DEAL_FUN_ARG);
	int mole2_user_user_login_ex(DEAL_FUN_ARG);
	int mole2_user_skill_add(DEAL_FUN_ARG);
	int mole2_user_skill_get_list(DEAL_FUN_ARG);
	int mole2_user_skill_add_level(DEAL_FUN_ARG);
	int mole2_user_skill_del(DEAL_FUN_ARG);
	int mole2_user_skill_add_exp(DEAL_FUN_ARG);
	int mole2_user_pet_skill_get_list(DEAL_FUN_ARG);

	int mole2_user_user_cure(DEAL_FUN_ARG);
	int mole2_user_user_set_hp_mp(DEAL_FUN_ARG);
	int mole2_user_pet_set_hp_mp(DEAL_FUN_ARG);

	int mole2_user_item_add(DEAL_FUN_ARG);
	int mole2_user_item_get_list(DEAL_FUN_ARG);
	int mole2_user_item_add_list(DEAL_FUN_ARG);
	int mole2_user_skill_buy(DEAL_FUN_ARG);
	int mole2_user_pet_skill_buy(DEAL_FUN_ARG);
	int mole2_user_item_buy(DEAL_FUN_ARG);
	int mole2_user_item_ranged_get_list(DEAL_FUN_ARG);

	int user_user_injury_state_set(DEAL_FUN_ARG);

	int user_pet_injury_state_set(DEAL_FUN_ARG);

	int user_user_injury_state_get(DEAL_FUN_ARG);

	int user_pet_injury_state_get(DEAL_FUN_ARG);

//	int user_task_reward_set(DEAL_FUN_ARG);

	int user_attire_info_get(DEAL_FUN_ARG);

	int user_attire_sell_set(DEAL_FUN_ARG);

	int user_create_role_set(DEAL_FUN_ARG);

	int user_user_levelup_set(DEAL_FUN_ARG);

	int user_pet_levelup_set(DEAL_FUN_ARG);

	int user_user_skill_up_set(DEAL_FUN_ARG);
	
	int mole2_user_task_item_add(DEAL_FUN_ARG);
	
	int mole2_user_task_item_del(DEAL_FUN_ARG);
	
	int mole2_user_task_item_get_list(DEAL_FUN_ARG);

	int user_user_skill_list_levelup_set(DEAL_FUN_ARG);

	int user_user_flag_set(DEAL_FUN_ARG);

	int rand_item_info_check(DEAL_FUN_ARG);

	int rand_item_info_set(DEAL_FUN_ARG);

	int user_pet_all_get(DEAL_FUN_ARG);

	int user_user_medicine_cure(DEAL_FUN_ARG);

	int user_pet_location_set(DEAL_FUN_ARG);

	int user_user_note_dream_set(DEAL_FUN_ARG);

	int user_pet_catch_set(DEAL_FUN_ARG);

	int user_pet_list_all_get(DEAL_FUN_ARG);

	int activation_code_check(DEAL_FUN_ARG);

	int user_map_flight_list_get(DEAL_FUN_ARG);

	int mole2_user_map_flight_check(DEAL_FUN_ARG);

	int user_map_flight_set(DEAL_FUN_ARG);

	int user_logout_position_set(DEAL_FUN_ARG);

	int user_handbook_list_get(DEAL_FUN_ARG);

	int item_handbook_count_add(DEAL_FUN_ARG);

	int monster_handbook_count_list_add(DEAL_FUN_ARG);

	int item_handbook_state_set(DEAL_FUN_ARG);

	int monster_handbook_state_set(DEAL_FUN_ARG);

	int user_title_list_get(DEAL_FUN_ARG);
	
	int user_honor_set(DEAL_FUN_ARG);

	int user_pet_follow_set(DEAL_FUN_ARG);
	
	int user_map_boss_position_set(DEAL_FUN_ARG);

	int user_map_boss_position_list_get(DEAL_FUN_ARG);

	int user_task_optdate_get(DEAL_FUN_ARG);

	int tash_handbook_content_set(DEAL_FUN_ARG);

	int user_attire_list_duration_set(DEAL_FUN_ARG);

	int user_attire_list_mend_set(DEAL_FUN_ARG);
	
	int user_attire_attrib_set(DEAL_FUN_ARG);
	
	int mole2_user_pet_get_in_store_list(DEAL_FUN_ARG);
	
	int mole2_user_pet_set(DEAL_FUN_ARG);

	int mole2_user_pet_delete(DEAL_FUN_ARG);
	
	int user_title_delete(DEAL_FUN_ARG);

	int pet_in_store_list_get(DEAL_FUN_ARG);

	int pet_in_store_location_set(DEAL_FUN_ARG);

	int user_item_storage_set(DEAL_FUN_ARG);

	int user_item_in_storage_list_get(DEAL_FUN_ARG);

	int user_attire_in_storage_list_get(DEAL_FUN_ARG);

	int user_battle_info_set(DEAL_FUN_ARG);

	int home_info_get(DEAL_FUN_ARG);

	int item_storage_add(DEAL_FUN_ARG);

	int mole2_user_other_user_info(DEAL_FUN_ARG);

	int user_mail_send_set(DEAL_FUN_ARG);

	int user_mail_simple_list_get(DEAL_FUN_ARG);

	int user_exp_info_set(DEAL_FUN_ARG);

	int user_mail_content_get(DEAL_FUN_ARG);

	int user_mail_list_del(DEAL_FUN_ARG);

	int user_mail_flag_set(DEAL_FUN_ARG);

	int mole2_task_set(DEAL_FUN_ARG);

	int user_energy_set(DEAL_FUN_ARG);

	int mole2_task_info_get(DEAL_FUN_ARG);

	int mole2_task_info_list_get(DEAL_FUN_ARG);

	int user_task_reward_set_cp(DEAL_FUN_ARG);

	int mole2_task_ser_buf_set(DEAL_FUN_ARG);

	int rand_item_present_set(DEAL_FUN_ARG);

	int mole2_task_delete(DEAL_FUN_ARG);

	int rand_item_activation_set(DEAL_FUN_ARG);

	int mole2_home_flag_set(DEAL_FUN_ARG);

	int set_winbossid(DEAL_FUN_ARG);

	int set_ban_flag(DEAL_FUN_ARG);
	
	int user_get_ban_flag(DEAL_FUN_ARG);
	
	int user_swap_pets(DEAL_FUN_ARG);
	
	int mole2_user_add_invited(DEAL_FUN_ARG);

	int mole2_user_get_invited(DEAL_FUN_ARG);

	int set_title_and_job_level(DEAL_FUN_ARG);

	int proto_get_expbox(DEAL_FUN_ARG);
	int proto_set_expbox(DEAL_FUN_ARG);

	int item_handbook_state_get(DEAL_FUN_ARG);
	int day_add_ssid_count(DEAL_FUN_ARG);
	int day_get_ssid_info(DEAL_FUN_ARG);
	int day_sub_ssid_total(DEAL_FUN_ARG);
	int mail_get_post_items(DEAL_FUN_ARG);
	int add_item_type_counter(DEAL_FUN_ARG);
	int get_item_type_counter(DEAL_FUN_ARG);
	int monster_handbook_range_get(DEAL_FUN_ARG);

}; /* -----  end of class  Croute_func  ----- */
#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

