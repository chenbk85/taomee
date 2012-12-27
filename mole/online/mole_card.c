#include <stdlib.h>
#include <libxml/tree.h>
#include <statistic_agent/msglog.h>

#include "proto.h"
#include "dbproxy.h"
#include "mole_card.h"
#include "logic.h"
#include "small_require.h"
static card_t card[CARD_ID_MAX];
static card_t new_card[CARD_ID_MAX];
int load_cards(const char *file)
{
	int i, err = -1;
	int cards_num = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT (cards_num, cur, "Count");
	if (cards_num < 0 || cards_num > CARD_ID_MAX) {
		ERROR_LOG ("error cards_num: %d", cards_num);
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Card"))){
			DECODE_XML_PROP_INT(card[i].id, cur, "ID");
			if (card[i].id != i)
				ERROR_RETURN(("card conf parse error: id=%d, num=%d", card[i].id, cards_num), -1);

			DECODE_XML_PROP_INT(card[i].type, cur, "Type");
			DECODE_XML_PROP_INT(card[i].colour, cur, "Colour");
			DECODE_XML_PROP_INT(card[i].value, cur, "Value");
			DECODE_XML_PROP_INT(card[i].event, cur, "Event");
			DECODE_XML_PROP_INT(card[i].star, cur, "Star");
			if ( card[i].type > CARD_TYPE_MAX) {
				ERROR_LOG("Card[%d] Type configuration Error", i);
				goto exit;
			}
			if (card[i].colour > CARD_COLOUR_MAX) {
				ERROR_LOG("Card[%d] Colour configuration Error", i);
				goto exit;
			}
			if (card[i].value > CARD_VALUE_MAX) {
				ERROR_LOG("Card[%d] Value configuration Error", i);
				goto exit;
			}
			if (card[i].event > CARD_EVENT_MAX) {
				ERROR_LOG("Card[%d] Event configuration Error", i);
				goto exit;
			}
			if (card[i].star> CARD_STAR_MAX) {
				ERROR_LOG("Card[%d] star configuration Error", i);
				goto exit;
			}
			++i;
		}
		if (i == cards_num)
			break;
		cur = cur->next;
	}

	if (i != cards_num) {
		ERROR_LOG ("parse %s failed, cards Count=%d, get Count=%d",
				file, cards_num, i);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}

int load_new_cards(const char *file)
{
	int i, err = -1;
	int cards_num = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	DECODE_XML_PROP_INT (cards_num, cur, "Count");
	if (cards_num < 0 || cards_num > CARD_ID_MAX) {
		ERROR_LOG ("error cards_num: %d", cards_num);
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Card"))){
			DECODE_XML_PROP_INT(new_card[i].id, cur, "ID");
			if (new_card[i].id != i)
				ERROR_RETURN(("card conf parse error: id=%d, num=%d", new_card[i].id, cards_num), -1);

			DECODE_XML_PROP_INT(new_card[i].type, cur, "Type");
			DECODE_XML_PROP_INT(new_card[i].colour, cur, "Colour");
			DECODE_XML_PROP_INT(new_card[i].value, cur, "Value");
			DECODE_XML_PROP_INT(new_card[i].event, cur, "Event");
			DECODE_XML_PROP_INT(new_card[i].star, cur, "Star");
			if ( new_card[i].type > CARD_TYPE_MAX) {
				ERROR_LOG("Card[%d] Type configuration Error", i);
				goto exit;
			}
			if (new_card[i].colour > CARD_COLOUR_MAX) {
				ERROR_LOG("Card[%d] Colour configuration Error", i);
				goto exit;
			}
			if (new_card[i].value > CARD_VALUE_MAX) {
				ERROR_LOG("Card[%d] Value configuration Error", i);
				goto exit;
			}
			if (new_card[i].event > CARD_EVENT_MAX) {
				ERROR_LOG("Card[%d] Event configuration Error", i);
				goto exit;
			}
			if (new_card[i].star> CARD_STAR_MAX) {
				ERROR_LOG("Card[%d] star configuration Error", i);
				goto exit;
			}
			++i;
		}
		if (i == cards_num)
			break;
		cur = cur->next;
	}

	if (i != cards_num) {
		ERROR_LOG ("parse %s failed, cards Count=%d, get Count=%d",
				file, cards_num, i);
		goto exit;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}



