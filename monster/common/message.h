/*
 * =====================================================================================
 *
 *       Filename:  message.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月12日 14时42分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_MESSAGE_H_20110712
#define H_MESSAGE_H_20110712

//与AS 通信的协议
enum
{
    as_msg_acquire_online           = 110, //as向登录服务器拉取一个可用的online服务器

    msg_reload_conf_file   = 1000, //发送给服务器的用于重读配置文件的协议

    online_msg_login_user	    = 40001, //online告知share-server有用户登录 (无需应答)
    as_msg_login_user           = 40002, //as向share-server请求最近登录用户信息
    as_msg_user_info		    = 40003, //as向share-server请求用户信息，用于分享个人房间

    as_msg_check_online             = 40100, //检测online是否处于死循环的协议，由公司统一的平台发过来(网络序)
    as_msg_acquire_userinfo         = 40101, //as向online服务器发注册请求
    as_msg_login_online             = 40102, //as向online服务器发起登录请求
    as_msg_bag_stuff                = 40103, //as向online拉取背包中的物品
    as_msg_room_info                = 40104, //as向online拉取房间信息
    as_msg_room_buf                 = 40105, //as向online更新房间buf
    as_msg_room_num                 = 40106, //as向online拉取房间个数
    as_msg_buy_stuff                = 40108, //as向online购买商品
    as_msg_get_puzzle               = 40109, //as向online拉取益智游戏信息
    as_msg_commit_puzzle            = 40110, //as向online提交益智游戏答题结果
    as_msg_get_friend_list          = 40111, //as向online拉取好友列表
    as_msg_get_user_info            = 40112, //as向online拉取单个用户信息
    as_msg_friend_op                = 40113, //as向online对好友进行操作
    as_msg_find_friend              = 40114, //as向online查找好友
    as_msg_add_friend               = 40115, //as向online添加好友
    as_msg_get_friend_apply         = 40116, //as向online拉取好友申请
    as_msg_friend_apply_op          = 40117, //as向online对好友申请的操作
    as_msg_get_pinboard             = 40118, //as向online拉取留言板信息
    as_msg_add_message              = 40119, //as向online新增一条留言
    as_msg_update_message_status    = 40120, //as向online更新留言的状态
    as_msg_rating_room              = 40121, //as向online顶用户的小屋
    as_msg_get_plant_info           = 40122, //as向online拉取种植园信息
    as_msg_grow_plant               = 40123, //as向online种种植园植物
    as_msg_maintain_plant           = 40124, //as向online维护种植园植物
    as_msg_attract_pet              = 40125, //as向online操作吸引到的小怪兽
    as_msg_get_stranger             = 40126, //as向online获得商业街上的陌生人
    as_msg_commit_game              = 40127, //as向online提交连连看结果
    as_msg_eat_food                 = 40128, //as向online提交怪兽吃食物
    as_msg_update_profile           = 40129, //as向online修改用户的profile信息
    as_msg_shop_item                = 40130, //as向online拉取商店物品列表
    as_msg_update_flag              = 40131, //as向online更新新手引导标志位
    as_msg_trade_stuff              = 40132, //as向online兑换物品
    as_msg_get_all_pet              = 40133, //as向online获取精灵园的所有精灵
    as_msg_enter_two_puzzle         = 40134, //as向online发出进入二人益智游戏请求
    as_msg_start_two_puzzle         = 40135, //as向online发出点击start按钮请求
    as_msg_answer_two_puzzle        = 40136, //as向online发出答题请求
    as_msg_interactive_element      = 40137, //as向online发出互动元素抽奖请求
    as_msg_stat_data                = 40138, //as向online发统计数据
    as_msg_latest_visit             = 40139, //as向online获得最近访问用户
    as_msg_get_all_badge            = 40140, //as打开成就面板，请求所有的成就项
    as_msg_acquire_badge_reward     = 40141, //as领取成就项奖杯
    as_msg_get_real_message         = 40142, //as向online查询用户实际可见的留言条数
    as_msg_get_unread_visit         = 40143, //as向online查询用户未读访客数量
    as_msg_get_game_level           = 40144, //as向online请求某个小游戏的关卡信息

    as_msg_get_factory              = 40145, //as向online请求合成工厂状态
    as_msg_create_stuff             = 40146, //as向online提交合成请求
    as_msg_cancel_create            = 40147, //as向online取消合成
    as_msg_dirty_word               = 40148, //as向online检测名字是否是脏词
    as_msg_modify_name              = 40149, //as向online修改怪兽名字
    as_msg_get_random_name            = 40151, //as向online获得随机的用户名字
    as_msg_get_bobo_reward          = 40152, //as向online领取卜卜报奖励
    as_msg_get_sun_reward           = 40153, //as向online领取阳关奖励包
    as_msg_read_bobo_newspaper      = 40154, //as向online发读取卜卜报协议
    as_msg_enter_npc_score          = 40155, //as向online发送进入大众点评协议
    as_msg_npc_score                = 40156, //as向online请求npc评分
    as_msg_get_game_change          = 40157, //as向online请求小游戏兑换信息
    as_msg_game_change              = 40158,//as向online发出兑换小游戏物品协议
    as_msg_get_finished_task        = 40159,//as向online拉取已完成任务列表
    as_msg_finish_task              = 40160,//as告知online完成一个任务
    as_msg_donate_info              = 40161, //as告知online进入援助计划面板
    as_msg_donate                   = 40162, //as告知online捐款
    as_msg_history_donate           = 40163,//as向online请求已经举办过的援助计划概况信息
	as_msg_enter_museum				= 40164,//as向online请求进入博物馆
	as_msg_get_museum_reward		= 40165,//as向online请求领取博物馆奖励
	as_msg_answer_museum_end		= 40166, //as告知online博物馆答题结束
    as_msg_commit_game_change       = 40167, //as提交蛋糕店游戏的分数信息
    as_msg_update_guide_flag       = 40168, //更新强制引导标识
    as_msg_get_a_activity_info      = 40169 ,//拉取单个活动的信息
    as_msg_get_activity_reward   = 40170,//领取单个活动的奖品
    as_msg_get_cur_activity     = 40171,//拉取当前的所有活动列表
	as_msg_get_activity_reward_ex = 40172,
	as_msg_prize_lottery         = 40173,
	as_msg_enter_show				=40174,   //进入大众点评
	as_msg_join_show					=40175, //参与大众点评
	as_msg_guess_show					=40176,//投票给大众点评候选人
    as_msg_history_show         = 40177,//拉取往届大众点评结果信息

    as_msg_friend_apply             = 42000, //online通知as有好友请求
    as_msg_close_conn               = 42001, //online定时广播，关掉断开的连接
    as_msg_level_up                 = 42002, //online通知as用户升级
    as_msg_match_two_puzzle         = 42003, //online通知AS二人益智游戏匹配信息
    as_msg_begin_two_puzzle         = 42004, //online通知as开始二人益智游戏比赛
    as_msg_opp_answer_two_puzzle    = 42005, //online通知as对手的答题信息
    as_msg_two_puzzle_result        = 42006, //online通知as二人益智游戏比赛结果
    as_msg_new_visit                = 42007, //online通知as有新的访客消息
    as_msg_new_badge                = 42008, //online通知as有新的成就项
    as_msg_encourage_guide          = 42009, //online通知as鼓励引导
    as_msg_notice_new_activity      = 42010, //通知有新的活动进展
    switch_msg_add_friend           = 43001, //switch通知的online增加好友信息到缓存
    ucount_msg_add_visit            = 43002, //向ucount查询有没有访问别人家
    multi_puzzle_add_coin           = 43003, //益智游戏多人挑战增加金币
    badge_update_status             = 43004, //成就项状态更改
    add_new_unlock_map              = 43005, //添加新解锁的物品
    activity_update_status          = 43006, //公测活动更新状态阿,坚持
};

/**
 * @brief 与account服务器通信的协议
 */
