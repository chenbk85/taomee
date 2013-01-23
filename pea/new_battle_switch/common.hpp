#ifndef __COMMON_HPP__
#define __COMMON_HPP__


#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <list>
#include <map>
#include <queue>
#include <set>


extern "C" {
#include <assert.h>
#include <glib.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/project/types.h>
#include <libtaomee/utils.h>
#include <libtaomee/project/utilities.h>
}

#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>

#include "pea_common.hpp"


#ifdef	likely
#undef  likely
#endif
#define likely(x) __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x) __builtin_expect(!!(x), 0)

#define DONT_SKIP_NOTIFIER		(0)
#define SKIP_NOTIFIER			(1)

/* 拉取房间列表时, 最多一次遍历500个房间, 还找不到足够的符合条件的房间, 就退出 */
#define MAX_ROOM_LIST_BATCH_LOOPS		(500)


struct player_t;
struct transcript_t;
struct seat_t;
struct team_t;
struct room_t;
struct online_t;



/* 按房间类型, 每个对战方应该分配多少个座位 */
enum team_max_seat_t {
	team_max_seat_pvp		= 4,
	team_max_seat_pve		= 4,
};

enum seat_lock_t {
	/*! 没有被锁住 (可以坐人, 如果没有人在位子上的话) */
	seat_lock_off			= 0,
	/*! 锁住, 不可坐人 (也不可在有人的时候把位子锁上) */
	seat_lock_on			= 1,
};

enum team_no_t {
	/*! 蓝队编号 */
	team_no_blue			= 0,
	/*! 红队编号 */
	team_no_red				= 1,

	max_team_no,
};

enum room_type_t {
	room_type_pvp			= 0,
	room_type_pve			= 1,

	max_room_type,
};

enum room_mode_t {
	/*! 自由模式(不要svr干预): 玩家自己等人一起打 */
	room_mode_free			= 0,
	room_mode_diy			= room_mode_free,

	/*! 竞技模式(要svr干预): 玩家让server帮忙匹配另外房间的人打 */
	room_mode_compete		= 1,
	room_mode_auto			= room_mode_compete,

	max_room_mode,
};

enum room_status_t {
	/*! 空状态 (不参与自动匹配, 也没有在对战中) */
	room_status_idle		= 0,
	/*! 寻找匹配状态 */
	room_status_asking		= 1,
	/*! 对战状态 */
	room_status_fighting	= 2,

	/*! 关闭状态 (等待被回收) */
	room_status_closed		= 3,

	room_status_max,
};

enum player_status_t {
	/*! 此时不可开战: 玩家还没准备好 */
	player_status_idle		= 0,
	/*! 此时可以开战: 玩家已经准备好了 */
	player_status_ready		= 1,
	/*! 此时正在开战: 玩家已经在对战中了 */
	player_status_fighting	= 2,
};




/* 副本, 一个房间同一时刻就指向一个副本 */
struct transcript_t {
	/* 难度 (从 0 开始) */
	uint32_t				maplv;
	/* 副本地图id */
	uint32_t				mapid;
	/* 开启该副本的条件 */
	uint32_t				cond;
};


/* 座位 */
struct seat_t {
	/*! 座位是否被锁住: 0: 没锁, 1: 锁了  */
	uint8_t					lock;
	/*! 座位编号 (0, 1, 2, ...) */
	uint8_t					id;

	/*! 座位上的玩家 (没坐人一定是NULL) */
	player_t				*player;

	/*! 座位所属的房间 (必定不为NULL) */
	team_t					*team;
};
/* <seat_no -> &seat_t> */
typedef map<uint32_t, seat_t*> seat_map_t;
typedef map<uint32_t, seat_t*>::iterator seat_map_iter_t;