int card_do_init(sprite_t* p, const uint8_t *body, int bodylen)
{
	DEBUG_LOG("CARD DO INIT [%d]", p->id);
	int my_card[31];
	//user will get 12 effective cards
	my_card[0] = 12;
	//stable first 12 card ids
	my_card[1] = 2;
	my_card[2] = 4;
	my_card[3] = 7;
	my_card[4] = 9;
	my_card[5] = 22;
	my_card[6] = 23;
	my_card[7] = 26;
	my_card[8] = 28;
	my_card[9] = 40;
	my_card[10] = 41;
	my_card[11] = 45;
	my_card[12] = 46;
	int i = 0;
	srand(time(NULL));
	int cnt = 0;
	for(i = 0; i < CARD_ID_MAX; i++) {
		if (card[i].value <= 10) {
			int j;
			int is_exist = 0;
			for (j = 0; j < 12; j++) {
				if ( card[i].id == my_card[j + 1]) {
					is_exist = 1;
				}
			}
			if (!is_exist) {
				my_card[cnt + 13] = card[i].id;
				cnt ++;
			}
		}
	}
	//randomize 17 times
	int random_cnt = 17;
	for (i = 0; i < random_cnt; i++) {
		int j = (int) ((double)random_cnt*rand()/(RAND_MAX + 1.0));
		int temp_id = my_card[random_cnt - i + 13];
		my_card[random_cnt - i + 13] = my_card[j + 13];
		my_card[j + 13] = temp_id;
	}
	return send_request_to_db(SVR_PROTO_CARD_DO_INIT, p, sizeof(my_card), my_card, p->id);
}

int card_get_info(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
	uint32_t user_id;
	int j =0;
	UNPKG_UINT32(body, user_id, j);
	DEBUG_LOG("GET MOLE CARD INFO REQUEST [%d %u]", p->id, user_id);
	return send_request_to_db(SVR_PROTO_CARD_GET_INFO, p, 0,0, user_id);

}

int add_basic_card(sprite_t* p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_CARD_ADD_BASIC, p, 0, 0, p->id);
}

int add_basic_card_avail(sprite_t* p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_CARD_ADD_AVAIL, p, 0, 0, p->id);
}

int card_do_init_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	do_buy_item(p, 160191, 1, 1, 0);
	do_buy_item(p, 12369, 1, 1, 0);

	uint32_t buff[2];
	buff[0] = 1; //count
	buff[1] = p->id;
	msglog(statistic_logfile, 0x02080000 + 12369, get_now_tv()->tv_sec, buff, sizeof(buff));
	/*表示没有领取过卡牌册*/
	*(uint32_t *)p->session = 0;
	/*int l = sizeof (protocol_t);
	init_proto_head(msg, PROTO_CARD_DO_INIT, l);
	send_to_self(p, msg, l, 1);*/
	return new_card_do_init(p, p->id);
}

/* @brief 初始化新的卡牌数据
 */
int new_card_do_init(sprite_t *p, uint32_t id)
{

	struct card_id_num {
		uint32_t num;
		uint32_t card_id[30];
	}__attribute__((packed)) new_card;

	new_card.num = 9;
	int i = 0;
	int j = 0;
	int index = 0;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			new_card.card_id[index] = 20 * i + j;
			index++;
		}
	}

	return send_request_to_db(SVR_PROTO_INIT_NEW_CARD, p, sizeof(new_card), &new_card, id);
}

/* @brief 卡牌初始化返回包，session值为0表示领取新旧开牌，如果为1表示领取新卡牌
 */
int new_card_do_init_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t step = *(uint32_t *)p->session;
	response_proto_uint32(p, p->waitcmd, step, 0);
	return 0;
}

int card_get_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	if (p->waitcmd == PROTO_FIX_CARD_GAME_BUG) {
		return get_card_game_info_callback(p, id, buf, len);
	}
	cards_info_t *my_cards = (cards_info_t*)buf;
	int i = 0;
	int ex_len = sizeof(uint32_t) * (my_cards->total_cnt + 6);
    CHECK_BODY_LEN(len, ex_len);
	int l = sizeof (protocol_t);

	int cnt = my_cards->total_cnt;
	PKG_UINT32(msg, (uint32_t)my_cards->flag,l);
	PKG_UINT32(msg, (uint32_t)my_cards->win_cnt,l);
	PKG_UINT32(msg, (uint32_t)my_cards->lost_cnt,l);
	PKG_UINT32(msg, cnt, l);
	for(i = 0; i < cnt; i++) {
		PKG_UINT8(msg, (uint8_t)(card[my_cards->id[i]].type + 1), l);
		PKG_UINT8(msg, card[my_cards->id[i]].value, l);
		PKG_UINT8(msg, card[my_cards->id[i]].event, l);
		PKG_UINT8(msg, card[my_cards->id[i]].colour, l);
		PKG_UINT8(msg, card[my_cards->id[i]].star, l);
	}
	if (p->waitcmd == PROTO_CARD_GET_INFO) {
		memcpy(p->session, msg + sizeof(protocol_t), l - sizeof(protocol_t));
		p->sess_len = l - sizeof(protocol_t);
		return send_request_to_db(SVR_PROTO_GET_NEW_CARD, p, 0, NULL, p->id);
	}
    DEBUG_LOG("MOLE CARD INFO [%d %d]", p->id, cnt);
	init_proto_head(msg, PROTO_CARD_GET_INFO, l);
	send_to_self(p, msg, l, 1);
	return 0;
}

