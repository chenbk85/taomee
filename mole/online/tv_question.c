/**
 * =====================================================================================
 *       @file  mole_question.c
 *      @brief  看电视回答问题
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
#include "tv_question.h"
#include "logic.h"
#include "login.h"
#include "message.h"
#include "exclu_things.h"
#include "time.h"

#define DAY_KAKU_QUES_CNT		8
#define DAY_JINYING_QUES_CNT	2
#define DAY_HAHA_QUES_CNT		4

//tv_ques_t tv_ques[356*2];
tv_ques_t tv_ques_1[50*2];//卡酷题目
tv_ques_t tv_ques_2[50*2];//金鹰题目
tv_ques_t tv_ques_3[50*2];//哈哈少儿题目
uint32_t tv_ques_max_1;
uint32_t tv_ques_max_2;
uint32_t tv_ques_max_3;

enum {
	KAKU_QUES = 1,
	JINYING_QUES = 2,
	HAHA_QUES = 3,
};

/*
 * @brief 根据日期二分查找题目
 * @prama date:日期
 * @prama type:1为卡酷卫视题目、2为金鹰卫视题目
 */
int find_question_idx_from_date(uint32_t date, uint32_t type)
{
	tv_ques_t *tv_ques = NULL;
	uint32_t tv_ques_max;
	uint32_t day_ques_cnt = 0;
	if (type == KAKU_QUES) {
		tv_ques = tv_ques_1;
		tv_ques_max = tv_ques_max_1;
		day_ques_cnt = DAY_KAKU_QUES_CNT;
	} 
	else if (type == JINYING_QUES) {
		tv_ques = tv_ques_2;
		tv_ques_max = tv_ques_max_2;
		day_ques_cnt = DAY_JINYING_QUES_CNT;
	}
	else if (type == HAHA_QUES) {
		tv_ques = tv_ques_3;
		tv_ques_max = tv_ques_max_3;
		day_ques_cnt = DAY_HAHA_QUES_CNT;
	}
	else {
		return -1;
	}

	int low = 0;
	int high = tv_ques_max - 1;
	int middle = (low + high) / 2;
	while (low <= high) {
		if (date == tv_ques[middle].date) {
			//if (middle % day_ques_cnt != 0) {//每天两题，所以当天的题目开始的IDX为偶数
			middle = middle - (middle % day_ques_cnt);//当天的题目开始的IDX
			//}
			return middle;
		} else if (date > tv_ques[middle].date) {
			low = middle + 1;
		} else {
			high = middle - 1;
		}
		middle = (low + high) / 2;
	}
	return -1;
}

