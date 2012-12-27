/**
 * =====================================================================================
 *       @file  paopao.cpp
 *      @brief  抛抛球游戏源文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  02/16/2009 10:34:45 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <vector>
#include <algorithm>
#include "rush.hpp"
#include <libtaomee++/random/random.hpp>
#include <ant/inet/pdumanip.hpp>
extern "C" {
#include <gameserv/proto.h>
#include <gameserv/timer.h>
#include <libxml/tree.h>
#include <util.h>
#include <string.h>
}


ques_t Rush::ques[1000];
int Rush::ques_total_cnt = 0;

int
call_rush_send_ques(void* owner, void* data)
{
    Rush *mrush = reinterpret_cast<Rush*>(data);
    mrush->send_ques();
    return 0;
}

void
Rush::send_ques()
{
    //decide ques type
    int len = sizeof(protocol_t);

    ques_type = (rand()%5 + 1);
    //ques_type = 5;
	ant::pack(pkg, ques_type, len);
    switch(ques_type) {
        case num_ques:
            {
                ques_content = taomee::ranged_random(9000000,99999999);
                ant::pack(pkg, ques_content, len);
            }
            break;
        case math_ques:
            ques_id = rand()%30 + 1;
            ant::pack(pkg, ques_id, len);
            break;
        case english_ques:
            ques_id = rand()%50 + 1;
            ant::pack(pkg, ques_id, len);
            break;
        case knowledge_ques:
            ques_id = rand()%ques_total_cnt + 1;
            ant::pack(pkg,ques[ques_id - 1].content, 256, len);
            ant::pack(pkg,ques[ques_id - 1].option_a, 256, len);
            ant::pack(pkg,ques[ques_id - 1].option_b, 64, len);
            ant::pack(pkg,ques[ques_id - 1].option_c, 64, len);
            ant::pack(pkg,ques[ques_id - 1].option_d, 64, len);
            break;
        case search_ques:
            {
                ant::pack(pkg, (rand()%2 + 1), len);
                uint8_t cha;
                cha = 97 + rand()%26;
                //cha = 97 + 22;
                ant::pack(pkg, cha, len);
            }
            break;
    }
	init_proto_head(pkg, proto_send_ques, len);
    send_to_players(m_grp, pkg, len);
    ques_send_time = now.tv_sec;
    ques_owner = 0;
    DEBUG_LOG("SEND QUES [%lu %d %d %d]", m_grp->id, ques_type, ques_id, ques_content);
    //REMOVE_TIMERS(&tmr);
    //ADD_TIMER_EVENT(&tmr, call_rush_send_ques, this, now.tv_sec + ques_timeout[ques_type - 1]);
}
int Rush::reward[3][4] = {
        {300, 100, 0, 0},
        {400, 200, 100, 0},
        {500, 300, 200, 100},
};

int
Rush::send_result(uint32_t uid, uint32_t result)
{

    int len = sizeof(protocol_t);
    ant::pack(pkg, uid, len);
    ant::pack(pkg, result, len);
	init_proto_head(pkg, proto_send_result, len);
    send_to_players(m_grp, pkg, len);
    uint32_t winner = 0;
    std::map<uint32_t, int>::iterator it;
    for(it = usr_score.begin(); it!=usr_score.end(); it++) {
        if(!result) {
            if(it->first == uid) {
                it->second += 3;
            }
        } else {
            if(it->first != uid) {
                it->second += 1;
            }
        }
        //DEBUG_LOG("PLAYER SCORE [%d %d]", it->first, it->second);
    }
    for(it = usr_score.begin(); it!=usr_score.end(); it++) {
        if(it->second >= 30) {
        //if(it->second >= 3) {
            winner = it->first;
            break;
        }
    }
    DEBUG_LOG("SEND RESULT [%lu %d %d]", m_grp->id, uid, result);
    if(winner) {
        std::map<int, std::vector<uint32_t> > reward_level;
        for(it = usr_score.begin(); it!=usr_score.end(); it++) {
            reward_level[it->second].push_back(it->first);
        }
        std::map<int, std::vector<uint32_t> >::reverse_iterator rit;
        int reward_rang = 0;
        for(rit = reward_level.rbegin(); rit!=reward_level.rend(); rit++) {
            //DEBUG_LOG("REWARD LEVEL[%lu %d %d]", m_grp->id, rit->first, rit->second.size());
            for(uint32_t i = 0; i < rit->second.size(); i++) {
                //DEBUG_LOG("REWARD PLAYER [%lu %d]", m_grp->id, rit->second[i]);
                for(int k = 0; k < m_grp->count; k++) {
                    if(m_grp->players[k]->id == rit->second[i]) {
                        game_score_t score;
                        memset(&score, 0, sizeof(score));
                        int coins = reward[m_grp->game->players - 2][reward_rang];
                        score.coins = coins;
                        score.exp = coins/10;
                        score.iq = coins/20;
						/*
                        if(reward_rang < 2) {
                            if(rand()%2) {
                                score.itmid =190423;
                                score.itmkind = 12;
                                score.itm_max= 999;
                            }
                        }
						*/
                        //DEBUG_LOG("SUBMIT PLAYER [%lu %d %d]", m_grp->id, rit->second[i], coins);
                        submit_game_score(m_grp->players[k], &score);
                    }
                }
            }
            reward_rang++;
        }
        DEBUG_LOG("WINNER [%lu %d]", m_grp->id, winner);

        return GER_end_of_game;
    }
    return 0;
}

