extern "C"
{
#include "dll.h"
#include "proto.h"
#include "game.h"
}

#include "games/mpog.hpp"

/**
 * @brief 当有一个组准备好之后，系统会调用这个函数，同时这也是系统自身的一个函数
 * @param p 一个游戏用户对象，它可能是一个观察者，也可能是一个直接参加者
 * @return 0, 游戏总会被创建，并且不会失败
 * @todo 如果游戏创建失败了，应该怎么处理
 */
extern "C" int
on_game_begin (sprite_t * p)
{
	assert (GAME_READY (p->group));

	game_group_t *grp = p->group;
	if( grp->game->handle == NULL )
	{
		ERROR_RETURN (("Invalid Game Game %s: grpid=%lu", grp->game->name, grp->id), -1);
	}
	mpog *game;
	if (IS_GAME_PLAYER (p))
	{//是一个游戏参与者，说明游戏已经准备好，新一个游戏对象
		game = reinterpret_cast < mpog * >(grp->game->create_game (grp));
		if (!game)
		{
			ERROR_RETURN (("Failed to new Game %s: grpid=%lu", grp->game->name, grp->id), -1);
		}
		grp->game_handler = game;
	}
	else
	{
		// TODO - might need to return 1 so as to delete the watcher - less possibly
		if (!(grp->game_handler))
		{
			return 0;
		}
		game = reinterpret_cast < mpog * >(grp->game_handler);
	}
	// SET_GAME_START should be called within a given game
	game->init (p);

	DEBUG_LOG ("%s On Game Begin\t[grpid=%lu, nplayers=%d]", grp->game->name, grp->id, grp->count);
	return 0;
}

/**
 * @brief 当游戏进行中，客户端与服务器之间传送数据，一个新的数据到达时，系统会交给这个函数处理
 * @param p 数据从哪里过来，即哪个用户
 * @param cmd 一个命令，一个数据包就是对应一个命令，这个命令是插件自行定义的，系统不参与任凭要求
 * @param body 数据开始
 * @param len 数据长度
 * @return 任务完成的状态，成功或错误码
 */
extern "C" int
on_game_data (sprite_t * p, int cmd, const uint8_t body[], int len)
{
	mpog *game = reinterpret_cast < mpog * >(p->group->game_handler);
	if (game == NULL)
	{
		ERROR_LOG("%s On Game Data\t[uid=%u, grpid=%lu, cmd=%d, len=%d] game_handler null",
			p->group->game->name, p->id, p->group->id, cmd, len);
		return 0;
	}

	DEBUG_LOG("%s On Game Data\t[uid=%u, grpid=%lu, cmd=%d, len=%d game_address: %x]",
		p->group->game->name, p->id, p->group->id, cmd, len, game);
	return game->handle_data (p, cmd, body, len);
}

extern "C" int
on_db_return (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret_code)
{
	if (!p->group)
	{
		DEBUG_LOG ("%d on_db_return group NULL", p->id);
		return -1;
	}

	if (!p->group->game_handler)
	{
		DEBUG_LOG ("%d on_db_return game_handler NULL", p->id);
		return -1;
	}
	mpog *game = reinterpret_cast < mpog * >(p->group->game_handler);

	int ret = game->handle_db_return (p, id, buf, len, ret_code);
	if (ret == GER_end_of_game)
	{
		end_multiplayer_game (p->group, p, ret);
		ret = 0;
	}
	return ret;
}

/**
 * @brief 系统内部的定时器处理函数，不期望出现超时
 * @param s 一个用户，针对这个用户的定时器超时了
 * @param data 没有使用
 * @return -1, 这是不应该发生的，游戏需要结束
 */
extern "C" int
on_timer_expire (void *s, void* /*data*/)
{
	sprite_t *p = reinterpret_cast < sprite_t * >(s);

	DEBUG_LOG ("%s On Timer Expired: uid=%u, grpid=%lu", p->group->game->name, p->id, p->group->id);

	SET_SPRITE_TIMEOUT (p);
	uint8_t reason = GER_timer_expire;
	leave_game_cmd (p, &reason, 1);

	return -1;
}

/**
 * @brief 用户定时处理器外壳，当一个插件设置的定时器超时会调用
 * @param p 哪个游戏组的定时器超时了
 * @param data 传递给插件的数据，内容也应该由插件定义
 * @return -1, 游戏结束；其它指明相应的状态
 */
extern "C" int
on_game_timer_expire (void *p, void *data)
{
	game_group_t *grp = reinterpret_cast < game_group_t * >(p);

	DEBUG_LOG ("%s On Game Timer Expired: grpid=%lu", grp->game->name, grp->id);

	if (data)
	{
		mpog *game = reinterpret_cast < mpog * >(grp->game_handler);
		int ret = game->handle_timeout (data);

		if (ret == GER_end_of_game)
		{
			end_multiplayer_game (grp, 0, ret);
			ret = -1;
		}
		return ret;
	}
	else
	{
		end_multiplayer_game (grp, 0, GER_timer_expire);
		return -1;
	}
}

/**
 * @brief 游戏结束时的清理工作
 * @param gamegrp 指明哪个游戏结束
 * @return 无
 */
extern "C" void
on_game_end (game_group_t * gamegrp)
{
	assert (gamegrp);

	DEBUG_LOG ("%s On Game End\t[grpid=%lu]", gamegrp->game->name, gamegrp->id);

	delete reinterpret_cast < mpog * >(gamegrp->game_handler);
	gamegrp->game_handler = 0;
}