enum
{
    account_msg_login_verify = 0xA026,    // 从account验证账户
    account_msg_check_session = 0xA024,   // 检查用户的session
    account_msg_get_verify_img = 0xA027,  // 获取验证码
    account_msg_active_user = 0xA128,     // 激活用户
    account_msg_is_active = 0xA029,       // 用户是否已经激活
    account_msg_is_mifan = 0x0060,        // 用户是否是米饭
};

//后台服务内部的通信协议
enum
{
    svr_msg_query_switch_status         = 1000,  //通过终端给switch发协议，请求switch内部的参数值
    svr_msg_query_ip                    = 1001,  //通过终端给switch发协议，请求查看指定ip上的online信息
    svr_msg_query_user                  = 1002,  //通过终端给switch发协议，请求查看用户在switch上的信息

    svr_msg_acquire_online              = 50001,  //login向switch请求一个可用的online服务器

    svr_msg_online_keepalive            = 50100, //online告知switch和multi自己时活跃的
    svr_msg_online_register             = 50101,  //online向switch以及multi发起注册请求
    svr_msg_login                       = 50102,  //用户登录online,告知switch
    svr_msg_logout                      = 50103,  //用户登出online,告知switch以及multi-server
    svr_msg_switch_friend_apply         = 50104,  //online告知switch有好友请求
    svr_msg_get_active_user             = 50105,  //online从switch出获取活跃用户id
    svr_msg_get_shop_item               = 50106,  //online向switch获取商店物品
    svr_msg_online_exit                 = 50107, //online告知switch以及multi自己退出
    svr_msg_online_encourage            = 50108, //小屋评分 每日挑战 引导