int Rush::ques_timeout[5] = {
    //10,15,20,15,10,
    20,20,25,20,15,
};

uint32_t Rush::math_ans[30] = {
    5,13,37,42,58,88,18,28,17,1,
    65,62,56,56,82,36,48,45,22,10,
    12,60,24,56,48,75,39,39,35,84,
};

const char* Rush::english_ans[] = {
    "pen","watch","umbrella","hat","map","apple","egg","pear","orange","bee",
    "rabbit","pig","duck","banana","strawberry","house","carpet","cat","fork","table",
    "box","cup","bed","cloud","bone","book","coat","milk","knife","bottle",
    "cabbage","eggplant","carrot","grape","watermelon","peach","dress","skirt","flower",
    "car","case","dog","tin","spoon","plate","glass","window","door","sun","bird"
};

int
Rush::add_score_self(sprite_t* p)
{
    usr_score[p->id] += 30;
    return 0;
}

int
Rush::add_score_other(sprite_t* p)
{
    return 0;
}

void
Rush::init(sprite_t* p)
{
}

//int
//Rush::handle_timeout(void* data)
//{
//    send_ques(NULL, NULL);
//}

Rush::Rush(game_group_t *grp):ques_type(0),player_ready_cnt(0)
{
    if(!ques_total_cnt) {
        load_questions("./conf/questions.xml");
    }
    m_grp = grp;
    INIT_LIST_HEAD(&tmr.timer_list);
    for(int i = 0; i < m_grp->count; i++) {
        usr_score[m_grp->players[i]->id] = 0;
    }
}

Rush::~Rush()
{
    REMOVE_TIMERS(&tmr);
}

