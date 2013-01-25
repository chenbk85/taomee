#ifndef  MOLE2_BTL_ENUM_H
#define  MOLE2_BTL_ENUM_H

/*命令定义*/	
enum  enum_mole2_btl_cmd{
	 challenge_battle_op_cmd		=	1300, /*挑战*/
	 accept_battle_op_cmd		=	1301, /*接受挑战*/
	 challenge_npc_battle_op_cmd		=	1304, /*挑战npc*/
	 bt_load_battle_cmd		=	1306, /*客户端上报加载进度*/
	 bt_battle_attack_cmd		=	1308, /*用户操作命令*/
	 bt_use_chemical_cmd		=	1309, /*使用药瓶*/
	 bt_noti_battle_attacks_info_cmd		=	1310, /*返回客户段打斗行动数据包*/
	 bt_load_battle_ready_cmd		=	1317, /*客户段初始化成功*/
	 bt_reject_challenge_cmd		=	1320, /*拒绝接受pk*/
	 bt_cancel_challenge_cmd		=	1336, /*取消对战*/
	 catch_pet_op_cmd		=	1338, /*捕捉宠物*/
	 bt_recall_pet_cmd		=	1339, /*换宠*/
	 bt_break_off_cmd		=	1340, /*掉线－－对战前*/
	 bt_kick_off_teamer_cmd		=	1347, /*掉线－－对战前*/

};

/*错误码定义*/	
enum  enum_mole2_btl_error{
	 __SUCC__		=	0, /*成功*/

};



#endif // MOLE2_BTL_ENUM_H
