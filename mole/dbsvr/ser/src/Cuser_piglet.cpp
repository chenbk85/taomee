/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2011 10:46:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet.h"
#include<algorithm>
#include<math.h>
#include<ctime>
#include "proto.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <sstream>

//计算出来状态
enum PIGLET_SATE{
	INFANCY = 0x01, //幼年状态
	YOUTH  =  0x02, //青年状态
	MATURE =  0x03, //成年状态
	DYING =   0x04, //濒死状态
	EXPIRE =  0x08, //死亡状态
	PREGNANT = 0x10, //怀孕状态
	BIRTH_INC = 0x20, //多仔状态
	VARIATE  =  0x40, //变异状态
	TRAIN_F  =  0x80,//体质训练
	TRAIN_S  =  0x100, //肉质训练
	ABLE_TO_PRG = 0x200,//可怀孕状态
};

//表中拉取的状态
enum PIGLET_STAGE{
	PREGNANT_BORN = 0x01, //怀孕，未出生猪仔
	NORMAL_SATE   = 0x02, //出生后在猪倌, 普通状态的猪
	ALREADY_FEED  = 0x04, //已经喂食过,因为要计算生长率 
	ABLE_INC      = 0x08, //装备了生两只猪仔道具状态
	ABLE_VARIATE  = 0x10, //装备变异道具状态
	SPEED_BORN    = 0x20,//催产状态

};
/*
 * 猪仔种类的生长阶段: 1, 2, 3
 */
static uint32_t  growth_span[][3] = {
	{1, 1, 3},
	{2, 4, 8},
	{2, 4, 8},
};

static uint32_t piglet_limit[] = {
	6, 6, 7, 7, 8, 8, 9, 9, 9, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14,
	   16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 20, 20, 20, 20, 20, 22,
	   22, 22, 24, 24, 24
};
/*
 * 肥肥猪种类表
 */
static uint32_t fat_piglet_sort[][4] = {
	{1593000, 0, 0, 6},
	{1593001, 0, 1, 6},
	{1593002, 1, 0, 2},
	{1593003, 1, 1, 2},
	{1593004, 2, 0, 2},
	{1593005, 2, 1, 2},
	{1593006, 3, 0, 3},
	{1593007, 3, 1, 3},
	{1593008, 4, 2, 3},
	{1593009, 4, 2, 3},
	{1593010, 5, 0, 3},
	{1593011, 5, 1, 3},
	{1593012, 6, 0, 2},
	{1593013, 6, 1, 2},
	{1593014, 7, 0, 4},
	{1593015, 7, 1, 4},
	{1593016, 8, 0, 3},
	{1593017, 8, 1, 3},
	{1593018, 9, 0, 5},
	{1593019, 9, 1, 5},
	{1593020, 10, 0, 1},
	{1593021, 10, 1, 1},
	{1593022, 11, 0, 2},
	{1593023, 11, 1, 2},
	{1593024, 12, 2, 3},
	{1593025, 12, 2, 3},
	{1593026, 13, 2, 6},
	{1593027, 13, 2, 6},
	{1593028, 14, 2, 5},
	{1593029, 14, 2, 5},
	{1593030, 15, 2, 10},
};	

/*
 * 获取肥肥猪仔种类
 */

int Cuser_piglet::get_piglet_real_breed(uint32_t pigletid, uint32_t &type)
{
	if(pigletid >= 1593000 && pigletid <= 1593029){
		type = fat_piglet_sort[pigletid - 1593000][1];
		return 0;
	}	
	else{
		return 1;
	}
}	
/*
 * 玩家猪倌等级与饲养猪仔个数关系
 */
uint32_t Cuser_piglet::get_level_piglet_limit(userid_t userid, uint32_t level)
{
	if(level == 0){
		return 6;
	}
	else{
		return piglet_limit[level - 1];
	}
}

uint32_t Cuser_piglet::get_piglet_type(uint32_t breed)
{
	if(breed >= 0 && breed < 10000){
		return 0;
	}
	else if( breed >=10000 && breed < 20000){
		return 1;
	}
	else{
		return 2;
	}
}

Cuser_piglet::Cuser_piglet(mysql_interface *db): 
	CtableRoute100x10(db, "USER", "t_user_piglet", "userid")
{
		
}

uint32_t Cuser_piglet::set_piglet_state(uint32_t breed, uint32_t now, uint32_t birthday,
	   	uint32_t feedtime, uint32_t feedtype, uint32_t pregnant, uint32_t lifetime, 
		uint32_t growth_stage, uint32_t train_cnt, uint32_t train_point, uint32_t sex)
{
	//设置怀孕状态
	uint32_t state = 0;
	if(sex == 1 && pregnant != 0){
		state |= PREGNANT;
	}
		
	//设置幼年，青年，成年状态
	uint32_t age = now - birthday; 
	uint32_t span = 0;
	uint32_t k = 0;
	for(; k < 3; ++k){
		span = span +  growth_span[get_piglet_type(breed)][k] * 24 * 3600;
		if(age <= span){
			if(k == 0){
				state |= INFANCY; 
			}
			else if(k == 1){
				state |= YOUTH;
			}
			else{
				state |= MATURE;
			}
			break;
		}//if
	}//for	
	if(k >= 3){
		state |= MATURE;
	}
	
	uint32_t live_time = (now - feedtime);
	//设置可怀孕状态
	if(sex == 1 && (state & MATURE) == MATURE && !(state & PREGNANT) && ((now - birthday) + 24*3600) < lifetime 
			&& live_time < feedtype*3600){
		DEBUG_LOG("[sex: %u]", sex);	
		state |= ABLE_TO_PRG; 
	}
	//设置濒死，死亡状态	
	if((live_time >= (feedtype*3600 + 5*24*3600)) && (live_time < (feedtype*3600 + 10*24*3600))){
		state |= DYING;
	}
    if( live_time >= (feedtype*3600 + 10*24*3600) || (now - birthday) >= lifetime){
		state &= ~DYING;
		state |= EXPIRE;
	}
	//设置是否增加繁殖装备道具状态
	uint32_t state_1 = growth_stage & ABLE_INC;
	if( state_1 == ABLE_INC){
		state |= BIRTH_INC;
	}
	//设置是否装备变异状态
	uint32_t state_2 = growth_stage & ABLE_VARIATE;
	if(state_2 == ABLE_VARIATE){
		state |= VARIATE;
	}
	//设置训练状态	
	if((train_cnt & 0x01) == 0x01 || train_point >= 80){
		state |= TRAIN_F;
	}
	if((train_cnt & 0x02) == 0x02 || train_point >= 80){
		state |= TRAIN_S;
	}
	return state;
}


int Cuser_piglet::birth(userid_t userid, uint32_t pigletid, char* nick, char* mother_nick, 
		char* father_nick, uint32_t breed, uint32_t sex, uint32_t weight, uint32_t charm, uint32_t strong, 
		uint32_t birthday, uint32_t growth_rate, uint32_t growth_stage, uint32_t  lifetime, 
		uint32_t parentid, uint32_t generate, uint32_t price, uint32_t state)
{
	DEBUG_LOG("breed: %u, pigid: %u,  birthday: %u, sex: %u === in birth function", breed, pigletid, birthday, sex);
	uint32_t feed_time = 0;
	if(state == 1){
		feed_time = (uint32_t)time(0); 
	}
	else{
		feed_time = (uint32_t)(time(0) - 12*3600); 
	}
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);
	
	char m_nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(m_nick_mysql, mother_nick, NICK_LEN);

	char f_nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(f_nick_mysql, father_nick, NICK_LEN);

	char dress_list[mysql_str_len(sizeof(user_piglet_dress))];
	memset(dress_list, 0, sizeof(dress_list));	

	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, '%s', '%s', '%s', %u, %u, %u, %u, \
		%u, %u, %u, %u, %u, %u, %u, 0, 0, 0, 0, 0, 0, 0, 0, 0, '%s', %u, %u, %u, 0, 0, 0, 0, 0)",
			this->get_table_name(userid),
			userid,
			pigletid,
			nick_mysql,
			m_nick_mysql,
			f_nick_mysql,
			breed,
			sex,
			weight,
			charm,
			strong,
			birthday,
			growth_rate,
			growth_stage,
			feed_time,
			lifetime,
			parentid,
			dress_list,
			generate,
			price,
			(uint32_t)time(0)
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet::update_fancy_born_state(userid_t userid, uint32_t index)
{
	uint32_t growth_stage = 0;
	select(userid, index, "growth_stage", &growth_stage);
	growth_stage &= ~PREGNANT_BORN; 
	growth_stage |= NORMAL_SATE;
	
	uint32_t now = time(0);
	int ret = this->update_two_cols(userid, index, "growth_stage", "birthday", growth_stage, now-12*3600);
	return ret;
}