    svr_msg_kick_user                   = 50201,  //switch告知online将用户踢下线
    svr_msg_online_friend_apply         = 50202,  //switch告知online有好友请求

    svr_msg_ucount_visit                = 50301,  //online告知ucount访问数去重
    svr_msg_ucount_thumb                = 50302,  //online告知ucount评分去重
    svr_msg_ucount_latest_visit         = 50303,  //online向ucount请求最近访客
    svr_msg_get_unread_count            = 50304,  //online向ucount请求未读消息的条数

    svr_msg_online_multi_enter          = 50401,  //online告知multi-server用户进入二人益智游戏
    svr_msg_multi_online_match          = 50402,  //multi-server推送二人益智游戏匹配信息给online
    svr_msg_online_multi_start          = 50403,  //online告知multi-server用户点击start按钮
    svr_msg_multi_online_start          = 50404, //multi-server推送进入开始游戏信息给online
    svr_msg_online_multi_answer         = 50405, //online告知multi-server用户答题信息
    svr_msg_multi_online_answer         = 50406, //multi-server推送对手答题信息给online
    svr_msg_multi_online_result         = 50407, //multi-server推送比赛结果信息给online

    svr_msg_get_maiji_pinboard          = 50501, //online同db-cache-server的通信，获取麦咭的留言板
    svr_msg_join_show                   = 50502,//online告知db-cache-server 用户报名参加大众点评
	svr_msg_vote_show 					= 50503, //大众点评，投票
	svr_msg_history_show				= 50504,//往届大众点评
	svr_msg_enter_show					= 50505,//进入大众点评


