/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_beauty_piglet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2011 02:08:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_beauty_piglet.h"

Croominfo_beauty_piglet::Croominfo_beauty_piglet(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO", "t_roominfo_beauty_piglet",
			"userid")
{

}


int Croominfo_beauty_piglet::insert(userid_t userid, uint32_t  award_flag, int32_t continuation)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%d)",
			this->get_table_name(userid),
			userid,
			award_flag,
			continuation
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo_beauty_piglet::update(userid_t userid, const char* col, int32_t val)
{
	sprintf(this->sqlstr, "update %s set %s = %d where userid = %u",
			this->get_table_name(userid),
			col,
			val,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Croominfo_beauty_piglet::update_all(userid_t userid, uint32_t award_flag, int continuation)
{
	sprintf(this->sqlstr, "update %s set award_flag = %u, continuation = %d where userid = %u",
			this->get_table_name(userid),
			award_flag,
			continuation,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Croominfo_beauty_piglet::get(userid_t userid, uint32_t *award_flag, int32_t *val)
{
	sprintf(this->sqlstr, "select award_flag, continuation from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*award_flag);
		INT_CPY_NEXT_FIELD(*val);
	STD_QUERY_ONE_END();
}

int Croominfo_beauty_piglet::set_attributes(userid_t userid, int count)
{
	enum{
		CT_VIC_FIFTH   = 0,//连续十五次胜利
		CT_VIC_TEN     = 1,//连续十次胜利
		CT_DEF_FIVE,       //连续五次失败
		CT_VIC_ONE,        //胜利一次
		CT_DEF_ONE         //失败一次
	};
	// 1表示胜利，-1表示失败
	uint32_t award_flag = 0;
	int32_t continuation = 0;

	int ret = get(userid, &award_flag, &continuation);
	uint32_t bit = 0;

	if(ret != SUCC)
	{
		if(count < 0 ){
			bit = 1 << 2*CT_DEF_ONE;
			//*out = CT_DEF_ONE;
		} 
		else{
			bit = 1 << 2*CT_VIC_ONE;
			//*out = CT_VIC_ONE;
		}
		ret = insert(userid, bit, count);	
	}
	else{
		int32_t real_val = continuation;
		//award_flag 字段 0表示不可以领取，1表示可领取，2表示已经领取	
		//award_flag 字段从低位起，每两位为1组，第一组表示连胜15次，第二组表示连胜10次， 第三组表示连败5次， 
		//第四组表示胜利1次， 第五组表示失败1次
		if(count > 0){
			if(continuation == 14){
				bit = 3 << CT_VIC_FIFTH*2;	
				if((bit & award_flag) == 0){
					award_flag |= (1 << CT_VIC_FIFTH*2);
					//*out = CT_VIC_FIFTH;
				}				
				real_val += 1;
			}
			else if(continuation == 9){
				bit = 3 << CT_VIC_TEN*2;
				if((bit & award_flag) == 0){
					award_flag |= (1 << CT_VIC_TEN*2);
					//*out = CT_VIC_TEN;
				}
				real_val += 1;
			}
			else if(continuation <= 0){
				bit = 3 << CT_VIC_ONE*2;
				if((bit & award_flag) == 0){
					award_flag |= ( 1 << CT_VIC_ONE*2);
					//*out = CT_VIC_ONE;
				}
				real_val = 1;
			}
			else{
				real_val += 1;
			}
		}
		else if(count < 0){
			if(continuation == -4){
				bit = 3 << CT_DEF_FIVE*2;
				if((bit & award_flag) == 0){
					award_flag |= (1 << CT_DEF_FIVE*2);
					//*out = CT_DEF_FIVE;
				}
				real_val -= 1;
			}
			else if(continuation >= 0){
				bit = 3 << CT_DEF_ONE*2;
				if((bit & award_flag) == 0){
					award_flag |= (1 << CT_DEF_ONE*2);
					//*out = CT_DEF_ONE;
				}
				real_val = -1;
			}
			else{
				real_val -= 1;
			}
		}
		if(real_val != continuation){
			ret = update_all(userid, award_flag, real_val);
		}		
	}//else
	return ret;
}

int Croominfo_beauty_piglet::set_award(uint32_t userid, uint32_t type, uint32_t *state)
{
	enum CP_SHOW_GET_ACHIEVE_RET
	{
		CSGAR_OK = 0,		//成功
		CSGAR_HAD_GET,		//已经获取
		CSGAR_INCONFORMITY,	//条件不满足
	};

	uint32_t award_flag = 0;
	int continuation = 0;
	int ret = get(userid, &award_flag, &continuation);
	if(ret == SUCC){
		uint32_t bit = (3 << type*2);
		if(((award_flag & bit) >> (type*2)) == 1){
			award_flag &=  ~(1 << type*2);
			award_flag |= (2 << type*2);
			ret = this->update_all(userid, award_flag, continuation);
			*state = CSGAR_OK;
		}
		else{
			*state = CSGAR_INCONFORMITY; 
		}
	}
	else{
		*state = CSGAR_INCONFORMITY; 
	}

	return SUCC;
	
}

int Croominfo_beauty_piglet::get_award_history(userid_t userid, uint32_t *history)
{
	uint32_t award_flag = 0;
	int continuation = 0;
	int ret = get(userid, &award_flag, &continuation);
	if(ret == SUCC){
		for(uint32_t k = 0; k < 4; ++k){
			*(history+k) = ((award_flag >> k*2) & 3); 
		}
	}

	return ret;	
}

int Croominfo_beauty_piglet::get_award_history_whole(uint32_t userid, uint32_t *history, int32_t* continuation)
{
	sprintf(this->sqlstr, "select award_flag, continuation from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*history);
		INT_CPY_NEXT_FIELD(*continuation);
	STD_QUERY_ONE_END();
}

