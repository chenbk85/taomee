
/**
 * @file mpog.hpp
 * @brief 提供一个抽象类，它作为所有插件的基类
 * @author easyeagel easyeagel@taomee.com
 * @company 淘米网络
 */

#ifndef MULTIPLAYER_ONLINE_GAME_HPP_
#define MULTIPLAYER_ONLINE_GAME_HPP_
extern "C"
{
#include "../game.h"
}

/**
 * @class mpog
 * @brief 这一个的接口类，游戏插件应该继承于它
 * @details 它提供一个游戏组对象的应该支持的接口，一个游戏插件应该提供这个样的接口，并且继承于这个类
 * @see sprite_t
 */
class mpog
{
  public:
	virtual ~ mpog ()
	{
	}

	/**
	 * @brief 当一个游戏对象被创建时系统会调用这个函数，以完成对对象的初始化
	 * @details 当一个游戏组准备好之后，系统会调用create_game函数，这个函数是C链接的
	 * 		系统通过dlopen来找到，create_game将返回一个mpog的派生类对象，而系统会使用
	 * 		这些接口
	 * @param p 针对哪个用户相关的对象进行初始化
	 * @see game_end_reason_t
	 */
	virtual void init (sprite_t * p) = 0;

	/**
	 * @brief 当有数据发送给相应游戏组时系统调用这个函数
	 * @param p 数据是哪个用户发来的
	 * @param cmd 数据是传递给哪个命令的
	 * @param body 数据体
	 * @param len 数据体长度
	 * @return 成功或命令执行的状态码
	 * @see game_end_reason_t
	 */
	virtual int handle_data (sprite_t * p, int cmd, const uint8_t body[], int len) = 0;

	/**
	 * @brief 存在一些游戏需要访问数据库，这里是一个异步的处理方式，很多时候插件作者无需写作
	 * @param sprite_t* p 哪个用户相关的数据库包
	 * @param void* buf 数据库返回的包
	 * @param int len 包的长度
	 * @param uint32_t ret_code 数据返回码
	 * @return 状态码: 0, 正常；其它表示特殊
	 * @see game_end_reason_t
	 */
	virtual int handle_db_return (sprite_t * p, uint32_t id, const void *buf, int len, uint32_t ret_code)
	{
		return 0;
	}

	/**
	 * @brief 对于每一个用户或游戏组都存在一些可能定时器的需要
	 * @details 如果插件用户使用了定时器，那么应该定义一个合适的处理函数，也就是重新实现这个函数
	 * @param data 传递给处理器的数据
	 * @see game_end_reason_t
	 */
	virtual int handle_timeout (void *data)
	{
		return 0;
	}
};

extern uint8_t pkg[4096];

#endif // MULTIPLAYER_ONLINE_GAME_HPP_