    svr_msg_db_add_role                 = 32768,  //0x8000新增一个role
    svr_msg_db_role_info                = 32769,  //0x8001查询用户的role信息
    svr_msg_db_get_bag_stuff            = 32770,  //0x8002拉取背包中的物品
    svr_msg_db_add_bag_stuff            = 32771,  //0x8003背包中增加物品
    svr_msg_db_update_bag_stuff         = 32772,  //0x8004更新背包中物品数量,该协议未使用
    svr_msg_db_room_num                 = 32773,  //0x8005拉取用户的房间数量
    svr_msg_db_room_info                = 32774,  //0x8006拉取单个房间信息
    svr_msg_db_add_room                 = 32775,  //0x8007增加一个房间
    svr_msg_db_update_role              = 32776,  //0x8008更新role表中的last_login_time
    svr_msg_db_update_room              = 32777,  //0x8009更新房间buf
    svr_msg_db_get_puzzle_info          = 32778,  //0x800a拉取益智游戏信息
    svr_msg_db_commit_puzzle            = 32779,  //0x800b更新用户完成益智游戏信息
    svr_msg_db_add_role_value           = 32780,  //0x800c增加用户role表里的字段值
    svr_msg_db_get_friend_id            = 32781,  //0x800d拉取好友id列表
    svr_msg_db_apply_for_friend         = 32782,  //0x800e新增一条好友申请
    svr_msg_db_get_friend_apply         = 32783,  //0x800f拉取好友申请信息
    svr_msg_db_set_friend_status        = 32784,  //0x8010设置好友状态信息
    svr_msg_db_get_friend_info          = 32785,  //0x8011拉取单个好友的基本信息
    svr_msg_db_get_pinboard_info        = 32786,  //0x8012拉取留言板信息
    svr_msg_db_add_message              = 32787,  //0x8013新增一条留言
    svr_msg_db_update_message_status    = 32788,  //0x8014更改一条留言的状态(包括删除)
    svr_msg_db_del_friend               = 32789,  //0x8015删除一个好友
    svr_msg_db_pet_op                   = 32790,  //0x8016增加删除小怪兽的操作
    svr_msg_db_add_plant                = 32791,  //0x8017新增一颗植物
    svr_msg_db_del_plant                = 32792,  //0x8018删除一颗植物
    svr_msg_db_update_plant             = 32793,  //0x8019更改植物的成长值并更新更改时间
    svr_msg_db_interactive              = 32794, //0x801a互动元素获取奖励
    svr_msg_db_get_day_restrict         = 32795,  //0x801b获得天限制表的值
    svr_msg_db_add_day_restrict         = 32796,  //0x801c增加天限制表的值
    svr_msg_db_get_profile              = 32797,  //0x801d获得用户的profile信息
    svr_msg_db_eat_food                 = 32798,  //0x801e怪兽吃食物
    svr_msg_db_modify_sign              = 32799,  //0x801f修改用户个性签名
    svr_msg_db_other_role_info          = 32800,  //0x8020查询其他用户的role信息
    svr_msg_db_update_login             = 32801,  //0x8021更改用户登录时的信息
    svr_msg_db_get_all_pet              = 32802,  //0x8022获得精灵园的所有精灵
    svr_msg_db_get_real_message_count   = 32803,  //0x8023获得用户实际可见的留言条数
    svr_msg_db_get_all_badge            = 32804,  //0x8024 获得所有的成就项
    svr_msg_db_set_unread_badge         = 32805, //0x8025告知db将未读的成就数置0
    svr_msg_db_update_badge             = 32806, //0x8026告知db更改成就项
    svr_msg_db_compose_stuff            = 32807, //0x8027告知db合成物品
    svr_msg_db_add_game_day_restrict    = 32808, //0x8028告知db更改小游戏的天限制
    svr_msg_db_get_game_day_restrict    = 32809, //0x8029从db获得小游戏的天限制
    svr_msg_db_add_game_level           = 32810,//0x802a更新小游戏的关卡信息
    svr_msg_db_get_game_level           = 32811, //0x802b获取小游戏的管卡信息
    svr_msg_db_modify_monster_name      = 32812, //0x802c修改怪兽的名字
    svr_msg_db_get_hole_reward          = 32813, //0x802d 获得所有种植园坑的阳光奖励
    svr_msg_db_maintain_plant           = 32814, //0x802e 维护种植园植物
    svr_msg_db_get_plant                = 32815, //0x802f 收获植物
    svr_msg_sun_reward_happy            = 32816, //0x8030 阳关奖励 奖励愉悦值
    svr_msg_db_sun_reward_exp           = 32817, //0x8031 阳关奖励 奖励经验值
    svr_msg_db_sun_reward_coins         = 32818, //0x8032 阳关奖励 奖励金币
    svr_msg_db_get_npc_score            = 32819, //0x8033 获取小屋评分
    svr_msg_db_set_npc_score            = 32820, //0x8034 设置小屋评分
    svr_msg_db_get_game_changed_stuff  = 32821, //0x8035 获取小游戏当前期已经兑换过的物品
    svr_msg_db_set_game_changed_stuff  = 32822, //0x8036 获取小游戏当前期已经兑换过的物品
    svr_msg_db_get_finished_task       = 32823, //0x8037 获取已完成任务列表
    svr_msg_db_finish_task             = 32824, //0x8038 完成某个任务
    svr_msg_db_factory_op              = 32825, //0x8039 合成工厂领取合成的物品

