#include "proto.h"
#include "item.h"
#include "fight_prize.h"
#include "dbproxy.h"

prize_range_t	prize_range[MAX_PRIZE_TYPE];
uint32_t		prize_range_cnt = 0;
prize_t			all_prize[MAX_PRIZE_COUNT];
uint32_t		all_prize_cnt = 0;

rand_prize_type_t	all_prize_type[MAX_PRIZE_TYPE_COUNT];
uint32_t		all_prize_type_cnt = 0;

fight_barrier_t	all_barrier[MAX_BARRIER_COUNT];
uint32_t		all_barrier_cnt = 0;
map_barrier_info_t map_barrier[MAX_FIGHT_MAP_COUNT];
uint32_t		all_map_cnt;

uint32_t rand_range_uint32(uint32_t a, uint32_t b)
{
	return (uint32_t)(rand() % ((b) - (a)) + (a));
}

uint32_t rand_in_two_things(uint32_t a, uint32_t b)
{
	return (rand() % 2) ? a : b;
}

uint32_t rand_in_three_things(uint32_t a, uint32_t b, uint32_t c)
{
	uint32_t val = rand() % 3;
	return (val == 0) ? (a) : ((val == 1) ? (b) : (c));
}

int cmp(const void *v1, const void *v2)
{
	return *(uint32_t*)v1 - *(uint32_t*)v2;
}

#define PACK_PRIZE_BUFF(buff, itm, len) \
	do { \
		buff[len++] = (itm->prize_id); \
		buff[len++] = (itm->prize_cnt); \
		buff[len++] = (itm->rate); \
		DEBUG_LOG("prize_id=%d prize_cnt=%d rate=%d len=%d", buff[len - 3], buff[len - 2], buff[len - 1],  len); \
	} while(0)

int load_prize_conf(const char* file)
{
    //TODO
    int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN( ( "Prize::load_prize_conf() Open [%s] Failed", file ), -1 );
	}

	cur = xmlDocGetRootElement(doc);
    if (!cur) {
        ERROR_LOG ("xmlDocGetRootElement error");
        ret = -1;
        goto exit;
    }

	int range_idx = 0;
	int prize_idx = 0;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"PRIZES"))) {
			if (range_idx >= MAX_PRIZE_TYPE) {
				ret = -1;
				ERROR_LOG("too many prize kind error");
				goto exit;
			}
			prize_range[range_idx].start_idx = prize_idx;

			uint32_t def_cnt = 0;
			DECODE_XML_PROP_UINT32(prize_range[range_idx].prize_type, cur, "prize_type");
			DECODE_XML_PROP_UINT32(prize_range[range_idx].start_id, cur, "start_id");
			DECODE_XML_PROP_UINT32(prize_range[range_idx].end_id, cur, "end_id");
			DECODE_XML_PROP_INT_DEFAULT(def_cnt, cur, "prize_cnt", 1);

			xmlNodePtr child = cur->xmlChildrenNode;
			uint32_t cur_prize_id = prize_range[range_idx].start_id;
			while (child) {
				if ((!xmlStrcmp(child->name, (const xmlChar *)"Prize"))) {
					if (prize_idx >= MAX_PRIZE_COUNT) {
						ret = -1;
						ERROR_LOG("too many prize count error");
						goto exit;
					}
					DECODE_XML_PROP_UINT32(all_prize[prize_idx].ID, child, "ID");

					//判断配表是否按照顺序,并且是否在范围内
					if (cur_prize_id != all_prize[prize_idx].ID || cur_prize_id > prize_range[range_idx].end_id) {
						ret = -1;
						ERROR_LOG("load_prize_conf: cur_prize_id = %u now_id=%u start= %u end=%u", cur_prize_id, all_prize[prize_idx].ID, prize_range[range_idx].start_id, prize_range[range_idx].end_id);
						goto exit;
					}
					DECODE_XML_PROP_UINT32(all_prize[prize_idx].prize_id,  child, "prize_id");
					item_t *itm = get_item_prop(all_prize[prize_idx].prize_id);
					if (!itm) {
						ret = -1;
						ERROR_LOG("load_prize_conf: wrong prizeid = %u", all_prize[prize_idx].prize_id);
						goto exit;
					}

					DECODE_XML_PROP_INT_DEFAULT(all_prize[prize_idx].prize_cnt,  child, "prize_cnt", def_cnt);
					DECODE_XML_PROP_INT_DEFAULT(all_prize[prize_idx].level,  child, "level", 1);
					DECODE_XML_PROP_INT_DEFAULT(all_prize[prize_idx].vip,  child, "vip", 0);
					DECODE_XML_PROP_INT_DEFAULT(all_prize[prize_idx].win,  child, "win", 0);
					DECODE_XML_PROP_INT_DEFAULT(all_prize[prize_idx].rate,  child, "rate", 30);
					prize_idx++;
					cur_prize_id++;
				}
				child = child->next;
			}
			prize_range[range_idx].end_idx = prize_idx;
			prize_range[range_idx].count = prize_range[range_idx].end_idx - prize_range[range_idx].start_idx;

			range_idx++;
		}
		cur = cur->next;
	}
	prize_range_cnt = range_idx;
	all_prize_cnt = prize_idx;