/* 战斗方 */
struct team_t {
	/*! 队伍编号 (目前: 0: blue, 1: red) */
	uint8_t					id;
	/*! 队伍里座位数的上限 */
	uint8_t					max_seat;
	/*! 座位 */
	seat_map_t				*seat_map;
	/*! 所属room (必定不为NULL) */
	room_t					*room;
};
/* <team_no -> &team_t> */
typedef map<uint8_t, team_t*> team_map_t;
typedef map<uint8_t, team_t*>::iterator team_map_iter_t;


/* 对战中的房间需要这个结构来维护对战相关的数据 */
struct battle_grp_t {
	/* online 与 btlsvr 连接的 fd 在 online 维护的连接数组中的下标 */
	uint32_t		fd_idx;
	/* btlsvr 分配的对战组id */
	uint32_t		btl_id;

	/*
	 * 参与对战的房间列表 (指针==0表示没有指向房间)
	 * 注意:
	 * (1) 可能由于某房间所有玩家断线导致 room 数组有空洞
	 * (即: 不能找到一个为 0 的 room[i] 就停止搜索, 一定要遍历完)
	 * (2) 没有 battle_grp 的 owner (如果非要找到一个房间的话, 就找第一个有效的room)
	 * (3) btlgrp 中必须至少有一个房间, 当最后一个房间被 del_room 时,
	 * 如果存在btlgrp, 要先把 btlgrp 销毁;
	 */
	room_t			*room[MAX_ROOM_PER_BATTLE];
};
/* <key(fd_idx+btl_id) -> &battle_grp_t> */
typedef map<uint64_t, battle_grp_t*> battle_grp_map_t;
typedef map<uint64_t, battle_grp_t*>::iterator battle_grp_map_iter_t;


/* 房间, TODO(zog): room/team/seat <--> player 的完全图关系链, 让访问更直接,
 * 但建立关系链的指针更多, 要仔细处理(建立指向和解除指向); */
struct room_t {
	/*! 房间id */
	uint32_t				id;
	/*! 创建时间 */
	int32_t					create_time;
	/*! 房间名 */
	char					name[MAX_ROOM_NAME_LEN];
	/*! 房间密码 (pass[0] == '\0' 表示没有密码, TODO(zog): 如果用户输入 0 呢?) */
	char					pass[MAX_ROOM_PWD_LEN];


	/*! 房间类型 (pvp/pve), seeto: room_type_t */
	uint8_t					type;
	/*! 房间模式 (自由等人/svr自动匹配), seeto: room_mode_t */
	uint8_t					mode;
	/*! 房间状态 (idle/asking/fighting);
	 * 注意: 在free模式下, 不允许切换到 asking 状态 */
	uint8_t					status;

	/*! 对战方 (目前就2方: 蓝队, 红队) */
	team_map_t				*team_map;

	/*! 房主 (不允许有空房, del_player 房间里最后人一个的时候, 先del_room);
	 * TODO(zog): 考虑用 closed 标记支持延迟销毁 (即允许较短时间的空房存在) */
	player_t				*owner;
	/*! 所属 online (必定不为NULL) */
	online_t				*online;

	/* TODO(zog): 临时的, transcript 完成后, 就删掉 */
	/* 难度 (从 0 开始) */
	uint32_t				maplv;
	/* 副本地图id */
	uint32_t				mapid;

	/*! 对战组信息, status==fighting时, 必须不空, 否则, 必须空,
	 * start_battle 时创建, end_battle 时销毁 */
	battle_grp_t			*btlgrp;

	/*! 副本 (TODO(zog): 指向只读配置中的某个结构) */
	transcript_t			*transcript;
};
/* <room_id -> &room_t> */
typedef map<uint32_t, room_t*> room_map_t;
typedef map<uint32_t, room_t*>::iterator room_map_iter_t;
typedef map<uint32_t, room_t*>::reverse_iterator room_map_riter_t;