	svr_msg_db_enter_museum			   = 32826, //0x803a进入博物馆，读取关卡信息
	svr_msg_db_get_museum_reward	   = 32827, //0x803b领取博物馆关卡奖品
	svr_msg_db_commit_museum_game 	   = 32828, //0x803c完成博物馆答题

    svr_msg_db_request_update_wealth  = 32829, //0x803d //捐助计划请求数据库更新用户的金币值或者物品

    svr_msg_db_get_activity_reward  =  32830, //0x803e获取单个活动已经领取过的奖品
	svr_msg_db_get_open_reward = 32831, //领取公测活动奖品
    svr_msg_update_activity_status = 32832,//更新公测活动的奖品状态呀
	svr_msg_db_get_invite_info = 32833,
	svr_msg_db_request_prize_lottery = 33834,
	svr_msg_db_get_activity_reward_ex = 33835,
	svr_msg_db_update_level_relative = 33836,
	svr_msg_db_get_dragon_boat_info = 33837,
	svr_msg_db_update_dragon_boat = 33838,


//以下消息id不分库分表处理,从0x8200开始
    svr_msg_db_insert_name              = 33280,  //0x8200插入用户名字对应的米米号
    svr_msg_db_search_name              = 33281,  //0x8201通过名字查找对应的米米号
    svr_msg_db_select_donate            = 33282, //0x8202 查看当前是第几期模卷计划
    svr_msg_db_select_history_donate    = 33283, //0x8203查看历届募捐计划
    svr_msg_db_donate_coins             = 33284, //0x8204 捐款
    svr_msg_db_encourage_guide       = 33285,//0x8205 查看击败人数

    svr_msg_db_request_history_donate = 33286, //0x8026 查看历届募捐计划
    svr_msg_db_request_cur_donate = 33287, //0x8027 查看当前届援助计划
    svr_msg_db_request_donate_coins = 33288, //0x8028 捐助金币
	svr_msg_db_request_enter_show 	=33289,
	svr_msg_db_request_join_show 		=33290,
	svr_msg_db_request_guess_show	 	=33291,


    svr_msg_service_role_info           = 0x8400, //客服查询用户角色信息
    svr_msg_service_friends_info        = 0x8401,//客服查询用户好友信息
    svr_msg_service_badge_info          = 0x8402, //客服查询用户勋章信息
    svr_msg_service_pinborad_info       = 0x8403, //客服查询用户留言板信息
    svr_msg_service_stuff_info          = 0x8404, //客服查询用户的道具信息
    svr_msg_service_game_info           = 0x8405, //客服查询用户小游戏关卡信息
    svr_msg_service_puzzle_info         = 0x8406, //客服查询用户益智游戏信息

    svr_msg_boke_role_info              = 0x8500, //博客系统查询用户角色信息
};

//错误码
enum
{
    //没有错误
    ERR_NO_ERR  = 0,