int Cuser_piglet::update_two_cols(uint32_t userid, uint32_t id, const char* col_1, const char* col_2, 
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet::update_inc_col(userid_t userid, uint32_t id, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u and id = %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::update_inc_two(userid_t userid, uint32_t id, const char* col_1, const char* col_2,
		uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u, %s = %s + %u where userid = %u and id = %u",
			this->get_table_name(userid),
			col_1,
			col_1,
			value_1,
			col_2,
			col_2,
			value_2,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet::select_two_cols(uint32_t userid, uint32_t id, const char* col_1, const char* col_2, 
		uint32_t *value_1, uint32_t * value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and id = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::update(uint32_t userid, uint32_t id, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::select(uint32_t userid, uint32_t id, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and id = %u",
			col,
			this->get_table_name(userid),
			userid,
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_piglets(userid_t userid, user_get_piglet_house_out_item *p_list, uint32_t* p_count,
		std::vector<birth_info_t> &vec, std::set<uint32_t> &sets, uint32_t feed_type, uint32_t tool_cnt,
		buff_tool_t *p_item)
{
	sprintf(this->sqlstr, "select id, pigletid, breed, sex, weight, transform, growth_rate, growth_stage, \
			birthday, pregnant_time, feed_time, lifetime, parentid, train_cnt, train_point, amuse_cnt, trans_time, \
			trans_last, cal_weight_time, generate, dress from %s where userid = %u and \
			shift = 0 and carry_flag = 0 order by id asc",
            this->get_table_name(userid),
            userid 
            );
	//user_piglet_dress dress;
	//memset(&dress, 0, sizeof(user_piglet_dress));

	piglet_base_attr *piglets = 0;
	uint32_t piglet_cnt = 0;
 	STD_QUERY_WHILE_BEGIN(this->sqlstr, &piglets, &piglet_cnt);
		INT_CPY_NEXT_FIELD((piglets + i)->id);
		INT_CPY_NEXT_FIELD((piglets + i)->pigletid);
		INT_CPY_NEXT_FIELD((piglets + i)->breed);
		INT_CPY_NEXT_FIELD((piglets + i)->sex);
		INT_CPY_NEXT_FIELD((piglets + i)->weight);
		INT_CPY_NEXT_FIELD((piglets + i)->transform);
		INT_CPY_NEXT_FIELD((piglets + i)->growth_rate);
		INT_CPY_NEXT_FIELD((piglets + i)->growth_stage);
		INT_CPY_NEXT_FIELD((piglets + i)->birthday);
		INT_CPY_NEXT_FIELD((piglets + i)->pregnant_time);
		INT_CPY_NEXT_FIELD((piglets + i)->feed_time);
		INT_CPY_NEXT_FIELD((piglets + i)->lifetime);
		INT_CPY_NEXT_FIELD((piglets + i)->parentid);
		INT_CPY_NEXT_FIELD((piglets + i)->train_cnt);
		INT_CPY_NEXT_FIELD((piglets + i)->train_point);
		INT_CPY_NEXT_FIELD((piglets + i)->amuse_cnt);
		INT_CPY_NEXT_FIELD((piglets + i)->trans_time);
		INT_CPY_NEXT_FIELD((piglets + i)->trans_last);
		INT_CPY_NEXT_FIELD((piglets + i)->cal_weight_time);
		if((piglets + i)->cal_weight_time == 0){
			(piglets + i)->cal_weight_time = (piglets + i)->feed_time;
		}
		INT_CPY_NEXT_FIELD((piglets + i)->generate);
		BIN_CPY_NEXT_FIELD((char*)(&(piglets +i)->dress), sizeof(user_piglet_dress));
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	uint32_t n = 0;
	uint32_t now = time(0);
	uint32_t can_birth_female = 0;

	/*
	 *如果挂载了恶魔卡，则恶魔卡生效
	 */	
	const uint32_t card_1 = 1613130;//丘比特祝福卡
	const uint32_t card_2 = 1613129; //恶魔卡
	//const uint32_t card_3 = 1613128; //天使卡

	uint32_t effect_state = 0, effect_cnt = 0, angel_state = 0; //cal_weight_state = 0;
	uint32_t ind[3] = { 0 };
	DEBUG_LOG("tool_cnt: %u", tool_cnt);

	for(uint32_t m = 0; m < tool_cnt; ++m){
		if((p_item + m)->itemid == card_2 && (p_item + m)->state == 0){
			if(now < ((p_item + m)->effect_time + (p_item + m)->lasttime)){
				ind[1] = m;
				effect_state = 1;
			}
		}
		else if((p_item + m)->itemid == card_1){
			DEBUG_LOG("itemid: %u, now: %u, last: %u", (p_item + m)->itemid, now, (p_item + m)->effect_time + (p_item+m)->lasttime);
			if(now < ((p_item + m)->effect_time + (p_item+m)->lasttime)){
				ind[0] = m;
				angel_state = 1;
			}
		}
	}

	for(uint32_t k = 0; k < piglet_cnt; ++k){
		//修正数据库猪仔种类错误数据
		if((piglets+k)->pigletid == 0){
			this->remove(userid, (piglets+k)->id);
			(piglets+k)->id = 0;    //标志该未出生猪仔,将被删掉
		}
		//if((piglets + k)->pigletid >= 1593000 && (piglets + k)->pigletid <= 1593030){
			//uint32_t mod_breed = 0, mod_sex = 0;
			//if((piglets + k)->breed != fat_piglet_sort[(piglets + k)->pigletid - 1593000][1]){
					//DEBUG_LOG("breed: %u, real_breed: %u, pigid: %u, index: %u", (piglets + k)->breed,
						   	//fat_piglet_sort[(piglets + k)->pigletid - 1593000][1], (piglets + k)->pigletid, (piglets+k)->id);
				//(piglets + k)->breed = fat_piglet_sort[(piglets + k)->pigletid - 1593000][1];
				//++mod_breed;
			//}
			//if((piglets + k)->sex != fat_piglet_sort[(piglets + k)->pigletid - 1593000][2]){
					//DEBUG_LOG("sex: %u, real_sex: %u, pigid: %u, index: %u", (piglets + k)->sex,
						   	//fat_piglet_sort[(piglets + k)->pigletid - 1593000][2], (piglets + k)->pigletid, (piglets+k)->id);
				//(piglets + k)->sex = fat_piglet_sort[(piglets + k)->pigletid - 1593000][2];
				//++mod_sex;
			//}
			////if((piglets +k)->weight > 500000){
				////if((piglets +k)->pigletid == 1593028){
					 ////(piglets +k)->weight = 250000 + rand()%10000;
				////}
				////else{
					////(piglets +k)->weight = 200000 + rand()%10000;
				////}
				////this->update(userid, (piglets + k)->id, "weight", (piglets +k)->weight);
			////}

			//if((piglets + k)->growth_rate == 0){
				//this->update(userid, (piglets + k)->id, "growth_rate", fat_piglet_sort[(piglets + k)->pigletid - 1593000][3]*10);
				//uint32_t inc_w = (now - (piglets + k)->birthday) / 3600 * fat_piglet_sort[(piglets + k)->pigletid - 1593000][3]
					//* pow(1.1,(piglets + k)->generate - 1 )*1.5;
				//this->update_inc_col(userid, (piglets + k)->id, "weight", inc_w);
			//}
			//if(mod_breed == 1 && mod_sex == 0){
				//this->update(userid, (piglets + k)->id, "breed", (piglets + k)->breed);
			//}
			//else if(mod_breed == 0 && mod_sex == 1){
				//this->update(userid, (piglets + k)->id, "sex", (piglets + k)->sex);
			//}
			//else if(mod_breed == 1 && mod_sex == 1){
				//this->update_two_cols(userid, (piglets + k)->id, "breed", "sex", (piglets + k)->breed, (piglets + k)->sex);
			//}
		//}	

		if(((piglets+k)->growth_stage & PREGNANT_BORN) == PREGNANT_BORN){
            if(k == 0){
                this->remove(userid, (piglets+k)->id);
                (piglets+k)->id = 0;    //标志该未出生猪仔,将被删掉
            }
            else{
                uint32_t j = 0;
                for(; j < k; ++j){
                    if((piglets + k)->parentid == (piglets + j)->id){
						break;
					}
                }//for
				if(j >= k){
					this->remove(userid, (piglets+k)->id);
					(piglets+k)->id = 0;    //标志该未出生猪仔,将被删掉
				}
            }//else
        }

		//母猪仔，需要考虑怀孕的情况
		uint32_t able_birth_cnt = 0;
		if((piglets + k)->sex == 1 && (piglets+k)->pregnant_time != 0){
			uint32_t dead_flag = check_piglet_dead((piglets + k)->feed_time, feed_type, 
					(piglets + k)->birthday, (piglets + k)->lifetime);					
			//如果猪死亡
			if(dead_flag != 0){
				//判断是死亡在先，还是产仔在先
				uint32_t expire_time = 0;
				if(dead_flag == 1){//猪仔饿死时间
					expire_time = (piglets+k)->feed_time + feed_type *3600 + 10*24*3600;
				}
				else{//猪仔老死时间
					expire_time = (piglets+k)->birthday + (piglets+k)->lifetime;
				}
					
				for(uint32_t i = k+1; i < piglet_cnt; ++i){
					if((piglets+i)->parentid == (piglets+k)->id && ((piglets+i)->growth_stage & PREGNANT_BORN) 
							== PREGNANT_BORN) {
						//产仔时间先于死去时间
						if((piglets+k)->pregnant_time+ 24*3600 <= expire_time){
							(piglets+i)->growth_stage &= ~PREGNANT_BORN ;	
							(piglets+i)->growth_stage |= NORMAL_SATE;
							++able_birth_cnt;
							//sets.insert((piglets+i)->breed);

							//丘比特祝福卡生效
							if(angel_state == 1){
								this->update_inc_col(userid, (piglets+i)->id, "weight", 1000);
							}
							this->update(userid, (piglets+i)->id, "growth_stage", (piglets+i)->growth_stage);
						}
						else{//死去时间早于产仔时间
							this->remove(userid, (piglets+i)->id);
							(piglets+i)->id = 0;	//标志该未出生猪仔,将被删掉
						}

					}
				}//for
				if(able_birth_cnt != 0){
					birth_info_t single ={ 0 };
                    single.index = k; 
                    single.birth_time = (piglets+k)->pregnant_time+ 24*3600;
                    single.cnt = able_birth_cnt;
                    vec.push_back(single);
					++can_birth_female;
				}
				(piglets+k)->pregnant_time = 0;
				(piglets+k)->growth_stage &= ~ABLE_INC;
	 			(piglets+k)->growth_stage &= ~ABLE_VARIATE; 
	 			(piglets+k)->growth_stage &= ~SPEED_BORN; 
				this->update_two_cols(userid, (piglets+k)->id, "pregnant_time", "growth_stage", 0, (piglets+k)->growth_stage);
			}
			else{//猪没有死亡
				//猪没有达到生仔时间
				if(((piglets + k)->pregnant_time + 3600*24) <= now){
					//满足了生仔时间
					for(uint32_t i = k+1; i < piglet_cnt; ++i){
						if((piglets+i)->parentid == (piglets+k)->id && ((piglets+i)->growth_stage & PREGNANT_BORN) 
								== PREGNANT_BORN){
							(piglets+i)->growth_stage &= ~PREGNANT_BORN;	
							(piglets+i)->growth_stage |= NORMAL_SATE;
							++able_birth_cnt;
							//sets.insert((piglets+i)->breed);

							//丘比特祝福卡生效
							if(angel_state == 1){
								this->update_inc_col(userid, (piglets+i)->id, "weight", 1000);
							}
							this->update(userid, (piglets+i)->id, "growth_stage", (piglets+i)->growth_stage);
						}	
					}//for	
					if(able_birth_cnt != 0){
						birth_info_t single ={ 0 };
						single.index = k;
						single.birth_time = (piglets+k)->pregnant_time+ 24*3600;
						single.cnt = able_birth_cnt;
						vec.push_back(single);
						++can_birth_female;
					}
					(piglets+k)->pregnant_time = 0;
					(piglets+k)->growth_stage &= ~ABLE_INC;
					(piglets+k)->growth_stage &= ~ABLE_VARIATE; 
					(piglets+k)->growth_stage &= ~SPEED_BORN; 
					this->update_two_cols(userid, (piglets+k)->id, "pregnant_time", "growth_stage", 0, (piglets+k)->growth_stage);
				
				}//if
			}//else

		}//if(sex==1)

		/*
		 * 计算猪的饥饿度和其他值
		 */
		if((piglets + k)->id != 0 && ((piglets + k)->growth_stage & 0x01) != 0x01){
			(p_list+n)->index = (piglets+k)->id;
			(p_list+n)->breed = (piglets+k)->breed;
			(p_list+n)->piglet_id = (piglets+k)->pigletid;
			(p_list+n)->sex = (piglets+k)->sex;
			if((piglets+k)->transform != 0 && (now - (piglets+k)->trans_time) >= (piglets+k)->trans_last){
				(p_list+n)->transform = 0;
			}
			else{
				(p_list+n)->transform = (piglets+k)->transform;
			}
			(p_list+n)->amuse_cnt = (piglets+k)->amuse_cnt;
			(p_list+n)->hungry_degree = cal_hungry_degree(now, (piglets+k)->feed_time, feed_type);
			(p_list+n)->state =  set_piglet_state((piglets + k)->breed, now, (piglets+k)->birthday, 
					(piglets+k)->feed_time, feed_type, (piglets+k)->pregnant_time, 
					(piglets+k)->lifetime, (piglets+k)->growth_stage, (piglets+k)->train_cnt,
					(piglets+k)->train_point, (piglets+k)->sex); 

			//恶魔卡生效
			if(((p_list + n)->state & DYING) != DYING && ((p_list + n)->state & EXPIRE) != EXPIRE){
				if(effect_state == 1 && effect_cnt < 3 && now > (piglets + k)->feed_time + 30*60){
					uint32_t distance = 0;
					if(5*24*3600 >= (now-(piglets+k)->feed_time)){
						distance = 5*24*3600 - (now-(piglets+k)->feed_time);
					}
					else{
						distance = 0;
					}
					if(distance != 0){
						this->update(userid, (p_list+n)->index, "feed_time", (piglets +k)->feed_time - distance);		
					}
					(p_list+n)->hungry_degree = cal_hungry_degree(now, (piglets+k)->feed_time - distance, feed_type);
					(p_list + n)->state = (p_list + n)->state | DYING;
					++effect_cnt;
				}
			}

			if((piglets+k)->dress.count > 1){
				memset(&(piglets+k)->dress, 0, sizeof(user_piglet_dress));
				this->put_on(userid,(piglets+k)->id, &(piglets+k)->dress);
					
			}
			for(uint32_t i = 0; i < (piglets+k)->dress.count; ++i){
				(p_list+n)->dress[i] = (piglets+k)->dress.itemid[i];
			}

			n = n + 1;
			sets.insert((piglets+k)->breed);
		}
		*p_count = n;
	}//for

	if(effect_cnt > 1){
		(p_item + ind[1])->state = 1;
	}
	
	if(piglets != 0){
		free(piglets);
	}
	return 0;
}

int Cuser_piglet::check_piglet_dead(uint32_t feed_time, uint32_t feed_type, uint32_t birthday, uint32_t lifetime)
{
	//饿死返回1， 老死返回2， 存活返回0
	uint32_t now = time(0);
	if(feed_time + feed_type * 3600 + 10*24 *3600 <= now){
		return 1;
	}
	if(now - birthday >= lifetime){
		return 2;
	}

	return 0;
}

int Cuser_piglet::su_get_all(uint32_t userid, su_user_get_all_piglets_out_item **pp_list, uint32_t *p_count)
{

	sprintf(this->sqlstr, "select id, pigletid, breed, sex, growth_stage, weight, birthday, pregnant_time, \
			feed_time, parentid, lifetime, train_cnt, train_point, amuse_cnt, carry_flag, transform, trans_time, \
			trans_last,	birth_cnt, generate, price, cal_weight_time from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pigletid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->breed);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sex);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth_rate);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attribute);
		INT_CPY_NEXT_FIELD((*pp_list + i)->birthday);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pregnant_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->feed_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mother_index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lifetime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->train_cnt);
		INT_CPY_NEXT_FIELD((*pp_list + i)->train_point);
		INT_CPY_NEXT_FIELD((*pp_list + i)->amuse_cnt);
		INT_CPY_NEXT_FIELD((*pp_list + i)->out_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->out_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->transform);
		INT_CPY_NEXT_FIELD((*pp_list + i)->trans_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->trans_last);
		INT_CPY_NEXT_FIELD((*pp_list + i)->paire_cnt);
		INT_CPY_NEXT_FIELD((*pp_list + i)->generate);
		INT_CPY_NEXT_FIELD((*pp_list + i)->price);
		INT_CPY_NEXT_FIELD((*pp_list + i)->cal_weight_time);
	STD_QUERY_WHILE_END();	
}

int Cuser_piglet::get_all_piglet(uint32_t userid, user_get_piglet_house_out_item** pp_list, uint32_t* p_count,
		uint32_t feed_type)
{
	sprintf(this->sqlstr, "select id, pigletid, breed, sex, transform, growth_stage, birthday, pregnant_time, \
			feed_time, lifetime, train_cnt, train_point, amuse_cnt, dress from %s where userid = %u and \
			growth_stage & 0x02 = 0x02",
			this->get_table_name(userid),
			userid
			);
	user_piglet_dress dress;
	memset(&dress, 0, sizeof(user_piglet_dress));
	uint32_t pregnant_time = 0, birthday = 0, lifetime = 0;
	uint32_t now = time(0), feed_time = 0, growth_stage = 0, train_cnt = 0;
	uint32_t train_point = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->piglet_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->breed);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sex);
		INT_CPY_NEXT_FIELD((*pp_list + i)->transform);
		INT_CPY_NEXT_FIELD(growth_stage);
		INT_CPY_NEXT_FIELD(birthday);
		INT_CPY_NEXT_FIELD(pregnant_time);
		INT_CPY_NEXT_FIELD(feed_time);
		INT_CPY_NEXT_FIELD(lifetime);
		INT_CPY_NEXT_FIELD(train_cnt);
		INT_CPY_NEXT_FIELD((*pp_list + i)->amuse_cnt);
		(*pp_list + i)->hungry_degree = cal_hungry_degree(now, feed_time, feed_type);
	    (*pp_list + i)->state =  set_piglet_state((*pp_list + i)->breed, now, birthday, feed_time, 
				feed_type, pregnant_time, lifetime, growth_stage, train_cnt, train_point, (*pp_list + i)->sex);
		BIN_CPY_NEXT_FIELD(&dress, sizeof(user_piglet_dress));
		for(uint32_t k = 0; k < 2; ++k){
			(*pp_list + i)->dress[k] = dress.itemid[k];
		}
	STD_QUERY_WHILE_END();	
}

int Cuser_piglet::get_weight_sex_dress(userid_t userid, uint32_t index, uint32_t *weight, uint32_t *sex,
		uint32_t* pigletid, user_piglet_dress* dress, uint32_t effect_time, uint32_t lasttime)
{
	sprintf(this->sqlstr, "select pigletid, sex, weight, growth_stage, growth_rate, feed_time, cal_weight_time, \
			shift, dress from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			index
			);
	uint32_t now = time(0), w_inc = 0;
	uint32_t growth_rate = 0, feed_time = 0, growth_stage = 0; 
	uint32_t cal_weight_time = 0, shift = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*pigletid);
        INT_CPY_NEXT_FIELD(*sex);
        INT_CPY_NEXT_FIELD(*weight);
        INT_CPY_NEXT_FIELD(growth_stage);
        INT_CPY_NEXT_FIELD(growth_rate);
        INT_CPY_NEXT_FIELD(feed_time);
        INT_CPY_NEXT_FIELD(cal_weight_time);
		if(cal_weight_time == 0){
			cal_weight_time = feed_time;
		}
        INT_CPY_NEXT_FIELD(shift);
		if(shift == 0){
			this->cal_growth_weight(growth_rate, feed_time, 12, growth_stage, now, &w_inc, 
					effect_time, lasttime, cal_weight_time);
			if(*weight < 150000){
				if(*weight + w_inc > 150000){
					*weight = 150000;
				}
				else{
					*weight = (*weight) + w_inc;
				}
			}
		}
		
        BIN_CPY_NEXT_FIELD((char*)dress, sizeof(user_piglet_dress));
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_fancy_piglet(uint32_t userid, uint32_t index, uint32_t feed_type, user_get_piglet_house_out_item* p_out)
{
	sprintf(this->sqlstr, "select id, pigletid, breed, sex, transform, growth_stage, birthday, pregnant_time, \
		feed_time, lifetime, train_cnt, train_point, dress from %s where userid = %u and id = %u and shift = 0",
		this->get_table_name(userid),
		userid,
		index
		);
	user_piglet_dress dress;
    memset(&dress, 0, sizeof(user_piglet_dress));
    uint32_t pregnant_time = 0, birthday = 0, lifetime = 0, train_cnt = 0, train_point = 0;
    uint32_t now = time(0), feed_time = 0, growth_stage = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(p_out->index);
        INT_CPY_NEXT_FIELD(p_out->piglet_id);
        INT_CPY_NEXT_FIELD(p_out->breed);
        INT_CPY_NEXT_FIELD(p_out->sex);
        INT_CPY_NEXT_FIELD(p_out->transform);
        INT_CPY_NEXT_FIELD(growth_stage);
        INT_CPY_NEXT_FIELD(birthday);
        INT_CPY_NEXT_FIELD(pregnant_time);
        INT_CPY_NEXT_FIELD(feed_time);
        INT_CPY_NEXT_FIELD(lifetime);
        INT_CPY_NEXT_FIELD(train_cnt);
        INT_CPY_NEXT_FIELD(train_point);
        p_out->hungry_degree = cal_hungry_degree(now, feed_time, feed_type);
        p_out->state =  set_piglet_state(p_out->breed, now, birthday, feed_time,
                feed_type, pregnant_time, lifetime, growth_stage, train_cnt, train_point, p_out->sex);
        BIN_CPY_NEXT_FIELD(&dress, sizeof(user_piglet_dress));
        for(uint32_t k = 0; k < dress.count; ++k){
            p_out->dress[k] = dress.itemid[k];
        }
	STD_QUERY_ONE_END();	
}

uint32_t Cuser_piglet::add_value(uint32_t now , uint32_t datetime)
{

	uint32_t difference = 0;
	if(now >= datetime){
		difference = now - datetime;
	}
	else{
		difference = 0;
		ERROR_LOG("first_parameter: %u, second_parameter: %u", now, datetime);
	}
	uint32_t val = difference / 3600;
	if(difference % 3600 >= 1800){
		val = val + 1;
	}
	return val;
}

int Cuser_piglet::cal_growth_weight(uint32_t growth_rate, uint32_t feed_time, uint32_t feed_type,
		uint32_t growth_stage, uint32_t now, uint32_t* weight, uint32_t effect_time, 
		uint32_t lasttime, uint32_t cal_weight_time)
{
	const uint32_t feed_rate = 2;
	uint32_t is_angel_card = 0, is_feed = 0;
	//挂载了天使卡
	if( effect_time != 0 && lasttime != 0){
		is_angel_card = 1;
	}
	//喂过食
	uint32_t stage_i = growth_stage & ALREADY_FEED;
	if(stage_i != 0){
		is_feed = 1;
	}
	//放入分馆时，计算了体重，魅力、强壮等值
	if(is_feed != 0 && feed_time != cal_weight_time){
		if(feed_time < cal_weight_time){
			uint32_t distance = cal_weight_time - feed_time;
			feed_type = feed_type*3600 > distance ? (feed_type*3600 - distance)/3600:0;
		}
	}

	if(is_angel_card == 0 && is_feed == 0){//没有挂载天使，且没有喂过食

		*weight =  add_value(now, cal_weight_time)* growth_rate;
	} 
	else if(is_angel_card == 1 && is_feed == 0){
		if(cal_weight_time < effect_time){
			if(now  <  effect_time + lasttime){
				*weight = growth_rate * add_value(effect_time, cal_weight_time) +
				   	2* growth_rate * add_value(now, effect_time);
			}			
			else{
				*weight = growth_rate * add_value(effect_time, cal_weight_time) + 2*growth_rate *
					add_value(effect_time+lasttime, effect_time) + growth_rate*add_value(now, effect_time+lasttime);
			}

		}
		else{
			if(now < effect_time + lasttime){
				*weight = 2*growth_rate*add_value(now, cal_weight_time);
			}
			else{
				*weight = 2*growth_rate*add_value(effect_time+lasttime, cal_weight_time) + growth_rate *
					add_value(now, effect_time+lasttime);
			}
		}
	}
	else if(is_angel_card == 0 && is_feed == 1){
		if(now  > cal_weight_time + feed_type*3600){
			*weight = feed_rate*growth_rate*feed_type + growth_rate*add_value(now, cal_weight_time+feed_type*3600);
		}	
		else{
			*weight = feed_rate*growth_rate*add_value(now, cal_weight_time);
		}
	}
	else if(is_angel_card == 1 && is_feed == 1){
		uint32_t feed_start = cal_weight_time;
		uint32_t feed_end = cal_weight_time+3600*feed_type;

		uint32_t card_start = effect_time;
		uint32_t card_end = effect_time + lasttime;
		
		if( card_end <= feed_start){
			if(now >= feed_start && now < feed_end){
				*weight = 2* growth_rate*add_value(now, feed_start);	
			}
			else if(now >= feed_end){
				*weight = 2*growth_rate*add_value(feed_end, feed_start) + growth_rate*add_value(now, feed_end);
			}
		}	
		else if(card_start <= feed_start && card_end >= feed_start && card_end <= feed_end){
				if(now < card_end){
					*weight = 4*growth_rate*add_value(now, feed_start);
				}
				else if( now >= card_end && now < feed_end){
					*weight = 4*growth_rate*add_value(card_end, feed_start)+2*growth_rate*add_value(now, card_end);	
				}
				else if(now >= feed_end){
					*weight = 4*growth_rate*add_value(card_end, feed_start)+ 2*growth_rate*add_value(feed_end, card_end)+
						growth_rate*add_value(now, feed_end);
				}
		}	
		else if(feed_start <= card_start && feed_end >= card_start && feed_end <= card_end){
				if(now >= card_start && now <= feed_end){
					*weight = 2*growth_rate*add_value(card_start, feed_start) + 4*growth_rate*add_value(now, card_start);
				}
				else if( now > feed_end && now <= card_end){
					*weight = 2*growth_rate*add_value(card_start, feed_start) + 4*growth_rate*add_value(feed_end, card_start)+
						2*growth_rate*add_value(now, feed_end);
				}
				else if( now > card_end){
					*weight = 2*growth_rate*add_value(card_start, feed_start)+4*growth_rate*add_value(feed_end, card_start)+
						2*growth_rate*add_value(card_end, feed_end) + growth_rate*add_value(now, card_end);
				}
		}
		else if(feed_end <= card_start){
			if(now >= card_start && now < card_end){
				*weight = 2*growth_rate*add_value(feed_end, feed_start)+growth_rate*add_value(card_start, feed_end) + 2*growth_rate*
					add_value(now, card_start);	
			}
			else if(now >= card_end){
				*weight = 2*growth_rate*add_value(feed_end, feed_start)+growth_rate*add_value(card_start, feed_end)+
					2*growth_rate*add_value(card_end, card_start) + growth_rate*add_value(now, card_end);
			}
		}
		else if(card_start <= feed_start && feed_end <= card_end){
			if(now >= cal_weight_time && now < feed_end){
				*weight = 4*growth_rate*add_value(now, feed_start);
			}
			else if(now >= feed_end && now < card_end){
				*weight = 4*growth_rate*add_value(feed_end, feed_start) + 2*growth_rate*add_value(now, feed_end);   
			}
			else if(now >= card_end){
				*weight = 4*growth_rate*add_value(feed_end, feed_start) + 2*growth_rate*add_value(card_end, feed_end)+
					growth_rate*add_value(now, card_end);
			}
		}
		else if(feed_start <= card_start && card_end <= feed_end){
			if(now >= card_start && now < card_end){
				*weight = 2*growth_rate*add_value(card_start, feed_start) + 4*growth_rate*add_value(now, card_start);
			}	
			else if(now >= card_end && now < feed_end){
				*weight = 2*growth_rate*add_value(card_start, feed_start) + 4*growth_rate*add_value(card_end, card_start)+
					2*growth_rate*add_value(now, card_end);
			}
			else if(now >= feed_end){
				*weight = 2*growth_rate*add_value(card_start, feed_start) + 4*growth_rate*add_value(card_end, card_start)+
					2*growth_rate*add_value(feed_end, card_end)+growth_rate*add_value(now, feed_end);
			}
		}
	}
			//DEBUG_LOG("4: --------history: %u , *weight: %u, effect: %u, last: %u, now: %u, feed_end: %u, cal_weight_time: %u", cur_history, *weight, effect_time, lasttime, now, feed_end, cal_weight_time);
	return 0;

}

uint32_t  Cuser_piglet::cal_hungry_degree(uint32_t now, uint32_t feedtime, uint32_t feed_type)
{
	uint32_t span = now - feedtime;
	if(span >  feed_type * 3600){
		return 100;
	}
	else{
		double decimal = (span * 1.0)/(3600 * feed_type);

		uint32_t ret = (uint32_t)(decimal*10);
		return ret*10;
	}
}

int Cuser_piglet::get_piglet_dress(userid_t userid, uint32_t index, uint32_t *price,
		uint32_t *weight, uint32_t *sex, uint32_t *piglet_id, user_piglet_dress* dress,
		uint32_t effect_time, uint32_t lasttime)
{
	sprintf(this->sqlstr, "select weight, price, sex, growth_rate, feed_time, cal_weight_time, \
			growth_stage, shift, pigletid, dress from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			index
			);
	uint32_t weight_inc = 0, growth_rate = 0, feed_time = 0, growth_stage = 0;
	uint32_t now = time(0), shift = 0;
	uint32_t cal_weight_time = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*weight);
		INT_CPY_NEXT_FIELD(*price);
		INT_CPY_NEXT_FIELD(*sex);
		INT_CPY_NEXT_FIELD(growth_rate);
		INT_CPY_NEXT_FIELD(feed_time);
		INT_CPY_NEXT_FIELD(cal_weight_time);
		if(cal_weight_time == 0){
			cal_weight_time = feed_time;
		}
		INT_CPY_NEXT_FIELD(growth_stage);
		INT_CPY_NEXT_FIELD(shift);
		if(shift == 0){
			cal_growth_weight(growth_rate, feed_time, 12, growth_stage, now, &weight_inc, 
					 effect_time, lasttime, cal_weight_time);
			 if(*weight < 150000){
				if(*weight + weight_inc > 150000){
					 *weight = 150000;
				}
				else{
					 *weight = *weight + weight_inc;
				}
			 }
		}
		INT_CPY_NEXT_FIELD(*piglet_id);
		BIN_CPY_NEXT_FIELD((char*)dress, sizeof(user_piglet_dress));
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_one_piglet(uint32_t userid, uint32_t index,  uint32_t feed_type, 
		user_get_single_piglet_info_out* p_out, uint32_t effect_time, uint32_t lasttime)
{
	sprintf(this->sqlstr, "select id, pigletid, nick, mother_nick, father_nick, breed, sex, \
			feed_time, lifetime, weight, charm, strong, transform, trans_time, birth_cnt, \
			amuse_cnt, train_point, train_cnt, birthday, shift, shift_time, growth_rate, \
			growth_stage, pregnant_time, cal_weight_time, dress, generate, pour_times from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			index
			);
	
	user_piglet_dress dress;
	memset(&dress, 0, sizeof(user_piglet_dress));
	uint32_t birthday = 0, pregnant_time = 0, growth_stage = 0, cal_weight_time = 0;
	uint32_t now = time(0), feed_time = 0, weight_inc = 0, train_cnt = 0;
	uint32_t shift = 0, shift_time = 0, pour_times;

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->index);
		INT_CPY_NEXT_FIELD(p_out->piglet_id);
		BIN_CPY_NEXT_FIELD(p_out->nick, NICK_LEN);
		BIN_CPY_NEXT_FIELD(p_out->mother_nick, NICK_LEN);
		BIN_CPY_NEXT_FIELD(p_out->father_nick, NICK_LEN);
		INT_CPY_NEXT_FIELD(p_out->breed);
		INT_CPY_NEXT_FIELD(p_out->sex);
		INT_CPY_NEXT_FIELD(feed_time);
		INT_CPY_NEXT_FIELD(p_out->lifetime);
		INT_CPY_NEXT_FIELD(p_out->weight);
		INT_CPY_NEXT_FIELD(p_out->charm);
		INT_CPY_NEXT_FIELD(p_out->strong);
		INT_CPY_NEXT_FIELD(p_out->transform);
		INT_CPY_NEXT_FIELD(p_out->trans_time);
		INT_CPY_NEXT_FIELD(p_out->birth_cnt);
		INT_CPY_NEXT_FIELD(p_out->amuse_cnt);
		INT_CPY_NEXT_FIELD(p_out->train_point);
		INT_CPY_NEXT_FIELD(train_cnt);
		INT_CPY_NEXT_FIELD(birthday);
		INT_CPY_NEXT_FIELD(shift);
		INT_CPY_NEXT_FIELD(shift_time);
		INT_CPY_NEXT_FIELD(p_out->growth_rate);
		INT_CPY_NEXT_FIELD(growth_stage);
		INT_CPY_NEXT_FIELD(pregnant_time);
		INT_CPY_NEXT_FIELD(cal_weight_time);
		uint32_t span_time = 0;
		if(shift == 1 || shift == 2){
			span_time = now - shift_time;
		}
		if(cal_weight_time == 0){
			cal_weight_time = feed_time;
		}
		if(pregnant_time != 0){
			p_out->birth_child_time = 24*3600 - (now - pregnant_time - span_time);
		}
		if(shift == 0){
			cal_growth_weight(p_out->growth_rate, feed_time, feed_type, growth_stage, now, &weight_inc,
					effect_time, lasttime, cal_weight_time);
			if(p_out->weight < 150000){
				if(p_out->weight + weight_inc > 150000){
					p_out->weight = 150000;
				}
				else{
					p_out->weight = p_out->weight + weight_inc; 
				}
			}
		}
		p_out->state =  set_piglet_state(p_out->breed, now, birthday + span_time, feed_time+span_time, feed_type,
			   	pregnant_time+span_time, p_out->lifetime, growth_stage, train_cnt, p_out->train_point, p_out->sex);
		p_out->hungry_degree = cal_hungry_degree(now, feed_time+span_time, feed_type);
		p_out->age = now - (birthday+span_time);
		BIN_CPY_NEXT_FIELD(&dress, sizeof(user_piglet_dress));
		for(uint32_t k = 0; k < dress.count; ++k){
			p_out->dress[k] = dress.itemid[k];
		}
		INT_CPY_NEXT_FIELD(p_out->generate);
		INT_CPY_NEXT_FIELD(pour_times);
		p_out->able_inject = check_able_inject_energy(feed_time, birthday,     
                                p_out->lifetime, p_out->sex, pregnant_time,    
                                p_out->breed, pour_times, p_out->generate, now);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::clear(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set train_cnt = 0, amuse_cnt = 0, growth_stage = \
			growth_stage & %u where userid = %u and shift = 0",
            this->get_table_name(userid),
			~SPEED_BORN,
            userid
            );

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_piglet::get_piglet_sum_hungry(uint32_t userid, uint32_t* sum_hungry, uint32_t feed_type)
{
	uint32_t *p_item = 0;
	uint32_t p_count = 0;
	uint32_t now = time(0);
	int ret = this->get_live_piglet_feedtime(userid, &p_item, &p_count);
	if(ret != SUCC){
		if(p_item != 0){
			free(p_item);
		}
	}
	else{
		for(uint32_t k = 0; k < p_count; ++k){
			*sum_hungry = *sum_hungry + cal_hungry_degree(now, *(p_item + k), feed_type);
		}//for

		free(p_item);
	}//else
	
	//如果饥饿度不满100，按100算
	if(*sum_hungry > 0 && *sum_hungry < 100){
		*sum_hungry = 100;
	}
	return 0;
}

int Cuser_piglet::get_piglet_counts(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and growth_stage & 0x01 != 0x01 \
			and shift = 0",
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_piglet_survive(userid_t userid, uint32_t *count)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and \
			growth_stage & 0x01 != 0x01 and %u < feed_time + 12*3600 + 10*24*3600 and \
			%u - birthday < lifetime and shift = 0",
		this->get_table_name(userid),
		userid,
		now,
		now
	   );

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*count); 
    STD_QUERY_ONE_END();
}
int Cuser_piglet::get_all_counts(userid_t userid, uint32_t* count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and shift = 0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet::get_live_piglet_feedtime(uint32_t userid, uint32_t** pp_list, uint32_t* p_count)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "select feed_time from %s where userid = %u and (growth_stage & 0x01) != 0x01 \
			and %u < (12 * 3600 + 10 *24 *3600 + feed_time) and (%u - birthday) <= lifetime and shift = 0",
			this->get_table_name(userid),
			userid,	
			now,
			now
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::get_pregnant_piglet(userid_t userid, user_transfer_piglet_in_item* p_in_item, uint32_t in_count,
		pregnant_piglet_t **p_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
	uint32_t flag = 0;
    for(uint32_t i = 0; i < in_count; ++i) {
		if((p_in_item+i)->index != 0 ){
			if(flag == 0){
				in_str << (p_in_item+i)->index;
			}
			else{
				in_str << "," << (p_in_item+i)->index;
			}
			++flag;
		}
    }
	sprintf(this->sqlstr, "select id, sex, breed, birthday, feed_time, lifetime, pregnant_time from %s \
			where userid = %u and id in(%s) and shift = 0",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->index);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->sex);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->breed);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->birthday);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->feedtime);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->lifetime);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->pregnant_time);
	STD_QUERY_WHILE_END();

}
int Cuser_piglet::check_performance_by_hungry(userid_t userid, uint32_t *piglet_index, uint32_t *state)
{
	std::ostringstream in_str;
	uint32_t flag = 0;
    for(uint32_t i = 0; i < 3; ++i) {
		if(*(piglet_index+i) != 0 ){
			if(flag == 0){
				in_str << *(piglet_index+i);
			}
			else{
				in_str << "," << *(piglet_index+i);
			}
			++flag;
		}
    }
	uint32_t  now = time(0);
	sprintf(this->sqlstr, "select feed_time from %s where userid = %u and id in(%s) and (growth_stage & 0x01) != 0x01 \
		and %u < (12 * 3600 + 10 *24 *3600 + feed_time) and (%u - birthday) <= lifetime and shift = 0",
		this->get_table_name(userid),
		userid,
		in_str.str().c_str(),
		now,
		now
		);		
	uint32_t *p_item = 0;
	uint32_t cnt = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_item, &cnt);
		INT_CPY_NEXT_FIELD(*(p_item + i));
	STD_QUERY_WHILE_END_WITHOUT_RETURN();
	if(cnt < flag){
		*state = 0;
	}
	else{
		for(uint32_t k = 0; k < cnt; ++k){
			uint32_t hun_degree = cal_hungry_degree(now, *(p_item+k), 12);
			if(hun_degree >= 100){
				*state = 0;
				return 0;
			}
		}
		*state = 1;
	}
	if(p_item != 0){
		free(p_item);
	}

	return 0;
}

