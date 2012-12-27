/**
 * =====================================================================================
 *       @file  mole_question.c
 *      @brief  摩尔答题
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  05/04/2009 11:41:10 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <libtaomee/bitmanip/bitmanip.h>
#include <libxml/tree.h>
#include "proto.h"
#include "dbproxy.h"
#include "mole_question.h"
#include "logic.h"
#include "login.h"
#include "message.h"
#include "exclu_things.h"

ques_t ques[1000];
uint32_t ques_max;

int get_ask_answer_info(sprite_t *p)
{
    return send_request_to_db(SVR_PROTO_GET_ASK_ANSWER_INFO, p, 0, NULL, p->id);
}

int get_ask_answer_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 12);
    int k = 0;
	UNPKG_H_UINT32 (buf, p->ques_id, k);
    if ((p->ques_id + 1) >= ques_max) {
        p->ques_id = p->ques_id % ques_max;
    }
	UNPKG_H_UINT32 (buf, p->ques_correct_cnt, k);
	UNPKG_H_UINT32 (buf, p->ques_total_cnt, k);

#if 0  // 26th bit already used
	if (!test_bit_on32(p->flag, 26)) {
		p->flag = set_bit_on32(p->flag, 26);
		db_set_user_flag(0, set_bit_on32(p->flag, 1), p->id);
		send_postcard("公民管理处", 0, p->id, 1000061, "让我们团结起来，抵制谣言！摩尔庄园绝不会关闭，永远陪伴在小摩尔的身边！", 0);
	}
#endif
	//return proc_final_login_step(p);
	return db_get_month_tasks(p, p->id);
}


int answer_question_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);

    if(p->ques_ans_flag) {
        p->ques_ans_flag = 0;
    } else {
        ERROR_RETURN(("answer question bot %d", p->id), -1);
    }


    if(p->ques_id >= ques_max) {
        ERROR_LOG("answer question id error\t\t[%d %d]", p->id, p->ques_id + 1);
        return -1;
    }

    int i = 0;
    int result = 0;
    uint32_t option = 0;
    uint32_t db_msg[2];

    uint32_t yxb_bonus[3] = {30, 40, 50};
    uint32_t iq_bonus[3] = {1, 2, 3 };

    int idx = p->ques_id;

    UNPKG_UINT32(body, option, i);
    if(option > 4) {
        ERROR_RETURN(("%d error answer %d", p->id, option), -1);
    }

    DEBUG_LOG("ANSWER QUESTION\t\t[%d, num[%d]: answer[%d], correct_answer[%d]]", p->id, ques[idx].id, option, ques[idx].ans);

    if((idx + 1) == ques_max) {
        p->ques_id = 0;
        db_msg[1] = 0;
    } else {
        db_msg[1] = idx;
        p->ques_id++;
    }

    int dift = ques[idx].difficulty - 1;
    if(option == ques[idx].ans) {

		/*答对的统计项*/
		uint32_t data[] = {1, 0};
		msglog(statistic_logfile, 0x02032200 + idx, now.tv_sec, data, sizeof(data));

        result = 1;
        db_msg[0] = 1;
        p->ques_correct_cnt++;
        p->ques_total_cnt++;
        do_db_attr_op(NULL, p->id, yxb_bonus[dift], 0, 0, iq_bonus[dift], 0, ATTR_CHG_roll_back, 0);
    } else {
		/*答错的统计项*/
		uint32_t data[] = {0, 1};
		msglog(statistic_logfile, 0x02032200 + idx, now.tv_sec, data, sizeof(data));

        result = 0;
        db_msg[0] = 0;
    }
	send_request_to_db(SVR_PROTO_SET_ASK_ANSWER_INFO, NULL, sizeof(db_msg), db_msg, p->id);

    int len = 0;
    uint8_t cli_msg[20];
    PKG_UINT32(cli_msg, result, len);
    PKG_UINT32(cli_msg, ques[idx].ans, len);
    PKG_UINT32(cli_msg, yxb_bonus[dift], len);
    PKG_UINT32(cli_msg, iq_bonus[dift], len);
    PKG_UINT32(cli_msg, p->ques_correct_cnt, len);
    response_proto_str(p, PROTO_ANSWER_QUESTION, len, cli_msg, 0);
    return 0;
}

int load_questions(const char *file)
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
			DECODE_XML_PROP_UINT32(ques[i].id, cur, "ID");
            //DEBUG_LOG("QUESTION %d",ques[i].id);
            if(ques[i].id != (i+1)) {
                ERROR_LOG("ques[%d] ID error %d",i, ques[i].id);
                err = -1;
                goto exit;
            }
			DECODE_XML_PROP_UINT32(ques[i].difficulty, cur, "Difficulty");
			DECODE_XML_PROP_STR(ques[i].content, cur, "Content");
			DECODE_XML_PROP_STR(ques[i].option_a, cur, "OptionA");
			DECODE_XML_PROP_STR(ques[i].option_b, cur, "OptionB");
			DECODE_XML_PROP_STR(ques[i].option_c, cur, "OptionC");
			DECODE_XML_PROP_STR(ques[i].option_d, cur, "OptionD");
            //DEBUG_LOG("ID[%d]",ques[i].id);
			DECODE_XML_PROP_UINT32(ques[i].ans, cur, "Answer");
            if(ques[i].ans > 4 || ques[i].ans < 1) {
                ERROR_LOG("ques[%d] answer error %d",i, ques[i].ans);
                err = -1;
                goto exit;
            }
            if(ques[i].difficulty> 3 || ques[i].difficulty < 1) {
                ERROR_LOG("ques[%d] difficulty error %d",i, ques[i].difficulty);
                err = -1;
                goto exit;
            }
			++i;
		}
		cur = cur->next;
	}
    ques_max = i;
    DEBUG_LOG("QUESTION TOTAL NUM %d", ques_max);
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}