    //10001 -- 19999 为后台系统错误
    //10001 -- 100100 为整个系统各模块的都可能出现的错误码
    ERR_SNDBUF_OVERFLOW = 10001,        //发送缓冲区溢出
    ERR_NO_AVAILABLE_ONLINE = 10002,    //暂时没有可用的online服务器
    ERR_INVALID_IP  = 10003,            //无效的IP地址
    ERR_USER_ID = 10004,                //米米号不合法
    ERR_MSG_LEN = 10005,                //接收到的消息长度错误
    ERR_MSG_TYPE = 10006,               //消息类型错误
    ERR_SYSTEM_ERR  = 100007,           //未知的系统错误
    ERR_REPEAT_LOGIN    = 100008,       //重复登录到同一台online
    ERR_LOGIN_AT_OTHER  = 100009,       //在其他online登录
    ERR_USER_UNONLINE   = 100010,       //用户已经下线
    ERR_MSG_TIMEOUT = 100011,           //请求服务端超时
    ERR_NET_ERROR   = 100012,           //网络错误 用于同db_proxy同db_server通信
    ERR_REPEAT_REQUEST  = 100013,       //重复请求
    ERR_LOGIN_VERIFY_FAILED = 100014,   //米米号密码没通过account验证
    ERR_LOGIN_VERIFY_SESSION = 100015,  //登录用户的session不能通过account的验证
    ERR_LOGIN_SESSION_FAILED = 100016,  //未通过session验证的请求
    ERR_GET_VERIFY_FAILED = 100017,     //获取验证码失败
    ERR_SQL_ERR     = 100018,           //mysql错误
    ERR_SQL_NO_RECORD     = 100019,     //mysql里没有对应的记录
    ERR_ALLOC_USER_CACHE     = 100020,  //分配用户缓存失败
    ERR_MEMCACHED     = 100021,         //操作memcache失败
    ERR_MEMCACHED_DATA_LENGTH = 100022, //从memcache获得的数据长度不对
    ERR_TOO_MANY_PKG = 100023,          //用户需要处理的包超过限制
    ERR_ALLOC_MEM = 100024,             //分配内存失败
    ERR_NOT_ALLOWED = 100025,           //不允许的客户端连接

    //100201 -- 100300 为同account平台之间的错误码
    ERR_MSG_ACCOUNT = 100201,           //给account平台发送请求失败
    ERR_MSG_IS_ACTIVE = 100202,         //从account获取用户是否激活失败
    ERR_MSG_ACTIVE_USER = 100203,       //从account激活用户失败
    ERR_MSG_IS_MIFAN = 100204,          //从account获得用户是否米饭失败

    //100301-100400为同multi-server之间的错误码
    ERR_MSG_MULTI_SERVER        = 100301, //给multi-server服务发送请求失败
    ERR_REPEAT_ENTER_TWO_PUZZLE = 100302, //重复进入二人益智游戏
    ERR_NOT_MATCHED_USER        = 100303, //未匹配的用户
    ERR_NOT_IN_GAMING_USER      = 100304, //未进入比赛的用户
    ERR_NOT_FOUND_MATCHER       = 100305, //在比赛的用户中无法找到对手,这错误应该时不可能发生的
    ERR_HAVE_FINISHED           = 100306, //已经答题结束的用户发送答题协议过来

    //100401 -- 100500 为同switch平台之间的错误码
    ERR_MSG_SWITCH = 100401,            //给switch服务发送请求失败

    ERR_MSG_UCOUNT = 100501,            //给唯一数服务发送请求失败

    //100601 -- 100800 为同db-proxy之间的错误码
    ERR_MSG_DB_PROXY = 100601,          //给db-proxy发送请求失败