int Cuser_piglet::change_attribute_value(userid_t userid, uint32_t weight_inc, uint32_t charm_inc,
	   	uint32_t strong_inc, uint32_t feed_type, uint32_t effect_time, uint32_t lasttime, uint32_t now)
{
	uint32_t count = 0;
	user_piglet_feed_attr_t *p_item = 0;
	sprintf(this->sqlstr, "select id, weight, charm, strong, breed, feed_time, growth_stage, growth_rate, \
			cal_weight_time from %s where userid = %u and (growth_stage & 0x02) = 0x02 and %u < (feed_time + %u) \
			and (%u - birthday) <= lifetime and shift = 0",
			this->get_table_name(userid),
			userid,
			now,
			feed_type*3600 + 10*24*3600,
			now
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_item, &count);
		INT_CPY_NEXT_FIELD((p_item + i)->index);
		INT_CPY_NEXT_FIELD((p_item + i)->weight);
		INT_CPY_NEXT_FIELD((p_item + i)->charm);
		INT_CPY_NEXT_FIELD((p_item + i)->strong);
		INT_CPY_NEXT_FIELD((p_item + i)->breed);
		INT_CPY_NEXT_FIELD((p_item + i)->feed_time);
		INT_CPY_NEXT_FIELD((p_item + i)->growth_stage);
		INT_CPY_NEXT_FIELD((p_item + i)->growth_rate);
		INT_CPY_NEXT_FIELD((p_item + i)->cal_weight_time);
		if((p_item+i)->cal_weight_time == 0){
			(p_item+i)->cal_weight_time = (p_item+i)->feed_time;
		}
	STD_QUERY_WHILE_END_WITHOUT_RETURN();

	for(uint32_t k = 0; k < count; ++k){
		uint32_t weight_growth = 0;
		this->cal_growth_weight((p_item+k)->growth_rate, (p_item+k)->feed_time, feed_type, \
				(p_item+k)->growth_stage, now, &weight_growth, effect_time, lasttime,
				(p_item + k)->cal_weight_time);
		this->add_attribute_value(userid, (p_item+k)->index, weight_growth+weight_inc, charm_inc, strong_inc,
				(p_item+k)->growth_stage, (p_item + k)->breed, now);
		
	}//for	
	if(p_item != 0){
		free(p_item);
	}
	return 0;

}