int load_tv_questions(const char *file, uint32_t type)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	tv_ques_t *tv_ques = NULL;
	uint32_t day_ques_cnt = 0;
	if (type == KAKU_QUES) {
		tv_ques = tv_ques_1;
		day_ques_cnt = DAY_KAKU_QUES_CNT;
	} 
	else if(type == JINYING_QUES){
		tv_ques = tv_ques_2;
		day_ques_cnt = DAY_JINYING_QUES_CNT;
	}
	else if(type == HAHA_QUES){
		tv_ques = tv_ques_3;
		day_ques_cnt = DAY_HAHA_QUES_CNT;
	}
	else{
		return -1;
	}

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load tv questions config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
        err = -1;
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"TV_question"))){
			DECODE_XML_PROP_UINT32(tv_ques[i].id, cur, "ID");
            //DEBUG_LOG("QUESTION %d",ques[i].id);
            if (tv_ques[i].id != (i+1)) {
                ERROR_LOG("ques[%d] ID error %d",i, tv_ques[i].id);
                err = -1;
                goto exit;
            }
			DECODE_XML_PROP_UINT32(tv_ques[i].date, cur, "Date");
			DECODE_XML_PROP_UINT32(tv_ques[i].prize_id, cur, "PrizeID");
			DECODE_XML_PROP_UINT32(tv_ques[i].prize_cnt, cur, "PrizeCnt");
			DECODE_XML_PROP_STR(tv_ques[i].content, cur, "Content");
			DECODE_XML_PROP_STR(tv_ques[i].option_a, cur, "OptionA");
			DECODE_XML_PROP_STR(tv_ques[i].option_b, cur, "OptionB");
			DECODE_XML_PROP_STR(tv_ques[i].option_c, cur, "OptionC");
			DECODE_XML_PROP_STR(tv_ques[i].option_d, cur, "OptionD");
			DECODE_XML_PROP_UINT32(tv_ques[i].answer, cur, "Answer");
        	/*不进行答案判断
			if (tv_ques[i].answer > 4 || tv_ques[i].answer < 1) {
                ERROR_LOG("tv_ques[%d] answer error %d", i, tv_ques[i].answer);
                err = -1;
                goto exit;
            }
			*/
			if (tv_ques[i].content == NULL) {
				ERROR_LOG("tv_ques[%d] content error", i);
				err = -1;
				goto exit;
			}
			if ((i % day_ques_cnt != 0) && tv_ques[i].date != tv_ques[i - 1].date) {
				ERROR_LOG("tv_ques[%d] date error %d", i, tv_ques[i].date);
				err = -1;
				goto exit;
			}
			++i;
		}
		cur = cur->next;
	}
	if (type == KAKU_QUES) {
		tv_ques_max_1 = i;
	} 
	else if (type == JINYING_QUES){
		tv_ques_max_2 = i;
	}
	else if (type == HAHA_QUES)
	{
		tv_ques_max_3 = i;
	}

	DEBUG_LOG("TV QUESTION TOTAL NUM %d", type == KAKU_QUES ? tv_ques_max_1 : tv_ques_max_3);
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load tv question file %s Size[%u]", file, i);
}

int get_tv_question_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);
	uint32_t type = 0;
	int i = 0;
	UNPKG_UINT32(body, type, i);

	uint32_t daytype = 0;
	if (type == KAKU_QUES) {
		daytype = 50000;
		*(uint32_t*)p->session = 1;
	}
	else if(type == JINYING_QUES) {
		daytype = 50001;
		*(uint32_t*)p->session = 2;
	} 
	else if(type == HAHA_QUES) {
		daytype = 50017;
		*(uint32_t*)p->session = 3;
	} 
	else {
		return send_to_self_error(p, p->waitcmd, -ERR_answer_tv_time_wrong, 1);
	}
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &daytype, p->id);
}