    ERR_MSG_PARSE_ROOM_BUF = 100801,    //解析用户的房间buf失败
    ERR_ADD_ROOM_BUF = 100802,          //增加用户的房间缓存失败
    ERR_MSG_CACHE_ROOM_BUF = 100803,    //缓存里找不到用户的房间buf
    ERR_MSG_UPDATE_CACHE_ROOM = 100804, //更新缓存里的房间信息失败
    ERR_ADD_BAG_CACHE = 100805,         //增加用户的背包缓存失败
    ERR_MSG_CACHE_BAG = 100806,         //缓存里找不到用户的背包信息
    ERR_MSG_UPDATE_CACHE_BAG = 100807,  //更新缓存里用户的背包信息失败
    ERR_MSG_CHANGE_ROOM_BUF = 100808,   //要更新的buf不能通过验证
    ERR_ADD_ROLE_CACHE = 100809,        //缓存用户的角色信息失败
    ERR_GET_ROLE_CACHE = 100810,        //获得缓存里的用户角色信息失败
    ERR_BUY_STUFF = 100811,             //不符合购买物品的条件限制
    ERR_PUZZLE_NUM = 100812,            //答对的数量大于总共答题的数量
    ERR_PUZZLE_TOO_MUCH = 100813,       //答对的数量大于总共能出现的题目的数量
    ERR_GET_CACHED_PUZZLE = 100814,     //获得缓存里的益智游戏信息失败
    ERR_REPEATLY_DAILY_PUZZLE = 100815, //当天已经玩过每日挑战
    ERR_PUZZLE_TYPE_INVALID = 100816,   //不存在的益智游戏类别
    ERR_MSG_TYPE_INVALID = 100817,      //不存在的留言类型
    ERR_CAN_REPORT_ONLY = 100818,       //不是自己的留言，只能进行举报操作
    ERR_FRIEND_OP = 100819,             //不支持的好友操作
    ERR_FRIEND_REQ_OP = 100820,         //不支持的好友申请操作
    ERR_FRIEND_PAGE_LARGE = 100821,     //拉取的好友页数太多,好友数不够
    ERR_REPEAT_THUMB_ROOM = 100822,     //重复顶用户的小屋
    ERR_PAGE_ZERO = 100823,             //不能拉取第0页
    ERR_MESSAGE_ZERO = 100824,          //留言不能为空
    ERR_MESSAGE_PER_PAGE = 100825,      //每页留言个数大于限制
    ERR_PUZZLE_LEVEL = 100826,          //等级比对应的益智游戏所要求的等级小
    ERR_STUFF_NOT_EXIST = 100827,       //购买的物品id不存在
    ERR_PLANT_EXIST_IN_HOLE = 100828,   //坑上已种过植物，不能重复种
    ERR_PLANT_NOT_EXIST = 100829,       //铲除没有种植物的坑
    ERR_NO_ATTRACT_PET = 100830,        //没有吸引到小怪兽，没有进行小怪兽的操作
    ERR_ATTRACT_PET_OP = 100831,        //没有定义的对吸引到小怪兽的操作
    ERR_ADD_PLANT_GROWTH = 100832,      //修改植物的成长值失败
    ERR_ROLE_NOT_EXISTS = 100833,       //用户没有注册游戏
    ERR_APPLY_FOR_FRIEND_AGAIN = 100834,//重复申请好友
    ERR_NOT_ENOUGH_SEED = 100835,       //没有足够的种子种植
    ERR_UPDATE_ROOM = 100836,           //保存用户更新房间信息失败
    ERR_GET_FRIEND_NUM = 100837,        //从缓存获得好友数量失败
    ERR_MAX_FRIEND_NUM = 100838,        //好友数量已到达上限
    ERR_CACHE_FRIEND_REQ = 100839,      //操作好友请求缓存信息失败
    ERR_PINBORAD_PAGE = 100840,         //留言板一页的留言数超过最大值
    ERR_PINBOARD_CACHE = 100841,        //缓存留言板信息失败
    ERR_FRIEND_APPLY_CACHE = 100842,    //缓存好友申请信息失败
    ERR_PLANT_NUM = 100843,             //吸引到小怪兽，但是种植园植物没有种满
    ERR_ADD_SELF_FRIEND = 100844,       //不能把自己加为好友
    ERR_ITEM_NOT_EXIST = 100845,        //使用的物品不存在
    ERR_ITEM_NOT_SEED = 100846,         //种植园种的不是种子
    ERR_ITEM_NOT_FOOD = 100847,         //吃的物品不是食物
    ERR_ALREADY_FRIEND = 100848,        //已经是好友，不能再添加
    ERR_SHOP_NOT_EXIST = 100849,        //放置随机产生物品的商店不存在
    ERR_PET_FULL = 100850,              //跟随的精灵已满
    ERR_HOLE_NOT_EXIST = 100851,        //植物要种的坑不存在
    ERR_GAME_TYPE = 100852,             //非法的小游戏id
    ERR_NOT_ENOUGH_FOOD = 100853,       //食物数量不够
    ERR_GET_SHOP = 100854,              //生成商店物品出错
    ERR_NOT_ENOUGH_STUFF = 100855,      //没有足够的物品兑换
    ERR_DIRTY_WORDS = 100856,           //不能含有脏词
    ERR_NOT_EXIST_INTERACTIVE   = 100857, //错误的互动元素编号
    ERR_TOO_MANY_STUFF   = 100858,      //物品数量超过上限
    ERR_NOT_TIME_GET_COMPOSE = 100859,  //没有到达领取合成物品的时间
    ERR_NOT_UNLOCK_LEVEL = 100860, //发送未解锁的关卡答题信息
    ERR_NOT_ENOUGH_material = 100861, //没有足够的合成材料
    ERR_HAS_GET_BOBO_REWARD = 100862, //已领取过卜卜报奖励
    ERR_BOBO_NOT_READ = 100863, //没有阅读过卜卜报，不能领奖
    ERR_NPC_ALREADY_SCORE = 100864, //npc当天已打过分
    ERR_HOLE_ID     = 100865, //错误的坑编号
    ERR_MAINTAIN_TYPE = 100866, //错误的维护类型
    ERR_MAINTAIN_TYPE_NOT_CONSISTENT = 100867, //发过来的维护类型与服务器记录的维护类型不一致
    ERR_REWARD_ID   = 100868,//错误的奖励id
    ERR_PET_ID      = 100869, //客户端传过来错误的精灵id
    ERR_PET_TOO_MANY = 100870, //拥有的精灵数量大于等于3个，删除一个才可以继续收养
    ERR_SCORED_TODAY = 100871, //今天已经评过了
    ERR_NO_THIS_GAME = 100872, //没有这个游戏
    ERR_STUFF_CHANGED = 100873, //已经兑换过这个物品
    ERR_TASK_LEVEL    = 100874, //任务等级条件未达到
    ERR_TASK_PRE_MISSION = 100875, //任务前置任务未完成
    ERR_DENOTE_ID       = 100876, //发过来的捐款id与数据库中记录的不符合
    ERR_HAVE_MAP_IN_FACTORY = 100877, //有物品正在合成，不可以提交合成信息
    ERR_CANNOT_REQUEST_NONDEP = 100878, //版署的号不可以请求非版署号
    ERR_CANNOT_REQUEST_DEP    = 100879, //用户不可以请求版署号的信息
    ERR_NOT_CURRENT_ACTIVITY = 100880, //请求的不是当前时间段内的活动
	ERR_NOT_REACH_REWARD_CONDITION = 100881, //没有满足活动领奖条件
    ERR_ENCOURAGE_TYPE = 100882, //错误的引导类型
    ERR_REQ_SHOW_ID    = 100883,//请求的大众点评届数不正确
    ERR_NOT_PHASE_ENTER = 100884, // 不是大众点评的报名阶段
    ERR_NOT_PHASE_VOTE = 100885,//不是大众点评的投票阶段
    ERR_NPC_NOT_SCORED = 100886, //小屋今天没有参与评分
    ERR_REPEAT_JOIN = 100887,//已经报过名
    ERR_REPEAT_VOTE = 100888, //已经投过票
    ERR_NOT_CANDIDATE = 100889, //不是候选人

};



#endif // H_MESSAGE_H_20110712