/* @brief 返回用户的新旧卡牌信息
 */
int old_new_card_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	struct new_card_info {
		uint32_t num;
		uint32_t card_id[100];
	}__attribute__((packed));

	int l = sizeof (protocol_t);
	memcpy(msg + l, p->session, p->sess_len);
	l += p->sess_len;
	struct new_card_info *p_new_card = (struct new_card_info *)(buf + 24);
	PKG_UINT32(msg, p_new_card->num, l);
	int i = 0;
	for(i = 0; i < p_new_card->num; i++) {
		PKG_UINT8(msg, (uint8_t)(card[p_new_card->card_id[i]].type + 1), l);
		PKG_UINT8(msg, new_card[p_new_card->card_id[i]].value, l);
		PKG_UINT8(msg, new_card[p_new_card->card_id[i]].event, l);
		PKG_UINT8(msg, new_card[p_new_card->card_id[i]].colour, l);
		PKG_UINT8(msg, new_card[p_new_card->card_id[i]].star, l);
	}
	init_proto_head(msg, PROTO_CARD_GET_INFO, l);
	send_to_self(p, msg, l, 1);
	return 0;
}


int add_basic_card_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
	int l = sizeof (protocol_t);
	uint32_t card_id = *((uint32_t*)buf);
	DEBUG_LOG("ADD BASIC CARD CALLBACK [%d %d]", p->id, card_id);
	int flag = 1;
	PKG_UINT32(msg, flag, l);
	PKG_UINT8(msg, (uint8_t)(card[card_id].type +1), l);
	PKG_UINT8(msg, card[card_id].value, l);
	PKG_UINT8(msg, card[card_id].event, l);
	PKG_UINT8(msg, card[card_id].colour, l);
	PKG_UINT8(msg, card[card_id].star, l);
	init_proto_head(msg, PROTO_CARD_ADD_BASIC, l);
	send_to_self(p, msg, l, 1);
	return 0;
}


int add_basic_card_avail_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
	int l = sizeof (protocol_t);
	uint32_t cnt= *((uint32_t*)(buf));
	DEBUG_LOG("CARD AVAIL COUNT [%d %u]",p->id, cnt);
	PKG_UINT32(msg, cnt, l);
	init_proto_head(msg, PROTO_CARD_CHECK_AVAIL_NUM, l);
	send_to_self(p, msg, l, 1);
	return 0;

}

int get_new_card_exp_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen,0);
	return send_request_to_db(SVR_PROTO_GET_NEW_CARD_EXP,p,0,NULL, p->id);
}

int get_new_card_exp_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t step = 1;
	uint32_t exp = *(uint32_t*)buf;
	uint32_t flag = *(uint32_t*)(buf + 4);

	if (p->waitcmd == PROTO_GET_NEW_CARD_FLAG) {
		uint32_t userid = *(uint32_t *)p->session;
		response_proto_uint32_uint32(p, p->waitcmd, userid, flag, 0);
		return 0;
	}

	switch(exp)
	{
		case 0 ... 499:
			step = 1;
			break;
		case 500 ... 1499:
			step = 2;
			break;
		case 1500 ... 2999:
			step = 3;
			break;
		case 3000 ... 4999:
			step = 4;
			break;
		default:
			exp = 5000;
			if(flag == 0) {
				step = 5;
			} else {
				step = 6;
			}
			break;
	}

	int bytes = sizeof(protocol_t);
	PKG_UINT32(msg,step,bytes);
	PKG_UINT32(msg,exp,bytes);
	init_proto_head(msg,p->waitcmd,bytes);
	return send_to_self(p,msg,bytes,1);
}

