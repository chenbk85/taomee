#ifndef __batserver_timerid_h__
#define __batserver_timerid_h__
enum 
{
	n_battle_load_expired = 1,
	n_attack_op_expired,
	n_reload_ai_script,
	n_warrior_load_expired,
};

enum
{
	BT_ERR_NORMAL = 0,
	BT_ERR_ID_ALREADY_EXIST = 1,
	BT_ERR_ID_NOT_EXIST = 2,
	BT_ERR_INVALID_TEAM_COUNT = 4,
	BT_ERR_WRONG_CHALLENGE = 5, // 接受挑战失败，挑战方并非挑战该接受者
	BT_ERR_REPEAT_ACCEPT = 6, // 重复接受战斗
	BT_ERR_INVALID_WARRIOR = 7, // 战斗状态中的宠物数量不对，只能为1或0
};

enum
{
	WAIGUA_RIZHI_TONGJI_FAZHAO = 1,
};
#endif
