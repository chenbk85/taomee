#include <new>
extern "C" {
#include  <libtaomee/log.h>
#include <gameserv/config.h>
}
#include "match.hpp"

/** 卡牌游戏初始化类，游戏初始化之后不应该被析构 */
Cmatch *p_match = NULL;

/**
 * @brief  每次新游戏时，获得处理初始化类的指针
 * @param  game_group_t* grp 新游戏的组
 * @return void* 初始化管理类指针
 */
extern "C" void* create_game(game_group_t* grp)
{
	return p_match;
}

/**
 * @brief  游戏服务器启动的时候读取配置信息
 * @param
 * @return
 */
extern "C" int game_init()
{
	if (p_match == NULL)
	{
		p_match = new Cmatch(NULL);

		if (p_match->load_tugs_conf(config_get_strval("new_tugs_conf")) < 0)
		{
            ERROR_LOG("LOAD TUG CONFIG XML ERROR");
        }

	}

	return 0;
}

extern "C" void game_destroy()
{

}

