/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_card.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"

Cuser_card::Cuser_card(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_card","userid")
{ 
}


int Cuser_card::init(userid_t userid,uint32_t lower_count ,user_cardlist *p_in)
{

	char cardlist_mysql[mysql_str_len(sizeof (*p_in) )];
	
	// convert cardlist to string
	set_mysql_string(cardlist_mysql,(char*)(&(p_in->count)), 
			p_in->count*(sizeof(p_in->cards[0]))+4); 

	sprintf( this->sqlstr, "insert into  %s  values (%u,0, 0, 0, %u, 0,0, '%s') ", 
			this->get_table_name(userid), userid,lower_count,cardlist_mysql );
	
	STD_INSERT_RETURN(this->sqlstr, USER_CARD_IS_INITED_ERR);
}

int Cuser_card::getCardGameInfo_db(userid_t userid, user_card_info *p_out)
{
	sprintf( this->sqlstr, "select flag, wincnt, losecnt,lowercnt,logdate,dayexp,  cardlist\
		   	from %s where userid=%u ", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->win_count);
		INT_CPY_NEXT_FIELD(p_out->lose_count);
		INT_CPY_NEXT_FIELD(p_out->lower_count);
		INT_CPY_NEXT_FIELD(p_out->logdate);
		INT_CPY_NEXT_FIELD(p_out->dayexp);
		BIN_CPY_NEXT_FIELD( &(p_out->cardList), sizeof(p_out->cardList));
	STD_QUERY_ONE_END();
}

int Cuser_card::getcard_opt(userid_t userid, user_card_opt_info *p_out)
{
	sprintf( this->sqlstr, "select flag,  wincnt, losecnt, logdate,dayexp  \
		   	from %s where userid=%u ", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->win_count);
		INT_CPY_NEXT_FIELD(p_out->lose_count);
		INT_CPY_NEXT_FIELD(p_out->logdate);
		INT_CPY_NEXT_FIELD(p_out->dayexp);
	STD_QUERY_ONE_END();
}


int Cuser_card::getCardGameInfo(userid_t userid, user_card_info *p_out)
{
	int ret;
	ret=this->getCardGameInfo_db(userid,p_out);
	if (ret==USER_ID_NOFIND_ERR ){
		//没有初始化
		ret=SUCC;			
		memset(p_out,0,sizeof(user_card_info));
	}else{
		//设置超级卡片个数
		p_out->super_count=p_out->cardList.count-30;		
		if ( p_out->super_count>30) return DB_DATA_ERR;
		for(uint32_t i=0;i<p_out->super_count;i++){
			p_out->cardList.cards[p_out->lower_count+i]= 
				p_out->cardList.cards[CARDLIST_INIT_LEN+i];
		}
		p_out->cardList.count=p_out->lower_count+p_out->super_count;			
	}
	return ret;
}
int Cuser_card::getCardGameInfo_ex(userid_t userid,user_card_getinfo_out *p_out)
{
	int ret;
	user_card_info info;
	ret=this->getCardGameInfo(userid, &info);
	if(ret!=SUCC) return ret;
						
	p_out->flag=info.flag;	
	p_out->lose_count=info.lose_count;	
	p_out->win_count=info.win_count;	
	p_out->lower_count=info.lower_count;	
	p_out->super_count=info.super_count;	
	p_out->cardList.count=info.cardList.count;
	memcpy(&(p_out->cardList.cards[0]),&(info.cardList.cards[0]),
			sizeof(info.cardList.cards[0])*info.cardList.count);
	return SUCC;
}



int Cuser_card::addCard(userid_t userid, uint32_t cardid)
{
	user_cardlist cardlist;
	int			ret;

	ret = this->getCardList(userid, &cardlist);
	if (ret!=SUCC) 
		return ret;

	ret = add_id_to_list((idlist*)&cardlist, cardid, CARDLIST_MAX );
	if ( ret != SUCC ) {
		if 	( ret == LIST_ID_MAX_ERR ) 
			return USER_CARDLIST_COUNT_MAX_ERR;
		else if ( ret == LIST_ID_EXISTED_ERR ) 
			return USER_CARDID_EXISTED_ERR;
		else
			return ret;
	}
	return this->setCardList(userid, &cardlist);
}

int Cuser_card::check_id(userid_t userid, uint32_t id, uint32_t *p_isexisted )
{
	user_cardlist cardlist;
	int			ret;
	ret = this->getCardList(userid, &cardlist);
	if (ret!=SUCC) 
		return ret;
	*p_isexisted=(uint32_t)check_id_in_list( (idlist*)&cardlist,id );

	return SUCC;


}

//
int Cuser_card::addWinLose(userid_t userid, uint32_t flag)
{
	user_card_opt_info opt_info;
	int ret;
	ret=this->getcard_opt(userid, &opt_info );
	if (ret!=SUCC) return ret; 

	uint32_t add_exp=0;
	if (flag==CARDGAME_SET_WIN){
		add_exp=WIN_BONUS;
		opt_info.win_count++;
	}else if (flag==CARDGAME_SET_LOSE) {
		add_exp=LOST_BONUS;
		opt_info.lose_count++;
	}else{
		return  ENUM_OUT_OF_RANGE_ERR;
	}
	//检查数字
	uint32_t tmp_all_value;		

	ret=day_add_do_count(&(opt_info.logdate),&(opt_info.dayexp),&tmp_all_value,
		  get_date(time(NULL)),add_exp,MAX_CARD_EXP_DAY	);
	if (ret==FAIL){//超过限制
		if (opt_info.dayexp>=MAX_CARD_EXP_DAY){
			return USER_CARD_MAX_EXP_DAY_ERR;
		}else{
			opt_info.dayexp=MAX_CARD_EXP_DAY;	
		}
	}
	return   this->update_win_lose(userid,&opt_info );

}

int Cuser_card::getCardList_db(userid_t userid, user_cardlist *p_cardlist)
{
	sprintf( this->sqlstr, "select cardlist \
		   	from %s where userid=%u ", 
			this->get_table_name(userid), userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_cardlist, sizeof(user_cardlist));
	STD_QUERY_ONE_END();
}

int Cuser_card::getCardList(userid_t userid, user_cardlist *p_cardlist)
{
	int ret;
	ret=this->getCardList_db(userid,p_cardlist );
	return ret;
}

//
int Cuser_card::setCardList_db(userid_t userid, user_cardlist *p_cardlist)
{
	char cardlist_mysql[mysql_str_len(sizeof (*p_cardlist) )];
	
	// convert cardlist to string
	set_mysql_string(cardlist_mysql,(char*)(&(p_cardlist->count)), 
			p_cardlist->count*(sizeof(p_cardlist->cards[0]))+4); 

	sprintf( this->sqlstr, "update %s \
		   	set cardlist='%s'\
		   	where userid=%u ", 
			this->get_table_name(userid), cardlist_mysql, userid);
	
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_card::setCardList(userid_t userid, user_cardlist *p_cardlist)
{
	int ret;
	ret=this->setCardList_db(userid,p_cardlist );
	return ret;
}

int Cuser_card::add_lower_card_db(userid_t userid)
{
	sprintf( this->sqlstr, "update %s  set lowercnt=lowercnt+1  where userid=%u ", 
			this->get_table_name(userid),  userid);

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Cuser_card::update_win_lose (userid_t userid,user_card_set_win_lose_in *p_in )
{
	sprintf( this->sqlstr, "update %s  set flag=%u, wincnt=%u, losecnt=%u,\
			logdate=%u,dayexp=%u \
		   	where userid=%u ", 
			this->get_table_name(userid), 
			p_in->flag,
			p_in->win_count,p_in->lose_count,
			p_in->logdate,p_in->dayexp,
			userid );

	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}



int Cuser_card::add_lower_card(userid_t userid,user_card_add_lower_card_out *p_out )
{
	int ret;	
	user_card_info info; 		

	ret=this->getCardGameInfo_db(userid,&info );	
	if(ret!=SUCC){ return ret; }

	uint32_t enable_count;
	enable_count=this->get_enable_count(&info);
	if (enable_count ==0 ){
		return  USER_CARDID_ADD_LOWER_MAX_GRADE_ERR;
	}
			

	ret=this->add_lower_card_db(userid);
	if(ret!=SUCC){ return ret; }

	p_out->cardid=info.cardList.cards[info.lower_count];
	return SUCC;
}

int Cuser_card::get_grade(uint32_t wincnt,uint32_t lost_cnt  )
{
    int scores = WIN_BONUS*(wincnt) + LOST_BONUS*(lost_cnt);
	int ret;
    if (scores < rank1_exp) ret=0;
    else if (scores < rank2_exp) ret=1;
    else if (scores < rank3_exp) ret=2;
    else if (scores < rank4_exp) ret=3;
    else if (scores < rank5_exp) ret=4;
    else if (scores < rank6_exp) ret=5;
    else if (scores < rank7_exp) ret=6;
    else if (scores < rank8_exp) ret=7;
    else if (scores < rank9_exp) ret=8;
    else ret=9;
	return ret;
}

int Cuser_card::get_enable_count(user_card_info*p_info)
{	
	uint32_t count;
	if (p_info->flag&0x01){
		count=30-p_info->lower_count; 
	}else{
		count=12+this->get_grade(p_info->win_count,p_info->lose_count)*2-p_info->lower_count; 
	}
	//越界了.
	if (count>10000000) {
		count=0;
	}
	return count;


}

int Cuser_card::get_enable_count(userid_t userid,user_card_get_enable_count_out *p_out )
{
	user_card_info info; 		
	int ret;
	ret=this->getCardGameInfo_db(userid,&info );	
	if(ret!=SUCC){ return ret; }
	p_out->count=this->get_enable_count(&info);
	return SUCC;
}