exit:
	xmlFreeDoc(doc);
	BOOT_LOG (ret, "Load Prize item file %s Size Is:[%u] [%u]", file, prize_range_cnt, all_prize_cnt);
}

int load_prize_type_conf(const char* file)
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN( ( "Prize::load_prize_type_conf() Open [%s] Failed", file ), -1 );
	}

	cur = xmlDocGetRootElement(doc);
    if (!cur) {
        ERROR_LOG ("xmlDocGetRootElement error");
        ret = -1;
        goto exit;
    }

	int type_idx = 0;
	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Prize_Type"))) {
			if (type_idx >= MAX_PRIZE_TYPE_COUNT) {
				ERROR_LOG("too many prize type error");
				ret = -1;
				goto exit;
			}
			DECODE_XML_PROP_INT(all_prize_type[type_idx].ID,  cur, "ID");

			int i;
			for (i = 1; i <= 5; i++) {
				char strname[20] = {0};
				sprintf(strname, "Card%1d_Type", i);

				xmlChar* str = xmlGetProp(cur, (xmlChar*)(strname));

				if (str == NULL) {
					ERROR_LOG("load_prize_type_conf has a name error");
					ret = -1;
					goto exit;
				}

				uint32_t len = 0;
				uint32_t slen = strlen((const char*)str);
				uint32_t prize_id;
				char c[12] = {0};
				int cnt = 0;

				while (len < slen) {
					int l = 0;
					sscanf((const char*)str + len, "%d%n", &prize_id, &l);
					len += l;
					if (len < slen) {
						sscanf((const char*)str + len, "%[^0-9]%n", c, &l);
						len += l;
					}
					if (prize_id > MAX_PRIZE_TYPE) {
						ERROR_LOG("error read prize id");
						ret = -1;
						goto exit;
					}
					all_prize_type[type_idx].prize_array[i-1].prize_list[cnt++] = prize_id;
					if (cnt >= 5) {
						ERROR_LOG("too many prize out count error");
						ret = -1;
						goto exit;
					}
				}
				all_prize_type[type_idx].prize_array[i-1].count = cnt;
			}

			type_idx++;
		}
		cur = cur->next;
	}


exit:
	xmlFreeDoc(doc);
	BOOT_LOG (ret, "Load Prize Type file %s Size Is:[%u]", file, type_idx);
}

