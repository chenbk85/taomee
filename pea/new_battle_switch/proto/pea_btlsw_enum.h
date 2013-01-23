#ifndef  PEA_BTLSW_ENUM_H
#define  PEA_BTLSW_ENUM_H

/*命令定义*/	
enum  enum_pea_btlsw_cmd{
	 btlsw_online_register_cmd		=	11001, /*online注册*/
	 btlsw_pvp_room_list_cmd		=	11002, /*拉取pvp房间列表*/
	 btlsw_pvp_create_room_cmd		=	11003, /*建立pvp房间*/
	 btlsw_pvp_room_set_attr_cmd		=	11005, /*修改pvp房间属性(回包是房间广播)*/
	 btlsw_set_room_seat_onoff_cmd		=	11006, /*开关房间座位*/
	 btlsw_pvp_join_room_cmd		=	11007, /*玩家加入pvp房间*/
	 btlsw_leave_room_cmd		=	11008, /*玩家离开房间(pvp/pve)*/
	 btlsw_pvp_room_start_cmd		=	11009, /*通知online，房间开始进入战斗状态*/
	 btlsw_room_player_set_attr_cmd		=	11011, /*房间中的玩家: 设置状态(准备/取消准备), 更换座位*/
	 btlsw_kick_room_player_cmd		=	11013, /*房间踢人(pvp/pve)*/
	 btlsw_pvp_room_start_battle_cmd		=	11015, /*online通知btlsw上的房主，btlsw转发给房间中所有的玩家，通知其加入相应的战斗服务器，正式开始战斗*/
	 btlsw_pvp_end_battle_cmd		=	11016, /*战斗结束*/
	 btlsw_get_room_full_info_cmd		=	11017, /*拉取自己所在房间的信息*/
	 btlsw_chat_msg_transfer_cmd		=	11019, /*房间聊天信息转发*/
	 btlsw_player_exit_hall_cmd		=	11020, /*玩家退出并注销*/
	 btlsw_player_enter_hall_cmd		=	11021, /*玩家进入大厅*/
	 btlsw_player_leave_notify_cmd		=	11022, /**/
	 btlsw_pvp_query_room_info_cmd		=	11023, /*按房间编号查询房间信息*/
	 btlsw_notify_room_seat_onoff_cmd		=	11024, /*座位上锁通知*/
	 btlsw_player_sitdown_notify_cmd		=	11025, /*玩家坐下/换座位通知*/
	 btlsw_chg_player_status_notify_cmd		=	11026, /*切换玩家状态(准备/取消准备)*/
	 btlsw_get_room_player_show_info_cmd		=	11027, /*拉取玩家的房间数据(供界面显示用)*/
	 btlsw_pvp_room_attr_notify_cmd		=	11028, /*pvp房间属性(房间名,密码, 副本地图id, 副本难度)变动通知*/

};

/*错误码定义*/	
enum  enum_pea_btlsw_error{
	 BTLSW_ERR_NOERROR		=	0, /*无错误*/
	 BTLSW_ERR_REQUEST_DATA		=	400001, /*请求数据包错误*/
	 BTLSW_ERR_NO_FIT_ROOM_FOUND		=	400002, /*没有找到合适的房间*/
	 BTLSW_ERR_NO_ROOM_FOUND		=	400003, /*没有找到给定的房间*/
	 BTLSW_ERR_ROOM_FULL		=	400004, /*房间满了*/
	 BTLSW_ERR_BEYOND_RIGHT		=	400005, /*玩家没有相应的权利*/
	 BTLSW_ERR_PASSWORD		=	400006, /*密码错误*/
	 BTLSW_ERR_SYSTEM_FAULT		=	400007, /*系统错误*/
	 BTLSW_ERR_ONLINE_ID		=	400008, /*online_id错误*/
	 BTLSW_ERR_CANT_START_ROOM		=	400009, /*房间不能开始*/
	 BTLSW_ERR_INV_ROOM_LIST_TURN		=	400010, /*无效的房间列表拉取方向(既不是往前翻, 也不是往后翻)*/
	 BTLSW_ERR_INV_ROOM_LIST_COUNT		=	400011, /*无效的房间数量*/
	 BTLSW_ERR_ALREADY_INROOM		=	400012, /*已经在房间里了*/
	 BTLSW_ERR_INV_ROOM_MODE		=	400013, /*房间还不支持这种对战模式*/
	 BTLSW_ERR_INV_PVP_MAPID		=	400014, /*无效的PVP地图*/
	 BTLSW_ERR_NOT_INROOM		=	400015, /*你不在房间中*/
	 BTLSW_ERR_NOT_ROOM_OWNER		=	400016, /*你不是房间的主子/堂口/话事人/扛把子/...*/
	 BTLSW_ERR_NOPLAYER_INROOM		=	400017, /*房间中没有这个人（跑路了?）*/
	 BTLSW_ERR_CANT_KICK_SELF		=	400018, /*不能自宫！（连online都没防住?）*/
	 BTLSW_ERR_ALREADY_IN_BATTLE		=	400019, /*房间在对战状态(不允许踢人、换位子等动作)*/
	 BTLSW_ERR_NOT_IN_BATTLE		=	400020, /*房间不在对战状态 (不能做这种操作)*/
	 BTLSW_ERR_INV_SEAT_ONOFF		=	400021, /*无效的座位锁控制开关(既不是0, 又不是1)*/
	 BTLSW_ERR_CANT_BE_ROOM_OWNER		=	400022, /*你是房主，你不能做这个操作*/
	 BTLSW_ERR_INV_TEAM_ID		=	400023, /*无效的队伍编号(找不到对应的队伍)*/
	 BTLSW_ERR_INV_SEAT_ID		=	400024, /*无效的座位编号*/
	 BTLSW_ERR_SEAT_DONT_EMPTY		=	400025, /*座位上有人，不能做这个操作*/
	 BTLSW_ERR_SEAT_IS_EMPTY		=	400026, /*座位上没人，不能做这个操作*/
	 BTLSW_ERR_INV_PLAYER_SET_WHICH		=	400027, /*无效的房间中玩家的设置类型 (0: 换位子, 1: 准备状态变更, 其它值: 非法)*/
	 BTLSW_ERR_INV_PLAYER_STATUS		=	400028, /*无效的玩家状态(只能是 0: 取消准备, 1: 准备)*/
	 BTLSW_ERR_SOMEONE_UNREADY		=	400029, /*还有人没准备好*/
	 BTLSW_ERR_ROOM_CANT_IN_FIGHTING		=	400030, /*房间当前还不能进入对战状态*/
	 BTLSW_ERR_SOMETEAM_UNREADY		=	400031, /*有一边队伍没有人*/

};



#endif // PEA_BTLSW_ENUM_H