/* @brief 跟NPC对话，送用户卡牌
 */
int add_new_card_by_npc_talk_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	uint32_t db_buf[] = {1, 12364};
	return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, sizeof(db_buf), db_buf, p->id);
}

/* @brief 如果有红衣，送卡牌
 */
int check_red_clothe_callback(sprite_t *p, uint32_t id, char *buf, int bodylen)
{
	uint32_t count = 0;
	uint32_t item_id = 0;
	uint32_t item_count = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t *)buf, &count, &item_id, &item_count);

	if (item_id == 12364 && item_count == 1) {
		return send_request_to_db(SVR_PROTO_GET_NEW_CARD, p, 0, NULL, p->id);
	}
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 得到用户新卡牌的数据返回包处理
 */
int get_new_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen)
{
	switch(p->waitcmd) {
		case PROTO_CHECK_HAVE_BASIC_CARD:
		{
			return check_if_have_basic_card_callback(p, id, buf, bodylen);
		}
		case PROTO_GET_SIG_CARD_FROM_NPC:
		{
			return add_random_card_callback(p, id, buf, bodylen);
		}
		case PROTO_CARD_GET_INFO:
		{
			return old_new_card_info_callback(p, id, buf, bodylen);
		}
		default:
			ERROR_RETURN(("WRONG CMDID %u %u", p->id, p->waitcmd), -1);
	}
	return 0;
}

/* @brife 根据用户拥有的卡牌随机一张卡牌给用户
 */
int add_random_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen)
{
	uint32_t count = 0;
	unpkg_host_uint32((uint8_t*)(buf + 24), &count);
	uint32_t all_card_id[100] = { };
	memcpy(all_card_id, (buf + 28), sizeof(all_card_id));

	uint32_t min_card_num = 9;
	if (count < min_card_num) {
		new_card_do_init(NULL, p->id);
	}

	if (!set_only_one_bit(p, 9)) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_get_card_from_tulin, 1);
	}
	struct add_num_card_id {
		uint32_t num;
		uint32_t card_id[3];
	}__attribute__((packed));
	struct add_num_card_id add_card = {3, {1240069 - 1240061, 1240089 - 1240061, 1240109 - 1240061}};
	send_request_to_db(SVR_PROTO_ADD_SEVERAL_CARD, NULL, sizeof(add_card), &add_card, p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 得到为用户加的卡牌的ID号
 */
int get_add_card_id(uint32_t all_card_id[], int count)
{
	uint32_t add_card_id[] = {1240070, 1240090, 1240110};
	int len = (sizeof(add_card_id) / sizeof(add_card_id[0]));
	random_array(add_card_id, len);
	int add_itm = 0;
	int j = 0;
	int i = 0;
	for (j = 0; j < len; j++) {
		for (i = 0; i < count; i++) {
			if (all_card_id[i] + 1240061 == add_card_id[j]) {
				break;
			}
		}
		if (i == count) {
			add_itm = add_card_id[j];
			break;
		}
	}
	return add_itm;
}

/* @brief 打乱数组
 */
void random_array(uint32_t array[], int len)
{
	int i = 0;
	for (i = 0; i < len; i++) {
		int idx_1 = rand() % len;
		int idx_2 = rand() % len;
		if (idx_1 != idx_2) {
			uint32_t temp = array[idx_1];
			array[idx_1] = array[idx_2];
			array[idx_2] = temp;
		}
	}
}

/* @brief 查询用户是否有基础卡牌
 */
int check_if_have_basic_card_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_NEW_CARD, p, 0, NULL, p->id);
}

/* 判断用户是否有基础卡牌
 */
int check_if_have_basic_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen)
{
	uint32_t min_card_num = 9;
	uint32_t count = 0;
	unpkg_host_uint32((uint8_t *)(buf + 24), &count);
	int ret = 0;
	if (count >= min_card_num) {
		ret = 1;
	}
	response_proto_uint32(p, p->waitcmd, ret, 0);
	return 0;
}

/* @brief 得到新卡片的经验值和是否打败土林长老的flag标志
 */
int get_new_card_exp_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	uint32_t userid = 0;
	CHECK_BODY_LEN(bodylen, sizeof(userid));
	unpack(body, sizeof(userid), "L", &userid);
	*(uint32_t *)p->session = userid;
	return send_request_to_db(SVR_PROTO_GET_NEW_CARD_EXP, p, 0, NULL, userid);
}