static int parse_barrier_drop_prize(xmlNodePtr child)
{
	while (child) {
		if ((!xmlStrcmp(child->name, (const xmlChar *)"Drop"))) {
			xmlChar* str = xmlGetProp(child, (xmlChar*)("ID"));
			if (str == NULL) {
				ERROR_LOG("Prize::load_barrier_conf() has a name error");
				return -1;
			}

			uint32_t len = 0;
			uint32_t slen = strlen((const char*)str);
			uint32_t prize_id;
			char c[12] = {0};
			int cnt = 0;
			uint32_t nidx[MAX_PRIZE_TYPE + 1] = {0};

			while (len < slen) {
				int l = 0;
				sscanf((const char*)str + len, "%d%n", &prize_id, &l);
				len += l;
				if (len < slen) {
					sscanf((const char*)str + len, "%[^0-9]%n", c, &l);
					len += l;
				}
				if ((prize_id / 1000) == 0 || (prize_id / 1000) > MAX_PRIZE_TYPE) {
					ERROR_LOG("error read prize id");
					return -1;
				}
				prize_t* prize = get_prize_from_prize_idx(prize_id);
				if (!prize) {
					ERROR_LOG("parse_barrier_drop_prize prize idx[%u] error", prize_id);
					return -1;
				}

				all_barrier[all_barrier_cnt].prize_list[cnt++] = prize_id;
				nidx[prize_id/1000]++;
				if (cnt > MAX_BARRIER_PRIZE_COUNT) {
					ERROR_LOG("parse_barrier_drop_prize: too many prize type");
					return -1;
				}
			}
			all_barrier[all_barrier_cnt].prize_cnt = cnt;
			qsort(all_barrier[all_barrier_cnt].prize_list, cnt, sizeof(uint32_t), cmp);

			int i = 0;
			for (i = 1; i <= MAX_PRIZE_TYPE; i++) {
				all_barrier[all_barrier_cnt].prize_idx[i] = all_barrier[all_barrier_cnt].prize_idx[i - 1] + nidx[i - 1];
			}
		}
		child = child->next;
	}
	return 0;
}

static int parse_map_barrier_info(xmlNodePtr cur)
{
	int map_barrier_idx = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Level"))) {
			if (all_barrier_cnt >= MAX_BARRIER_COUNT) {
				ERROR_LOG("parse_map_barrier_info max barrier error");
				return -1;
			}

			DECODE_XML_PROP_INT(all_barrier[all_barrier_cnt].ID,  cur, "ID");
			DECODE_XML_PROP_INT(all_barrier[all_barrier_cnt].exp_win,  cur, "ExpWin");
			DECODE_XML_PROP_INT(all_barrier[all_barrier_cnt].exp_lose,  cur, "ExpLose");
			DECODE_XML_PROP_INT(all_barrier[all_barrier_cnt].finish,  cur, "CPoint");
			DECODE_XML_PROP_INT(all_barrier[all_barrier_cnt].prize_type,  cur, "PrizeType");
			DECODE_XML_PROP_INT_DEFAULT(all_barrier[all_barrier_cnt].lose_prize_type,  cur, "LosePrizeType", 0);

			//将关卡ID加入到地图信息中
			map_barrier[all_map_cnt].barrier_id[map_barrier_idx++] = all_barrier[all_barrier_cnt].ID;

			if (parse_barrier_drop_prize(cur->xmlChildrenNode) == -1) {
				return -1;
			}
			all_barrier_cnt++;
		}
		cur = cur->next;
	}
	map_barrier[all_map_cnt].barrier_cnt = map_barrier_idx;
	return 0;
}

int load_barrier_conf(const char* file)
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN( ( "Prize::load_barrier_conf() Open [%s] Failed", file ), -1 );
	}

	cur = xmlDocGetRootElement(doc);
    if (!cur) {
        ERROR_LOG ("xmlDocGetRootElement error");
        ret = -1;
        goto exit;
    }

	uint32_t count = 0;
	DECODE_XML_PROP_INT( count, cur, "Count");
	if( count <= 0 ) {
        goto exit;
    }


	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Map"))) {
			if (all_map_cnt > MAX_FIGHT_MAP_COUNT) {
				ERROR_LOG("max map count error");
				ret = -1;
				goto exit;
			}
			DECODE_XML_PROP_INT(map_barrier[all_map_cnt].map_id,  cur, "ID");
			if (parse_map_barrier_info(cur->xmlChildrenNode) == -1) {
				ret = -1;
				goto exit;
			}
			all_map_cnt++;
		}
		cur = cur->next;
	}

exit:
	xmlFreeDoc(doc);
	BOOT_LOG(ret, "Load fight barrier file %s mapsize:[%u] size:[%u]", file, all_map_cnt, all_barrier_cnt);
}

/*
 * @brief 通过地图ID得到地图的关卡
 */
