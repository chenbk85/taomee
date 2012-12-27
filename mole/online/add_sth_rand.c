#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "add_sth_rand.h"
#include "exclu_things.h"
#include "gold_bean_serv.h"

#define  MAX_ENTRY_ID 100

static rand_t rands[MAX_ENTRY_ID];

static int parse_single_item(item_rand_unit_t* iut, int* cnt, xmlNodePtr cur)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j == MAX_RAND_EXCHANGE_ITEMS) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop(id))) {
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			if (iut[j].count < 0) {
				ERROR_RETURN(("error count=%d, item=%d", iut[j].count, id), -1);
			}

			decode_xml_prop_float_default(&iut[j].rand_start, cur, "RandStart", 0.0);
			if (iut[j].rand_start > 100.00) {
				ERROR_RETURN(("error rand_start=%f, item=%d", iut[j].rand_start, id), -1);
			}

			decode_xml_prop_float_default(&iut[j].rand_end, cur, "RandEnd", 0.0);
			if (iut[j].rand_end > 100.00 || iut[j].rand_end < iut[j].rand_start) {
				ERROR_RETURN(("error rand_end=%f, item=%d", iut[j].rand_end, id), -1);
			}
			j++;
		}
		cur = cur->next;
	}
	*cnt = j;

	return 0;
}

static int parse_itemin_itemout(rand_t *eit, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")) {
			cur = chl->xmlChildrenNode;
			if (parse_single_item(eit->in, &(eit->incnt), cur) != 0) {
				return -1;
			}
		}

		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")) {
			cur = chl->xmlChildrenNode;
			if (parse_single_item(eit->out[eit->outkind_cnt], &(eit->outcnt[eit->outkind_cnt]), cur) != 0) {
				return -1;
			}
			++(eit->outkind_cnt);
		}
		chl = chl->next;
	}

	return 0;
}