int Cuser_piglet::add_attribute_value(uint32_t userid, uint32_t index, uint32_t weight_inc, uint32_t charm_inc,
		uint32_t strong_inc, uint32_t growth_stage, uint32_t breed, uint32_t now)
{
	sprintf(this->sqlstr, "update %s set weight = if(weight >= 150000, weight, if((weight + %u) > \
		150000, 150000, (weight+%u))), growth_stage = (0x04 | growth_stage), \
		feed_time = %u, cal_weight_time = %u where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			weight_inc,
			weight_inc,
			now,
			now,
			userid,
			index
			);
	

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::add_nutrition_value(userid_t userid, uint32_t weight_inc, uint32_t charm_inc, uint32_t strong_inc)
{
	sprintf(this->sqlstr, "update %s set weight = weight + %u, charm = charm + if((breed >= 10000 and breed < 20000), \
		%u, 0), strong = strong + if(breed >= 30000, %u, 0) where userid = %u and (growth_stage & 0x01) != 0x01 \
		and shift = 0",
			this->get_table_name(userid),
			weight_inc,
			charm_inc,
			strong_inc,
			userid
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_piglet::get_max_index(userid_t userid, uint32_t *max_index)
{
	sprintf(this->sqlstr, "select MAX(id) from %s where userid = %u and shift = 0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*max_index);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_newest_piglet(uint32_t userid, uint32_t feed_type, user_get_piglet_house_out_item* p_out)
{
	uint32_t max_index = 0;	
	get_max_index(userid, &max_index);
	int ret = get_fancy_piglet(userid, max_index, feed_type, p_out);
	return ret;
}

int Cuser_piglet::get_revalant_train(uint32_t userid, uint32_t index, uint32_t *train_point, uint32_t *train_cnt, 
		uint32_t *weight, uint32_t *charm, uint32_t *strong, uint32_t *breed)
{
	sprintf(this->sqlstr, "select train_point, train_cnt, weight, charm, strong, breed from %s where \
			userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*train_point);
		INT_CPY_NEXT_FIELD(*train_cnt);
		INT_CPY_NEXT_FIELD(*weight);
		INT_CPY_NEXT_FIELD(*charm);
		INT_CPY_NEXT_FIELD(*strong);
		INT_CPY_NEXT_FIELD(*breed);
	STD_QUERY_ONE_END();	

}

int Cuser_piglet::train_piglet(uint32_t userid, uint32_t index, uint32_t type, uint32_t *train_point, 
		user_train_piglet_out *out)
{
	uint32_t train_cnt = 0;
	uint32_t  breed = 0;
	out->index = index;
	int ret = get_revalant_train(userid, index, train_point, &train_cnt, &out->weight, 
			&out->charm, &out->strong, &breed);
	if(ret == SUCC && *train_point < 80){
		if((train_cnt & 0x01) == 0 || (train_cnt & 0x02) == 0){
			if(type == 0){
				train_cnt |= 0x01;
			}
			else{
				train_cnt |= 0x02;
			}
			//uint32_t sort = get_piglet_type(breed);
			uint32_t point = (*train_point + 2) > 80 ? 80: (*train_point + 2);
			//if(sort == 0){
				//out->weight += 5*10;
				//this->update_two_cols(userid, index, "weight", "train_point", out->weight, point);
				//out->weight = 5;
				//out->strong = 0;
				//out->charm = 0;
			//}
			//else if(sort == 1){
				//out->charm += 5;
				//this->update_two_cols(userid, index, "charm", "train_point", out->charm, point);
				//out->charm = 5;
				//out->weight = 0;
				//out->strong = 0;
			//}
			//else{
				//out->strong += 5;
				//this->update_two_cols(userid, index, "strong", "train_point", out->strong, point);
				//out->strong = 5;
				//out->charm = 0;
				//out->weight = 0;
			//}
			if(out->weight < 150000){
				if(out->weight + 5*10 > 150000){
					out->weight = 150000;
				}
				else{
					 out->weight = out->weight + 5*10;
				}
			}
			
			this->update_two_cols(userid, index, "weight", "train_point", out->weight, point);
			out->weight = 5;
			this->update(userid, index, "train_cnt", train_cnt);
			out->state = 1;	

		}
	}
	return ret; 
}

int Cuser_piglet::change_piglet_nick(userid_t userid, uint32_t index, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);

	sprintf(this->sqlstr, "update %s set nick = '%s' where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			nick_mysql,
			userid,
			index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_piglet::remove(userid_t userid, uint32_t index)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and id = %u",
			this->get_table_name(userid), 
			userid,
			index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::get_mail_piglet_make_pair(userid_t userid, uint32_t index, uint32_t* feed_time,
		uint32_t *birthday, uint32_t* lifetime, uint32_t *breed, uint32_t *birth_cnt)
{
	sprintf(this->sqlstr, "select breed, feed_time, birthday, lifetime, birth_cnt from %s where userid = %u \
			and id = %u and sex = 0 and growth_stage & 0x01 != 0x01 and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*breed);
		INT_CPY_NEXT_FIELD(*feed_time);
		INT_CPY_NEXT_FIELD(*birthday);
		INT_CPY_NEXT_FIELD(*lifetime);
		INT_CPY_NEXT_FIELD(*birth_cnt);
	STD_QUERY_ONE_END();	
}
uint32_t Cuser_piglet::get_piglet_growth_state(uint32_t birthday,
		uint32_t lifetime, uint32_t breed, uint32_t now)
{
	//幼年：0， 青年：1， 成年：2
	uint32_t span = 0;
	for(uint32_t k = 0; k < 3; ++k){
		DEBUG_LOG("k: %u", k);
		if((now - birthday) <= span + growth_span[get_piglet_type(breed)][k]*24*3600){
			return k;
		}
		span = span + growth_span[get_piglet_type(breed)][k]*24*3600;
	}
	return 2;
}

int Cuser_piglet::check_use_more_child_tool(userid_t userid, uint32_t index, uint32_t *growth,
		uint32_t *growth_stage)
{
	sprintf(this->sqlstr, "select birthday, lifetime, breed, growth_stage from %s \
			where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	uint32_t birthday = 0, lifetime = 0, breed = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(birthday);
		INT_CPY_NEXT_FIELD(lifetime);
		INT_CPY_NEXT_FIELD(breed);
		INT_CPY_NEXT_FIELD(*growth_stage);
	STD_QUERY_ONE_END_WITHOUT_RETURN();	

	uint32_t now = time(0);
	*growth = get_piglet_growth_state(birthday, lifetime, breed, now);
	return 0;

}

int  Cuser_piglet::lengthen_lifetime(uint32_t userid, uint32_t index, uint32_t breed)
{
	uint32_t birthday = 0, lifetime = 0;
	this->select_two_cols(userid, index, "birthday", "lifetime", &birthday, &lifetime);
	uint32_t now = time(0);
	uint32_t growth = get_piglet_growth_state(birthday, lifetime, breed, now);
	if(growth != 2){
	  uint32_t blank = (1 + growth_span[get_piglet_type(breed)][0] + growth_span[get_piglet_type(breed)][1])
		  *24*3600 - (now - birthday); 	
	  this->update_two_cols(userid, index, "birthday", "lifetime", birthday - blank, lifetime + blank);
	  return 0;
	}
	else{
		return 1;
	}
}

int Cuser_piglet::check_mail_piglet_make_pair(userid_t userid, uint32_t index, uint32_t *state)
{
	uint32_t feed_time = 0, birthday = 0, lifetime = 0, breed = 0;
	uint32_t birth_cnt = 0;
	get_mail_piglet_make_pair(userid, index, &feed_time, &birthday, &lifetime, &breed, &birth_cnt);
	*state = 0;	

	int ret = check_able_make_pair(feed_time, birthday, lifetime, breed, birth_cnt);
	if(ret == 1){
		int ret = this->update(userid, index, "birth_cnt", birth_cnt+1); 
		if( ret == SUCC){
			*state = 1;
		}
	}
	return 0;
}

int Cuser_piglet::check_common_piglet(uint32_t feed_time, uint32_t birthday, uint32_t lifetime,
		uint32_t breed)
{
	//判断饥饿度    
    uint32_t now = time(0);
    uint32_t hungry_degree = cal_hungry_degree(now, feed_time, 12);
	DEBUG_LOG("hungry_degree: %u, span: %u, lifetime: %u", hungry_degree, now - birthday, lifetime);
    if( hungry_degree >= 100){
        return 0;
    }
	//判断是否老死
	if(now - birthday >= lifetime){
		return 0;
	}
    //判断是否成年
    uint32_t growth_state = get_piglet_growth_state(birthday, lifetime, breed, now);
	DEBUG_LOG("growth_state: %u", growth_state);
    if(growth_state != 2){
        return 0;
    }
	return 1;
}

int Cuser_piglet::check_able_make_pair(uint32_t feed_time, uint32_t birthday, uint32_t lifetime,
		uint32_t breed, uint32_t birth_cnt)
{
	//判断是否存活
	int ret = check_piglet_dead(feed_time, 12, birthday,  lifetime);
	DEBUG_LOG("ret: %u", ret);
	if(ret != 0){
		return 0;
	}
    //判断是否成年
    uint32_t now = time(0);
    uint32_t growth_state = get_piglet_growth_state(birthday, lifetime, breed, now);
	DEBUG_LOG("growth_state: %u", growth_state);
    if(growth_state != 2){
        return 0;
    } 
	//判断是否达到配对次数  
	if(birth_cnt >= 10){
		DEBUG_LOG("birth_cnt: %u", birth_cnt);
		return 0;
	}
	return 1;
}

int Cuser_piglet::check_satisfy_process(userid_t userid, uint32_t index, uint32_t sort, uint32_t *state)
{
	uint32_t pigletid = 0, feed_time = 0, birthday = 0, lifetime = 0, breed = 0;
	sprintf(this->sqlstr, "select pigletid, feed_time, birthday, lifetime, breed from %s where \
			userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(pigletid);
		INT_CPY_NEXT_FIELD(feed_time);
		INT_CPY_NEXT_FIELD(birthday);
		INT_CPY_NEXT_FIELD(lifetime);
		INT_CPY_NEXT_FIELD(breed);
	STD_QUERY_ONE_END_WITHOUT_RETURN();	
	DEBUG_LOG("sort: %u, breed: %u, feed_time: %u, birthday: %u, lifetime: %u", 
			sort, breed, feed_time, birthday, lifetime);
	//修正数据库猪仔种类错误数据
	if(pigletid >=  1593000 && pigletid <= 1593029){
		if(breed != fat_piglet_sort[pigletid - 1593000][1]){
			breed = fat_piglet_sort[pigletid - 1593000][1];
		}
	}

	if(sort == breed){
		*state = check_common_piglet(feed_time, birthday, lifetime, breed);
	}
	return 0;
}

int Cuser_piglet::get_outgo_count(userid_t userid, uint32_t *count_in_piglet)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and carry_flag = 1 \
			and breed >= 10000 and shift = 0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count_in_piglet);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet::check_able_carry_piglet(userid_t userid, uint32_t index, 
		user_piglet_carry_piglet_out *out)
{
	uint32_t birthday = 0, feed_time = 0, breed = 0, lifetime = 0, pigletid = 0;
	uint32_t transform_id = 0, trans_start = 0, trans_last = 0;

	user_piglet_dress dress;
	memset(&dress, 0, sizeof(user_piglet_dress));
	int ret = get_fbbl_attr_itemid(userid, index, &birthday, &feed_time, &breed,
		   	&lifetime, &pigletid, &transform_id, &trans_start, &trans_last, &dress);
	if(ret == SUCC and breed >= 10000){
		int ret = check_piglet_dead(feed_time, 12, birthday,  lifetime);
		uint32_t live_time = time(0) - feed_time;
		uint32_t dying = 0;
		if((live_time >= (12*3600 + 5*24*3600)) && (live_time < (12*3600 + 10*24*3600))){
			dying = 1;
		}
		if(ret == 0 && dying == 0){
			uint32_t now = time(0);
			out->state = 1;
			out->pigletid = pigletid;	
			out->stage = get_piglet_growth_state(birthday, lifetime, breed, now);
			if(now - trans_start >= trans_last){
				out->trans_time = 0; 
				out->transfrom_id = 0;
				if(transform_id != 0){
					this->update(userid, index, "transform", 0);	
				}
			}
			else{
				out->transfrom_id = transform_id;	
				out->trans_time = trans_last - (now - trans_start);
			}
			for(uint32_t k = 0; k < 2; ++k){
				out->dress[k] = dress.itemid[k];	
			}
		}
	}
	else{
		out->state = 0;
	}
	return 0;

}

int Cuser_piglet::clear_carry_flag(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set carry_flag = 0 where userid = %u",
			this->get_table_name(userid),
			userid
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}
int Cuser_piglet::get_all_able_make_pair_mail_pig(uint32_t userid, user_get_able_make_pair_mail_piglet_out_item 
		**pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, pigletid, nick, breed, birthday, feed_time, lifetime, birth_cnt, \
			generate from %s where userid = %u and sex = 0 and growth_stage & 0x01 != 0x01 and shift = 0",
			this->get_table_name(userid),
			userid
			);
	uint32_t breed = 0, birthday = 0, feed_time = 0, lifetime = 0, birth_cnt = 0;
	uint32_t index = 0, flag = 0, generate = 0, tmp_count = 0, k = 0;
	uint32_t pigletid = 0;
	char nick[NICK_LEN] = { 0 };
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, &tmp_count);
		INT_CPY_NEXT_FIELD(index);
		INT_CPY_NEXT_FIELD(pigletid);
		BIN_CPY_NEXT_FIELD(nick, NICK_LEN);
		INT_CPY_NEXT_FIELD(breed);
		INT_CPY_NEXT_FIELD(birthday);
		INT_CPY_NEXT_FIELD(feed_time);
		INT_CPY_NEXT_FIELD(lifetime);
		INT_CPY_NEXT_FIELD(birth_cnt);
		INT_CPY_NEXT_FIELD(generate);
		flag = check_able_make_pair(feed_time, birthday, lifetime, breed, birth_cnt);
		if(flag == 1){
			(*pp_list + k)->index = index;
			(*pp_list + k)->pigletid = pigletid;
			(*pp_list + k)->breed = breed;
			(*pp_list + k)->generate = generate;
			memcpy((*pp_list + k)->nick, nick, NICK_LEN);
			++(*p_count);
			++k;
		}
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::get_no_born_child(userid_t userid, uint32_t parent_index,
	   	uint32_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id from %s where userid = %u and parentid = %u \
			and (growth_stage & 0x01) = 0x01 and shift = 0",
			this->get_table_name(userid),
			userid,
			parent_index
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::get_no_born_state_child(userid_t userid, uint32_t parent_index,
	   	no_born_piglet_st **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, pigletid, breed from %s where userid = %u and parentid = %u \
			and (growth_stage & 0x01) = 0x01 and shift = 0",
			this->get_table_name(userid),
			userid,
			parent_index
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pigletid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->breed);
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::update_female_pregnent(userid_t userid, uint32_t index, uint32_t pregnent_time)
{
	uint32_t sex  = 0, growth_stage = 0;
	int ret = this->select_two_cols(userid, index, "sex", "growth_stage", &sex, &growth_stage);
	if(ret == SUCC && sex == 1){
		//growth_stage &= ~ABLE_INC; 
		//growth_stage &= ~ABLE_VARIATE; 
		sprintf(this->sqlstr, "update %s set  pregnant_time  = %u, growth_stage = %u where \
				userid = %u and id = %u and shift = 0",
				this->get_table_name(userid),
				pregnent_time,
				growth_stage,
				userid,
				index
				);
		STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
	}

	return 1;
}
int Cuser_piglet::get_nick(userid_t userid, uint32_t index, char* nick)
{
	sprintf(this->sqlstr, "select nick from %s where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(nick, NICK_LEN);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_fbbl_attr_itemid(uint32_t userid, uint32_t index, uint32_t *birthday, uint32_t *feed_time,
		uint32_t *breed, uint32_t *lifetime, uint32_t *pigletid, uint32_t *transform_id, uint32_t *trans_start,
		uint32_t *trans_last, user_piglet_dress *dress)
{
	sprintf(this->sqlstr, "select pigletid, breed, birthday, feed_time, lifetime, transform, \
			trans_time, trans_last, dress from %s where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*pigletid);
		INT_CPY_NEXT_FIELD(*breed);
		INT_CPY_NEXT_FIELD(*birthday);
		INT_CPY_NEXT_FIELD(*feed_time);
		INT_CPY_NEXT_FIELD(*lifetime);
		INT_CPY_NEXT_FIELD(*transform_id);
		INT_CPY_NEXT_FIELD(*trans_start);
		INT_CPY_NEXT_FIELD(*trans_last);
	    BIN_CPY_NEXT_FIELD((char*)dress, sizeof(user_piglet_dress));
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_fbbl_attr(uint32_t userid, uint32_t index, uint32_t *birthday, uint32_t *feed_time,
		uint32_t *breed, uint32_t *lifetime)
{
	sprintf(this->sqlstr, "select breed, birthday, feed_time, lifetime from %s \
			where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*breed);
		INT_CPY_NEXT_FIELD(*birthday);
		INT_CPY_NEXT_FIELD(*feed_time);
		INT_CPY_NEXT_FIELD(*lifetime);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::give_new_life(userid_t userid, uint32_t index)
{
	uint32_t now = time(0);
	uint32_t breed = 0, birthday = 0, feed_time = 0, lifetime = 0;

	this->get_fbbl_attr(userid, index, &birthday, &feed_time, &breed, &lifetime);
	int res = check_piglet_dead(feed_time, 12, birthday, lifetime);
	DEBUG_LOG("res: %u", res);
	if(res == 0){
		sprintf(this->sqlstr, "update %s set lifetime = lifetime + 10*24*3600 where userid = %u \
				and id = %u and shift = 0",
				this->get_table_name(userid),
				userid,
				index
				);
	}
	else{
		uint32_t blank = (growth_span[get_piglet_type(breed)][0] + growth_span[get_piglet_type(breed)][1]+1) *
			3600*24;
		sprintf(this->sqlstr, "update %s set birthday = %u, feed_time = %u, lifetime = %u where userid = %u \
			and id = %u and shift = 0",
			this->get_table_name(userid),
			now - blank,
			now - 12*3600,
			10*3600*24 + blank,
			userid,
			index
			);

	}
	
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::change_pregnant_state(userid_t userid, uint32_t index)
{
	uint32_t growth_stage = 0;
	this->select(userid, index,  "growth_stage", &growth_stage);
	growth_stage &= ~ABLE_INC;
	growth_stage &= ~ABLE_VARIATE;
	growth_stage &= ~SPEED_BORN;	
	
	sprintf(this->sqlstr, "update %s set pregnant_time = %u, growth_stage = %u where \
			userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			0,
			growth_stage,
			userid,
			index
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::get_another_piglets(userid_t userid, user_get_another_pighouse_out_item** pp_list,
		uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, pigletid from %s where userid = %u and shift = 1",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->piglet_id);
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::get_for_cal_weight(userid_t userid, uint32_t index, uint32_t *growth_rate, uint32_t *feed_time, 
		uint32_t *growth_stage, uint32_t *cal_weight_time)
{
	sprintf(this->sqlstr, "select growth_rate, feed_time, growth_stage, cal_weight_time \
			from %s where userid = %u and id = %u and shift = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*growth_rate);
		INT_CPY_NEXT_FIELD(*feed_time);
		INT_CPY_NEXT_FIELD(*growth_stage);
		INT_CPY_NEXT_FIELD(*cal_weight_time);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::transfer_piglet(userid_t userid, uint32_t index, uint32_t effect_time, 
		uint32_t lasttime, uint32_t flag)
{
 	uint32_t weight_inc = 0, growth_rate = 0, feed_time = 0, growth_stage = 0;	
	uint32_t cal_weight_time = 0;
	uint32_t now = time(0);
	cal_growth_weight(growth_rate, feed_time, 12, growth_stage, now, &weight_inc,
			effect_time, lasttime, cal_weight_time);
	if(weight_inc != 0){
		uint32_t weight_in_table = 0;
		this->select(userid, index, "weight", &weight_in_table);
		if(weight_in_table < 150000){
			if(weight_in_table + weight_inc > 150000){
				this->update(userid, index, "weight", 150000);
			}
			else{
				this->update(userid, index, "weight", weight_in_table + weight_inc);
			}
		}
		//this->update_inc_col(userid, index, "weight", weight_inc);
		this->update(userid, index, "cal_weight_time", now);
	}
	int ret = this->update_two_cols(userid, index, "shift", "shift_time", flag, now);

	return ret;
}

int Cuser_piglet::put_in_primary(userid_t userid, uint32_t index, uint32_t flag)
{
	uint32_t shift = 0, shift_time = 0;	
	uint32_t now = time(0);
	this->select_two_cols(userid, index, "shift", "shift_time", &shift, &shift_time);
	sprintf(this->sqlstr, "update %s set birthday = birthday + %u, feed_time = feed_time + %u, \
			pregnant_time = if(pregnant_time != 0, pregnant_time + %u, pregnant_time), \
			trans_time = if(trans_time != 0, trans_time + %u, trans_time), cal_weight_time = \
			cal_weight_time + %u, shift = 0, shift_time = 0, energy = 0 where userid = %u and id = %u and shift = %u",
			this->get_table_name(userid),
			now - shift_time,
			now - shift_time,
			now - shift_time,
			now - shift_time,
			now - shift_time,
			userid,
			index,
			flag
			);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet::get_secondary_piglet_cnt(userid_t userid, uint32_t *cnt_in_secondary, uint32_t flag)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and shift = %u",
			this->get_table_name(userid),
			userid,
			flag
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*cnt_in_secondary);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::get_breed_dress(userid_t userid, uint32_t index, uint32_t *breed, user_piglet_dress *dress)
{
	sprintf(this->sqlstr, "select breed, dress from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*breed);
	    BIN_CPY_NEXT_FIELD(dress, sizeof(user_piglet_dress));
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::put_on(userid_t userid, uint32_t index, user_piglet_dress *dress)
{

	char dress_list[mysql_str_len(sizeof(user_piglet_dress))];
	memset(dress_list, 0, sizeof(dress_list));	
	set_mysql_string(dress_list, (char*)dress, sizeof(user_piglet_dress));
	sprintf(this->sqlstr, "update %s set dress = '%s' where userid = %u and id = %u",
			this->get_table_name(userid),
			dress_list,
			userid,
			index
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_piglet::check_able_inject_energy(uint32_t feed_time, uint32_t birthday, uint32_t lifetime,
	   	uint32_t sex, uint32_t pregnant, uint32_t breed, uint32_t pour_times, uint32_t generate, uint32_t now)
{
	uint32_t survive = check_piglet_dead(feed_time, 12, birthday, lifetime);
	DEBUG_LOG("===survive: %u, sex: %u, pregnant: %u", survive, sex, pregnant);
	if(survive != 0 || (sex == 1 && pregnant !=0)){
		return 0;
	}
	uint32_t status = get_piglet_growth_state(birthday, lifetime, breed, now);
	DEBUG_LOG("=====status: %u ", status);
	if(status != 2){
		return 0;
	}

   uint32_t max_inject = 0;
   if(sex == 2){
        max_inject = 5;
    }
    else{
        uint32_t n_ret[]={ 1, 2, 3, 4, 5};
		if(generate != 0){
			max_inject = n_ret[generate-1];
		}
		else{
			max_inject = 1;
		}
    }
	DEBUG_LOG(" =====max_inject: %u, pour_tims: %u", max_inject, pour_times);
	if(pour_times >= max_inject){
		return 0;
	}

	DEBUG_LOG(" now ok!");
	return 1;
}

int Cuser_piglet::get_machine_piglets(uint32_t userid, uint32_t *p_count, 
		user_get_piglet_machine_work_out_item_1 **pp_list)
{
	*p_count = 0;
	
	sprintf(this->sqlstr, "select id, pigletid, breed, sex, dress, energy, \
			pour_times, generate from %s where userid = %u and shift = 2 and work = 0",
			this->get_table_name(userid),
			userid
			);
	user_piglet_dress dress;
	memset(&dress, 0, sizeof(user_piglet_dress));
	uint32_t pour_times = 0, generate = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->piglet_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->breed);
		INT_CPY_NEXT_FIELD((*pp_list + i)->sex);
		BIN_CPY_NEXT_FIELD(&dress, sizeof(user_piglet_dress));
		for(uint32_t k = 0; k < dress.count; ++k){
			(*pp_list + i)->dress[k] = dress.itemid[k];
		}
		INT_CPY_NEXT_FIELD((*pp_list + i)->energy);
		INT_CPY_NEXT_FIELD(pour_times);
		INT_CPY_NEXT_FIELD(generate);
		if(pour_times != 0){
			(*pp_list+i)->max_energy = get_inject_max((*pp_list+i)->sex, generate) - (pour_times-1)*10;
		}
		else{
			(*pp_list+i)->max_energy = get_inject_max((*pp_list+i)->sex, generate);
		}
		DEBUG_LOG("[energy: %u   max_energy: %u]", (*pp_list + i)->energy, (*pp_list+i)->max_energy);
	STD_QUERY_WHILE_END();
}

int Cuser_piglet::get_engergy_for_mining(uint32_t userid, uint32_t index, uint32_t* energy)
{
	sprintf(this->sqlstr, "select energy from %s where userid = %u and id = %u and shift = 2 and work = 0",
			this->get_table_name(userid),
			userid,
			index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*energy);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::send_to_mining(uint32_t userid, uint32_t pigletid, uint32_t need_energy,
		uint32_t *state)
{
	uint32_t energy = 0;
	int ret = get_engergy_for_mining(userid, pigletid, &energy);
	if(ret == SUCC){
		if(energy >= need_energy){
			update_two_cols(userid, pigletid, "energy", "work", energy-need_energy, 3);
			*state = 1;
		}
		else{
			*state = 0;
		}
	}
	else{
		*state = 0;
	}
	return SUCC;
}

int Cuser_piglet::get_info_for_inject_engergy(uint32_t userid, uint32_t index, uint32_t* sex,
		uint32_t* generate, uint32_t* energy, uint32_t *pour_times)
{
	sprintf(this->sqlstr, "select sex, generate, energy, pour_times from %s where \
			userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			index	
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*sex);
		INT_CPY_NEXT_FIELD(*generate);
		INT_CPY_NEXT_FIELD(*energy);
		INT_CPY_NEXT_FIELD(*pour_times);
	STD_QUERY_ONE_END();	
}

uint32_t Cuser_piglet::get_inject_max(uint32_t sex, uint32_t generate)
{
	if(sex == 2){
		return 100;		
	}
	else{
		uint32_t n_ret[]={ 40, 30, 20, 10, 0};
		return 100 - n_ret[generate-1];
	}
	
}

int Cuser_piglet::inject_energy(userid_t userid, uint32_t index, uint32_t *state, uint32_t dose)
{
	uint32_t sex = 0, generate = 0, energy = 0, pour_times = 0;	
	int ret = get_info_for_inject_engergy(userid, index, &sex, &generate, &energy, &pour_times); 
	DEBUG_LOG("[sex: %u, generate: %u, energy: %u, pour_times: %u]", sex, generate, energy, pour_times);
	if(ret == SUCC){
		uint32_t max_inject = get_inject_max(sex, generate);		
		DEBUG_LOG("max_inject: %u", max_inject);
		if(dose == 0){
			if(energy >= max_inject - pour_times*10){
				*state = 0;
			}
			else{
				if(max_inject - pour_times*10 <= 50){
					*state = 0;	
				}
				else{
					*state = 1;
					ret = this->update_two_cols(userid, index, "energy", "pour_times", 
							max_inject - pour_times*10, pour_times+1);
				}
			}
		}
		else{
			if(pour_times > 0){
				pour_times = pour_times - 1;
			}
			DEBUG_LOG("[energy: %u, pour_times: %u]", energy, max_inject-pour_times*10);
			if(energy < max_inject - pour_times*10){
				uint32_t need_dose = 0;
				if(energy + dose > max_inject - pour_times*10){
					need_dose = max_inject - pour_times*10;	
				}	
				else{
					need_dose = energy + dose;
				}
				this->update(userid, index, "energy", need_dose);
				*state = 1;
				DEBUG_LOG("[energy: %u, pour_times: %u, state: %u]", energy, max_inject-pour_times*10, *state);
			}//if	
		}
		
	}//if(ret==SUCC)
	return ret;
}

int Cuser_piglet::finish_mining(userid_t userid, uint32_t in_count, user_finish_some_map_mining_in_item *p_in_list)
{
	
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_list + i)->piglet_id;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "update %s set work = 0 where userid = %u and id in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_piglet::finish_working(userid_t userid, uint32_t in_count, uint32_t *p_in_list)
{
	
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << *(p_in_list + i);
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "update %s set work = 0 where userid = %u and id in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_piglet::get_some_piglet_for_work(userid_t userid, uint32_t in_count, user_piglet_melt_ore_in_item *p_in_item, uint32_t **p_list, uint32_t *out_count)
{
	
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->piglet_index;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select energy from %s where userid = %u and shift = 2 and work = 0 and id in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_list, out_count);
		INT_CPY_NEXT_FIELD(*(*p_list + i));
	STD_QUERY_WHILE_END();
}


int Cuser_piglet::get_some_piglet_itemid(userid_t userid, uint32_t in_count, user_get_piglet_mining_info_out_item *p_in_item, 
		piglet_item_t **pp_list, uint32_t *out_count)
{
	
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->piglet_id;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select id, pigletid from %s where userid = %u and shift = 2 and work = 3 and id in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, out_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->index);
		INT_CPY_NEXT_FIELD((*pp_list + i)->piglet_itemid);
	STD_QUERY_WHILE_END();
}



int Cuser_piglet::get_special_piglets(userid_t userid, uint32_t shift, uint32_t *values)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and shift = %u",
			this->get_table_name(userid),
			userid,
			shift
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*values);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet::check_piglets_whether_work(uint32_t userid, uint32_t count, user_piglet_melt_ore_in_item *p_in_item, 
				uint32_t *temp_state, uint32_t work_type, uint32_t other_flag)
{
	uint32_t *p_tmp = 0;
	uint32_t ret_cnt = 0;	
	get_some_piglet_for_work(userid, count, p_in_item, &p_tmp, &ret_cnt);
	if(ret_cnt < count){
		*temp_state = 0;
	}
	else{
		if(other_flag == 0){
			uint32_t k = 0;
			for(; k< ret_cnt; ++k){
				if(*(p_tmp+k) < 10)//目前假设需要能量10
				{
					*temp_state = 0;
					break;
				}
			}
			if(k >= ret_cnt){
				*temp_state = 1;
			}

		}
		else{
			if(work_type == 1){
				uint32_t k = 0;
				for(; k< ret_cnt; ++k){
					if(*(p_tmp+k) < 50)//目前假设需要能量50
					{
						*temp_state = 0;
						break;
					}
				}
				if(k >= ret_cnt){
					*temp_state = 1;
				}

			}
			else{
				uint32_t k = 0;
				for(; k< ret_cnt; ++k){
					if(*(p_tmp+k) < other_flag)//目前假设需要能量50
					{
						*temp_state = 0;
						break;
					}
				}
				if(k >= ret_cnt){
					*temp_state = 1;
				}

			}
		}
	}
	if(p_tmp != 0){
		free(p_tmp);
	}
	return SUCC;
}

int Cuser_piglet::start_working( userid_t userid, user_piglet_melt_ore_in_item *p_in_item, 
		uint32_t in_count, uint32_t need_energy)
{
	std::ostringstream in_str;
    for(uint32_t i = 0; i < in_count; ++i){
        in_str << (p_in_item + i)->piglet_index;
        if(i < in_count - 1){
            in_str << ',';
        }
    }
	if(need_energy == 0){
		need_energy = 10;
	}
	sprintf(this->sqlstr, "update %s set work = 1, energy = if(energy >= %u, energy - %u, energy) \
			where userid = %u and id in (%s)",
            this->get_table_name(userid),
			need_energy,
			need_energy,
            userid,
            in_str.str().c_str()
            );

    STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);	
	
}