map_barrier_info_t* get_map_barrier_from_id(uint32_t map_id)
{
	int i = 0;
	for (i = 0; i < all_map_cnt; i++) {
		if (map_barrier[i].map_id == map_id) {
			return (map_barrier + i);
		}
	}
	return NULL;
}

/*
 * @brief 得到kind类型的奖励范围
 */
prize_range_t* get_prize_range_from_kind(uint32_t kind)
{
	if (kind > prize_range_cnt) {
		ERROR_LOG( "GetPrizeRange invalid kind:[%d]", kind );
		return NULL;
	}
	return prize_range + (kind - 1);
}

/*
 * @brief 根据prize_idx的得到prize
 */
prize_t* get_prize_from_prize_idx(uint32_t idx)
{
	uint32_t kind = idx / 1000;
	prize_range_t* range = get_prize_range_from_kind(kind);
	//DEBUG_LOG("get_prize_from_prize_idx: idx=%u std_id=%u count=%u", idx, range->start_id, range->count);
	if (range != NULL && idx - range->start_id < range->count) {
		return all_prize + (range->start_idx + (idx - range->start_id));
	}

	ERROR_LOG( "get_prize_from_prize_idx invalid idx:[%u]", idx );
	return NULL;
}

/*
 * @brief 将奖励物品itm打包到buf中(没有用到)
 * @return: 0成功 -1失败
 */
int pack_prize_buff(uint32_t* buff, prize_t* itm, int* len)
{
	if (itm == NULL) {
		return -1;
	}
	buff[(*len)++] = itm->prize_id;
	buff[(*len)++] = itm->prize_cnt;
	buff[(*len)++] = itm->rate;
	DEBUG_LOG("prize_id=%d prize_cnt=%d rate=%d len=%d", buff[*len - 3], buff[*len - 2], buff[*len - 1], *len);
	return 0;
}

/*
 * @brief 得到某一关卡内的kind类型的奖励列表
 * @param barrier_id:关卡ID,从0开始
 * @param kind:奖励类型
 * @param *count:返回的改类型的数量
 * @return:
 */
uint32_t* get_one_type_prize_in_barrier(uint32_t barrier_id, uint32_t kind, uint32_t* count)
{
	*count = all_barrier[barrier_id].prize_idx[kind + 1] - all_barrier[barrier_id].prize_idx[kind];
	if (*count != 0) {
		return all_barrier[barrier_id].prize_list + all_barrier[barrier_id].prize_idx[kind];
	}
	return NULL;
}

/*
 * @prief 游戏结算框：游戏结束后得到奖励的物品列表
 * @prama *buff：奖励的物品列表
 * @prama levelID:关卡ID
 * @param win：战斗是否胜利
 * @param exp_lvl：玩家等级（暂时没有用到，关卡已经限制了等级，奖励物品就不用限制了）
 * @return :0成功 -1失败
 */
int get_prize_list(void *buff, uint32_t barrier_id, uint32_t win, uint32_t exp_lvl)
{
	int ret = 0;
	int len = 0;

	if (barrier_id >= all_barrier_cnt) {
		ERROR_LOG("invalid barrier_id:%u ", barrier_id);
		return -1;
	}
	uint32_t prizetype = all_barrier[barrier_id].prize_type;
	if (win == 0 && all_barrier[barrier_id].lose_prize_type != 0) {
		prizetype = all_barrier[barrier_id].lose_prize_type;
	}
	DEBUG_LOG("GetPrizeList: levelID=%d, prizetype = %d", barrier_id, prizetype);

	//随机生成5中奖励
	uint32_t type_array[MAX_PRIZE_OUT_COUNT] = {0};
	uint32_t type_cnt[MAX_PRIZE_TYPE + 1] = {0};
	int idx;
	for (idx = 0; idx < MAX_PRIZE_OUT_COUNT; idx++) {
		type_array[idx] = (uint32_t)rand_one_prize_type( (all_prize_type[prizetype - 1].prize_array) + idx);
		type_cnt[type_array[idx]] += 1;
	}
	DEBUG_LOG("rand prize type: %u %u %u %u %u", type_array[0], type_array[1], type_array[2], type_array[3], type_array[4]);

	uint32_t prize_list[5] = {0};
	uint32_t* plist = prize_list;
	uint32_t* tmpbuff = (uint32_t*)buff ;

	//for 如果有个类型有值
	for (idx = 1; idx <= MAX_PRIZE_TYPE; idx++) {
		if (type_cnt[idx] > 0) {
			ret = rand_one_type_barrier_prize(plist, barrier_id, idx, type_cnt[idx]);//生成type_cnt[idx]个卡片

			//打包
			uint32_t j;
			for (j = 0; j < type_cnt[idx] && ret != -1; j++) {
				prize_t* prize = get_prize_from_prize_idx(plist[j]);
				//DEBUG_LOG("get prize %u from idx: %u", prize->prize_id, plist[j]);

				if (prize != NULL) {
					PACK_PRIZE_BUFF(tmpbuff, prize, len);
				} else {
					return -1;
				}
			}
			plist = plist + type_cnt[idx];
		}
	}

	return ret;
}