int load_rand_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load items config failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	int ex_count = 0;
	int i, err = -1;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (ex_count > MAX_ENTRY_ID - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			rands[i - 1].id = i;

			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].vip, cur, "VIP", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].day_type, cur, "DayType", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].day_count, cur, "DayCount", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].count_start, cur, "CountStart", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].count_end, cur, "CountEnd", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].day_begin, cur, "DayBegin", 0);
			DECODE_XML_PROP_INT_DEFAULT(rands[i - 1].day_end, cur, "DayEnd", 0);

			chl = cur->xmlChildrenNode;
			if ( (parse_itemin_itemout(&rands[i-1], chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}
	err = 0;

exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load exchange item file %s", file);
}

/* @brief 随机一个物品
 * @param entry_id 配置表里的entry id号
 * @param itemout_index 多个itemout中的下标
 * @return 返回itemout中的某个itme的下标
 */
static uint32_t rand_single_item(int entry_id, int itemout_index)
{
	float rand_num = (rand() % 10000) / 100.00;
	uint32_t i = 0;
	for (i = 0; i < rands[entry_id - 1].outcnt[itemout_index]; i++) {
		if (rand_num >= rands[entry_id - 1].out[itemout_index][i].rand_start
			&& rand_num < rands[entry_id - 1].out[itemout_index][i].rand_end) {
			break;
		}
	}
	return i;
}

/* @brief 发送请求到DB
 * @param entry_id 配置表里的entry id号
 * @pram itemout_index 多个itemout里的下标号
 * @param count 扣除物品应该是配置表里的count倍，通常为1
 * @param array_off itemout里面的哪个item
 */
static int send_to_db(sprite_t *p, int entry_id, int itemout_index, int count, int array_off)
{
	uint8_t buff[1024] = { };

	int out_count = 1;
	/*没随机到*/
	if (array_off == rands[entry_id - 1].outcnt[itemout_index]) {
		out_count = 0;
	}

	int j = 0;
	PKG_H_UINT32(buff, rands[entry_id - 1].incnt, j);
	PKG_H_UINT32(buff, out_count, j);
	PKG_H_UINT32(buff, ATTR_CHG_task_fin, j);
	PKG_H_UINT32(buff, 0, j);

	int i = 0;
	for (i = 0; i != rands[entry_id - 1].incnt; ++i) {
	    if (count == 0)
		{
		    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}

		if (pkg_item_kind(p, buff, rands[entry_id -1].in[i].itm->id, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(buff, rands[entry_id - 1].in[i].itm->id, j);
		PKG_H_UINT32(buff, rands[entry_id - 1].in[i].count * count, j);
	}

	if (out_count) {
		if (pkg_item_kind(p, buff, rands[entry_id - 1].out[itemout_index][array_off].itm->id, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(buff, rands[entry_id - 1].out[itemout_index][array_off].itm->id, j);
		PKG_H_UINT32(buff, rands[entry_id - 1].out[itemout_index][array_off].count, j);
		PKG_H_UINT32(buff, rands[entry_id - 1].out[itemout_index][array_off].itm->max, j);
	}

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
}

/* @brief 校验用户传过来的数据是否正确
 * @param entry_id 配置表里的entry id号
 * @param itemout_index 多个itmeout中的哪一个
 * @param count 扣除配置表里的itemin中物品的count倍
 */
static int check_entry_id(sprite_t *p, uint32_t entry_id, uint32_t itemout_index, uint32_t count)
{
	if ((entry_id == 0) || (entry_id > MAX_ENTRY_ID) || (!rands[entry_id - 1].id)
	|| itemout_index >= MAX_EXCHG_OUT_KIND) {
		ERROR_LOG("WRONG INDEX %u %u %u %u", entry_id, itemout_index, count, p->id);
		//send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		return -1;
	}

	if (entry_id >= 13 && entry_id <= 16)
	{
	    uint32_t info_m[4] = {0};
        info_m[0] = p->id;
        info_m[1] = 1;
        info_m[2] = count;
        info_m[3] = 0;
	    msglog(statistic_logfile, 0x04020483, now.tv_sec, info_m, sizeof(info_m));
	}

    if (rands[entry_id - 1].count_start && rands[entry_id - 1].count_end)
    {
	    if (count < rands[entry_id - 1].count_start || count > rands[entry_id - 1].count_end)
	    {
	        return -1;
	    }
	}

	switch(entry_id) {
		case 1 ... 5:
		{
			const uint32_t max_count = 10000;
			uint32_t min_limit[] = {0, 10, 100, 1000, 5000};
			if (count < min_limit[entry_id - 1] || count > max_count) {
				return -1;
			}
			break;
		}
	}
	return 0;
}

/* @brief 根据概率，获取某个物品
 * @brief info  保存entry id, itemout index， count
 */
static int add_random_sth(sprite_t *p, add_sth_rand_t info, uint32_t day_cnt)
{
	uint32_t array_off = rand_single_item(info.entry_id, info.itemout_index);
    if (info.entry_id == 31 || info.entry_id == 32 || info.entry_id == 33 || info.entry_id == 34 || info.entry_id == 39)
    {
        if ((get_now_tm()->tm_wday >= 1) && (get_now_tm()->tm_wday <= 4))
        {
            if (day_cnt == 6)
            {
                array_off = 0;
            }
        }
        else
        {
             if (day_cnt == 3)
            {
                array_off = 0;
            }
        }
    }
	if(35 == info.entry_id)
	{
		if( day_cnt == ( get_now_tm()->tm_wday + 4 ) )
	 	{
	 		array_off = 0;
	 	}
		//统计拦截大盗贼参与人数
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0408A345, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if(63 == info.entry_id){
		if (day_cnt == 3)
		{
			array_off = 4;
		}
	}
	else if ( 64 == info.entry_id )	{
		//统计照镜子失败的次数
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409BDA9, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	if (send_to_db(p, info.entry_id, info.itemout_index, info.count, array_off) == -1) {
		return -1;
	}
	info.count = array_off;
	memcpy(p->session, &info, sizeof(add_sth_rand_t));
	return 0;
}

/* @brief 随机的得到某个物品
 */
int get_sth_accord_rand_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	add_sth_rand_t info = { };
	CHECK_BODY_LEN(bodylen, sizeof(add_sth_rand_t));
	/*entry_id 配置表的entry id号, itemout_index如果配置多个itemout,指的是哪个itemout,
	 count 指扣除的物品是配置表里配置的count倍*/
	unpack(buf, sizeof(add_sth_rand_t), "LLL", &info.entry_id, &info.itemout_index, &info.count);
	if( info.count == 0 ) 
	{
		info.count = 1;
	}

	DEBUG_LOG("rand info %u %u %u %u", p->id, info.entry_id, info.itemout_index, info.count);

	if (check_entry_id(p, info.entry_id, info.itemout_index, info.count) == -1) {

		ERROR_LOG("CHECK ERROR %u %u %u %u", p->id, info.entry_id, info.itemout_index, info.count);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (info.entry_id == 31 || info.entry_id == 35)
	{
	    if (!(get_now_tm()->tm_hour >= 18 && get_now_tm()->tm_hour < 21))
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }
	}
	if (info.entry_id == 32)
	{
	    if (!(get_now_tm()->tm_hour >= 19 && get_now_tm()->tm_hour < 21))
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }
	}
	if (info.entry_id == 33 || info.entry_id == 34 || info.entry_id == 39)
	{
	    if (!(get_now_tm()->tm_hour >= 20 && get_now_tm()->tm_hour < 21))
		{
			return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
		}
	}

	/*校验是否是超拉*/
	if (rands[info.entry_id - 1].vip && !ISVIP(p->flag)) {
		ERROR_LOG("NOT VIP %u", p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	//时间限制
	if( rands[info.entry_id - 1].day_begin && rands[info.entry_id - 1].day_end ) {
		uint32_t year = get_now_tm()->tm_year + 1900;
		uint32_t month= get_now_tm()->tm_mon + 1;
		uint32_t day = get_now_tm()->tm_mday;
#ifdef TW_VER
		//台湾版本延迟一周发布
		//day += 7;
#endif
		uint32_t cur_date = year*10000+month*100+day;
		if( ( cur_date < rands[info.entry_id - 1].day_begin ) || ( cur_date > rands[info.entry_id - 1].day_end ) )
		{
			return send_to_self_error( p, p->waitcmd, -ERR_cannot_ask_for_this_item, 1 );
		}
	}

	/*每天限制*/
	if (rands[info.entry_id - 1].day_type != 0) {
		memcpy(p->session, &info, sizeof(info));
		return db_set_sth_done(p, rands[info.entry_id - 1].day_type, rands[info.entry_id - 1].day_count, p->id);
	}

	if (info.entry_id == 18) {
		memcpy(p->session, &info, sizeof(info));
		return send_request_to_db(SVR_PROTO_GET_BEAN_LOTTERY_COUNT, p, 0, NULL, p->id);
	}

	if (info.entry_id == 40) {
		memcpy(p->session, &info, sizeof(info));
		uint32_t type = info.entry_id;
		return send_request_to_db(SVR_PROTO_ROOMINFO_GET_RANDOM_GAIN_TIMES, p, 4, &type, p->id);
	}
	
	return add_random_sth(p, info, 0);
}

/* @brief 每天限制的回调函数
 */
int day_limit_callback(sprite_t *p, uint32_t day_cnt)
{
	add_sth_rand_t info = { };
	memcpy(&info, p->session, sizeof(info));
	
	return add_random_sth(p, info, day_cnt);
}

/* @brief 随机购买物品的回调函数
 */
int get_sth_accord_rand_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t entry_id = 0;
	uint32_t itemout_index = 0;
	uint32_t array_off = 0;
	uint32_t count = 0;

	unpkg_host_uint32_uint32_uint32((uint8_t*)p->session, &entry_id, &itemout_index, &array_off);
	if (array_off == rands[entry_id - 1].outcnt[itemout_index]) {
		response_proto_uint32_uint32_uint32(p, p->waitcmd, entry_id, 0, 0, 0);
	} else  {
		response_proto_uint32_uint32_uint32(p, p->waitcmd, entry_id,
	    rands[entry_id - 1].out[itemout_index][array_off].itm->id,
		rands[entry_id - 1].out[itemout_index][array_off].count, 0);
		
//		add_other_gifts(p, entry_id, rands[entry_id - 1].out[itemout_index][array_off].itm->id);

		if (entry_id >= 13 && entry_id <= 16)
	    {
	        if (rands[entry_id - 1].out[itemout_index][array_off].itm->id == 16012)
	        {
	            uint32_t info_m[4] = {0};
                info_m[0] = p->id;
                info_m[1] = 1;
                info_m[2] = 0;
                info_m[3] = rands[entry_id - 1].out[itemout_index][array_off].count;
	            msglog(statistic_logfile, 0x04020483, now.tv_sec, info_m, sizeof(info_m));
	        }
	    }

		count = rands[entry_id - 1].out[itemout_index][array_off].count;
	}

	if(entry_id == 18) {
		uint32_t msg_buff[3] = {p->id,1,count};
		msglog(statistic_logfile,0x04020482,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(entry_id == 30) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0406FC53,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		msglog(statistic_logfile,0x0406FC54,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
    else if(entry_id == 32) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x04088335,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 33) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0408A338,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 34) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0408A342,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 42) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0409B45C,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 43) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0409B45F,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 45) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0409B490,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 46) {
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile,0x0409B495,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 55) {
	    uint32_t msg_buff[2] = {p->id, 1};
	    if (ISVIP(p->flag)){
		    msglog(statistic_logfile, 0x0409BD92, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
		else{
		    msglog(statistic_logfile, 0x0409BD93, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if (entry_id == 63) {
	    uint32_t msg_buff[2] = {p->id, 1};
	    msglog(statistic_logfile, 0x0409C3DD, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (entry_id == 40) {
		//需要广播的物品列表
		const uint32_t angel_id[] = {1353303, 1353310};
		int i = 0;
		for (i = 0; i < sizeof(angel_id) / sizeof(uint32_t); i++) {
			if (angel_id[i] == rands[entry_id - 1].out[itemout_index][array_off].itm->id) {
				char txt[256] = {0};
#ifndef TW_VER
				int msg_len = sprintf(txt, "恭喜%s在淘淘乐街缤纷弹弹弹抽出%s", p->nick, rands[entry_id - 1].out[itemout_index][array_off].itm->name);
#else
				int msg_len = sprintf(txt, "恭喜%s在淘淘樂街繽紛轉轉彈抽出%s", p->nick, rands[entry_id - 1].out[itemout_index][array_off].itm->name);
#endif
				tell_flash_some_msg_across_svr(p, CBMT_TAOTAOLE_DRAW, msg_len, txt);
				break;
			}
		}
	}
	else if( entry_id == 74 )
	{
		//圣诞超拉礼盒
		uint32_t msgbuff[2] = { p->id, 1 };
		msglog( statistic_logfile, 0x0409BF37, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	else if (entry_id == 79) {
	    uint32_t msg_buff[2] = {p->id, 1};
	    msglog(statistic_logfile, 0x0409C370, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	
	return 0;
}


int get_bean_lottery_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t flag;
	UNPKG_H_UINT32(buf, flag, i);

	uint32_t entry_id = 0;
	uint32_t itemout_index = 0;
	uint32_t count = 0;
	uint32_t array_off = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t*)p->session, &entry_id, &itemout_index, &count);
	if (flag == 0) {
		array_off = rand_single_item(entry_id, itemout_index);
	} else {
		array_off = 6;		//array_off = 6时送500个
	}

	*(uint32_t *)(p->session + 8) = array_off;

	send_request_to_db(SVR_PROTO_SET_BEAN_LOTTERY_COUNT, NULL, 0, NULL, p->id);
	return send_to_db(p, entry_id, itemout_index, count, array_off);
}


int get_gold_compass_chance_data_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 8);
	int i = 0;
	uint32_t m_flag = 0;
	uint32_t y_flag = 0;
	UNPKG_H_UINT32(buf, m_flag, i);
	UNPKG_H_UINT32(buf, y_flag, i);

	uint32_t entry_id = 0;
	uint32_t itemout_index = 0;
	uint32_t count = 0;
	uint32_t array_off = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t*)p->session, &entry_id, &itemout_index, &count);

	array_off = rand_single_item(entry_id, itemout_index);
	if (entry_id == 63){
	    if (array_off == 9){
	        if (m_flag){
	            array_off = 9;
	        }
	        else{
	            array_off = 8;
	        }
	    }
	    else if (array_off == 10){
	        if (y_flag){
	            array_off = 10;
	        }
	        else{
	            array_off = 8;
	        }
	    }
    }

	*(uint32_t *)(p->session + 8) = array_off;

	
	return send_to_db(p, entry_id, itemout_index, count, array_off);
}



int get_random_gain_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	int i = 0;
	uint32_t times = 0;
	UNPKG_H_UINT32(buf, times, i);

	uint32_t entry_id = 0;
	uint32_t itemout_index = 0;
	uint32_t count = 0;
	uint32_t array_off = 0;
	unpkg_host_uint32_uint32_uint32((uint8_t*)p->session, &entry_id, &itemout_index, &count);
	if (times == 10) {
	    int rand_value = rand()%10;
	    if (rand_value < 5){
		    array_off = 2;
		}
		else{
		    array_off = 3;
		}
	}
	else if (times == 20) {
		array_off = 4;
	}
	else
	{
	    array_off = rand_single_item(entry_id, itemout_index);
	}

	*(uint32_t *)(p->session + 8) = array_off;

	uint32_t flag = 0;
	if (times == 20){
	    flag = 1;
	}
	uint32_t  dbbuf[2] = {entry_id, flag};
	send_request_to_db(SVR_PROTO_ROOMINFO_CHANGE_RANDOM_GAIN_TIMES, NULL, sizeof(dbbuf), dbbuf, p->id);

	return send_to_db(p, entry_id, itemout_index, count, array_off);
}

int add_other_gifts(sprite_t* p, uint32_t entry_id, uint32_t itemid)
{
	if (entry_id == 63) {//黄金罗盘，送包月抵用券
		uint32_t ticket_type = 0;
		if (itemid == 1351286) {//5元
			ticket_type = 2;
		} else if (itemid == 1351287) {//20元
			ticket_type = 3;
		} else if (itemid == 1351288) {//2元
			ticket_type = 1;
		}
		if (ticket_type != 0) {
			add_vip_month_ticket(NULL, ticket_type, p->id);
		}
	}

	if (entry_id == 63) {//黄金罗盘，送超拉		
		struct user_info_t{
            uint32_t	flag;
            char		nick[USER_NICK_LEN];
	    }__attribute__((packed));

	    struct user_info_t info = {};
	    memcpy(info.nick, p->nick, USER_NICK_LEN);
    	if (itemid == 1351283){
	            info.flag = 1;
	            send_request_to_db(SVR_PROTO_SYSARG_SET_GOLD_COMPASS_DATA, NULL, sizeof(struct user_info_t), &info, p->id);
	    		char txt[256] = {0};
#ifndef TW_VER
				int msg_len = sprintf(txt, "恭喜%s获得超级拉姆一个月大奖。",p->nick);
				send_postcard("克劳神父", 0, p->id, 1000302, "", 0);
#else
				int msg_len = sprintf(txt, "恭喜%s獲得超級拉姆一個月大獎。",p->nick);
				send_postcard("克勞神父", 0, p->id, 1000302, "", 0);
#endif
				tell_flash_some_msg_across_svr(p, CBMT_GOLD_COMPASS_MONTH, msg_len, txt);
	    }
	    else if (itemid == 1351284){
	            info.flag = 2;
	            send_request_to_db(SVR_PROTO_SYSARG_SET_GOLD_COMPASS_DATA, NULL, sizeof(struct user_info_t), &info, p->id);
	    		char txt[256] = {0};
#ifndef TW_VER
				int msg_len = sprintf(txt, "恭喜%s获得超级拉姆一整年超级大奖。",p->nick);
				send_postcard("克劳神父", 0, p->id, 1000303, "", 0);
#else
				int msg_len = sprintf(txt, "恭喜%s獲得超級拉姆一整年超級大獎。",p->nick);
				send_postcard("克勞神父", 0, p->id, 1000303, "", 0);
#endif
				tell_flash_some_msg_across_svr(p, CBMT_GOLD_COMPASS_YEAR, msg_len, txt);

	    }
    }

	return 0;
}

