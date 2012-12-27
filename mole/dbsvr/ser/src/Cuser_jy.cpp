/*
 * =====================================================================================
 *
 *       Filename:  Cuser_jy.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_jy.h"
#include "proto.h"
#include "serlib/db_utility.hpp"
#include <msglog.h>

/**
 * 种子的配置表，其代表的意思按顺序如下：
 * 种子类型号  果实类型号  良好状态生长率  干旱状态生长率  虫害状态生长率  干旱/虫害生长率  干旱时间间隔 
 * 虫害发生概率  结成果实所需的养成值 果实过期的天数 {良好状态下果实数  干旱状态下果实数   虫害状态下果实数 
 * 干旱/虫害状态下果实数}, 收获获取的经验值， 能够授粉的最小成长值，能够授粉的最大成长值，授粉的次数
 */
struct seed_info  seed_info_t[] =
{ 

	//胡萝卜
	{1230001, 190028, 30, 20, 20, 10, 24,  2, 840,  14,  {10,  8, 8, 6}, 30, 0, 0, 0},
	//茄子
	{1230002, 190027, 30, 20, 20, 10, 24,  2, 1020, 14,  {4,  2, 2, 1}, 42, 0, 0, 0},
	//南瓜
	{1230003, 190022, 30, 20, 20, 10, 30,  2, 1620, 14,  {2,  1, 1, 1}, 35, 0, 0, 0},
	//苹果
	{1230004, 190141, 30, 20, 20, 10, 35,  2, 1800, 14,  {8,  5, 5, 3}, 32, 0, 0, 0},
	//草莓
	{1230005, 190142, 30, 20, 20, 10, 24,  2, 2040, 14,  {6,  3, 3, 1}, 35, 0, 0, 0},
	//香蕉
	{1230006, 190166, 30, 20, 20, 10, 30,  2, 1800, 14,  {10, 5, 5, 1}, 33, 0, 0, 0},
	//牵牛花
	{1230007, 190167, 20, 10, 10, 5,  24,  2, 1000, 30, {5,  4, 4, 2}, 30, 0, 0, 0},
	//小麦
	{1230008, 190186, 30, 20, 20, 10, 24,  2, 780,  14,   {5,  2, 2, 1}, 28, 0, 0, 0},
	//毛毛豆
	{1230009, 190196, 30, 20, 20, 10, 35,  2, 1800,  14,  {10,  8, 8, 6}, 40, 0, 0, 0},
	//西瓜
	{1230010, 190201, 30, 20, 20, 10, 30,  2, 1620,  14,  {2,  1, 1, 1}, 25, 0, 0, 0},
	//水蜜桃
	{1230011, 190202, 30, 20, 20, 10, 30,  2, 1800,  14,  {5,  3, 3, 1}, 40, 0, 0, 0},
	//毛毛花
	{1230012, 190203, 20, 10, 10, 5, 24,  2, 1000,  30,  {3,  2, 2, 1}, 40, 0, 0, 0},
	//白菜
	{1230013, 190216, 30, 20, 20, 10, 24,  2, 840,  14,  {3,  2, 2, 1}, 55, 0, 0, 0},
	//荔枝
	{1230014, 190228, 30, 20, 20, 10, 30,  2, 1800,  14,  {5,  3, 3, 1}, 35, 0, 0, 0},
	//黑森林奇异果
	{1230015, 190230, 30, 20, 20, 10, 30,  2, 1620,  14,  {2,  1, 1, 1}, 35, 0, 0, 0},
	//卡兰花
	{1230016, 190350, 20, 10, 10, 5, 24,  2, 1000,  14,  {4,  2, 2, 1}, 35, 0, 0, 0},
	//萤火草
	{1230017, 190351, 30, 20, 20, 10, 30,  2, 1800,  14,  {5,  4, 4, 3}, 40, 0, 0, 0},
	//蒲兰花
	{1230018, 190352, 20, 10, 10, 5, 24,  2, 1000,  14,  {4,  2, 2, 1}, 35, 0, 0, 0},
	//葡萄
	{1230019, 190379, 20, 10, 10, 5, 24,  2, 1800,  14,  {5,  3, 3, 1}, 35, 0, 0, 0},
	//椰子
	{1230020, 190380, 20, 10, 10, 5, 24,  2, 1800,  14,  {5,  3, 3, 1}, 40, 0, 0, 0},
	//石榴
	{1230021, 190388, 30, 20, 20, 10, 30, 2, 1800,  14,  {5,  3, 3, 1}, 50, 0, 0, 0},
	//美味向日葵
	{1230022, 190395, 30, 20, 20, 10, 30, 2, 1800,  14,  {5,  3, 3, 1}, 40, 0, 0, 0},
	//葫芦
	{1230023, 190418, 30, 20, 20, 10, 30, 2, 2040,  14,  {5,  3, 3, 2}, 30, 0, 0, 0},
	//玉米
	{1230024, 190419, 30, 20, 20, 10, 30, 2, 2040,  14,  {5,  3, 3, 2}, 30, 0, 0, 0},
	//百合花
	{1230025, 190519, 30, 20, 20, 10, 24, 2, 1000,  14,  {3,  3, 3, 1}, 30, 0, 0, 0},
	//满天星种子
	{1230026, 190429, 100, 50, 50, 25, 24, 2, 3600, 14, {20, 15, 15, 10},  20, 0, 0, 0},
	//菊花种子
	{1230027, 190430, 30, 20, 20, 10, 24, 2, 2040,  14,  {3,  3, 3, 1}, 30, 1080, 2040, 10},
	//石竹花种子
	{1230028, 190431, 30, 20, 20,  10, 24, 2, 2040,  14,  {3,  3, 3, 1}, 20, 0, 0, 0},
	//蒲公英种子
	{1230029, 190433, 30, 20, 20,  10, 24, 2, 2040,  14,  {3,  3, 3, 1}, 20, 0, 0, 0},
	//叮当花种子
	{1230030, 190432, 30, 20, 20, 10, 24, 2, 1440,  14,  {3,  3, 3, 1}, 30, 0, 0, 0},
	//万圣节南瓜种子
	{1230031, 190437, 30, 20, 20, 10, 30, 2, 1620,  14,  {3,  2, 2, 1}, 30, 0, 0, 0},
	//糖豆豆
	{1230032, 190442, 30, 20, 20, 10, 30, 2, 450,   14,  {2,  2, 2, 2}, 30, 0, 0, 0},
	//棉花
	{1230033, 190458, 30, 20, 20, 10, 30, 2, 1440,  14,  {3,  3, 3, 1}, 30, 0, 0, 0},
	//四色百合花种子
	{1230034, 190519, 30, 20, 20, 10, 24, 2, 2040,  14,  {3,  3, 3, 1}, 30, 1080, 2040, 10},
	//多彩辣椒种子
	{1230035, 190575, 30, 20, 20, 10, 24, 5, 2040,  14,  { 5, 3, 3, 1}, 30, 1080, 2040, 10},
	//摇钱树种子(金元宝)
	{1230036, 190601, 40, 20, 20, 10, 30, 2,  980,  14,  { 2, 2, 2, 2},  5,    0,    0,  0},
	//摇钱树种子(银元宝)
	{1230037, 190602, 40, 20, 20, 10, 30, 2,  980,  14,  { 2, 2, 2, 2},  5,    0,    0,  0},
	//魔灯花种子
	{1230038, 190614, 40, 20, 20, 10, 24, 2, 1800,  14,  { 8, 5, 5, 1},  10,   0,    0,  0},
	//香樟树树苗
	{1230039, 190638, 40, 20, 20, 10, 24, 2, 2880,  14,  { 3, 2, 2, 1},  10,   0,    0,  0},
	//青瓜种子
	{1230040, 190653, 40, 20, 20, 10, 24, 2, 1120,  14,  {15, 12, 12, 10},  15,   0,    0,  0},
	//马铃薯
	{1230041, 190654, 40, 20, 20, 10, 24, 2, 1120,  14,  {20, 15, 15, 10},  15,   0,    0,  0},
	//白杨树苗
	{1230042, 190661, 40, 20, 20, 10, 24, 2, 1920,  14,  {5, 4, 4, 3},  20,   0,   0,  0},
	//圣女果
	{1230043, 190660, 40, 20, 20, 10, 24, 2, 1120,  14,  {20, 15, 15, 10},  15,   0,   0,  0},
	//樱花树树苗
	{1230044, 190662, 40, 20, 20, 10, 24, 2, 1920,  14,  {35, 25, 25, 20},  15,   0,   0,  0},
	//七色花种子
	{1230045, 190025, 40, 20, 20, 10, 24, 2, 960,  14,  {10, 8, 8, 5},  15,   0,   0,  0},
	//棉花糖种子
	{1230046, 190667, 40, 20, 20, 10, 24, 2, 960,  14,  {15, 12, 12, 10},  10,   0,   0,  0},
	//神奇糖豆种子
	{1230047, 190670, 40, 20, 20, 10, 24, 2, 880,  14,  {12, 12, 12, 12},  15,   0,   0,  0},
	//QQ小芝麻种子
	{1230048, 190683, 40, 20, 20, 10, 24, 2, 960,  14,  {30, 25, 25, 15},  10,   0,   0,  0},
	//桑树树苗
	{1230049, 190683, 40, 20, 20, 10, 24, 2, 960,  14,  {0,0,0,0},  10,   0,   0,  0},
	//翠竹竹笋
	{1230050, 190847, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5},  20, 0, 0, 0},
	//橘子树苗
	{1230051, 190821, 100, 50, 50, 25, 24, 2, 4000,  14,  {10, 8, 8, 5}, 20, 0, 0,  0},
	//神奇向阳花
	{1230052, 190744, 40, 20, 20, 10, 24, 2, 960,  14,  {4, 3, 3, 2},  10,   0,   0,  0},
	//新型玉米
	{1230053, 190747, 3600, 3600, 3600, 3600, 24, 2, 300, 14, {1, 1, 1, 1}, 10, 0, 0, 0},
	//多产南瓜种子
	{1230054, 190022, 100, 50, 50, 25, 24, 2, 2800, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//普通苞子花
	{1230055, 190807, 100, 100, 100, 100, 9999, 0, 600, 14, {1, 1, 1, 1}, 5, 0, 0, 0},
	//超级苞子花
	{1230056, 190807, 100, 100, 100, 100, 9999, 0, 600, 14, {2, 2, 2, 2}, 20, 0, 0, 0},
	//枞树种子
    {1230057, 190810, 100, 50, 50, 50, 24, 2, 7200, 14, { 1, 1, 1, 1}, 20, 0, 0, 0},
    //囧蘑菇种子
    {1230058, 190811, 100, 50, 50, 25, 24, 2, 2800, 14,{10, 8, 8, 5 }, 40, 0, 0, 0},
	//紫色脚印花种子
	{1230059, 190814, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//蓝色脚印花种子
	{1230060, 190815, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//绿色脚印花种子
	{1230061, 190816, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//橙色脚印花种子
	{1230062, 190817, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//白色脚印花种子
	{1230063, 190818, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//红色脚印花种子
	{1230064, 190819, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//黄色脚印花种子
	{1230065, 190812, 100, 50, 50, 25, 24, 2, 3600, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
	//摩尔拉雅姜茎块 
	{1230066, 190809,  100, 50, 50, 25, 24, 2, 3600, 14, { 5, 3, 3, 3 }, 20, 0, 0, 0},

	{1230067,      0,   0,  0,  0,  0,  0, 0,    0,  0, { 0, 0, 0, 0 },  0, 0, 0, 0},

	// 爆豆
	{1230068, 190823, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//神马豆
	{1230069, 190824, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//憨豆
	{1230070, 190825, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//霹雳豆
	{1230071, 190826, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//豆小怏
	{1230072, 190827, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//King 豆
	{1230073, 190828, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//雷豆豆
	{1230074, 190829, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//航天豆
	{1230075, 190830, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//浮云豆
	{1230076, 190831, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//超人豆
	{1230077, 190832, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//衰仔豆
	{1230078, 190833, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//爆发豆
	{1230079, 190834, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},
	//摇滚豆
	{1230080, 190835, 100, 50, 50, 25, 24, 2, 2400, 14, {5, 5, 5, 5 }, 20, 0, 0, 0},

	//下面四个留作待用
	{1230081, 190858, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},
	{1230082, 190859, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},
	{1230083, 190860, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},
	{1230084, 190861, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},

	//康乃馨种子
	{1230085, 190862, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},
	//爱心种子
	{1230086, 190887, 100, 50, 50, 25, 24, 2, 600, 14, {5, 4, 4, 3 }, 30, 0, 0, 0},
	//灵芝
	{1230087, 190903, 30, 20, 20, 10, 24, 2, 2040, 14, {3, 3, 3, 1 }, 20, 0, 0, 0},
	//腊梅
	{1230088, 190904, 100, 50, 50, 25, 24, 2, 2400, 14, {1, 1, 1, 1}, 20, 0, 0, 0},
	//红包树
	{1230089, 190905, 100, 50, 50, 25, 24, 2, 3600, 14, {5, 3, 3, 3 }, 20, 0, 0, 0},
	//红玫瑰花
	{1230090, 190906, 100, 50, 50, 25, 24, 2, 3600, 14, {5, 3, 3, 3 }, 20, 0, 0, 0},
	//黑玫瑰花
	{1230091, 190907, 100, 50, 50, 25, 24, 2, 3600, 14, {5, 3, 3, 3 }, 20, 0, 0, 0},
	//牡丹花
	{1230092, 190908, 100, 50, 50, 25, 24, 2, 3600, 14, {5, 3, 3, 3 }, 20, 0, 0, 0},
	//樱桃树种子 
	{1230093, 190916, 100, 50, 50, 25, 24, 2, 2040, 14, {10, 8, 8, 5 }, 40, 0, 0, 0},
};

/**
 * 种子的配置表，其代表的意思按顺序如下：
 * 种子类型号  果实类型号  良好状态生长率  干旱状态生长率  虫害状态生长率  干旱/虫害生长率  干旱时间间隔 
 * 虫害发生概率  结成果实所需的养成值 果实过期的天数 {良好状态下果实数  干旱状态下果实数   虫害状态下果实数 
 * 干旱/虫害状态下果实数}, 收获获取的经验值， 能够授粉的最小成长值，能够授粉的最大成长值，授粉的次数
 */

//加入新的植物同时要加入图鉴当中

pollinate_fruit pollination_fruitid[] =
{
	{1230034,{190519, 190425, 190425, 190520, 190520, 190521}},
	{1230035,{190575, 190575, 190576, 190576, 190577, 190577}},
	{1230027,{190430, 190522, 190523, 190524, 190525, 190526}},
};

const pollinate_fruit* get_pollination_fruitid(const uint32_t attireid)
{
	const size_t n=sizeof(pollination_fruitid)/sizeof(pollination_fruitid[0]);
	
	const pollinate_fruit* result=0;
	for(size_t i=0; i<n; ++i)
	{
		if(pollination_fruitid[i].plant_id==attireid)
		{
			result = pollination_fruitid + i;
		}
	}

	return result;
}

uint32_t get_fruitid_by_pollinate_times(const uint32_t attireid, const size_t t)
{
	const pollinate_fruit* p=get_pollination_fruitid(attireid);
	if(NULL==p)
	{
		return 0;
	}

	if(t<=10)
	{
		return p->fruitid[t/2];
		} else {
		return p->fruitid[0];
	}

}

#define POLLINATE_FRUIT_NUM (sizeof(pollination_fruitid) / sizeof(pollination_fruitid[0]))

/*种子类型数目*/
#define SEED_INFO_NUM sizeof(seed_info_t) / sizeof(seed_info)

static inline const seed_info& get_seed_info(const uint32_t seed_id)
{
	uint32_t index = seed_id - MIN_SEED_ID;
	return seed_info_t[index];
}

bool attire_id_is_fruitid_in_pic_list(const uint32_t id)
{
	if (id == 190601 || id == 190602) {//这两个不在图鉴中
		return false;
	}
	for (size_t i = 0; i < SEED_INFO_NUM; ++i) {
		if (seed_info_t[i].fruitid == id) {
			return true;
		}
	}
	return false;
}

/**
 * @brief 家园的构造函数
 */
Cuser_jy::Cuser_jy(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_jy","id")
{ 
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}


/**
 * @brief 得到种子对应的果实号 
 * @param seedid 子类型号
 * @param *p_fruitid 果实的类型号
 */
int Cuser_jy::get_fruitid(uint32_t seedid, uint32_t pollinate_num, uint32_t *p_fruitid)
{
	//得到果实ID号
	uint32_t ret = 0;
	uint32_t index = seedid - MIN_SEED_ID;
	if (index >= SEED_INFO_NUM) {
		ERROR_LOG("seedid %u", seedid);
		return ENUM_OUT_OF_RANGE_ERR;
	}
	if (pollinate_num == 100) {
		*p_fruitid =seed_info_t[index].fruitid;
	} else {
		ret = this->get_fruitid_accrod_pollinate_num(seedid, pollinate_num, p_fruitid);
	}
	if (*p_fruitid == 190807) {//收获苞子花
		uint32_t buff[2] = {(seedid == 1230055) ? 1 : 2, 0};
		msglog(this->msglog_file, 0x020D2004, time(NULL), buff, sizeof(buff));
	}
	return ret; 
}

/**
 * @brief 根据授粉次数得到果实
 * @param seedid 果实的ID号
 * @param pollinate_num 授粉的次数
 * @param p_fruitid 得到果实的ID号
 */
int Cuser_jy::get_fruitid_accrod_pollinate_num(uint32_t seedid, uint32_t pollinate_num, uint32_t *p_fruitid)
{
	*p_fruitid = 0;
	if (pollinate_num > 10) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	for (uint32_t i = 0; i < POLLINATE_FRUIT_NUM; i++) {
		if (pollination_fruitid[i].plant_id == seedid) {
			*p_fruitid = pollination_fruitid[i].fruitid[pollinate_num/2];
		}
	}	
	if (*p_fruitid == 0) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/**
 * @brief 得到收获果实得到的种植能手的经验值
 * @param seedid 种子的ID号
 * @param p_exp 得到的经验值
 */
int Cuser_jy::get_exp(uint32_t seedid, uint32_t *p_exp)
{
	//得到果实ID号
	seedid = seedid - MIN_SEED_ID;
	if (seedid >= SEED_INFO_NUM) {
		ERROR_LOG("seedid %u", seedid);
		return ENUM_OUT_OF_RANGE_ERR;
	}
	*p_exp=seed_info_t[seedid].exp;
	return SUCC;
}


/**
 * @brierf 判断果实是否过期
 * @param  *p_out_item 存储果实信息
 * @return FRUIT_IS_BAD_ERR果实过期，SUCC果实未过期
 */
int Cuser_jy::fruit_is_bad(jy_item *p_out_item)
{
	uint32_t index = p_out_item->attireid - MIN_SEED_ID;
	if (index >= SEED_INFO_NUM) {
		return ENUM_OUT_OF_RANGE_ERR;
	}
	if (time(NULL) - p_out_item->mature_time > seed_info_t[index].fruitsick * 3600 * 24) {
		return FRUIT_IS_BAD_ERR;
	}
	return SUCC;
}

/* @brief 得到某个种子的信息，并计算成熟时间和目前的生长速率
 * @param userid  米米号
 * @param id 种子的唯一编号
 * @param jy_item 返回种子的信息 
 */
int Cuser_jy :: get_ex(userid_t userid, uint32_t id, jy_item *p_item)
{
	uint32_t ret = this->get(userid, id, p_item);
	if (ret != SUCC) {
		return ret;
	}
	//ret = this->fill_attribute(p_item);
	//uint32_t grow_rate = 0;
	//if ((p_item->attireid - MIN_SEED_ID) > SEED_INFO_NUM) {
		//return VALUE_OUT_OF_RANGE_ERR;
	//}
	//if (p_item->sickflag == 0) {
		//grow_rate = seed_info_t[p_item->attireid - MIN_SEED_ID].healthy_grow_rate;
	//} else if (p_item->sickflag == 1) {
		//grow_rate = seed_info_t[p_item->attireid - MIN_SEED_ID].drought_grow_rate;
	//} else if (p_item->sickflag == 2) {
		//grow_rate = seed_info_t[p_item->attireid - MIN_SEED_ID].insect_grow_rate;
	//} else if (p_item->sickflag == 3) {
		//grow_rate = seed_info_t[p_item->attireid - MIN_SEED_ID].di_grow_rate;
	//} else {
		//return VALUE_OUT_OF_RANGE_ERR;
	//}
	//p_item->diff_mature = (uint32_t)(((seed_info_t[p_item->attireid - MIN_SEED_ID].mature - p_item->value)
			              //* 1.0 / grow_rate) * 60);
	//p_item->cur_grow = grow_rate; 
	ret = this->fill_attribute(p_item);
	return ret;
}

/**
 * @brief 从数据库中得到一条种子的记录
 * @param userid 用户ID号
 * @param id 种子的ID号
 * @param p_out_item 存储种子记录的信息
 * @return SUCC 成功，其他失败
 */
int Cuser_jy :: get(userid_t userid, uint32_t id, jy_item *p_out_item)
{
	sprintf(this->sqlstr, "select id, attireid, x, y, value, sickflag, fruitnum,\
 			cal_value_time, water_time, kill_bug_time, status, mature_time, earth, pollinate_num,\
			last_muck_date,\
			muck_times \
			from %s where userid= %u and id=%u", 
			this->get_table_name(userid), userid, id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SEEDID_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(p_out_item->id);
		INT_CPY_NEXT_FIELD(p_out_item->attireid);
		INT_CPY_NEXT_FIELD(p_out_item->x);
		INT_CPY_NEXT_FIELD(p_out_item->y);
		INT_CPY_NEXT_FIELD(p_out_item->value);
		INT_CPY_NEXT_FIELD(p_out_item->sickflag);
		INT_CPY_NEXT_FIELD(p_out_item->fruitnum);
		INT_CPY_NEXT_FIELD(p_out_item->cal_value_time);
		INT_CPY_NEXT_FIELD(p_out_item->water_time);
		INT_CPY_NEXT_FIELD(p_out_item->kill_bug_time);
		INT_CPY_NEXT_FIELD(p_out_item->status);
		INT_CPY_NEXT_FIELD(p_out_item->mature_time);
		INT_CPY_NEXT_FIELD(p_out_item->earth);
		INT_CPY_NEXT_FIELD(p_out_item->pollinate_num);
	STD_QUERY_ONE_END();
}

int Cuser_jy :: get(userid_t userid, uint32_t id, jy_item_muck *p_out_item)
{
	sprintf(this->sqlstr, "select id, attireid, x, y, value, sickflag, fruitnum,\
 			cal_value_time, water_time, kill_bug_time, status, mature_time, earth, pollinate_num,\
			last_muck_date,\
			muck_times \
			from %s where userid= %u and id=%u", 
			this->get_table_name(userid), userid, id);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SEEDID_NOFIND_ERR );
		INT_CPY_NEXT_FIELD(p_out_item->item.id);
		INT_CPY_NEXT_FIELD(p_out_item->item.attireid);
		INT_CPY_NEXT_FIELD(p_out_item->item.x);
		INT_CPY_NEXT_FIELD(p_out_item->item.y);
		INT_CPY_NEXT_FIELD(p_out_item->item.value);
		INT_CPY_NEXT_FIELD(p_out_item->item.sickflag);
		INT_CPY_NEXT_FIELD(p_out_item->item.fruitnum);
		INT_CPY_NEXT_FIELD(p_out_item->item.cal_value_time);
		INT_CPY_NEXT_FIELD(p_out_item->item.water_time);
		INT_CPY_NEXT_FIELD(p_out_item->item.kill_bug_time);
		INT_CPY_NEXT_FIELD(p_out_item->item.status);
		INT_CPY_NEXT_FIELD(p_out_item->item.mature_time);
		INT_CPY_NEXT_FIELD(p_out_item->item.earth);
		INT_CPY_NEXT_FIELD(p_out_item->item.pollinate_num);
		INT_CPY_NEXT_FIELD(p_out_item->last_muck_date);
		INT_CPY_NEXT_FIELD(p_out_item->muck_times);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if(!db_utility::is_today(p_out_item->last_muck_date)) //时间超过清理
	{
		p_out_item->muck_times=0;
		p_out_item->last_muck_date=(uint32_t)std::time(0);
	}

	return SUCC;

}

/**
 * @brief 对种子进行浇水
 * @param p_out_item 存储种子的相关信息
 * @return SUCC
 */
int Cuser_jy :: water_seed(jy_item *p_out_item)
{
	uint32_t cur_time = time(NULL);
	uint32_t index = p_out_item->attireid - MIN_SEED_ID;
	if (index >= SEED_INFO_NUM) {
		return ENUM_OUT_OF_RANGE_ERR;
	}
	/*如果种子处于非干旱状态，返回不需要浇水*/
	if (p_out_item->sickflag != DROUGHT && p_out_item->sickflag != DROUGHT_AND_INSECT) {
		return SUCC;
	}

	p_out_item->value = p_out_item->value * 3600;
	/*如果处于干旱状态，计算成长值，设置标志为健康*/
	if (p_out_item->sickflag == DROUGHT) {
		p_out_item->value += (cur_time - p_out_item->cal_value_time)
			* seed_info_t[index].drought_grow_rate;
		p_out_item->sickflag = 0;
	}
	/*如果处于即干旱又虫害的状态，计算成长值，设标志为虫害状态*/
	if (p_out_item->sickflag == DROUGHT_AND_INSECT) {
		p_out_item->value += (cur_time - p_out_item->cal_value_time) 
			* seed_info_t[index].di_grow_rate;
		p_out_item->sickflag = 2;
	}
	p_out_item->value = p_out_item->value / 3600; 
	/*判断是否成熟，如成熟计算果实和更新果实数目*/
	if (p_out_item->value >= seed_info_t[index].mature) {
		p_out_item->value = seed_info_t[index].mature;
		p_out_item->mature_time = cur_time;
		p_out_item->fruitnum = seed_info_t[index].fruits[p_out_item->sickflag];
	}
	uint32_t ret = this->fill_attribute(p_out_item);
	p_out_item->cal_value_time = cur_time;
	p_out_item->water_time = cur_time; 

	return ret;

}

/**
 * @brief 种子杀虫
 * @param p_out_item 一个种子的记录信息
 * @return SUCC
 */
int Cuser_jy :: kill_bug(jy_item *p_out_item)
{
	uint32_t cur_time = time(NULL);
	uint32_t index = p_out_item->attireid - MIN_SEED_ID;
    /*如果没有虫害，返回*/
	if (p_out_item->sickflag != INSECT && p_out_item->sickflag != DROUGHT_AND_INSECT) {
		return SUCC;
	}
	p_out_item->value = p_out_item->value * 3600;
	 /*如果为虫害，计算生长值和更新状态标志*/
	if (p_out_item->sickflag == INSECT) {
		p_out_item->value += (cur_time - p_out_item->cal_value_time)
			* seed_info_t[index].insect_grow_rate;
		p_out_item->sickflag = 0;
	}
	/*如果即为虫害又为干旱，计算生长值和更新状态标志*/
	if (p_out_item->sickflag == DROUGHT_AND_INSECT) {
		p_out_item->value += (cur_time - p_out_item->cal_value_time)
			* seed_info_t[index].di_grow_rate;
		p_out_item->sickflag = 1;
	}
	p_out_item->value = p_out_item->value / 3600; 
	/*计算是否成熟*/
	if (p_out_item->value >= seed_info_t[index].mature) {
		p_out_item->mature_time = cur_time;
		p_out_item->value = seed_info_t[index].mature;
		p_out_item->fruitnum = seed_info_t[index].fruits[p_out_item->sickflag];
	}
	uint32_t ret = this->fill_attribute(p_out_item);
	p_out_item->cal_value_time = cur_time;
	p_out_item->kill_bug_time = cur_time; 
	return ret;
}

/* @brief 得到家园种子列表，并计算离成熟的时间和目前的生长速率
 * @param userid 米米号
 * @param p_count 得到的家园植物的数目
 * @param pp_out_item 返回种子的信息
 */
int Cuser_jy::get_list_db_diff(userid_t userid, uint32_t *p_count, jy_item **pp_out_item)
{
	uint32_t ret = this->get_list_db(userid, p_count, pp_out_item);
	if (ret != SUCC) {
		return ret;
	}
	//jy_item *p_item = NULL;
	//for (uint32_t i = 0; i < *p_count; i++) {
		//p_item = (*pp_out_item + i);
		////ret = this->fill_attribute(p_item);
		//if (ret != SUCC) {
			//return ret;
		//}
	//}
	return SUCC;
}

/**
 * @brief 得到种子的信息 
 * @param userid用户ID号
 * @param *p_count记录的数目
 * @param **pp_out_item存储种子记录
 */
int Cuser_jy::get_list_db(userid_t userid, uint32_t *p_count, jy_item **pp_out_item )
{
	sprintf( this->sqlstr, "select id, attireid, x, y, value, sickflag, fruitnum,\
 			cal_value_time, water_time, \
			kill_bug_time, status, mature_time, earth, pollinate_num from %s \
			where userid=%u ", 
			this->get_table_name(userid),userid); 
	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->id);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->x);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->y);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->value);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->sickflag);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->fruitnum);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->cal_value_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->water_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->kill_bug_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->status);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->mature_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->earth);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->pollinate_num);
	STD_QUERY_WHILE_END();
}

/**
 * @brief 得到种子X,Y坐标 
 * @param userid用户ID号
 * @param *p_count记录的数目
 * @param **pp_out_item存储种子X,Y位置
 */
int Cuser_jy::get_xy_count(userid_t userid, uint32_t *p_count, uint32_t x, uint32_t y)
{
	sprintf(this->sqlstr, "select count(1)  from %s where userid=%u and x=%u and y=%u ", 
			this->get_table_name(userid),
			userid,
			x,
			y
		   ); 
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

/**
 * @brief 得到养成值
 * @param *value 养成值
 * @param calc_value_time 更新时间
 * @param drought_time 干旱时间
 * @param insect_time 虫害时间
 * @param id 种子编号
 * @param state 种子健康状态
 * @return SUCC 
 */
int Cuser_jy :: get_value_ex(uint32_t *value, uint32_t calc_value_time, uint32_t drought_time, 
		                 uint32_t insect_time, uint32_t id)
{
	*value*=3600;		
	int ret = this->get_value(value,calc_value_time, drought_time, insect_time, id);
	*value = *value / 3600;
	return ret;
}

int Cuser_jy :: get_value(uint32_t *value, uint32_t calc_value_time, uint32_t drought_time, 
		                 uint32_t insect_time, uint32_t id)
{
	time_t cur_time;
	cur_time = time(NULL);
	id = id - MIN_SEED_ID;
	if (id >= SEED_INFO_NUM) {
		DEBUG_LOG("get_value id %u", id);
		return ENUM_OUT_OF_RANGE_ERR;
	}

	/*calc_value_time <= drought_time <= insect_time, 初始状态为健康状态，计算value值*/
	if (drought_time <= insect_time && calc_value_time <= drought_time) {
		*value += (drought_time - calc_value_time) * seed_info_t[id].healthy_grow_rate;
		*value += (insect_time - drought_time) * seed_info_t[id].drought_grow_rate;
		*value += (cur_time - insect_time) * seed_info_t[id].di_grow_rate;
		return SUCC;
	}

	/*calc_value_time <= insect_time <= drought_time,初始状态为健康状态， 计算value值*/
	if (insect_time <= drought_time && calc_value_time <= insect_time) {
		*value += (insect_time - calc_value_time) * seed_info_t[id].healthy_grow_rate;
		*value += (drought_time - insect_time) * seed_info_t[id].insect_grow_rate;
		*value += (cur_time - drought_time) * seed_info_t[id].di_grow_rate;
		return SUCC;
	}

	/*insect_time < calc_value_time < drought_time,当insect_time为零或者为calc_value_time
	 * 计算value值
	 */
	if ((insect_time == calc_value_time || insect_time == 0) && calc_value_time <= drought_time) {
		if (insect_time == 0) {
			*value += (drought_time - calc_value_time) * seed_info_t[id].healthy_grow_rate;
			*value += (cur_time - drought_time) * seed_info_t[id].drought_grow_rate;
		} else {
			*value += (drought_time - calc_value_time) * seed_info_t[id].insect_grow_rate;
			*value += (cur_time - drought_time) * seed_info_t[id].di_grow_rate;
		}
		return SUCC;
	}

	/*drought_time < calc_value_time < insect_time, 当drought_time为零 或者drought_time为
	 * calc_value_time,计算value值
	 */
	if ((drought_time == calc_value_time || drought_time ==0) && calc_value_time <= insect_time) {
		if (drought_time == 0) {
			*value += (insect_time - calc_value_time) * seed_info_t[id].healthy_grow_rate;
			*value += (cur_time- insect_time) * seed_info_t[id].insect_grow_rate;
		} else {
			*value += (insect_time - calc_value_time) * seed_info_t[id].drought_grow_rate;
			*value += (cur_time- insect_time) * seed_info_t[id].di_grow_rate;
		}
		return SUCC;
	}

	/*如果处于健康状态，计算Value值*/
	if (drought_time == 0 && insect_time == 0) {
		*value += (cur_time - calc_value_time) * seed_info_t[id].healthy_grow_rate;
		return SUCC;
	}
	return VALUE_TIME_ERR;
}

/**
 * @brief 得到种子的健康状态 
 * @param water_time 上次浇水时间
 * @param calc_value_time 上次更新时间
 * @param id 种子的编号
 * @param *state 更新前种子的健康状态
 * @return SUCC
 */
int Cuser_jy :: drought_insect_good(uint32_t water_time, uint32_t calc_value_time, uint32_t id, uint32_t *state,
		                           uint32_t *drought_time, uint32_t *insect_time)
{
	uint32_t cur_time = time(NULL);
	uint32_t i;
	id = id - MIN_SEED_ID;
	if (id >= SEED_INFO_NUM) {
		DEBUG_LOG("drought id %u, seed_num %lu",id, SEED_INFO_NUM);
		return ENUM_OUT_OF_RANGE_ERR;
	}
	*insect_time = 0;
	*drought_time = 0;

	/*处于即干旱和虫害时间，返回*/
	if (*state == DROUGHT_AND_INSECT) {
		*drought_time = calc_value_time;
		*insect_time = calc_value_time;
		return SUCC;
	}

	/*如处于虫害时间，判断是否干旱*/
	if (*state == INSECT) {
		*insect_time = calc_value_time;
		if (cur_time - water_time > seed_info_t[id].drought_rate * 3600) {
			(*state)++;
			*drought_time = water_time + seed_info_t[id].drought_rate * 3600;
			if (*drought_time < calc_value_time) {
				*drought_time = calc_value_time;
			}
		}
		return SUCC;
	}
	
	/*如处于干旱时间，判断是否是虫害时间*/
	if (*state == DROUGHT) {
		*drought_time = calc_value_time;
		for (i = calc_value_time; i < cur_time; i += 3600) {
			if ((uint32_t)rand() % 100 < seed_info_t[id].insect_rate) {
				*state += 2;
				*insect_time = i;
				break;
			}
		}
	return SUCC;
	}

	/*如果处于健康状态，判断是否虫害和干旱*/
	if (*state == HEALTH) {
		if (cur_time - water_time > seed_info_t[id].drought_rate * 3600) {
			(*state)++;
			*drought_time = water_time + seed_info_t[id].drought_rate * 3600;
			if (*drought_time < calc_value_time) {
				*drought_time = calc_value_time;
			}
		}

		for (i = calc_value_time; i < cur_time; i += 3600) {
			if ((uint32_t)rand() % 100 < seed_info_t[id].insect_rate) {
				*state += 2;
				*insect_time = i;
				break;
			}	
		}
	}

	return SUCC;
}

/**
 * @brief 对数据库相关信息进行更新
 * @param userid 用户ID
 * @param **pp_out_item 存储种子信息指针
 * @return SUCC  成功， 其他失败
 */
int Cuser_jy :: update_seed_info(userid_t userid, jy_item *p_out_item)
{
	
	sprintf( this->sqlstr, "update %s set value = %u, sickflag = %u,\
			 fruitnum = %u, cal_value_time = %u, mature_time = %u, status=%u,\
			 water_time = %u, kill_bug_time = %u\
			where id=%u", this->get_table_name(userid),
			(*p_out_item).value, (*p_out_item).sickflag,
			(*p_out_item).fruitnum, (uint32_t)time(NULL),
			(*p_out_item).mature_time,(*p_out_item).status, 
			(*p_out_item).water_time, (*p_out_item).kill_bug_time, (*p_out_item).id);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 对数据库相关信息进行更新
 * @param userid 用户ID
 * @param *pp_out_item 存储种子信息指针
 * @return SUCC  成功， 其他失败
 */
int Cuser_jy :: update_seed_info(userid_t userid, jy_item_muck *p_out_item)
{

sprintf( this->sqlstr, "update %s set\
	value = %u, \
	sickflag = %u,\
	fruitnum = %u,\
	cal_value_time = %u,\
	mature_time = %u,\
	status=%u,\
	water_time = %u,\
	kill_bug_time = %u,\
	last_muck_date = %u,\
	muck_times = %u\
	where id=%u",
	this->get_table_name(userid),
	(*p_out_item).item.value,
	(*p_out_item).item.sickflag,
	(*p_out_item).item.fruitnum,
	(*p_out_item).item.cal_value_time,
	(*p_out_item).item.mature_time,
	(*p_out_item).item.status, 
	(*p_out_item).item.water_time,
	(*p_out_item).item.kill_bug_time,
	(*p_out_item).last_muck_date,
	(*p_out_item).muck_times,
	(*p_out_item).item.id);
STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 对数据库相关信息进行更新
 * @param userid 用户ID
 * @param **pp_out_item 存储种子信息指针
 * @param i 第几条记录
 * @return SUCC  成功， 其他失败
 */
int Cuser_jy :: update_seed_info_ex(userid_t userid, user_set_seed_ex_in *p_out_item)
{
	
	sprintf( this->sqlstr, "update %s set value = %u, sickflag = %u,\
			 fruitnum = %u, cal_value_time = %u, mature_time = %u, status=%u,\
			 water_time = %u, kill_bug_time = %u, earth = %u, pollinate_num = %u\
			where id=%u", this->get_table_name(userid),
			p_out_item->value, 
			p_out_item->sickflag,
			p_out_item->fruitnum,
			p_out_item->cal_value_time,
			p_out_item->mature_time,
			p_out_item->status, 
			p_out_item->water_time,
			p_out_item->kill_bug_time, 
			p_out_item->earth, 
			p_out_item->pollinate_num, 
			p_out_item->id 
		);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 更新果实数目 
 * @param id种子的ID号
 * @param fruitnum果实的数目
 */
int Cuser_jy :: update_fruit(userid_t userid,uint32_t id, uint32_t fruitnum )
{
	
	sprintf( this->sqlstr, "update %s set fruitnum = %u \
			where id=%u", 
			this->get_table_name(userid), fruitnum , id);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 对种子的信息进行更新
 * @param userid 用户ID号
 * @param *p_count 种子记录的个数
 * @param **pp_out_item 存储种子记录信息的指针
 * @return 
 */
int Cuser_jy :: get_list(userid_t userid,  uint32_t *p_count, jy_item **pp_out_item, uint32_t end_time, 
		uint32_t& insect_kill, uint32_t& plant_water )
{
	uint32_t i;
    int ret;

	/*从数据库得到种子的信息*/
	ret = this->get_list_db(userid, p_count, pp_out_item);
	if (ret != SUCC) {
		return ret;
	}

	uint32_t cur_time = time(NULL);

	/*循环处理用户种子的每条记录*/
	for (i = 0; i < *p_count; i++) {
		jy_item *p_item = (*pp_out_item)+i;
		if ((end_time < cur_time) && (end_time > p_item->water_time))
		{
			p_item->water_time = end_time;
			plant_water = plant_water + (end_time - p_item->water_time)/(60*60*24);
		}

		if ((end_time < cur_time) && (end_time > p_item->kill_bug_time))
		{
			p_item->kill_bug_time = end_time;
			insect_kill = insect_kill + ((end_time - p_item->kill_bug_time)/(60*60*24))*((uint32_t)rand()%10)*0.1;
		}

		ret = this->cal_seed(userid,p_item,end_time,insect_kill,plant_water);
		if(ret!=SUCC){
			free(*pp_out_item);
			return ret;
		}
	}
	
	return 0;
}

int  Cuser_jy::cal_seed(userid_t userid, jy_item *p_item, uint32_t end_time, uint32_t &insect_kill, uint32_t& plant_water)
{

	int ret;
	uint32_t drought_time, insect_time;
	/*if (time(NULL) - p_item->cal_value_time < 3600) {
		 return SUCC;
	 }*/
	 /*如果果实已经过期返回*/
	if (p_item->fruitnum != 0 && p_item->status == 1) {
		 return SUCC;
	}

	/*如果已成熟，如过期设置过期标志*/
	if (p_item->fruitnum != 0 && p_item->status == 0) {
		uint32_t fruit_time = seed_info_t[p_item->attireid - MIN_SEED_ID].fruitsick * 24 * 3600;
		if ((time(NULL) - p_item->mature_time) > fruit_time) {
			p_item->status = 1;
			ret =this->update_seed_info(userid, p_item);
			if (ret != SUCC) {
			return ret;
			}
		}	
		return SUCC;
	}

	/*得到种子的健康状态*/
	ret = this->drought_insect_good(p_item->water_time, p_item->cal_value_time, 
				p_item->attireid, &p_item->sickflag, &drought_time, &insect_time);
	if (ret != SUCC) {
		return ret;
	}
	
	uint32_t cur_time = time(NULL);
	if (p_item->sickflag > 0)
	{
		if (end_time >= cur_time)
		{
			switch (p_item->sickflag)
			{
				case DROUGHT:
					{
						p_item->sickflag = 0;
						p_item->water_time = cur_time;
						drought_time = 0;
						plant_water = plant_water + 1 + (cur_time - p_item->water_time)/(60*60*24);
						break;
					}
				case INSECT:
					{
						p_item->sickflag = 0;
						p_item->kill_bug_time = cur_time;
						insect_time = 0;
						insect_kill = insect_kill + 1 + ((cur_time - p_item->kill_bug_time)/(60*60*24))*((uint32_t)rand()%10)*0.1;
						break;
					}
				case DROUGHT_AND_INSECT:
					{
						p_item->sickflag = 0;
						p_item->water_time = cur_time;
						p_item->kill_bug_time = cur_time;
						drought_time = 0;
						insect_time = 0;
						plant_water = plant_water + 1 + (cur_time - p_item->water_time)/(60*60*24);
						insect_kill = insect_kill + 1 + ((cur_time - p_item->kill_bug_time)/(60*60*24))*((uint32_t)rand()%10)*0.1;
						break;
					}
				default:
					{
						break;
					}
			}
		}
	}

	/*计算种子的value值*/
	ret = this->get_value_ex(&p_item->value, p_item->cal_value_time, drought_time,
			 insect_time, p_item->attireid);
	if (ret != SUCC) {
		return ret;
	}
	/*如果种子成熟，记录成熟时间和果实数目*/
	if (p_item->value >= seed_info_t[p_item->attireid - MIN_SEED_ID].mature && p_item->fruitnum == 0) {
		p_item->value = seed_info_t[p_item->attireid - MIN_SEED_ID].mature;
		p_item->mature_time = time(NULL);
		p_item->fruitnum = seed_info_t[p_item->attireid - MIN_SEED_ID].fruits[p_item->sickflag];
	}
	ret = this->fill_attribute(p_item);
	if (ret != SUCC) {
		return ret;
	}
	/*更新种子信息*/	
	ret =this->update_seed_info(userid, p_item);
	return ret;
}

/* @brief 根据花是否能够授粉决定授粉次数的初始值 
 * @param seedid 种子的ID号
 * @param p_init 应该初始化的数值
 */
int Cuser_jy :: get_init_pollinate_num(uint32_t seedid, uint32_t *p_init)
{
	*p_init = 0;
	uint32_t max_pollinate_num = 0;
	uint32_t ret = get_flower_max_pollinate_num(seedid, &max_pollinate_num);
	if (ret != SUCC) {
		return ret;
	}
	if (max_pollinate_num == 0) {
		*p_init = 100;
	}
	return SUCC;
}

/**
 * @brief 种植种子
 * @param userid 用户ID号
 * @param now 现在时间
 * @param *p_in 插入的种子信息
 * @param *p_id 种子的ID号
 */
int Cuser_jy::add(userid_t userid, uint32_t now, user_add_seed_in *p_in, uint32_t  *p_id)
{
	uint32_t init_pollinate_num = 0;
	uint32_t ret = this->get_init_pollinate_num(p_in->attireid, &init_pollinate_num);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t value = 0;
	uint32_t sickflag = 1;
	uint32_t mature_time = 0;
	if (p_in->attireid == 1230036) {//种植摇钱树种子
		if ((rand() % 100) < 30) {
			p_in->attireid = 1230036;
		} else {
			p_in->attireid = 1230037;
		}
	} else if (p_in->attireid == 1230053) {//种植新型玉米种子
		value = 300;
		sickflag = 0;
		mature_time = now;
	} else if(p_in->attireid == 1230068){//爆豆种植
		uint32_t rand_arr[] = {5, 13, 21, 29, 37, 44, 52, 60, 68, 76, 84, 92, 100};
		uint32_t bean[] = {1230068, 1230069, 1230070, 1230071, 1230072, 1230073, 1230074, 1230075, 
						1230076, 1230077, 1230078, 1230079, 1230080};
		uint32_t random = rand() % 100;
		for(uint32_t i = 0; i< sizeof(rand_arr)/sizeof(uint32_t); ++i){
			if(random < rand_arr[i]){
				p_in->attireid = bean[i];
				break;
			}
		}
	}else if (p_in->attireid == 1230055 || p_in->attireid == 1230056) {
		sickflag = 0;
		uint32_t buff[2] = {0, 1};//种植普通和超级苞子花种子
		msglog(this->msglog_file, (p_in->attireid == 1230055)?0x020D2005:0x020D2006, time(NULL), buff, sizeof(buff));
	}

	sprintf( this->sqlstr, "insert into %s values (\
		0,%u,%u,%u,%u,%u,%u,0, %u, %u,%u,0, 0, %u, %u, 0x00, 0, 0)", 
			this->get_table_name(userid), 
			userid, p_in->attireid,p_in->x,p_in->y,value,sickflag,
			now,now,now,init_pollinate_num,mature_time
	   	);

	STD_INSERT_GET_ID(this->sqlstr, DB_ERR , *p_id);
}

/**
 * @brief 删除种子
 * @param userid 用户的ID号
 * @param id 种子的ID号
 */
int Cuser_jy::del(userid_t userid,  uint32_t  id)
{
	sprintf(this->sqlstr, "delete from %s where id=%u and userid = %u " ,
			this->get_table_name(userid),
			id,
			userid
	   	   );
	STD_SET_RETURN_EX (this->sqlstr, SEEDID_NOFIND_ERR );	
}

/**
 * @brief 得到用户种植的种子的总数
 * @param userid 用户ID号
 * @param *p_count 用户种植的种子数目
 */
int Cuser_jy::get_count(userid_t userid, uint32_t *p_count )
{

	sprintf(this->sqlstr, "select count(1) from %s \
			where  userid=%u", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

/* @brief 得到偷取果实者的ID号 
 * @param userid 用户的ID号
 * @param id 果树的唯一编号
 * @param p_list 存放偷取果实者的ID号
 */
int Cuser_jy :: get_thiever(userid_t userid, uint32_t id, thiever_info *p_list) 
{
	sprintf(this->sqlstr, "select thieve_list from %s where id = %u",
			this->get_table_name(userid),
			id
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		BIN_CPY_NEXT_FIELD(p_list , sizeof(thiever_info));
    STD_QUERY_ONE_END();
}
 
/* @brief 更新偷取果实的ID号 
 * @param userid 用户的ID号
 * @param id 果树的唯一编号
 * @param p_list 存放偷取果实的ID号
 */
int Cuser_jy :: update_thiever_fruitnum(userid_t userid, uint32_t id, thiever_info *p_list, uint32_t fruitnum) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))]; 
     set_mysql_string(mysql_list, (char*)p_list,
         4 + sizeof(p_list->thiever[0]) * p_list->count);
	sprintf(this->sqlstr, "update %s set thieve_list = '%s', fruitnum=%u where id = %u",
			this->get_table_name(userid),
			mysql_list,
			fruitnum,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


/* @brief 判断是否能偷取果实，并对相应的信息进行更新 
 * @param userid 用户的ID号
 * @param thiever_id 偷取果实者的ID号
 * @attireid 返回偷取的是哪种果实
 */
int Cuser_jy :: thieve_fruit(userid_t userid, userid_t thiever_id, uint32_t id, jy_item *p_out)
{
	/* 得到种子的信息 */
    int ret = this->get(userid, id, p_out);
	if (ret != SUCC) {
		return ret;
	}
	/* 判断是否成熟 */
    if (p_out->mature_time == 0) {
        return USER_NOFIND_FRUIT_ERR;
	}
	/* 判断果实是否过期 */	
	 ret = this->fruit_is_bad(p_out);
	 if (ret != SUCC) {
		 return ret;
	 }
	/* 判断是否到允许偷取的时间 */
	if ((p_out->attireid != 1230032) && (p_out->attireid != 1230036) && (p_out->attireid != 1230037)) {
    	uint32_t now = time(0);
    	if (now - p_out->mature_time < 7 * 24 * 3600) {
	        return TIME_IS_NOT_ENOUGH_ERR;
		}
	}
	/* 得到偷取信息，判断是否偷取过 */
	thiever_info poach = { };
	ret = this->get_thiever(userid, id, &poach);
	if (ret != SUCC) {
		return ret;
	}
	if (p_out->earth != 0) {
		return USER_NOFIND_FRUIT_ERR;
	}
	/* 此植物只允许偷取四次 */
	if ((p_out->attireid == 1230032) || (p_out->attireid == 1230036) || (p_out->attireid == 1230037)) {
		if (poach.count >= 4) {
			DEBUG_LOG("====%u", poach.count);
			return THIEVER_THIS_MANY_TIMES_ERR;
		}
	} 
	if (poach.count > 20) {
		return VALUE_OUT_OF_RANGE_ERR;
	}

	for (uint32_t i = 0; i < poach.count; i++) {
		if (thiever_id == poach.thiever[i]) {
			if (p_out->attireid == 1230032) {
				return HAVE_THIEVE_GUAI_TANG_ERR;
			} else {
				return HAVE_THIEVE_FRUIT_ERR;
			}
		}
	}
	/* 如没偷取过，记录其ID号 */
	poach.thiever[poach.count] = thiever_id;
	poach.count++;
	/* 果实数减一，并判断是否果实数为零，为零则铲除果树 */
	if ((p_out->attireid == 1230032) || (p_out->attireid == 1230036) || (p_out->attireid == 1230037)) {
		/* 有人偷取这个果实，不减加二 */
		p_out->fruitnum += 2;
	} else {
		p_out->fruitnum--;
	}
	if (p_out->fruitnum > 20) {
		p_out->fruitnum = 20;
		return THIEVER_THIS_MANY_TIMES_ERR;
	}
	if (p_out->fruitnum == 0) {
		//this->del(userid, id);
		p_out->earth = 1;
		this->update_earth(userid, id);
	} else {
		ret = this->update_thiever_fruitnum(userid, id, &poach, p_out->fruitnum);
		if (ret != SUCC) {
			return ret;
		}
	}
	return 0;
}

/* @brief 设置需要翻地标志为1
 * @param userid 用户的ID号
 * @param id 种子的唯一标号
 */
int Cuser_jy :: update_earth(userid_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "update %s set earth = 1 where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

/* @brief  更新某个字段的数值
 * @param userid 米米号
 * @param id 植物的编号
 * @param p_column 要得到的字段的名称
 * @param value 要更新的数值
 */
int Cuser_jy :: update_column(userid_t userid, uint32_t id, const char *p_column, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			p_column,
			value,
			userid,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

/* @brief 得到某个字段的数值 
 * @param userid 米米号
 * @param id 动物的ID号
 * @param p_column 字段的名称
 * @param p_value 返回字段的数值
 */
int Cuser_jy :: get_column(userid_t userid, uint32_t id, const char *p_column, uint32_t *p_value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and id = %u",
			p_column,
			this->get_table_name(userid),
			userid,
			id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr, ID_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_value);
    STD_QUERY_ONE_END();
}

/* @brief 得到某个植物的需要的授粉的次数
 * @param seedid 植物种类的ID号
 * @param p_num 总共能授粉的次数
 */
int Cuser_jy :: get_flower_max_pollinate_num(uint32_t seedid, uint32_t *p_num)
{
	//得到果实ID号
	uint32_t index = seedid - MIN_SEED_ID;
	if (index >= SEED_INFO_NUM) {
		ERROR_LOG("seedid %u", seedid);
		return ENUM_OUT_OF_RANGE_ERR;
	}
	*p_num =seed_info_t[index].pollinate_num;
	return SUCC;
}

/* @brief 得到某个植物能够授粉的最小和最大成长值 
 * @param seedid 植物种类的ID号
 * @param p_min 能够授粉的最小值 
 * @param p_max 能够授粉的最大值
 */
int Cuser_jy :: get_pollinate_min_max_grow(uint32_t seedid, uint32_t *p_min, uint32_t *p_max)
{
	//得到果实ID号
	uint32_t index = seedid - MIN_SEED_ID;
	if (index >= SEED_INFO_NUM) {
		ERROR_LOG("seedid %u", seedid);
		return ENUM_OUT_OF_RANGE_ERR;
	}
	*p_min =seed_info_t[index].pollinate_min_grow;
	*p_max =seed_info_t[index].pollinate_max_grow;
	return SUCC;
}

/* @brief 蝴蝶给花授粉 
 * @param userid 用户的米米号
 * @param id 植物的编号
 * @param p_item 保存植物的信息
 */
int Cuser_jy :: butterfly_pollinate(uint32_t userid, uint32_t id, jy_item *p_item)
{
	uint32_t ret = this->get(userid, id, p_item);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t max_pollinate_num = 0;
	ret = this->get_flower_max_pollinate_num(p_item->attireid, &max_pollinate_num);
	if (ret != SUCC) {
		return ret;
	}
	if (p_item->pollinate_num >= max_pollinate_num) {
		return THIS_PLANT_POLLINATE_ENOUGH_ERR;
	}
	uint32_t min = 0;
	uint32_t max = 0;
	ret = this->get_pollinate_min_max_grow(p_item->attireid, &min, &max);
	if (ret != SUCC) {
		return ret;
	}
	if ((p_item->value < min) || (p_item->value > max)) {
		return THIS_STAGE_CNANOT_POLLINATE_ERR;
	}
	p_item->pollinate_num++;
	ret = this->update_column(userid, id, "pollinate_num", p_item->pollinate_num);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->fill_attribute(p_item);
	return ret;
}

/* @brief 得到植物的生长速率
 * @param attireid 植物的ID号
 * @param sick_flag 植物健康状态标志
 * @param p_grow_rate 目前的生长速率
 */
int Cuser_jy :: get_grow_rate(uint32_t attireid, uint32_t sick_flag, uint32_t *p_grow_rate)
{
	if (attireid - MIN_SEED_ID >= SEED_INFO_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	if (sick_flag == 0) {
		*p_grow_rate = seed_info_t[attireid - MIN_SEED_ID].healthy_grow_rate;
	} else if (sick_flag == 1) {
		*p_grow_rate = seed_info_t[attireid - MIN_SEED_ID].drought_grow_rate;
	} else if (sick_flag == 2) {
		*p_grow_rate = seed_info_t[attireid - MIN_SEED_ID].insect_grow_rate;
	} else if (sick_flag == 3) {
		*p_grow_rate = seed_info_t[attireid - MIN_SEED_ID].di_grow_rate;
	} else {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/**
 * @brief 得到植物还有多长时间成熟
 * @param attireid 植物的种类的ID号
 * @param grow_rate 目前的生长速率
 * @param cur_value 植物的目前的生长值
 * @param p_diff_mature 返回还需要多少时间成熟，以分钟为单位
 */
int Cuser_jy :: get_diff_mature(uint32_t attireid, uint32_t grow_rate, uint32_t cur_value, uint32_t *p_diff_mature)
{
	if (attireid - MIN_SEED_ID >= SEED_INFO_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	if (cur_value >= seed_info_t[attireid - MIN_SEED_ID].mature) {
		*p_diff_mature = 0;
	} else {
		*p_diff_mature = (uint32_t)(((seed_info_t[attireid - MIN_SEED_ID].mature - cur_value)
		              * 1.0 / grow_rate) * 60);
	}

	return SUCC;
}

/**
 * @brief 填充植物的一些属性信息
 * @param p_in 保存植物信息的结构体
 */
int Cuser_jy :: fill_attribute(jy_item *p_in)
{
	uint32_t ret = this->get_grow_rate(p_in->attireid, p_in->sickflag, &p_in->cur_grow);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->get_diff_mature(p_in->attireid, p_in->cur_grow, p_in->value, &p_in->diff_mature);
	if (ret != SUCC) {
		return ret;
	}
	return SUCC;
}

void Cuser_jy::plant_muck_clean(struct jy_item_muck& jy_item, user_plant_growth_add_by_hour_in& in)
{
	jy_item.muck_times+=1;

	const seed_info& sinfo=get_seed_info(jy_item.item.attireid);
    uint32_t grow_rate=sinfo.healthy_grow_rate; ///< 正常状态下的生长率
	uint32_t add_to_value=in.hours*grow_rate;
	jy_item.item.value += add_to_value;
	if(jy_item.item.value >= sinfo.mature)
	{
		jy_item.item.value = sinfo.mature;
		jy_item.item.fruitnum= sinfo.fruits[jy_item.item.sickflag]; //结果
		time_t t=std::time(0);
		jy_item.item.mature_time=t;
		jy_item.item.cal_value_time=t;
	}

	fill_attribute(&jy_item.item);
}

int Cuser_jy :: get_mature_fruit(userid_t userid, uint32_t id, uint32_t *p_mature_fruit)
{

	uint32_t attireid = 0;
	uint32_t ret = this->get_column(userid, id, "attireid", &attireid);
	if (ret != SUCC)
	{
		return ret;
	}

	if ((attireid - MIN_SEED_ID) >=  SEED_INFO_NUM)
	{
		return ID_NOT_EXIST_ERR;
	}	

	seed_info& sinfo = seed_info_t[attireid - MIN_SEED_ID];
	if(sinfo.pollinate_num==0)//不能授粉
	{
		*p_mature_fruit = seed_info_t[attireid - MIN_SEED_ID].fruitid;
		return SUCC;
	}

	//现在可以授粉，那么它的内容也不能
	uint32_t pollinate_num=0;
	ret=this->get_column(userid, id, "pollinate_num", &pollinate_num);
	if (ret!=SUCC)
	{
		return ret;
	}

	*p_mature_fruit = get_fruitid_by_pollinate_times(attireid, pollinate_num);
	if(*p_mature_fruit)
	{
		*p_mature_fruit = seed_info_t[attireid - MIN_SEED_ID].fruitid;
	}

	return SUCC;
}


/**
 * @brief 得到成熟种子的信息 
 * @param *p_count记录的数目
 * @param **pp_out_item存储种子记录
 */
int Cuser_jy::get_jy_item_list(userid_t userid, uint32_t *p_count, jy_item **pp_out_item )
{
	sprintf( this->sqlstr, "select id, attireid, x, y, value, sickflag, fruitnum,\
 			cal_value_time, water_time, \
			kill_bug_time, status, mature_time, earth, pollinate_num from %s \
			where userid=%u and fruitnum != 0 and earth != 1 ", 
			this->get_table_name(userid),userid); 
	
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->id);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->x);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->y);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->value);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->sickflag);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->fruitnum);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->cal_value_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->water_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->kill_bug_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->status);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->mature_time);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->earth);
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->pollinate_num);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 得到家园中种植的植物的种类和数量
 */
int Cuser_jy::get_jy_attireid_and_cnt(userid_t userid, uint32_t *p_count, user_get_attireid_and_cnt_out_item** pp_list)
{
	sprintf(this->sqlstr, "select attireid, count(*) from %s where userid = %u group by attireid",
			this->get_table_name(userid), userid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count) ;
		INT_CPY_NEXT_FIELD((*pp_list + i)->attire_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attire_cnt);
	STD_QUERY_WHILE_END();
}