/*
 * @brief 关卡barrier_id中，随机得到kind类型的奖励cnt个，放到prize_list中
 * @param *prize_list:存放奖品的列表
 * @param barrier_id:关卡ID
 * @param kind:类型
 * @param cnt:数量
 * @return: 0成功 -1失败
 */
int rand_one_type_barrier_prize(uint32_t* prize_list, uint32_t barrier_id, uint32_t kind, uint32_t cnt)
{
	if (cnt > 5) {
		ERROR_LOG("GetRandPrize: count out of range");
		return -1;
	}

	uint32_t size = 0;
	uint32_t* p_list = get_one_type_prize_in_barrier(barrier_id, kind, &size);
	int ret = rand_some_prize_in_array(prize_list, p_list, size, cnt);

	if (ret == -1) {
		ERROR_LOG("GetRandPrize  error");
	}
	return ret;
}

/*
 * @brief 在prize_type_array_t中随机得到其中的一个
 */
int rand_one_prize_type(prize_type_array_t* type_array)
{
	if (type_array->count > 0) {
		return type_array->prize_list[rand() % type_array->count];
	}
	return -1;
}

/*
 * @brief 在size大小的plist中，随机生成count个数放到prize中
 * @return: 0成功 -1失败
 */
int rand_some_prize_in_array(uint32_t* prize, const uint32_t* plist, uint32_t size, uint32_t count)
{
	int ret = 0;
	if (count > size) {
		ERROR_LOG("RandPrizeInList: error count=%d size=%d", count, size);
		return -1;
	}

	//奖励表中生成cnt个奖励品
	int i;
	for (i = 0; i < (int)count; i++)
	{
		uint32_t index = rand() % size;

		//判断是否生成一样的了
		while (1) {
			int j;
			for (j = 0; j < i && *(plist + index) != *(prize + j); j++);
			if (j == i) {
				break;
			}
			//index = rand() % size;
			index = ((index + 1) == size) ? 0 : (index + 1);
		}
		//DEBUG_LOG("rand a prize: %d", *(plist + index));
		*(prize + i) = *(plist + index);
	}

	return ret;
}

/*
 * @brief: 在奖励表中随机生成kind类型的count个奖励的index放到prize中(没用到)
 * @return: 0成功 -1失败
 */
int rand_one_prize_in_prize_conf(uint32_t* prize, uint32_t kind, uint32_t count)
{
	int ret = 0;

	prize_range_t* prange = get_prize_range_from_kind(kind);

	if ( prange == NULL || count > prange->count) {
		ERROR_LOG("rand_one_prize_in_prize_conf: range error");
		return -1;
	}

	//奖励表中生成cnt个奖励品
	int i;
	for (i = 0; i < (int)count; i++) {
		uint32_t index = rand_range_uint32(prange->start_idx, prange->end_idx);
	//	DEBUG_LOG("rand_prize_in_prize_conf prize from prize file: index = %d", index);

		//判断是否生成一样的了
		while (1) {
			int j;
			for (j = 0; j < i && index != *(prize + j); j++);
			if (j == i) {
				break;
			}
			//index = rand_range_uint32(ptmpRange->start_idx, ptmpRange->end_idx);
			index = ((index + 1) == prange->end_idx) ? prange->start_idx : (index + 1);
		}
		*(prize + i) = index;
	}
	return ret;
}