int get_tv_question_callback(sprite_t* p, uint32_t id, char* buf, int len)
{    
	CHECK_BODY_LEN(len, 4);
	uint32_t day_stata = *(uint32_t*)buf;

	uint32_t type = *(uint32_t*)p->session;
	tv_ques_t *tv_ques = NULL;
	uint32_t day_ques_cnt = 0;
	if (type == KAKU_QUES) {
		tv_ques = tv_ques_1;
		day_ques_cnt = DAY_KAKU_QUES_CNT;
	} 
	else if(type == JINYING_QUES){
		tv_ques = tv_ques_2;
		day_ques_cnt = DAY_JINYING_QUES_CNT;
	}
	else if(type == HAHA_QUES){
		tv_ques = tv_ques_3;
		day_ques_cnt = DAY_HAHA_QUES_CNT;
	}

	int ques_idx = find_question_idx_from_date(get_today(), type);
	if (ques_idx == -1) {
		send_to_self_error(p, p->waitcmd, -ERR_answer_tv_time_wrong, 1);
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, day_ques_cnt * 2, l);
	int i = 0;
	for (i = 0; i < day_ques_cnt * 2; i++) {
		PKG_UINT32(msg, tv_ques[ques_idx + i].id, l);

		uint32_t answer = 0;
		if (i < day_ques_cnt && (day_stata & (1 << i)) ) {//答过题
			answer = (day_stata & (1 << (i + 8))) ? 2 : 1;//是否答对
		}
		PKG_UINT32(msg, answer, l);
		PKG_UINT32(msg, tv_ques[ques_idx + i].prize_id, l);
		PKG_UINT32(msg, tv_ques[ques_idx + i].prize_cnt, l);
		PKG_UINT32(msg, tv_ques[ques_idx + i].answer, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int answer_tv_question_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	CHECK_VALID_ID(p->id);
	int i = 0;
	uint32_t type = 0;
	uint32_t question_id = 0;
	uint32_t answer = 0;
	UNPKG_UINT32(body, type, i);
	UNPKG_UINT32(body, question_id, i);
	UNPKG_UINT32(body, answer, i);

	uint32_t tv_ques_max = 0;
	uint32_t daytype = 0;
	tv_ques_t *tv_ques = NULL;
	uint32_t day_ques_cnt = 0;
	if (type == KAKU_QUES) {
		daytype = 50000;
		*(uint32_t*)p->session = KAKU_QUES;
		tv_ques = tv_ques_1;
		tv_ques_max = tv_ques_max_1;
		day_ques_cnt = DAY_KAKU_QUES_CNT;
	} 
	else if (type == JINYING_QUES) {
		daytype = 50001;
		*(uint32_t*)p->session = JINYING_QUES;
		tv_ques = tv_ques_2;
		tv_ques_max = tv_ques_max_2;
		day_ques_cnt = DAY_JINYING_QUES_CNT;
	} 
	else if (type == HAHA_QUES) {
		daytype = 50017;
		*(uint32_t*)p->session = HAHA_QUES;
		tv_ques = tv_ques_3;
		tv_ques_max = tv_ques_max_3;
		day_ques_cnt = DAY_HAHA_QUES_CNT;
	} 
	else {
		return send_to_self_error(p, p->waitcmd, -ERR_answer_tv_time_wrong, 1);
	}

	if (question_id <= 0 || question_id > tv_ques_max || get_today() != tv_ques[question_id - 1].date) {
		return send_to_self_error(p, p->waitcmd, -ERR_answer_tv_time_wrong, 1);
	}
	uint32_t right = (answer == tv_ques[question_id - 1].answer) ? 1 : 0;
	*(uint32_t*)(p->session + 4) = question_id;
	*(uint32_t*)(p->session + 8) = right;

	//0x01表示回答的是第一题，0x10表示第一题答对
	uint32_t ques_idx = 1 << ((question_id - 1) % day_ques_cnt);
	if (right == 1) {
		ques_idx |= (ques_idx << 8);
	}

	uint32_t max_v = 0;
	if (type == JINYING_QUES){
	    max_v = 0x303;
	}
	else if (type == KAKU_QUES)
	{
	   max_v = 0xFFFF; 
	}
	else if (type == HAHA_QUES)
	{
	   max_v = 0xFFF; 
	}
	
	uint32_t db_buff[] = {daytype, max_v, ques_idx};
	//p->waitcmd = PROTO_ANSWER_TV_QUESTION;
	return send_request_to_db(SVR_PROTO_SET_STH_DONE, p, 12, db_buff, p->id);
}

int answer_tv_question_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    
	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C346, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	uint32_t type = *(uint32_t*)p->session;
	uint32_t question_id = *(uint32_t*)(p->session + 4);
	uint32_t right = *(uint32_t*)(p->session + 8);
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, question_id, l);
	PKG_UINT32(msg, right, l);

	tv_ques_t *tv_ques = NULL;
	if (type == KAKU_QUES) {
		tv_ques = tv_ques_1;
	}
	else if(type == JINYING_QUES){
		tv_ques = tv_ques_2;
	}
	else if(type == HAHA_QUES){
		tv_ques = tv_ques_3;
	}

	if (right == 1) {
		//只送一个东西
		db_exchange_single_item_op(p, 0, tv_ques[question_id - 1].prize_id, tv_ques[question_id - 1].prize_cnt, 0);
		PKG_UINT32(msg, tv_ques[question_id - 1].prize_id, l);
		PKG_UINT32(msg, tv_ques[question_id - 1].prize_cnt, l);

//		msglog(statistic_logfile, (ques_idx==1) ? 0x0408B3DD : 0x0408B3DE, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

