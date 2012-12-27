///
///=====================================================================================
///      @file  paopao.hpp
///     @brief 抛抛球游戏执行类
///
/// 转发游戏消息包，决定风向
///
///  @internal
///    Created  01/16/2009 10:37:01 AM
///   Revision  1.0.0.0
///   Compiler  gcc/g++
///    Company  TaoMee .Inc
///  Copyright  Copyright (c) 2009, sylar
///
///    @author  sylar, sylar@taomee.com
/// This source code is wrote for TaoMee.Inc, ShangHai CN.
///=====================================================================================
///

#ifndef RUSH_HPP
#define RUSH_HPP
#include <map>
extern "C" {
#include  <libtaomee/log.h>
#include  <gameserv/game.h>
}
#include "../mpog.hpp"

enum rushques_proto
{
	proto_send_ques         = 30500,
	proto_send_ans          = 30501,
	proto_send_result       = 30502,
	proto_rush              = 30503,
	proto_client_data       = 30506,
	proto_player_ready      = 30505,
};

enum ques_type_t
{
    num_ques = 1,
    math_ques = 2,
    english_ques =3,
    knowledge_ques = 4,
    search_ques =5,
};

typedef struct _q_t{
    int  id;
    char content[256];
    char option_a[64];
    char option_b[64];
    char option_c[64];
    char option_d[64];
    int  ans;
} __attribute__((packed)) ques_t;


/**
 * @class Rush
 * @brief 处理各种游戏信息
 */
class Rush: public mpog
{
	public:
		Rush(game_group_t *grp);
        ~Rush();
		void init(sprite_t* p);
		//int  handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code);
		//int  handle_timeout(void* data);
		int  handle_data(sprite_t* p, int cmd, const uint8_t body[], int len);
        void send_ques();

	private:
        int load_questions(const char* file);
        int send_result(uint32_t uid, uint32_t result);
        int add_score_other(sprite_t* p);
        int add_score_self(sprite_t* p);
		game_group_t *m_grp;
        uint32_t ques_type;
        uint32_t ques_id;
        uint32_t ques_content;
        uint32_t ques_owner;
        uint32_t player_ready_cnt;
        time_t ques_send_time;

struct timer_head_t {
    list_head_t timer_list;
}__attribute__((packed));

        timer_head_t tmr;
        int score[4];
		std::map<uint32_t, int> usr_score;
		std::map<int, uint32_t> usr_level;
        static int ques_total_cnt;
        static int ques_timeout[5];
        static ques_t ques[1000];
        static uint32_t math_ans[30];
        static int reward[3][4];
        static const char* english_ans[];
};
#endif
