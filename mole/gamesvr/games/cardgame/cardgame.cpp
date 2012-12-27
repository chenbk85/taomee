#include <new>
extern "C" {
#include  <libtaomee/log.h>
#include <gameserv/config.h>
}
#include "match.hpp"

/** 卡牌游戏初始化类，游戏初始化之后不应该被析构 */
Match *match = NULL;
/**
 * @brief  每次新卡牌游戏时，获得处理卡牌初始化类的指针
 * @param  game_group_t* grp 新游戏的组
 * @return void* 卡牌初始化管理类指针
 */
extern "C" void* create_game(game_group_t* grp)
{
	return match;
}

/**
 * @brief  游戏服务器启动的时候读取卡牌配置信息
 * @param
 * @return
 */
extern "C" int game_init()
{
	if (match == NULL) {
		match = new Match(NULL);

		if (match->load_cards_conf(config_get_strval("cards_conf")) < 0) {
			ERROR_LOG("LOAD CARDS CONFIG XML ERROR");
		}

	}
	return 0;
}

extern "C" void game_destroy()
{
}