/* TODO(zog): 这三个结构体能否用协议系统里定义的那三个相同的代替? */
struct pmodel_info_t {
	uint32_t				eye_model;
	uint32_t				resource_id;
};
struct proom_pet_info_t {
	uint32_t				pet_id;
	uint32_t				pet_level;
	uint32_t				skill_1;
	uint32_t				skill_2;
	uint32_t				skill_3;
	uint32_t				uni_skill;
};
struct psimple_equip_info_t {
	uint32_t				equip_id;
};
struct player_show_info_t {
	pmodel_info_t				model_info;
	proom_pet_info_t			pet_info;
	psimple_equip_info_t		equip_info[MAX_EQUIP_ON_PLAYER];
};

typedef queue<cached_pkg_t *> cache_pkg_queue_t;
struct player_t {
	/*! 米米号 */
	uint32_t			id;
	/*! 角色注册时间 */
	int32_t				role_tm;
	/*! 昵称 */
	char				nick[MAX_NICK_SIZE];

	/*! 生成实例的时间戳 */
	int32_t				alloc_time;


	/*! 正在处理的cmd */
	uint32_t			waitcmd;
	/*! 流水号 */
	uint32_t			seq;
	/*! 玩家状态, 0: 没准备好, 1: 准备好开战了 */
	uint8_t				status;

	/*! 所在 seat (应与room字段同为 NULL, 或同不为 NULL) */
	seat_t				*seat;

	/*! 所在 online (方便广播消息) */
	online_t			*online;

	/*! 延迟处理的包 (等待 waitcmd 完成处理) */
	cache_pkg_queue_t	*cache_pkg_queue;

	/*! 最后一次收包或发包的时间 (单位: UTC秒) */
	int32_t				last_pkg_time;

	player_show_info_t	show_info;

	/* 注意(zog): 房间中没有多阶级之分, 只有房主和其它人之分, 因此不需要权限字段 */
};
/* 注意: player 在此用 (uid+role_tm) 做key;
 * 当然, 如果用户可以用同一个uid在不同的区(服)同时注册角色,
 * 则他们同时登陆时, 就不能同时进入房间了 */
/* <(uid, role_tm) -> &player_t> */
typedef map<uint64_t, player_t*> player_map_t;
typedef map<uint64_t, player_t*>::iterator player_map_iter_t;

/*
 * 有cache_pkg的player的map;
 * 注意: del_player时, 要通过 KEY(uid+role_tm) 把player从中删除;
 */
typedef map<uint64_t, player_t*> cached_player_map_t;
typedef map<uint64_t, player_t*>::iterator cached_player_map_iter_t;


struct online_t {
	/*! 与 online 的连接 */
	fdsession_t 		*fdsess;
	/*! online id */
	uint32_t			id;

	/*! 处于空状态的房间map (既没开启申请匹配状态, 又不在对战状态) */
	room_map_t			*idle_map;
	/*! 处于寻找匹配状态的map */
	room_map_t			*asking_map;
	/*! 处于对战状态的map */
	room_map_t			*fighting_map;


	/*! PVE 处于空状态的房间map (既没开启申请匹配状态, 又不在对战状态) */
	room_map_t			*pve_idle_map;
	/*! PVE 处于寻找匹配状态的map */
	room_map_t			*pve_asking_map;
	/*! PVE 处于对战状态的map */
	room_map_t			*pve_fighting_map;


	/*! 延迟回收的pvp房间 (这个列表中的房间一定已经没人了) */
	room_map_t			*closed_map;

	/*! 该 online 连过来的所有 player */
	player_map_t		*player_map;
};
/* <online_connfd -> &online_t> */
typedef map<uint32_t, online_t*> online_fd_map_t;
typedef map<uint32_t, online_t*>::iterator online_fd_map_iter_t;
/* <online_id -> &online_t> */
typedef map<uint32_t, online_t*> online_olid_map_t;
typedef map<uint32_t, online_t*>::iterator online_olid_map_iter_t;



extern online_fd_map_t		online_fd_map;
extern online_olid_map_t	online_olid_map;

extern uint8_t team_max_seat[max_room_type];


#endif // __COMMON_HPP__
