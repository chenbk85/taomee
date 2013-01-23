#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>	
#include <async_serv/dll.h>
}

#include <map>
#include <set>
#include <list>

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "room.hpp"
#include "map.hpp"
#include "pet.hpp"
#include "mail.hpp"
#include "proto.hpp"
#include "player_extra_info.hpp"
#include "task.hpp"

struct base_room;
struct room_t;
struct battle_grp_t;
struct item_bag;

extern uint32_t player_num;

#define c_bucket_size 200

extern GHashTable*  all_players;


enum module_t
{
    MODULE_BEGIN = 0,
    MODULE_USER = 1<<1,
    MODULE_BAG = 1<<2,
    MODULE_PET = 1<<3,
    MODULE_MAIL = 1<<4,
    MODULE_EQUIP = 1<<5,
    MODULE_FRIEND = 1<<6,
    MODULE_TASK = 1<<7,
    MODULE_PRIZE = 1 <<8,


    MODULE_END
};

typedef struct nick{
	char user_nick[MAX_NICK_SIZE];
}nick_t;

class player_t 
{
public:
	uint16_t    	waitcmd;
    // waitcmd对应的协议信息
    const bind_proto_cmd_t * bind_cmd;
	GQueue*     	pkg_queue;
	int         	fd;
	uint32_t        seqno;
    uint32_t        ret;
	fdsession_t*    fdsess;
	list_head_t 	wait_cmd_player_hook;
	list_head_t 	timer_list;
	// list_head_t     playerhook;
	list_head_t 	maphook;

	void clear_waitcmd()
	{
		waitcmd = 0;	
        bind_cmd = NULL;
	}
public:

    // 标识哪些模块已经载入，具体见module_t
    uint64_t        module;

    bool check_module(uint64_t module)
    {
        return (0 != (this->module & module));
    }

    void set_module(uint64_t module)
    {
        this->module |= module;
    }

public:
	////////战斗服务器句柄//////////////////
	battle_grp_t*   battle_grp;
	bool            check_in_battle()
	{
		return battle_grp != NULL;
	}
public:
	////////包裹///////////////////////////
	item_bag *     bag;
    // 回购包裹
    c_redeem_bag * redeem_bag;
public:
	////////房间//////////////////////////
	room_t			room;
public:
	////////社区地图//////////////////////
	uint32_t get_cur_map_id();
	
	map_line_players* get_cur_map_line();

    void reset_map();

	bool check_in_map();

	bool init_player_map();

	bool final_player_map();

	map_t*          cur_map;
	int32_t		    cur_map_line_index;
	uint32_t        x_pos;
	uint32_t        y_pos;

	uint32_t        last_map_id;
	uint32_t        last_map_x;
	uint32_t        last_map_y;
public:
	///////装备//////////////////////////
	item_ptr        head;
	item_ptr        eye;
	item_ptr        glass;
	item_ptr        body;
	item_ptr        tail;
	item_ptr        suit;

	bool init_player_equip();

	bool final_player_equip();

	bool set_player_equip(item* p_item, uint32_t equip_pos);

	bool check_valid_equip_pos(uint32_t pos);

	item_ptr        get_equip_by_pos(uint32_t body_index);
public:
	////////玩家属性//////////////////////
	uint32_t 		id;
	uint32_t 		server_id;
	uint32_t        role_tm;
	uint32_t        eye_model;
	uint32_t        resource_id;
	char        	nick[MAX_NICK_SIZE];
	uint32_t		forbid_friends_me;

    // 经验计算器，由通用经验计算器导入不同数值而来
	base_exp2level_calculator*  exp2level_calc;
	// 玩家属性集合，包含人物和宠物 和 合并属性
    c_player_attr	*merge_attr;
    extra_info_t    *extra_info;

	bool            init_attr();
	bool            final_attr();
    
    // 获取人物属性
	uint32_t        get_player_attr_value(uint16_t attr_type);
	// 设置人物属性
    void            set_player_attr_value(uint32_t attr_type, uint32_t value);

	void 			add_player_attr(uint32_t attr_type, int value);
    // 人物和精灵的合体属性
	uint32_t 		get_merge_attr_value(uint16_t attr_type);

    void            export_db_user_id(db_user_id_t * p_db_user_id);
    bool            init_extra_info();
    bool            final_extra_info();
	void 			init_player_attr();
public:
	///////一些登陆相关的信息//////////////////
	uint32_t        last_login_tm;
	uint32_t        last_off_line_tm;
    char            session[SESSION_LEN];
public:
    /////// 精灵 //////////////////////////////
    // 背包中的精灵
    pet_vec_t pet;
    // 出战的精灵
    c_pet * fight_pet;
    // 辅助的精灵
    pet_vec_t assist_pet;
public:
	std::map<uint32_t, mail_data> * m_read_mails;
	std::map<uint32_t, mail_data> * m_unread_mails;
public:
	//好友，黑名单
	std::set<uint64_t> friends;
	std::set<uint64_t> blacklist;
public:
	// 任务
	/*! 玩家的任务列表 (全量信息, 包括 buff) */
	tskmap_t		*tskmap;

	/*! 更新 buff 时的cache, 为了保证跟db强一致, 更新的流程如下:
	 * (1) task_setbuff()->task_setbuff_iface() 中把要修改的 buff 复制到 buff_cache 中;
	 * (2) 尝试更新 buff_cache, 如果失败则直接返回客户端失败, 否则到 (3)
	 * (3) 把 buff_cache 发送给db更新 buff;
	 * (4.1) 如果db没有返回或者返回失败, 将直接返回客户端更新失败(此时任务的buff并没有改变)
	 * (4.2) 如果db返回成功, 则用 buff_cache 覆盖掉任务的 buff, 然后返回客户端成功;
	 *
	 * 注意: buff_cache 的 size 必须和 task_t::buff 的 size 一样, 不能大也不能小;
	 */
	char			buff_cache[MAX_TASK_BUFF_LEN];
    uint32_t        cache_tskid;
    /*! 客户端设置buff的时候需要返回设置的step,DB协议不带STEP 所以缓存在这里*/
    uint32_t        cache_step;
public:
    //奖励
    std::map<uint32_t, uint32_t>player_prize;
    /*NOTICE(singku)
     *! 由于奖励项是一个个发送到DB的,DB返回后需要知道奖励的ID和下一个奖励项
     * 所以将奖励的进行状态缓存在这里！因此每个玩家一次只能处理一个奖励，
     * 不能有多个奖励同时进行！否则缓存的状态将会乱掉。所有的奖励都
     * 必须由客户端主动发协议领取,这样通过waitcmd的机制控制一次只有奖励
     * 被处理
    */
    /*获取奖励状态开关, 如果是true 那么表示所有东西的增加都是通过奖励增加的
     * 奖励处理完毕后关闭此开关
     */
    bool process_prize_state;
    /*缓存当前处理的奖励ID*/
    uint32_t cache_prize_id;
    /*缓存的当前处理的可选奖励项pos 奖励完成后清除*/
    uint32_t cache_optional_prize_pos;
    /*缓存的当前处理的奖励项的条目序号 奖励完成后清除*/
    uint32_t cache_prize_seq;
};

player_t* alloc_player(uint32_t uid, fdsession_t* fdsess);

void dealloc_player(player_t* p);

player_t* get_player_by_fd(int fd);

player_t* get_player(userid_t uid);

player_t* add_player(player_t* player);

void del_player(player_t* p);

void init_players();

void final_players();

void free_player(void* p);

#endif

