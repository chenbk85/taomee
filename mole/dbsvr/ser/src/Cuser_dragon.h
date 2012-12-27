

#ifndef  CUSER_DRAGON_INCL
#define  CUSER_DRAGON_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

typedef struct
{
	uint32_t eggid;
	uint32_t dragonid;
	uint32_t limit;
	char *dragon_name;
}  def_dragon_info;

#define NOT_DRAGON(ID) (ID > 1350010 && ID < 1350111 && ID != 1350043)

class Cuser_dragon : public CtableRoute100x10 {
private:
	char *msglog_file;
public:
	Cuser_dragon(mysql_interface * db); 

	int log_dragon(userid_t userid,uint32_t dragonid,int num);
	uint32_t get_perm_count(uint32_t mole_exp,uint32_t upgrade,uint32_t isvip);
	uint32_t get_item_growth(uint32_t itemid);

	uint32_t get_dragon_id_by_egg(uint32_t egg_id);
	def_dragon_info* get_def_dragon_info(uint32_t dragonid);

	int add(userid_t userid, uint32_t dragonid);
	int del(userid_t userid, uint32_t dragonid);
	int get_dragons(userid_t userid, get_dragon_list_out_item **p_out, uint32_t *p_count);
	int get_dragon_info(userid_t userid, uint32_t dragonid, uint32_t &p_growth, uint32_t &p_state);
	int set_dragon_state(userid_t userid, uint32_t dragonid, uint32_t state);
	int add_dragon_growth(userid_t userid, uint32_t dragonid, uint32_t growth);
	int set_name(userid_t userid, uint32_t dragonid, char *name);
	int get_dragon_count(userid_t userid,uint32_t &count);
	int get_dragon_count_by_isvip(userid_t userid, uint32_t is_vip, uint32_t&count);
	int get_unlock_count(userid_t userid,uint32_t &count);

	int clear_state_2(userid_t userid);
	int clear_state_1(userid_t userid);
	int lock_all_dragons(userid_t userid);
	int add_current_growth(userid_t userid, uint32_t growth);
	int get_current_dragon(userid_t userid, get_current_dragon_out *p_out);
	int get_angel_dragon(userid_t userid, uint32_t angelid, uint32_t &count);
	int set_dragonid(userid_t userid, uint32_t dragonid, uint32_t new_dragonid);
};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