int Rush::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
#define CHECK_VALID_RUSH(p_, time_limit_) do{\
    if((now.tv_sec - ques_send_time) <= time_limit_) {\
        ERROR_RETURN(("can not answer [%d]", p->id), 0);\
    }\
} while(0)

    int i = 0;
    switch(cmd)
    {
        case proto_rush:
            if(ques_type == num_ques) {
                CHECK_VALID_RUSH(p, 5);
            } else {
                CHECK_VALID_RUSH(p, 3);
            }
            if(!ques_owner) {
                ques_owner = p->id;
                int mlen = sizeof(protocol_t);
                ant::pack(pkg, p->id, mlen);
                //DEBUG_LOG("GOT IT [%lu %d]", m_grp->id, p->id);
			    init_proto_head(pkg, proto_rush, mlen);
                send_to_players(m_grp, pkg, mlen);
            }
            break;
        case proto_send_ans:
            if(ques_owner != p->id) {
                ERROR_RETURN(("can not answer2 %d", p->id), 0);
            }
            if(len == 8) {
                //DEBUG_LOG("CLIENT ANSWER ERROR [%d]", p->id);
                return send_result(p->id, 1);
            }
            switch(ques_type){
                case num_ques:
                    {
                        CHECK_BODY_LEN(len, 4);
                        uint32_t result;
                        ant::unpack(body, result, i);
                        DEBUG_LOG("CLIENT ANSWER [%lu %d %d %d]", m_grp->id, p->id, result, ques_content);
                        if(result == ques_content) {
                            return send_result(p->id, 0);
                        } else {
                            return send_result(p->id, 1);
                        }
                    }
                    break;
                case math_ques:
                    {
                        CHECK_BODY_LEN(len, 4);
                        uint32_t result;
                        ant::unpack(body, result, i);
                        DEBUG_LOG("CLIENT ANSWER [%lu %d %d %d]", m_grp->id, p->id, result, math_ans[ques_id - 1]);
                        if(result == math_ans[ques_id - 1]) {
                            return send_result(p->id, 0);
                        } else {
                            return send_result(p->id, 1);
                        }
                    }
                    break;
                case english_ques:
                    {
                        CHECK_BODY_LEN(len, 16);
                        //strcmp
                        DEBUG_LOG("ENGLISH ANS [%lu %d %s %s]", m_grp->id, p->id, (char*)body, english_ans[ques_id - 1]);
                        if(!strcmp((char*)body, english_ans[ques_id - 1])) {
                            return send_result(p->id, 0);
                        } else {
                            return send_result(p->id, 1);
                        }
                    }
                    break;
                case knowledge_ques:
                    {
                        CHECK_BODY_LEN(len, 4);
                        int result;
                        ant::unpack(body, result, i);
                        DEBUG_LOG("CLIENT ANSWER [%lu %d %d %d]", m_grp->id, p->id, result, ques[ques_id - 1].ans);
                        if(result == ques[ques_id - 1].ans) {
                            return send_result(p->id, 0);
                        } else {
                            return send_result(p->id, 1);
                        }
                    }
                    break;
                case search_ques:
                    {
                        CHECK_BODY_LEN(len, 4);
                        uint32_t result;
                        ant::unpack(body, result, i);
                        DEBUG_LOG("CLIENT ANSWER [%lu %d %d]", m_grp->id, p->id, result);
                        if(!result) {
                            return send_result(p->id, 0);
                        } else {
                            return send_result(p->id, 1);
                        }
                    }
                    break;
            }
            break;
        case proto_client_data:
            {
                int mlen = sizeof(protocol_t);
                //memcpy(pkg + mlen, body, len);
                for(int k = 0; k < len; k ++) {
                    ant::pack(pkg, body[k], mlen);
                }
			    init_proto_head(pkg, proto_client_data, mlen);
                send_to_players(m_grp, pkg, mlen);
                //DEBUG_LOG("SEND DATA [%lu %d]", m_grp->id, p->id);
            }
            break;
        case proto_player_leave:
            usr_score.erase(p->id);
            if(!usr_score.size()) {
                return GER_end_of_game;
            }
            break;
        case proto_player_ready:
            {
                int mlen = sizeof(protocol_t);
                ant::pack(pkg, p->id, mlen);
                init_proto_head(pkg, proto_player_ready, mlen);
                send_to_players(m_grp, pkg, mlen);
                player_ready_cnt++;
                DEBUG_LOG("PLAYER READY [%lu %d %d]", m_grp->id, p->id, player_ready_cnt);
                if(player_ready_cnt == m_grp->count) {
                    send_ques();
                    player_ready_cnt = 0;
                }
            }
            break;
    }
    return 0;
}

int
Rush::load_questions(const char *file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load questions config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
        err = -1;
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Question"))){
			DECODE_XML_PROP_INT(ques[i].id, cur, "ID");
            //DEBUG_LOG("QUESTION %d",ques[i].id);
            if(ques[i].id != (i+1)) {
                ERROR_LOG("ques[%d] ID error %d",i, ques[i].id);
                err = -1;
                goto exit;
            }
			DECODE_XML_PROP_STR(ques[i].content, cur, "Content");
			DECODE_XML_PROP_STR(ques[i].option_a, cur, "OptionA");
			DECODE_XML_PROP_STR(ques[i].option_b, cur, "OptionB");
			DECODE_XML_PROP_STR(ques[i].option_c, cur, "OptionC");
			DECODE_XML_PROP_STR(ques[i].option_d, cur, "OptionD");
            //DEBUG_LOG("ID[%d]:Content[%s], A[%s],B[%s],C[%s],D[%s]",ques[i].id,ques[i].content, ques[i].option_a,ques[i].option_b,ques[i].option_c,ques[i].option_d);
			DECODE_XML_PROP_INT(ques[i].ans, cur, "Answer");
            if(ques[i].ans > 4 || ques[i].ans < 1) {
                ERROR_LOG("ques[%d] answer error %d",i, ques[i].ans);
                err = -1;
                goto exit;
            }

			++i;
		}
		cur = cur->next;
	}
    ques_total_cnt = i;
    //DEBUG_LOG("QUESTION TOTAL NUM %d", i);
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}


