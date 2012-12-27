#ifndef CONF_TEST_HPP_
#define CONF_TEST_HPP_

#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"


#define warning_log(fmt,args...) \
	printf(GREEN"warning: "WHITE""fmt"\n", ##args);

#define error_log(fmt,args...) \
	printf(LIGHT_RED"error:   "WHITE""fmt"\n", ##args);

#define noti_log(fmt,args...) \
	printf(LIGHT_GRAY"notice:  "WHITE""fmt"\n", ##args);







enum item_type {
	noitem = 0,
	it_clothes = 1,
	it_item = 2,
};

struct item_t {
	uint32_t	catid;
	uint32_t	db_catid;
	uint32_t	max;

	uint32_t	id;
	uint32_t	setid;
};

struct task_item_t	{
	uint32_t	give_type;
	uint32_t	give_id;
	uint32_t	give_cnt;
};

struct task_bound_t {
	uint32_t	id;
	uint32_t	item_cnt;
	struct task_item_t	items[10];
};

struct task_t {
	uint32_t	id;
	uint32_t	type;
	uint32_t	need_lv[2];
	struct task_bound_t	in;
	struct task_bound_t	out[5];
};

struct unique_item_pos_t {
	uint32_t item_id;
	uint8_t bit_pos;
	unique_item_pos_t(uint32_t id, uint32_t pos): item_id(id), bit_pos(pos){
	}
};

/*! max rewards count for a single periodical action */
const uint32_t c_max_reward_count = 20;
/*! max costs count for a single periodical action */
const uint32_t c_max_cost_count = 20;
enum {
	flag_pos_daily_active_start = 20,
	max_restr_count = 3000,
};
/*! base type value for weekly action */
//const uint32_t c_weekly_action_type_base = 100000;
/*! base type value for monthly action */
//const uint32_t c_monthly_action_type_base = 200000;
/*! base type value for special wild */
//const uint32_t c_swild_type_base = 10000;

enum {
	daily_active_for_day = 0,
	daily_active_for_week,
	daily_active_for_month,
	daily_active_for_year,
	daily_active_for_ever,
};

enum {
	fumo_top_cannot_reward = 0,
	fumo_top_can_reward	   = 1,
	fumo_top_have_reward   = 2,
};
struct cost_reward_t {
	/*! cost item  */
	uint32_t give_type;
    /*! distinct role if attire */
    uint32_t role_type;
	/*! cost item  */
	uint32_t give_id;
	/*! cost amount */
	uint32_t count;
	/*! odds*/
	uint32_t odds;
} ;


struct restriction_t {
	/*! restriction id */
	uint32_t	id;
	/*! restriction name */
	char		name[20];
	/*! restriction type */
	uint32_t	type;
	/*! user type 0 for role, 1 for userid*/
	uint8_t		userflg;
	uint32_t    dbpos;
	/*! restriction flag: 0 for daily restriction and 1 for weekly one */
	uint32_t	restr_flag;
	/*! restriction top limit in one day or week */
	uint32_t	toplimit;
    /*! limit use level */
    uint32_t    uselv[2];
	/*! restriction vip limit in one day or week */
	uint32_t	vip;
    /*! year_vip limit flag */
	uint8_t	    year_vip;
	/*! time limit range */
	uint32_t	tm_range[2];
	/*! count of costs */
	uint32_t	cost_count;
	/*! costs */
	cost_reward_t	costs[c_max_cost_count];
	/*! count of rewards */
	uint32_t	rew_count;
	/*! is it have odds*/
	uint32_t odds_flg;
	/*! rewards */
	cost_reward_t	rewards[c_max_reward_count];
};




#endif
