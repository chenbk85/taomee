/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_animal.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/03/2009 11:55:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include<algorithm>
#include<ctime>
#include "proto.h"
#include "Cuser_farm_animal.h"



//加新的动物要注意同时加入图鉴配置当中
static stru_animal_info animal_attribute[] =
{
	//红猪
	{1270001, 190241, 190242, 190028, 20, 10, 10, 5, 24, 12, 900,  {2, 1, 1, 1}, 1,10, 24, 120, 14, 10, 0, 1},
	//鸭子
	{1270002, 190238, 190024, 190028, 20, 10, 10, 5, 24, 24, 600,  {5, 2, 2, 1}, 1,20, 24, 120, 10, 20, 0, 1},
	//鱼
	{1270003, 190239, 190240, 190232, 20, 10, 10, 5,  0, 24, 600,  {5, 1, 1, 1}, 1,10, 24, 120, 10, 20, 0, 0},
	//小毛毛头
	{1270004, 	 0, 190243, 190196, 0,  0,  0,  0, 24, 24, 600,  {5, 2, 2, 1}, 1,10, 24, 120, 12, 10, 0, 1},
	//嘟噜噜
	{1270005, 190244, 190243, 190196, 20, 10, 10, 5, 24, 24, 600,  {5, 2, 2, 1}, 1,10, 24, 120,  8, 20, 0, 1},
	//公羊
	{1270006, 190245,      0, 190351, 20, 10, 10, 5, 24, 12, 800,  {0, 0, 0, 0}, 0, 0, 24, 120, 14, 20, 0, 1},
	//母羊
	{1270007, 190247, 190248, 190351, 20, 10, 10, 5, 24, 12, 600,  {3, 2, 2, 1}, 1,10, 24, 120, 14, 20, 0, 1},
	//蚕宝宝
	{1270008, 190249,      0, 190412, 20, 10, 10, 5,  0, 12, 600,  {0, 0, 0, 0}, 0, 0,  24, 120,  8, 15, 0, 4},
	//怪怪鱼
	{1270009, 190250, 190251, 190232, 20, 10, 10, 5,  0, 24, 800,  {5, 1, 1, 1}, 1,10, 24, 120, 12,  5, 0, 0},
	//糯米团子
	{1270010, 190306, 190307, 190289, 20, 10, 10, 5,  24, 24, 960,  {1, 1, 1, 1}, 1,1, 24, 120, 20, 15, 0, 1},
	//胖胖皖熊
	{1270011, 190310, 190311, 190289, 20, 10, 10, 5,  24, 24, 960,  {1, 1, 1, 1}, 1,3, 24, 120, 20, 40, 0, 1},
	//雪花母鸡
	{1270012, 190308, 190309, 190472, 20, 10, 10, 5,  24, 24, 600,  {5, 2, 2, 1}, 8,20, 24, 120, 12, 20, 0, 1},
	//小猫4
	{1270013, 190250, 190251, 190232, 20, 10, 10, 5,  0, 24, 800,  {5, 1, 1, 1}, 1,10, 24, 120, 12, 20, 0, 1},
	//小猫5
	{1270014, 190250, 190251, 190232, 20, 10, 10, 5,  0, 24, 800,  {5, 1, 1, 1}, 1,10, 24, 120, 12, 20, 0, 1},
	//眼镜白兔
	{1270015, 190252, 190253, 190028, 20, 10, 10, 5, 24, 24, 700,  {1, 1, 1, 1},  1,1, 24, 120, 12, 20, 0, 1},
	//帅帅灰兔
	{1270016, 190254, 	 0, 190028, 20, 10, 10, 5, 24, 24, 700,  {0, 0, 0, 0}, 0, 0, 24, 120, 12, 20, 0, 1},
	//肥肥花兔
	{1270017, 190256, 	 0, 190028, 20, 10, 10, 5, 24, 24, 960,  {0, 0, 0, 0}, 0, 0, 24, 120, 12, 20, 0, 1},
	//火鸡蛋
	{1270018, 190252, 	 0, 	 0, 20, 10, 10, 5, 24, 24, 700,  {0, 0, 0, 0},  0, 0, 24, 120, 12, 20, 0, 1},
	//母火鸡幼崽
	{1270019, 190257, 190258, 190472, 20, 10, 10, 5, 24, 24, 2400, {1, 1, 1, 0}, 1, 6, 24, 120, 12, 30, 0, 1},
	//公火鸡幼崽
	{1270020, 190259,      0, 190472, 20, 10, 10, 5, 24, 24, 1400, {0, 0, 0, 0}, 0, 0, 24, 120, 12, 20, 0, 1},
	//小龙虾
	{1270021, 190260,      0,      0, 20, 20, 20, 20,  0, 24, 1960, {0, 0, 0, 0}, 0,0,  0, 120, 12,  5, 0, 0},
	//蝶尾金鱼
	{1270022, 190261, 190262, 190232, 20, 10, 10, 5,  0, 12, 1440, {1, 0, 0, 0}, 1, 5, 48, 120, 10, 10, 0, 0},
	//鲦鱼
	{1270023, 190263, 190264, 190232, 20, 10, 10, 5,  0, 24,  960, {1, 0, 0, 0}, 1, 3, 24, 120, 14, 10, 0, 0},
	//鲫鱼
	{1270024, 190265, 190266, 190232, 20, 10, 10, 5,  0, 24,  960, {1, 0, 0, 0}, 1, 5, 24, 120, 14, 10, 0, 0},
	//狗鱼
	{1270025, 190267, 190268, 190232, 20, 10, 10, 5,  0, 48, 1440, {1, 0, 0, 0}, 1, 5, 24, 120, 14, 10, 0, 0},
	//黄风蝶
	{1270026, 190269,      0, 190510, 20, 10, 10, 5,  0, 24,  960, {0, 0, 0, 0}, 0, 0, 24, 120, 14,  5, 1, 2},
	//蓝风蝶
	{1270027, 190270,      0, 190510, 20, 10, 10, 5,  0, 24,  960, {0, 0, 0, 0}, 0, 0, 24, 120, 24,  5, 2, 2},
	//菲尼克斯
	{1270028, 190271,      0, 190196, 20, 10, 10, 5, 24, 24,  2400, {0, 0, 0, 0}, 0,0, 24, 120, 14, 40, 0, 1},
	//菲尼克斯蛋
	{1270029,      0,      0,      0, 20, 10, 10, 5, 24, 24,  960, {0, 0, 0, 0}, 0, 0, 24, 120, 14,  5, 0, 1},
	//鲈鱼
	{1270030, 190272, 190273, 190232, 20, 10, 10, 5,  0, 24, 960, {5, 1, 1, 1 }, 1,10, 24, 120, 10, 20, 0, 0},
	//草鱼
	{1270031, 190274, 190275, 190232, 20, 10, 10, 5,  0, 24, 960, {5, 1, 1, 1 }, 1,10, 24, 120, 10, 20, 0, 0}, 
	//青鱼
	{1270032, 190276, 190277, 190232, 20, 10, 10, 5,  0, 24, 960, {5, 1, 1, 1 }, 1,10, 24, 120, 10, 20, 0, 0},
	//胖头鱼
	{1270033, 190338, 190339, 190232, 100, 50, 50, 50, 0, 24, 4800, { 5, 1, 5, 1}, 1, 5, 24, 120, 20, 10, 0, 0},
	//鲢鱼
	{1270034, 190280, 190281, 190232, 20, 10, 10, 5,  0, 24, 960, {5, 1, 1, 1 }, 1,10, 24, 120, 10, 20, 0, 0},
	//泡泡眼金鱼
	{1270035, 190282, 190283, 190232, 20, 10, 10, 5,  0, 24, 960, {1, 0, 0, 0 }, 1,4, 24, 120, 10, 20, 0, 0},
	//驯鹿
	{1270036, 190284,      0, 190351, 20, 10, 10, 5, 24, 24, 800, {0, 0, 0, 0 }, 0, 0, 24, 120, 14, 30, 0, 1},
	//松鼠
	{1270037, 190285, 190286, 190419, 20, 10, 10, 5, 24, 24, 960, {2, 1, 1, 1 }, 1, 2, 24, 120, 14, 10, 0, 1},
	//企鹅
	{1270038, 190287, 190288, 190289, 20, 10, 10, 5, 24, 36, 960, {1, 1, 1, 0 }, 1, 3, 24, 120, 14,  8, 0, 1},
	//磷虾
	{1270039, 190289, 190290, 190232, 20, 10, 10, 5, 0 , 24, 600, {1, 0, 0, 0 }, 1, 3, 24, 120, 14,  2, 0, 0},
	//小公虎
	{1270040, 190292, 	   0, 190613, 20, 10, 10, 5, 24, 24,7200, {0, 0, 0, 0 }, 0, 0, 24, 120, 60, 100, 0, 1},
	//小母虎
	{1270041, 190291, 190295, 190613, 20, 10, 10, 5, 24, 24,4800, {3, 1, 1, 0 }, 1, 3, 24, 120, 60, 80, 0, 1},
	//宝石灵龟
	{1270042, 190293, 190294, 190614, 20, 10, 10, 5, 24, 12,1440, {4, 2, 2, 0 }, 1, 4, 48, 120, 14, 30, 0, 1},
	//公奶牛
	{1270043, 190297,      0, 190351, 20, 10, 10, 5, 24, 24,1440, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 10, 0, 1},
	//母奶牛
	{1270044, 190298, 190299, 190351, 20, 10, 10, 5, 24, 24,1440, {1, 1, 1, 0 }, 1, 5, 24, 120, 30, 20, 0, 1},
	//栗子刺猬
	{1270045, 190300, 190301, 190141, 20, 10, 10, 5, 24, 24,1200, {1, 1, 1, 0 }, 1, 5, 24, 120, 14, 20, 0, 1},
	//闪光皮皮
	{1270046, 190302, 190303, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//小豆芽
	{1270047, 190304, 190305, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//不使用
	{1270048, 190304, 190305, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//不使用
	{1270049, 190304, 190305, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//不使用
	{1270050, 190304, 190305, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//不使用
	{1270051, 190304, 190305, 190196, 20, 10, 10, 5, 24, 24,1680, {1, 1, 1, 0 }, 1, 3, 24, 120, 20, 20, 0, 1},
	//哼哼小野猪
	{1270052, 190312, 190313, 190196, 20, 10, 10, 5, 24, 24,3360, {1, 1, 1, 1 }, 1, 3, 24, 120, 20, 90, 0, 1},
	//哼哼小公猪
	{1270053, 190314,      0, 190196, 20, 10, 10, 5, 24, 24,1920, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 30, 0, 1},
	//派对礼宾犬(母)
	{1270054, 190315,      0, 190738, 20, 10, 10, 5, 24, 24,960, {0, 0, 0, 0 },  0, 0, 24, 120, 20, 0, 0, 1},
	//扭扭小贡丸
	{1270055, 190318, 190319, 190196, 20, 10, 10, 5, 24, 24,1440, {1, 1, 1, 0 }, 1, 8, 24, 120, 20, 60, 0, 1},
	//不使用
	{1270056, 190318, 190319, 190028, 20, 10, 10, 5, 24, 24,1440, {1, 1, 1, 0 }, 1, 8, 24, 120, 20, 60, 0, 1},
	//不使用
	{1270057, 190318, 190319, 190028, 20, 10, 10, 5, 24, 24,1440, {1, 1, 1, 0 }, 1, 8, 24, 120, 20, 60, 0, 1},
	//派对礼宾犬(公)
	{1270058, 190333,      0, 190738, 20, 10, 10, 5, 24, 24,960, {0, 0, 0, 0 },  0, 0, 24, 120, 20, 80, 0, 1},
	//咕噜毛毛
	{1270059, 190321, 190322, 190351, 20, 10, 10, 5, 24, 24,960, {2, 1, 1, 1 },  1, 3, 24, 120, 20, 80, 0, 1},
	//咕噜球球
	{1270060, 190320,      0, 190351, 20, 10, 10, 5, 24, 24,960, {2, 1, 1, 1 },  0, 0, 24, 120, 20, 80, 0, 1},
	//不使用
	{1270061, 190320,      0, 190351, 20, 10, 10, 5, 24, 24,960, {2, 1, 1, 1 },  0, 0, 24, 120, 20, 80, 0, 1},
	//不使用
	{1270062, 190320,      0, 190351, 20, 10, 10, 5, 24, 24,960, {2, 1, 1, 1 },  0, 0, 24, 120, 20, 80, 0, 1},
	//双头小羊驼
	{1270063, 190328, 190329, 190351, 20, 10, 10, 5, 24, 24,960, {1, 1, 1, 1 },  1, 3, 24, 120, 20, 100, 0, 1},
	//蘑蘑菇
	{1270064, 190324, 190325, 190196, 20, 10, 10, 5, 24, 24,960, {1, 1, 1, 1 },  1, 3, 24, 120, 20, 50, 0, 1},
	//蘑蘑菌
	{1270065, 190327,      0, 190196, 20, 10, 10, 5, 24, 24,960, {1, 1, 1, 1 },  0, 0, 24, 120, 20, 50, 0, 1},
	//月光光兔
	{1270066, 190330, 190331, 190028, 20, 10, 10, 5, 24, 24,960, {1, 1, 1, 1 },  1, 4, 24, 120, 20, 20, 0, 1},
	//月亮亮兔
	{1270067, 190332,      0, 190028, 20, 10, 10, 5, 24, 24,960, {1, 1, 1, 1 },  0, 0, 24, 120, 20, 20, 0, 1},
	//小马驹
	{1270068, 190334,      0, 190351, 20, 10, 10, 5, 24, 24,1920, {0, 0, 0, 0},  0, 0, 24, 120, 20, 20, 0, 1},
	//飞天小神马
	{1270069, 190335, 190336, 190351, 20, 10, 10, 5, 24, 24,1920, {1, 1, 1, 1},  1, 3, 24, 120, 20, 20, 0, 1},
	//速成小鸭
	{1270070, 190337, 	   0, 190028, 3600, 3600, 3600, 3600, 9999, 9999, 6, {0, 0, 0, 0}, 0, 0, 24, 120, 20, 5, 0, 1},
	//脚印红猪
	{1270071, 190340, 190341, 190028, 100, 50, 50, 25, 24, 24, 4500, {5, 3, 3, 2 }, 5, 2, 24, 120, 20, 40, 0, 1},
	//大耳蓝兔
	{1270072, 190342,     0,  190028, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},
	//大耳白兔
	{1270073, 190343,     0,  190028, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},

	{1270074,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	//卡酷年
	{1270075, 190836,       0, 190738, 100, 50, 50, 25, 24, 24, 4800, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},

	{1270076,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270077,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270078,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270079,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270080,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270081,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270082,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270083,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270084,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270085,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270086,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270087,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270088,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270089,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270090,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270091,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270092,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270093,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270094,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270095,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270096,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270097,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270098,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270099,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270100,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270101,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270102,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270103,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270104,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270105,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270106,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270107,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270108,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270109,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270110,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270111,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270112,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270113,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270114,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270115,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270116,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270117,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270118,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270119,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270120,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270121,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270122,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270123,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270124,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	{1270125,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	
	//长鼻菇
	{1270126, 190841,     0,   190738, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},	
	//彩羽鸡
	{1270127, 190842,     0,   190738, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},	
	//果冻花
	{1270128, 190843,     0,   190738, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},	
	//咕唧鸟
	{1270129, 190844,     0,   190738, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},	
	//刺球花
	{1270130, 190845,     0,   190738, 100, 50, 50, 25, 24, 24, 3600, {0, 0, 0, 0 }, 0, 0, 24, 120, 20, 50, 0, 1},	
	//奶牛花花
	{1270131, 190871,     0,   190351, 20, 10, 10, 5, 24, 24,1440, {1, 1, 1, 0 }, 1, 5, 24, 120, 30, 20, 0, 1},
	{1270132,      0,      0,      0,   0,  0,  0,  0,  0,  0,    0, {0, 0, 0, 0 }, 0, 0,  0,   0,  0,  0, 0, 0},
	//燕子
	{1270133, 190919,  190918,  190196, 20, 10, 10, 5, 24, 24,900, {5, 3, 3, 2 }, 2, 5, 24, 120, 20, 40, 0, 1},
	//下金蛋的鹅
	{1270134, 190950,  190949,  190028, 20, 10, 10, 5, 24, 24,4800, {1, 1, 1, 1 }, 1, 1, 24, 120, 20, 50, 0, 1},
};

/* 动物幼崽的ID号， 成年动物的ID号，动物产品ID号，喂食的物品的种类，
 * 正常状态生长率，饥渴状态的生长率，饥饿状态下的生长率，饥渴/干旱状态下的生长率,
 * 干旱间隔时间，饥饿间隔时间，成熟值，{正常状态下的产值，饥渴状态下的产值，干旱状态下的产值，干旱/饥渴状态下的产值 },
 * 产值的最小数，产值的最大数，每隔多长时间产一次, 多少VALUE值等于一头产出,
 * 初始亲密度，收获增加的技能点,昆虫受粉次数，动物的总类(0表示水生，1表示陆生，2表示昆虫)
 */



/*
 * 天使蛋ID号， 能够从多少个动物随机， 随机第一个动物的起始数值， 随机第一个动物的终止数值， 随机出的动物ID，
 * 随机出的动物的等级， 【如果有第二个或更多动物，接着配置第二个，或更多动物】
 */
static  user_farm_egg_rand_info rand_egg_array[] =
{
	//笨笨天使蛋
	{1270076, 21, {{0, 12, 1270005, 0 },{12, 24, 1270015, 0},{24, 36, 1270037, 0},{36, 46, 1270064, 0},{46, 58, 1270045, 0},
				   {58, 70, 1270016, 0},{70, 74, 1270005, 1},{74, 78, 1270015, 1},{78, 81, 1270064, 1},{81, 83, 1270129, 1},
				   {83, 85, 1270130, 1},{85, 88, 1270005, 2 },{88,91, 1270015, 2},{91,93, 1270064, 2},{93,94,1270129, 2},
                   {94, 95, 1270130, 2},{95,96, 1270005, 3},{96,97, 1270015, 3},{97,98, 1270064, 3},{98,99, 1270129, 3},
				   {99,100, 1270130, 3}}},
	//纯白天使蛋
	{1270077, 18, {{0, 10, 1270005, 1}, {10, 20, 1270015, 1},{20, 30, 1270064, 1},{30, 35, 1270126, 1},{35, 42, 1270130, 1},
				  {42,50, 1270129, 1}, {50, 56, 1270005, 2},{56, 62, 1270015, 2},{62, 68, 1270064, 2},{68, 71,1270126, 2 },
				  {71, 75,1270130,2},{75, 80, 1270129, 2},{80, 84, 1270005, 3},{84,88,1270015, 3},{88,92,1270064, 3},
				  {92, 94,1270126, 3},{94,97,1270130, 3},{97, 100,1270129, 3}}},
	//白银天使蛋
	{1270078, 15, {{0, 5, 1270005, 2 }, {5, 10, 1270015, 2},{10, 15, 1270064, 2},{15, 24, 1270126, 2},{24, 33, 1270130, 2},
				  {33, 42, 1270129, 2},{42, 50, 1270128, 2},{50, 52, 1270005, 3},{52, 54, 1270015, 3},{54,58,1270064,3},
				  {58,62,1270126, 3},{62,66,1270130, 3},{66,70,1270129, 3 },{70,75,1270128, 3},{75,100, 1270127, 3}}},
	//黄金天使蛋
	{1270079, 5, {{0, 20, 1270126, 3 }, {20, 40, 1270130, 3},{40, 60, 1270129, 3},{60, 80, 1270128, 3},{80, 100, 1270127, 3}}}
};
/**
 * @brief 由于历史原因 @ref animal_attribute 中的一些动物是不能养殖的
 * 		这就需要把它们删除，通过另一个表实现
 */
static uint32_t animal_is_not_need[]=
{
	1270004, //小毛毛头
	1270013, //小猫4
	1270014, //小猫5
	1270018, //火鸡蛋
	1270029, //菲尼克斯蛋
};

inline static bool is_not_need_animal(const uint32_t attireid)
{
	size_t n=sizeof(animal_is_not_need)/sizeof(animal_is_not_need[0]);
	const uint32_t* end=animal_is_not_need+n;
	const uint32_t* begin=animal_is_not_need;

	return end!=std::find(begin, end, attireid);
}

//动物种类的数目
#define ANIMAL_NUM (sizeof(animal_attribute) / sizeof(stru_animal_info))
#define BASE	1270001

bool attire_id_is_mature_id_in_pic_list(const uint32_t id)
{
	if (id == 0) {
		return false;
	}
	for (size_t size = 0; size < ANIMAL_NUM; ++size) {
		if (animal_attribute[size].mature_id == id) {
			return true;
		}
	}
	return false;
}

/* @brief 农场类的构造函数 */
Cuser_farm_animal :: Cuser_farm_animal(mysql_interface * db )
	     : CtableRoute100x10( db,"USER","t_user_animal","userid"),
		 insetct_animal(2), aquatic_animal(0), terrestrial_animal(1),silkworm(4)
{

}


uint32_t Cuser_farm_animal::fruit_max(const uint32_t animal_type)
{
	const uint32_t index=animal_type-BASE;
	if(index<ANIMAL_NUM)
	{
		return animal_attribute[index].fruit_max;
	}
	//如果在数组中找不到，默认值为10
	return 10;
}

/* @brief 得到蝴蝶的授粉次数
 * @param animal_type 动物种类的ID号
 * @param p_pollinate_num 动物一天最多授粉的次数
 */
uint32_t Cuser_farm_animal::get_butterfly_max_polliname_num(const uint32_t animal_type, uint32_t *p_pollinate_num)
{
	const uint32_t index=animal_type-BASE;
	if(index<ANIMAL_NUM)
	{
		*p_pollinate_num =  animal_attribute[index].pollinate_num;
	} else {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}


/* @brief 得到动物的总类
 * @param animal_type 动物种类的ID号
 * @param p_kind返回动物的总类
 */
uint32_t Cuser_farm_animal::get_animal_kind(const uint32_t animal_type, uint32_t *p_kind)
{
	const uint32_t index=animal_type - BASE;
	if(index<ANIMAL_NUM)
	{
		*p_kind =  animal_attribute[index].animal_kind;
	} else {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/**
 * @brief 插入动物的记录
 * @param userid  用户ID号
 * @param animalid 动物ID号
 */
int Cuser_farm_animal::insert(userid_t userid, uint32_t animalid, uint32_t type, uint32_t *p_id,
	   	uint32_t *p_max_output, uint32_t level)
{
	if(is_not_need_animal(animalid))
	{
		return ANIMAL_IS_NO_NEED_ERR ;
	}

	uint32_t now = time(NULL);
	uint32_t index = animalid - BASE;
	if (index >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t max_output = 0;
   	this->rand_max_output(animalid, &max_output);
	*p_max_output = max_output;
	sprintf(this->sqlstr, "insert into %s values (\
		        NULL, %u, %u, 0, 0, %u, %u, 0, 0, %u, 0, %u, %u, %u, 0, 0, 0, 0, %u, %u)",
			this->get_table_name(userid),
			userid,
			animalid,
			now,
			now,
			now,
			type,
			now,
			animal_attribute[index].favor,
			max_output,
			level
		  );
	STD_INSERT_GET_ID (this->sqlstr, DB_ERR, *p_id);
}

/* @brief 删除记录信息 
 * @userid 用户的ID号
 * @id  记录的ID号
 */
int Cuser_farm_animal :: del(userid_t userid, uint32_t id)
{
	sprintf( this->sqlstr, "delete from  %s where id = %u and userid = %u",
			this->get_table_name(userid),
			id,
			userid
		  );
	STD_SET_RETURN_EX(this->sqlstr, ANIMAL_IS_NOT_EXIST_ERR);
}

/* @brief 得到某种动物的产出
 * @param animalid 某种动物的ID号 
 * @param p_output_id 动物产出的ID号
 */
int Cuser_farm_animal :: get_output_id(uint32_t animalid, uint32_t *p_output_id)
{
	uint32_t offset = animalid - BASE; 
	if (offset >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_output_id = animal_attribute[offset].output_id;
	return SUCC;
}

/* @brief 得到某种动物的技能点
 * @param animalid 某种动物的ID号 
 * @param p_output_id 返回的技能点 
 */
int Cuser_farm_animal :: get_exp(uint32_t animalid, uint32_t *p_output_id)
{
	uint32_t i = 0;
	for (i = 0; i < ANIMAL_NUM; i++) {
		if (animal_attribute[i].mature_id == animalid) {
			break;	
		}
	}
	if (i >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_output_id = animal_attribute[i].exp;
	return SUCC;
}


/* @brief 得到某个动物是水生动物还是陆生动物
 * @param animalid 动物的种类的ID号
 * @param p_type 返回动物的类型
 */
int Cuser_farm_animal :: get_animal_type(uint32_t animalid, uint32_t *p_type)
{
	uint32_t offset = animalid - BASE;
	if (offset > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_type = 1;
	if (animal_attribute[offset].drought_interval == 0) {
		*p_type = 0;
	}
	return SUCC;
}

/* @brief 得到某个幼崽对应的成年动物的ID号 
 * @param animalid 某种动物的ID号
 * @parma p_adultid 对应的成年动物的ID号
 */
int Cuser_farm_animal :: get_adult_id(uint32_t animalid, uint32_t *p_adultid)
{
	uint32_t offset = animalid - BASE;
	if (offset > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_adultid = animal_attribute[offset].mature_id;
	return SUCC;
}

/* @brief 得到某种动物的干旱间隔时间 
 * @param animalid 动物的ID号
 * @param p_id 返回干旱时间间隔
 */
int Cuser_farm_animal :: get_drought_interval(uint32_t animalid, uint32_t *p_id)
{
	uint32_t offset = animalid - BASE;
	if (offset >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_id = animal_attribute[offset].drought_interval;
	return SUCC;
}

/* @brief 得到某个动物的信息
 * @param userid 用户的米米号
 * @param id 动物的唯一标号
 * @param p_list 保存东西信息的结构体，作为返回值
 */
int Cuser_farm_animal :: get_one_animal_info(userid_t userid, uint32_t id, animal_select *p_list, uint32_t *p_animal_type)
{
	sprintf(this->sqlstr, "select id, animalid, state, growth, eattime, drinktime, total_output,\
			               output_time, lastcal_time, mature, favor_time, favor, outgo,\
						   animal_flag, pollinate_num, pollinate_tm, max_output, level, animal_type from %s where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, ANIMAL_IS_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(p_list->id);
		INT_CPY_NEXT_FIELD(p_list->animal_id);
		INT_CPY_NEXT_FIELD(p_list->state);
		INT_CPY_NEXT_FIELD(p_list->growth);
		INT_CPY_NEXT_FIELD(p_list->eat_time);
		INT_CPY_NEXT_FIELD(p_list->drink_time);
		INT_CPY_NEXT_FIELD(p_list->total_output);
		INT_CPY_NEXT_FIELD(p_list->output_time);
		INT_CPY_NEXT_FIELD(p_list->lastcal_time);
		INT_CPY_NEXT_FIELD(p_list->mature_time);
		INT_CPY_NEXT_FIELD(p_list->favor_time);
		INT_CPY_NEXT_FIELD(p_list->favor);
		INT_CPY_NEXT_FIELD(p_list->outgo);
		INT_CPY_NEXT_FIELD(p_list->animal_flag);
		INT_CPY_NEXT_FIELD(p_list->pollinate_num);
		INT_CPY_NEXT_FIELD(p_list->pollinate_tm);
		INT_CPY_NEXT_FIELD(p_list->max_output);
		INT_CPY_NEXT_FIELD(p_list->level);
		INT_CPY_NEXT_FIELD(*p_animal_type);
	STD_QUERY_ONE_END();
}


/**
 * @brief 得到一个用户全部动物的信息，排除那些逃跑已经7天以上的动物
 * @param userid 用户ID号
 * @param pp_list 保存返回的信息
 * @param p_count 保存返回的记录数
 * @note  需要释放pp_list分配的空间
 */
int Cuser_farm_animal::get_animal(userid_t userid, animal_select **pp_list, uint32_t *p_count)
{
		//删除逃跑7天的动物，此时用户再也无法找回动物
	uint32_t ct=std::time(0);
	const uint32_t second_7_day=7*24*60*60;

	char go_out_more_than_7[200]={};
	sprintf(go_out_more_than_7,
			"((outgo & 0x02 = 0x02) and (%u - lastcal_time > %u))",
			ct,
			second_7_day
		);
	sprintf(this->sqlstr, "delete from %s where  userid=%u and %s",
			this->get_table_name(userid),
			userid,
			go_out_more_than_7
		);
	int cont;
	this->db->exec_update_sql(this->sqlstr, &cont);

	sprintf(this->sqlstr, "select id, animalid, state, growth, eattime, drinktime, total_output,\
			               output_time, lastcal_time, mature , animal_type, favor_time, favor,\
						   outgo, animal_flag, pollinate_num, pollinate_tm, max_output, level\
						   from %s where userid = %u and (outgo & 0x02 != 0x02)",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth);
		INT_CPY_NEXT_FIELD((*pp_list + i)->eat_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->drink_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->total_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->output_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lastcal_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mature_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor);
		INT_CPY_NEXT_FIELD((*pp_list + i)->outgo);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_tm);
		INT_CPY_NEXT_FIELD((*pp_list + i)->max_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
	STD_QUERY_WHILE_END();
}

/* @brief 得到某个类型动物的信息,并计算其亲密度 
 * @param userid 米米号
 * @param type 动物类型，0表示水生动物，1表示陆生动物
 * @parma pp_list 保存动物信息的结构体
 * @param p_count 返回符合条件的动物的数目
 */
int Cuser_farm_animal :: get_animal_by_type_ex(userid_t userid, uint32_t type, animal_select **pp_list, uint32_t *p_count)
{
	uint32_t ret = this->get_animal_by_type(userid, type, pp_list, p_count);
	if (ret != SUCC) {
		return ret;
	}
	//animal_select *p_temp = NULL;
	//for (uint32_t i = 0; i < *p_count; i++) {
		//p_temp = (*pp_list + i);
		//p_temp->favor = (uint32_t)((p_temp->favor * 1.0 / animal_attribute[p_temp->animal_id - BASE].favor) * 100);
	//}
	return SUCC;
}


/* @brief 得到某个类型动物的信息 
 * @param userid 米米号
 * @param type 动物类型，0表示水生动物，1表示陆生动物
 * @parma pp_list 保存动物信息的结构体
 * @param p_count 返回符合条件的动物的数目
 */
int Cuser_farm_animal :: get_animal_by_type(userid_t userid, uint32_t type, animal_select **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, animalid, state, growth, eattime, drinktime, total_output,\
		   output_time, lastcal_time, mature, animal_type, favor_time, favor, outgo,\
		   animal_flag, pollinate_num, pollinate_tm, max_output, level  from %s \
		   where userid = %u and animal_type = %u ",
			this->get_table_name(userid),
			userid,
			type 
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth);
		INT_CPY_NEXT_FIELD((*pp_list + i)->eat_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->drink_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->total_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->output_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lastcal_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mature_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor);
		INT_CPY_NEXT_FIELD((*pp_list + i)->outgo);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_tm);
		INT_CPY_NEXT_FIELD((*pp_list + i)->max_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
	STD_QUERY_WHILE_END();
}

/* @brief 得到某个用户的全部动物的信息 
 * @param user 用户的米米号
 * @param pp_list 保存动物信息的结构体，作为返回值
 * @param p_count 返回动物的个数 
 */
int Cuser_farm_animal :: get_all_animal(userid_t userid, stru_farm **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, userid, animalid, state, growth, eattime, drinktime, total_output,\
		   output_time, lastcal_time, mature,animal_type, favor_time, favor, outgo, pollinate_num, max_output\
		   from %s where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animalid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth);
		INT_CPY_NEXT_FIELD((*pp_list + i)->eattime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->drinktime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->total_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->output_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lastcal_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mature);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor);
		INT_CPY_NEXT_FIELD((*pp_list + i)->outgo);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->max_output);
	STD_QUERY_WHILE_END();
}

/* @brief 更新记录信息
 * @param userid 用户的ID号
 * @param p_list 需要更新的信息
 */
int Cuser_farm_animal :: update(userid_t userid, animal_select *p_list)
{
	sprintf(this->sqlstr, "update %s set state = %u, growth = %u, eattime = %u, drinktime = %u,\
		                   total_output = %u, output_time = %u, lastcal_time = %u, mature = %u,\
						   favor_time = %u, favor = %u, outgo = %u where id = %u",
						   this->get_table_name(userid),
						   p_list->state,
						   p_list->growth,
						   p_list->eat_time,
						   p_list->drink_time,
						   p_list->total_output,
						   p_list->output_time,
						   p_list->lastcal_time,
						   p_list->mature_time,
						   p_list->favor_time,
						   p_list->favor,
						   p_list->outgo,
						   p_list->id
	    );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/* @brief 更新某个动物的所有信息 
 * @param userid 用户的米米号
 * @param p_list 包含动物信息的结构体
 */
int Cuser_farm_animal :: update_all(userid_t userid, stru_farm *p_list)
{
	sprintf(this->sqlstr, "update %s set  animalid = %u,\
			state = %u, growth = %u, eattime = %u, drinktime = %u, total_output = %u,\
			output_time = %u, lastcal_time = %u, mature = %u, animal_type = %u, favor_time = %u\
			, favor = %u, outgo = %u, pollinate_num = %u, max_output = %u  where id = %u",
						   this->get_table_name(userid),
						   p_list->animalid,
						   p_list->state,
						   p_list->growth,
						   p_list->eattime,
						   p_list->drinktime,
						   p_list->total_output,
						   p_list->output_time,
						   p_list->lastcal_time,
						   p_list->mature,
						   p_list->animal_type,
						   p_list->favor_time,
						   p_list->favor,
						   p_list->outgo,
						   p_list->pollinate_num,
						   p_list->max_output,
						   p_list->id
	    );
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}


/* @brief 得到动物的成年值
 * @param animal_id 动物中种类的ID号
 * @param *p_mature 返回动物的成年值
 */
int Cuser_farm_animal :: get_mature(uint32_t animal_id, uint32_t *p_mature)
{
	uint32_t index = animal_id - BASE;
	if (index > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_mature = animal_attribute[index].mature;
	return SUCC;
}
	
/* @brief 得到动物目前的生长速率
 * @param animal_id 动物种类的ID号
 * @param state 动物的状态
 * @param *p_cur_grow 返回动物的成长速率
 */
int Cuser_farm_animal :: get_cur_grow(uint32_t animal_id, uint32_t state,  uint32_t *p_cur_grow)
{
	uint32_t index = animal_id - BASE;
	if (index > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	switch(state) {
		case 0:
			*p_cur_grow = animal_attribute[index].health_grow_rate;
			break;
		case 1:
			*p_cur_grow = animal_attribute[index].drought_grow_rate;
			break;
		case 2:
			*p_cur_grow = animal_attribute[index].hungry_grow_rate;
			break;
		case 3:
			*p_cur_grow = animal_attribute[index].both_grow_rate;
			break;
		default:
			return VALUE_OUT_OF_RANGE_ERR;
	}
	return SUCC;
}

/**
 * @brief 更新每条记录的喂养时间和饲料房里饲料的数目 
 * @param userid 用户的ID号
 * @param p_list 保存动物的信息链表
 * @param p_record_num 存放动物的记录数
 * @param p_farm 存放饲料房的信息
 * @param state 水池的状态
 */
int Cuser_farm_animal :: cal_list(userid_t userid, animal_select *p_list,  noused_homeattirelist *p_farm,
		          uint32_t water_time, uint32_t *p_output, uint32_t *p_runout, uint32_t child_flag,
				  uint32_t end_time, uint32_t& animal_drink, uint32_t& animal_catch)
{
	uint32_t eat_time = 0;
	uint32_t drink_time = 0;
	uint32_t now = time(0);
	if ((end_time < now) && (end_time > p_list->drink_time))
	{
		p_list->drink_time = end_time;
		animal_drink = animal_drink + (end_time - p_list->drink_time)/(60*60*24);
	}

	uint32_t animal_id = p_list->animal_id - BASE;
	if (animal_id > ANIMAL_NUM || (animal_id >= 75 && animal_id <= 124) ) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	/* 逃跑的不计算  */
	if ((p_list->outgo & 0x02) == 0x02) {
		return SUCC;
	}
	uint32_t food_index = 0;
	/* 得到饲料房里的信息  */
	attire_noused_item *p_food = p_farm->item;

	/* 查看动物吃那种食物 */
	for (food_index = 0; food_index < p_farm->count; food_index++) {
		if ((p_food + food_index)->attireid == animal_attribute[animal_id].food_type) {
			break;
		}
	}
	/*得到可消耗食物的数量*/
	uint32_t eat_foodnum = 0;
	if (food_index == p_farm->count) {
		eat_foodnum = 0;
	} else {
		eat_foodnum = (p_food + food_index)->count;
	}
	uint32_t before_eat = eat_foodnum;
	/* 设置最后一次吃食物的时间 */
	if (((p_list->outgo & 0x04) == 0x04) || (p_list->outgo == 0)) {
		this->set_eattime(p_list, &eat_foodnum);
		if (food_index < p_farm->count) {
			(p_food + food_index)->count = eat_foodnum; 
		}
		/* 得到饥饿和饥渴时间 */
		this->get_sick_time(p_list, &drink_time, &eat_time, water_time); 
	}

	if ((drink_time > 0) || ((p_list->state & 0x01) == 0x01))
	{
		if (end_time >= now)
		{
			drink_time = 0;
			p_list->drink_time = now;
			p_list->state = p_list->state & 0xFFFFFFFE;
			animal_drink = animal_drink + 1 + (now - p_list->drink_time)/(60*60*24);
		}
	}

	/* 计算好感度 */
	if ((p_list->animal_id != 1270008) && (p_list->outgo == 0 || ((p_list->outgo & 0x04) == 0x04))
		&& (p_list->animal_type ==1 || p_list->animal_type == 2)) {
		this->cal_favor(drink_time, eat_time, &p_list->favor_time, &p_list->favor, &p_list->outgo, p_runout);
		if ((p_list->outgo & 0x02)== 0x02)
		{
			//小公虎、小母虎、派对礼宾犬、绅士派对犬、卡酷年 极品圣光兽不可以出逃
			if ((p_list->animal_id == 1270040) || (p_list->animal_id == 1270041)
				|| (p_list->animal_id == 1270054) || (p_list->animal_id == 1270058)
				|| (p_list->animal_id == 1270075) || p_list->level == 3)
			{
				p_list->favor = 1;
				p_list->outgo = p_list->outgo & 0xFFFFFFFD;
			}
			if (end_time >= now)
			{
				p_list->favor = p_list->favor + 5;
				p_list->outgo = p_list->outgo & 0xFFFFFFFD;
				animal_catch++;
			}
		}
	}
	p_list->favor += (before_eat - eat_foodnum) * 2;
	if (p_list->favor > animal_attribute[animal_id].favor) {
		p_list->favor = animal_attribute[animal_id].favor;
		p_list->state &= 0x01;
		//p_list->eat_time = time(NULL);
	}
	if (p_list->mature_time == 0) {
		/* 计算动物的成长值 */
		this->cal_growth_ex(animal_id, p_list->lastcal_time, eat_time, drink_time, &p_list->growth);
		/* 如果成年，设置成年时间和产出时间 */
		if (p_list->growth >= animal_attribute[animal_id].mature) {
			if (p_list->mature_time == 0) {
				p_list->mature_time = now;
			}
			p_list->output_time = now;
		
			uint32_t diff = p_list->growth - animal_attribute[animal_id].mature;
			*p_output = diff / animal_attribute[animal_id].output_value;
			//判断家园里是否有公兔和母兔，如果都有能产出，没有不能产出
			if (p_list->animal_id == 1270015) {
				if (child_flag != 3) {
					*p_output = 0;
				}
			}
			uint32_t temp = p_list->total_output + *p_output;
			//if (temp > animal_attribute[animal_id].fruit_max) {
			if (temp > p_list->max_output) {
				//*p_output = animal_attribute[animal_id].fruit_max - p_list->total_output;
				*p_output = p_list->max_output - p_list->total_output;
				//p_list->total_output = animal_attribute[animal_id].fruit_max;
				p_list->total_output = p_list->max_output;
			} else {
				p_list->total_output += *p_output;
			}
			p_list->growth = animal_attribute[animal_id].mature;
		}
	} else {
		/* 如果成年计算产出 */
		/* 如果已经大于最大产出，则返回 */
		//if (p_list->total_output < animal_attribute[animal_id].fruit_max) {
		if (p_list->total_output < p_list->max_output) {
			/* 计算产出 */
			uint32_t num = (now - p_list->output_time) / (animal_attribute[animal_id].output_interval * 3600);
			if (num >= 1) {
				//判断家园里是否有公兔和母兔，如果都有能产出，没有不能产出
				if (p_list->animal_id == 1270015) {
					if (child_flag != 3) {
						num = 0;
					}
				}
				//配对是不能产出
				if ((p_list->outgo & 0x08) == 0x08) {
					num = 0;
				}
				*p_output = animal_attribute[animal_id].output[p_list->state] * num;
				uint32_t temp_total = p_list->total_output + *p_output; 
				//p_list->total_output += *p_output;
				//if (temp_total > animal_attribute[animal_id].fruit_max) {
				if (temp_total > p_list->max_output) {
					//*p_output = animal_attribute[animal_id].fruit_max - p_list->total_output;
					*p_output = p_list->max_output - p_list->total_output;
					//p_list->total_output = animal_attribute[animal_id].fruit_max;
					p_list->total_output = p_list->max_output;
				} else {
					p_list->total_output += *p_output;
				}
				p_list->output_time += animal_attribute[animal_id].output_interval * 3600 * (*p_output);
			}
		}
	}
	/* 更新时间 */
	p_list->lastcal_time = time(0);
	/* 更新动物的数据 */
	uint32_t ret = this->update(userid, p_list);
	if (ret != SUCC) {
		return ret;
	}
	//龙虾不需吃东西
	if (p_list->animal_id == 1270021) {
		p_list->state = 0;
	}
	ret = this->fill_attribute(p_list);
	return SUCC;
}

/**
 * @brief 计算动物的好感度 
 * @param drink_time 动物的饥渴发生时间，0表示没有发生饥渴 
 * @param eat_time 动物的饥饿发生时间，0表示没有发生饥饿
 * @param p_favor_time 动物上次计算好感度的时间，并更新此数值
 * @param p_favor 动物的好感度，并更新此值
 */
int Cuser_farm_animal :: cal_favor(uint32_t drink_time, uint32_t eat_time,
		                           uint32_t *p_favor_time, uint32_t *p_favor, uint32_t *p_outgo, uint32_t *p_runout)
{
	uint32_t now = time(NULL);
	uint32_t day = (now - *p_favor_time) / (3600 * 24);
	if (day == 0) {
		return SUCC;
	}
	*p_favor_time += day * 3600 * 24;
	if ((eat_time == 0) && (drink_time == 0)) {
		return SUCC;
	}
	uint32_t temp = *p_favor_time;
	for (uint32_t i = 1; i <= day; i++) {
			temp = temp + i * 3600 * 24;
			if (drink_time == 0 && temp < eat_time) {
				continue;
			}
			if (drink_time == 0 && temp > eat_time) {
				if (*p_favor == 0) {
					break;
				}
				if (*p_favor == 1) {
					*p_outgo |= 2;
					*p_favor = 0;
					*p_runout = 1;
					break;
				}
				*p_favor -= 1;
				continue;
			}
			if (eat_time == 0 && temp < drink_time) {
				continue;
			}
			if (eat_time == 0 && temp > drink_time) {
				if (*p_favor == 0) {
					break;
				}
				if (*p_favor == 1) {
					*p_outgo |= 2;
					*p_favor = 0;
					*p_runout = 1;
					break;
				}
				*p_favor -= 1;
				continue;
			}
			if (temp > drink_time && temp < eat_time) {
				if (*p_favor == 0) {
					break;
				}
				if (*p_favor == 1) {
					*p_outgo |= 2;
					*p_favor = 0;
					*p_runout = 1;
					break;
				}
				*p_favor -= 1;
				continue;
			}
			if (temp > eat_time && temp < drink_time) {
				if (*p_favor == 0) {
					break;
				}
				if (*p_favor == 1) {
					*p_outgo |= 2;
					*p_favor = 0;
					*p_runout = 1;
					break;
				}
				*p_favor -= 1;
				continue;
			}
			if (temp > eat_time && temp > drink_time) {
				if (*p_favor == 0) {
					break;
				}
				if (*p_favor == 1 || *p_favor == 2) {
					*p_favor = 0;
					*p_outgo |= 2;
					*p_runout = 1;
					break;
				}
				*p_favor -= 2;
				continue;
			}
	}
	return SUCC;
}

/**
 * @brief 重新设置喂养时间和饥饿状态 
 * @param animal_last_eat 一个动物的相关记录
 * @param food_num 动物所需的食物，目前拥有的数量
 */
int Cuser_farm_animal :: set_eattime(animal_select *p_list, uint32_t *food_num)
{
	uint32_t now = time(0);
	uint32_t animal_id = p_list->animal_id - BASE;
	if (animal_id > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t interval = 3600 * animal_attribute[animal_id].hungry_interval;
	/* 动物处于饥饿状态 */
	if(interval == 0){
		return 0;
	}
	uint32_t need_food = (now - p_list->eat_time) / interval;
	if (need_food != 0) {
		if (need_food > (*food_num)) {
			p_list->eat_time += (*food_num) * interval; 
			*food_num = 0;
			p_list->state |= 0x02;
		} else {
			*food_num -= need_food;
			p_list->eat_time += need_food * interval;
			p_list->state &= 0x01;
		}
	} else {
		p_list->state &= 0x01;
	}
	return 0;
}

/**
 * @brief 根据前期的状态，计算出饥饿，干旱时间 
 * @param p_animal 一个动物的状态信息
 * @param drink_time 发生干旱的时间
 * @param eat_time 发生饥饿的时间
 */
int Cuser_farm_animal :: get_sick_time(animal_select *p_animal, uint32_t *drink_time, uint32_t *eat_time, uint32_t water_time)
{
	//uint32_t now = time(0);
	uint32_t animalid = p_animal->animal_id - BASE;
	if (animalid > ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t drought_interval = animal_attribute[animalid].drought_interval * 3600; 
	uint32_t hungry_interval = animal_attribute[animalid].hungry_interval * 3600;
	uint32_t state = p_animal->state;
	if (drought_interval  == 0) {
		*drink_time = 0;
	} else if (state & 0x01) {
		*drink_time = p_animal->lastcal_time;
	} else if ( water_time  == 0) {
		*drink_time = 0;
	} else {
		p_animal->state |= 0x01;
	}

	if (state & 0x02) {
		if (p_animal->eat_time + hungry_interval < p_animal->lastcal_time) {
			*eat_time = p_animal->lastcal_time;
		} else {
			*eat_time = p_animal->eat_time + hungry_interval;
			if (*eat_time < p_animal->lastcal_time) {
				*eat_time = p_animal->lastcal_time;
			}
		}
	} else {
		*eat_time = 0;
	}
	return 0;
}


/**
 * @brief 对计算成长值的函数进行包裹
 * @param 参数意义同cal_growth函数
 */
int Cuser_farm_animal :: cal_growth_ex(uint32_t id, uint32_t cal_time, uint32_t eat_time,
		                 uint32_t drink_time, uint32_t *value)
{
	uint32_t animal_value = (*value) * 3600;
	this->cal_growth(id, cal_time, eat_time, drink_time, &animal_value);
	*value = animal_value / 3600;
	return SUCC;
}

/**
 * @brief 计算成长值 
 * @param cal_time 上次计算时间
 * @param eat_time 饥饿发生时间
 * @param drink_time   饥渴发生时间
 * @param value 计算的成长值
 */
int Cuser_farm_animal :: cal_growth(uint32_t id, uint32_t cal_time, uint32_t eat_time,
		                 uint32_t drink_time, uint32_t *value)
{
	uint32_t now = time(NULL);
	uint32_t health_grow_rate = animal_attribute[id].health_grow_rate;
	uint32_t hungry_grow_rate = animal_attribute[id].hungry_grow_rate;
	uint32_t drought_grow_rate = animal_attribute[id].drought_grow_rate;
	uint32_t both_grow_rate = animal_attribute[id].both_grow_rate;

	if (drink_time == 0 && eat_time == 0) {
		*value += (now - cal_time) * health_grow_rate;
		return SUCC;
	}

	if (drink_time == 0 && eat_time >= cal_time) {
		*value += (eat_time - cal_time) * health_grow_rate;
		*value += (now - eat_time) * hungry_grow_rate;
		return SUCC;
	}

	if (eat_time == 0 && drink_time >= cal_time) {
		*value += (drink_time - cal_time) * health_grow_rate;
		*value += (now - drink_time) * drought_grow_rate;
		return SUCC;
	}

	if (drink_time <= cal_time && cal_time <= eat_time) {
		*value += (eat_time - cal_time) *  drought_grow_rate ;
		*value += (now - eat_time) * both_grow_rate;
		return SUCC;
	}

	if (eat_time <= cal_time && cal_time <= drink_time) {
		*value += (drink_time - cal_time) * hungry_grow_rate;
		*value += (now - cal_time) * both_grow_rate;
		return SUCC;
	}

	if (eat_time <= drink_time && drink_time <= cal_time) {
		*value += (now - cal_time) * both_grow_rate;
		return SUCC;
	}

	if (cal_time <= drink_time && drink_time <= eat_time) {
		*value += (drink_time - cal_time) * health_grow_rate;
		*value += (eat_time - drink_time) * drought_grow_rate;
		*value += (now - eat_time) * both_grow_rate;
		return SUCC;
	}

	if (cal_time <= eat_time && eat_time <= drink_time) {
		*value += (eat_time - cal_time) * health_grow_rate;
		*value += (drink_time - eat_time) * hungry_grow_rate;
		*value += (now - drink_time) * both_grow_rate;
		return SUCC;
	}
	return 0;
}

/**
 * @brief 清楚陆生动物的饥渴状态 
 * @param userid 用户的米米号
 * @param p_list 保存从数据库拉取的信息
 * @param p_count 拉取的动物的数目
 */
int Cuser_farm_animal :: animal_drink(userid_t userid, animal_select **p_list, uint32_t *p_count)
{
	int ret = this->get_animal_by_type(userid, 1, p_list, p_count);
	if (ret != SUCC) {
		return ret;
	}
	for (uint32_t i = 0; i < *p_count; i++) {
		(*p_list + i)->state &= 0x02;
		int ret = this->update(userid, (*p_list + i));
		if (ret != SUCC) {
			return ret;
		}
	}
	return SUCC;
}

/**
 * @brief 得到陆生动物的数目 
 * @param userid  用户的米米号
 * @param p_count 陆生动物的数目
 */
int Cuser_farm_animal :: land_animal_count(userid_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and animal_type =1 and ((outgo & 0x02) != 0x02)",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	        INT_CPY_NEXT_FIELD(*p_count);
    STD_QUERY_ONE_END();
}

/**
 * @brief 得到水生动物的数目 
 * @param userid 用户的米米号 
 * @param p_count 返回水生动物的数目
 */
int Cuser_farm_animal :: water_animal_count(userid_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and animal_type = 0 and outgo != 2",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

/**
 * @brief 设置带出标志 0表示正常，1表示带出
 * @param userid 用户的米米号
 * @param id 动物的唯一ID号
 * @param flag 设置的标志位
 */
int Cuser_farm_animal :: set_or_cancel_outgo(userid_t userid, uint32_t id, uint32_t flag)
{
	sprintf(this->sqlstr, "update %s set outgo = %u where id = %u and userid = %u",
			this->get_table_name(userid),
			flag,
			id,
			userid
		   );	
	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}

/**
 * @brief 根据ID号得到动物的种类 
 * @param userid 米米号
 * @param id 动物的ID号
 * @param p_animalid 动物的种类
 */
int Cuser_farm_animal :: get_animal_id(userid_t userid, uint32_t id, uint32_t *p_animalid)
{
	sprintf(this->sqlstr, "select  animalid from %s where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_animalid);
	STD_QUERY_ONE_END();
}

/**
 * @brief 设置动物的吃食时间
 * @param userid 用户的米米号
 * @param id 动物的唯一ID号
 */
int Cuser_farm_animal :: set_eat_time(userid_t userid, uint32_t id)
{
	uint32_t animalid = 0;
	uint32_t ret = this->get_animal_id(userid, id, &animalid);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t index = animalid - BASE;
	if (index >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t now = time(NULL);
	sprintf(this->sqlstr, "update %s set eattime = %u, state = state&0x01, favor = %u where id = %u\
		   			       and userid = %u and animalid in (1270006, 1270007, 1270038)",
			this->get_table_name(userid),
			now,
			animal_attribute[index].favor,
			id,
			userid
		   );	
	STD_SET_RETURN_EX(this->sqlstr, THIS_ANIMAL_CANNOT_EAT_GRASS_ERR);
}

/**
 * @brief 得到带出动物的数目
 * @param userid 米米号
 * @param p_count 返回的带出动物的数目
 */
int Cuser_farm_animal :: get_outgo_count(userid_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select  count(*) from %s where userid = %u and (outgo & 0x01 = 0x01)",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

/**
 * @brief 清楚动物带出的标志
 * @param userid 用户的米米号
 */
int Cuser_farm_animal :: update_outgo_zero(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set outgo = (outgo & 0xFFFE) where userid = %u",
			this->get_table_name(userid),
			userid
		   );	
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 得到某个动物初始好感值
 * @param animal_id 动物种类的ID号 
 */
int Cuser_farm_animal :: get_init_favor(uint32_t animal_id) 
{
	uint32_t index = animal_id - BASE;
	if (index >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	return animal_attribute[index].favor;
}

/**
 * @breif 得到成熟鱼的ID号和对应的幼崽ID号
 * @param userid
 * @param pp_list
 * @param p_out
 */
int Cuser_farm_animal :: get_mature_fish(userid_t userid, user_farm_get_mature_fish_out **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, animalid  from %s where userid = %u and mature > 0 and animal_type=0",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animalid);
	STD_QUERY_WHILE_END();
}

/**
 * @brief 返回捕到的鱼的种类和数目
 * 
 */
int Cuser_farm_animal :: net_catch_fish(userid_t userid, user_farm_net_catch_fish_out_item **pp_list, uint32_t *p_out_count)
{
	user_farm_get_mature_fish_out *p_out = NULL;
	uint32_t mature_count = 0;
	uint32_t ret = this->get_mature_fish(userid, &p_out, &mature_count);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t count = 0;
	if (mature_count <= 5) {
		count = mature_count;
	} else {
		count = rand() % 3 + 3;
	}
	uint32_t temp[5][2] = {	};
	uint32_t animal_count = 0;
	for (uint32_t i = 0; i < count; i++) {
		for(uint32_t j = 0; j < count; j++) {
			if (temp[j][0] == 0) {
				temp[j][0] = (p_out + i)->animalid;
				temp[j][1] = 1;
				animal_count++;
				break;
			} else if (temp[j][0] == (p_out + i)->animalid) {
				temp[j][1] += 1;
				break;
			}
		}
	}
	for (uint32_t i = 0; i < count; i++) {
		ret = this->del(userid, (p_out + i)->id);
		if (ret != SUCC) {
			return ret;
		}
	}
	free(p_out);
	for(uint32_t i = 0; i < animal_count; i++) {
		uint32_t mature_id = 0;
		ret = this->get_adult_id(temp[i][0], &mature_id);
		if (ret != SUCC) {
			return ret;
		}
		temp[i][0] = mature_id;
	}
	*p_out_count = animal_count;
	*pp_list = (typeof(*pp_list))malloc(sizeof(typeof(**pp_list)) * (*p_out_count));
	memset(*pp_list, 0, sizeof(typeof(**pp_list)) *(*p_out_count));
	memcpy(*pp_list, temp, sizeof(typeof(**pp_list)) *(*p_out_count));
	return SUCC;
}

/**
 * @brief 得到outgo字段的值
 * @param userid 
 */
int Cuser_farm_animal :: get_outgo(userid_t userid, uint32_t id, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select outgo  from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cuser_farm_animal :: get_wool(userid_t userid, uint32_t id)
{
	uint32_t outgo = 0;
	uint32_t ret = this->get_outgo(userid, id, &outgo);
	if (ret != SUCC) {
		return ret;
	}
	if ((outgo & 0x04) != 0) {
		return YOU_HAVE_GET_WOOL_ERR;
	}
	outgo |= 0x04;
	ret = this->set_or_cancel_outgo(userid, id, outgo);
	return ret;
}



/**
 * @brief  得到某种动物的数目 
 * @param userid  用户的米米号
 * @param p_count 动物的数目
 */
int Cuser_farm_animal :: type_animal_count(userid_t userid, uint32_t type, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and animal_type = %u and ((outgo & 0x02) != 0x02)",
			this->get_table_name(userid),
			userid,
			type
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	        INT_CPY_NEXT_FIELD(*p_count);
    STD_QUERY_ONE_END();
}

/**
 * @breif 放生昆虫
 * @param userid 用户的米米号
 * @param id 动物的唯一编号
 * @param type 那种类型的动物，0表示水生，1表示陆生，2表示昆虫房
 * 		由于蚕宝宝移到昆虫屋后，其type值为1，而养在昆虫屋中蚕宝宝type值为2，故删除sql中的animal_type
 */
int Cuser_farm_animal :: release_insect(userid_t userid, uint32_t id, uint32_t type)
{
	//sprintf(this->sqlstr, "delete from  %s where id = %u and userid = %u and animal_type = %u",
	sprintf(this->sqlstr, "delete from  %s where id = %u and userid = %u",
			this->get_table_name(userid),
			id,
			userid
		  );
	STD_SET_RETURN_EX(this->sqlstr, ANIMAL_IS_NOT_EXIST_ERR);
}

/**
 * @brief 得到某个类型动物的信息 
 * @param userid 米米号
 * @param type 动物类型，0表示水生动物，1表示陆生动物
 * @parma pp_list 保存动物信息的结构体
 * @param p_count 返回符合条件的动物的数目
 */
int Cuser_farm_animal :: get_land_animal_insect(userid_t userid, animal_select **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, animalid, state, growth, eattime, drinktime, total_output,\
		   output_time, lastcal_time, mature, animal_type, favor_time, favor, outgo, animal_flag,\
		   pollinate_num, pollinate_tm, max_output, level from %s \
		   where userid = %u and (animal_type = 1 or animal_type=2)",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth);
		INT_CPY_NEXT_FIELD((*pp_list + i)->eat_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->drink_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->total_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->output_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->lastcal_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->mature_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->favor);
		INT_CPY_NEXT_FIELD((*pp_list + i)->outgo);
		INT_CPY_NEXT_FIELD((*pp_list + i)->animal_flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pollinate_tm);
		INT_CPY_NEXT_FIELD((*pp_list + i)->max_output);
		INT_CPY_NEXT_FIELD((*pp_list + i)->level);
	STD_QUERY_WHILE_END();
}

/**
 * @breif 更新某个字段的值 
 * @param usrid 米米号
 * @param p_char 要更新的字段
 * @param value 字段的数值
 */
int Cuser_farm_animal :: update_colum_sql(userid_t userid, uint32_t id, const char *p_char, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			p_char,
			value,
			userid,
			id
		   );	
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}


/**
 * @brief 设置动物标志的某个位 
 * @param userid 用户的米米号
 * @param index 设置哪个位
 */
int Cuser_farm_animal :: update_animal_flag_sql(userid_t userid, uint32_t index, uint32_t id)
{
	sprintf(this->sqlstr, "update %s set animal_flag = (animal_flag | (1 << %u)) where userid = %u and id = %u",
			this->get_table_name(userid),
			index - 1,
			userid,
			id
		   );	
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}


/**
 * @brief 得到某种生动物的数目 
 * @param userid  用户的米米号
 * @param attireid 动物种类的ID号
 * @param p_count 陆生动物的数目
 */
int Cuser_farm_animal :: get_one_animal_count_sql(userid_t userid, uint32_t attireid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and animalid = %u and ((outgo & 0x02) != 0x02)",
			this->get_table_name(userid),
			userid,
			attireid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	        INT_CPY_NEXT_FIELD(*p_count);
    STD_QUERY_ONE_END();
}


/**
 * @brief 得到某个动物的信息
 * @param userid 用户的米米号
 * @param id 动物的唯一标号
 * @param p_list 保存东西信息的结构体，作为返回值
 */
int Cuser_farm_animal :: get_one_animal_info_ex(userid_t userid, uint32_t id, animal_select *p_list,
	                     uint32_t *p_animal_type, uint32_t *p_pollinate_tm) {
	sprintf(this->sqlstr, "select id, animalid, state, growth, eattime, drinktime, total_output,\
			               output_time, lastcal_time, mature, favor_time, favor, outgo,\
						   animal_flag, pollinate_num, pollinate_tm, max_output,\
						   animal_type, pollinate_tm from %s where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, ANIMAL_IS_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(p_list->id);
		INT_CPY_NEXT_FIELD(p_list->animal_id);
		INT_CPY_NEXT_FIELD(p_list->state);
		INT_CPY_NEXT_FIELD(p_list->growth);
		INT_CPY_NEXT_FIELD(p_list->eat_time);
		INT_CPY_NEXT_FIELD(p_list->drink_time);
		INT_CPY_NEXT_FIELD(p_list->total_output);
		INT_CPY_NEXT_FIELD(p_list->output_time);
		INT_CPY_NEXT_FIELD(p_list->lastcal_time);
		INT_CPY_NEXT_FIELD(p_list->mature_time);
		INT_CPY_NEXT_FIELD(p_list->favor_time);
		INT_CPY_NEXT_FIELD(p_list->favor);
		INT_CPY_NEXT_FIELD(p_list->outgo);
		INT_CPY_NEXT_FIELD(p_list->animal_flag);
		INT_CPY_NEXT_FIELD(p_list->pollinate_num);
		INT_CPY_NEXT_FIELD(p_list->pollinate_tm);
		INT_CPY_NEXT_FIELD(p_list->max_output);
		INT_CPY_NEXT_FIELD(p_list->level);
		INT_CPY_NEXT_FIELD(*p_animal_type);
		INT_CPY_NEXT_FIELD(*p_pollinate_tm);
	STD_QUERY_ONE_END();
}

/**
 * @brief 设置动物标志的某个位 
 * @param userid 用户的米米号
 * @param index 设置哪个位
 */
int Cuser_farm_animal :: update_two_colum_sql(userid_t userid, uint32_t id, const char *p_colum1,
	                 	const char *p_colum2, uint32_t colum1_value, uint32_t colum2_value)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			p_colum1,
			colum1_value,
			p_colum2,
			colum2_value,
			userid,
			id
		   );	
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 蝴蝶给花授粉
 * @param usrid 米米号
 * @param id 动物的编号
 */
int Cuser_farm_animal :: butterfly_pollinate(userid_t userid, uint32_t id)
{
	animal_select item = { };
	uint32_t animal_type = 0;
	uint32_t last_pollinate_tm = 0;
	uint32_t ret = this->get_one_animal_info_ex(userid, id, &item, &animal_type, &last_pollinate_tm);
   	if (ret != SUCC) {
		return ret;
	}
	if (last_pollinate_tm != (uint32_t)get_date(time(NULL))) {
		item.pollinate_num = 0;
		last_pollinate_tm = get_date(time(NULL));
	}
	uint32_t max_pollinate_num = 0;
	ret = this->get_butterfly_max_polliname_num(item.animal_id, &max_pollinate_num);
	if (ret != SUCC) {
		return ret;
	}
	if (item.pollinate_num >= max_pollinate_num) {
		return THIS_ANIMAL_POLLINATE_TOO_MANY_TIMES_ERR;
	}
	item.pollinate_num++;
	ret = this->update_two_colum_sql(userid, item.id,  "pollinate_num", "pollinate_tm",
		    	item.pollinate_num, last_pollinate_tm);
	return ret;
}

/**
 * @breif 把动物的好感度转换为百分比
 * @param animal_id 动物种类的ID号
 * @param p_favor 动物的好感度
 */
int Cuser_farm_animal ::  set_favor(uint32_t animal_id, uint32_t *p_favor)
{
	*p_favor = (uint32_t)((*p_favor * 1.0 / animal_attribute[animal_id - BASE].favor) * 100);
	return SUCC;
}

/**
 * @brief 计算动物里成年还有多长时间
 * @param animal_id 动物种类的ID号
 * @param grow_rate 动物的生长速率
 * @param cur_value 动物目前的成长值
 * @param diff_mature 返回还有多长时间达到成年，时间为分钟
 */
int Cuser_farm_animal :: set_diff_mature(uint32_t animal_id, uint32_t grow_rate, uint32_t cur_value, uint32_t *p_diff_mature)
{
	*p_diff_mature = (uint32_t)((animal_attribute[animal_id - BASE].mature - cur_value) * 1.0 / grow_rate * 60);
	return SUCC;
}

/**
 * @brief 得到动物还剩可授粉的次数
 * @param animal_id 动物的种类的ID号
 * @param p_pollinate_num 返回可授粉的次数
 */
int Cuser_farm_animal :: set_pollinate_num(uint32_t animal_id, uint32_t pollinate_tm, uint32_t *p_pollinate_num)
{
	uint32_t limit = 0;
	if(animal_id == 1270044){
		limit = 5;	
	}
	else if(animal_id == 1270131){
		limit = 3;
	}
	if (animal_id == 1270044 || animal_id == 1270131) 
	{
		if(*p_pollinate_num >= limit)
		{
			*p_pollinate_num = 100;
		}
		else
		{
			if (pollinate_tm != (uint32_t)get_date(time(NULL)))
			{
				*p_pollinate_num = 1;
			}
			else
			{
				*p_pollinate_num = 0;
			}
		}

		return SUCC;
	}
	
	if (*p_pollinate_num == 100) {
		return SUCC;
	}
	if ((int)pollinate_tm != get_date(time(NULL))) {
		*p_pollinate_num = 0;
	}
	*p_pollinate_num = animal_attribute[animal_id - BASE].pollinate_num - *p_pollinate_num; 
	return SUCC;
}

/**
 * @brief 得到动物还可以产出多少
 * @param animal_id 动物种类的ID号
 * @param p_total_output 返回动物还可以产出多少
 */
int Cuser_farm_animal :: set_total_output(uint32_t max_output, uint32_t *p_total_output)
{
	if (max_output < *p_total_output) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_total_output = max_output - (*p_total_output);
	return SUCC;
}

/**
 * @brief 填充动物的属性
 * @param p_item 保存动物信息的结构体
 */
int Cuser_farm_animal :: fill_attribute(animal_select *p_item)
{
	if (p_item->animal_id - BASE >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	this->get_cur_grow(p_item->animal_id, p_item->state, &p_item->cur_grow);
	this->set_favor(p_item->animal_id, &p_item->favor);
	this->set_diff_mature(p_item->animal_id, p_item->cur_grow, p_item->growth, &p_item->diff_mature);
	this->set_pollinate_num(p_item->animal_id, p_item->pollinate_tm, &p_item->pollinate_num);
	this->set_total_output(p_item->max_output, &p_item->total_output);	
	return SUCC;
}

/**
 * @brief 对一些动物进行特殊的校验
 * @param userid 米米号
 * @param animalid 动物总类的ID号
 */
int Cuser_farm_animal :: special_animal_check(uint32_t userid, uint32_t animalid)
{
	uint32_t ret = 0;
	switch(animalid) {
	case 1270028:/* 只能有一只 */
	{
		uint32_t count = 0;
		ret = this->get_one_animal_count_sql(userid, 1270028, &count);
		if (ret != SUCC) {
			return ret;
		}
		if (count >= 1) {
			return THIS_ANIMAL_OUT_OF_LIMIT_ERR;
		}
		break;
	}
	default:
		return SUCC;
	}
	return SUCC;
}

/**
 * @brief 对牧场动物的总数进行校验
 * @param userid 米米号
 * @param type 动物的分类，
 */
uint32_t Cuser_farm_animal :: check_animal_max_limit(userid_t userid, uint32_t type)
{
	uint32_t total_count = 0;
	uint32_t ret = 0;
	if (type == this->aquatic_animal) {//0表示是水生动物
		ret = this->water_animal_count(userid, &total_count);
		if (ret != SUCC) {
			return ret;
		}
		if (total_count >= 30) {
			return WATER_ANIMAL_THAN_MAX_ERR;
		}
	} else if (type == this->terrestrial_animal) {//1表示陆生动物
		ret  = this->land_animal_count(userid, &total_count);
		if (ret != SUCC) {
			return ret;
		}
		if (total_count >= 15) {
			return LAND_ANIMAL_THAN_MAX_ERR;
		}
	} else if (type == this->insetct_animal || type == this->silkworm) {//2或4表示昆虫房
		ret = this->type_animal_count(userid, type, &total_count);
		if (ret != SUCC) {
			return ret;
		}
		if (total_count >= 8) {
			return INSECT_THAN_MAX_ERR;
		}
	}

	return SUCC;
}

uint32_t Cuser_farm_animal :: check_both_buck_doe(animal_select **pp_list, uint32_t count, uint32_t *p_flag)
{
	*p_flag = 0;
	for (uint32_t i = 0; i < count; i++) {
		//花兔，灰兔（公兔）
		if (((*pp_list + i)->animal_id == 1270016) || ((*pp_list + i)->animal_id == 1270017)) {
			*p_flag |= 0x01;
		//白兔（母兔）
		} else if ((*pp_list + i)->animal_id == 1270015) {
			*p_flag |= 0x02;
		}
	}
	return SUCC;
}

uint32_t Cuser_farm_animal :: rand_max_output(uint32_t animal_id, uint32_t *p_count)
{
	if (animal_id - BASE >= ANIMAL_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t min = animal_attribute[animal_id - BASE].fruit_min;
	uint32_t max = animal_attribute[animal_id - BASE].fruit_max;
	if (max != 0 && min <= max) {
		*p_count = (rand() % (max - min + 1)) + min;
	} else {
		*p_count = 0;
	}
	return SUCC;
}


uint32_t Cuser_farm_animal :: set_animal_flag( userid_t userid, uint32_t id, uint32_t index)
{ 
	animal_select temp;
	uint32_t animal_type;
	if (index < 1 || index > 32) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t ret = this->get_one_animal_info(userid, id, &temp, &animal_type);
	if (ret != SUCC) {
		return ret;
	}
	if (temp.mature_time == 0) {
		return ANIMAL_NOT_ADULT_ERR;
	}
	if (temp.animal_flag & (1 << (index -1))) {
		return ANIMAL_NOT_ADULT_ERR;
	}
	return this->update_animal_flag_sql(userid, index, id);
}

/**
 * @brief 挤牛奶 
 * @param usrid 米米号
 * @param id 动物的编号
 */
int Cuser_farm_animal ::milk_cow(userid_t userid, uint32_t id)
{
	animal_select item = { };
	uint32_t animal_type = 0;
	uint32_t last_pollinate_tm = 0;
	uint32_t ret = this->get_one_animal_info_ex(userid, id, &item, &animal_type, &last_pollinate_tm);
   	if (ret != SUCC) {
		return ret;
	}

	if (item.animal_id != 1270044 && item.animal_id != 1270131)
	{
		return ANIMAL_NOT_MILK_COW_ERR;
	}
	if (item.mature_time == 0) {
		return ANIMAL_NOT_ADULT_ERR;
	}
	uint32_t limit = 0;
	if(item.animal_id == 1270044){
		limit = 5;
	}
	else{
		limit = 3;
	}
	if (item.pollinate_num < limit)
	{
		if (last_pollinate_tm != (uint32_t)get_date(time(NULL))) {
			item.pollinate_num++;
			last_pollinate_tm = get_date(time(NULL));
		}
		else
		{
			return USER_ANIMAL_TODAY_MILK_COW_AREADY_ERR;
		}
	}
	else
	{
		return  USER_ANIMAL_MILK_COW_TIME_OVER_ERR;
	}

	ret = this->update_two_colum_sql(userid, item.id,  "pollinate_num", "pollinate_tm",
		    	item.pollinate_num, last_pollinate_tm);

	return ret;
}

/*
 * @brief 得到牧场养的动物的种类和数量
 */
int Cuser_farm_animal::get_animal_attireid_and_cnt(userid_t userid, 
		uint32_t *p_count, user_get_attireid_and_cnt_out_item** pp_list)
{
	sprintf(this->sqlstr, "select animalid, count(*) from %s where userid = %u group by animalid",
			this->get_table_name(userid), userid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count) ;
		INT_CPY_NEXT_FIELD((*pp_list + i)->attire_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attire_cnt);
	STD_QUERY_WHILE_END();
}

/*                         
 * @brief 删除昆虫       
 */                        
int Cuser_farm_animal::del_insect(userid_t userid, uint32_t animalid)
{                          
    sprintf(this->sqlstr, "delete from %s where userid = %u and animalid = %u",
            this->get_table_name(userid),
            userid,        
            animalid);     
        
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief 天使蛋随机出动物
 */
int Cuser_farm_animal::judge_egg(uint32_t &animalid, uint32_t &level)
{
	uint32_t random= rand() % 100;
	for(uint32_t k = 0; k < (sizeof(rand_egg_array) / sizeof(user_farm_egg_rand_info)); ++k){
		if( rand_egg_array[k].eggid == animalid){
			for( uint32_t i = 0; i< rand_egg_array[k].count; ++i){
				if( random >= rand_egg_array[k].rand_egg[i].rand_start && 
						random < rand_egg_array[k].rand_egg[i].rand_end){
					animalid = rand_egg_array[k].rand_egg[i].animalid;
					level = rand_egg_array[k].rand_egg[i].level;
				}
			}
		}
	}
	return SUCC;	
}

/*
 * @brief 加速动物成长
 */
int Cuser_farm_animal::speed_cal_growth(userid_t userid, uint32_t id, user_farm_speed_animal_growth_out *out
		,uint32_t up_growth_value, uint32_t &output)
{
	uint32_t animal_type = 0;
	uint32_t retu = get_one_animal_info(userid, id, out, &animal_type);
	if(retu == ANIMAL_IS_NOT_EXIST_ERR){
		 return 0;		
	}	
	uint32_t have_child = 0;
	this->check_both_buck_doe(&out, 1, &have_child);
	uint32_t offset = out->animal_id - BASE;
	uint32_t speed_grow_value = up_growth_value * animal_attribute[offset].health_grow_rate;
	if(out->growth + speed_grow_value >= animal_attribute[offset].mature){

		 /* 如果成年，设置成年时间和产出时间 */
		uint32_t growth_sum = out->growth + speed_grow_value;
		out->output_time = (uint32_t)time(0);
		uint32_t diff = growth_sum - animal_attribute[offset].mature;
		output = diff / animal_attribute[offset].output_value;
		//判断家园里是否有公兔和母兔，如果都有能产出，没有不能产出
		if (out->animal_id == 1270015) {
			if (have_child != 3) {
				output = 0;
			}
		}
		uint32_t temp = out->total_output + output;
		if (temp > out->max_output) {
			output = out->max_output - out->total_output;
			out->total_output = out->max_output;
		} else {
			out->total_output += output;
		}

		//置返回值
		out->growth = animal_attribute[offset].mature;
		out->mature_time = (uint32_t)time(0);
		out->diff_mature = 0;
	
	}else{
		out->growth += speed_grow_value;
		uint32_t cur_grow = 0;
		get_cur_grow(out->animal_id, out->state,  &cur_grow);
		set_diff_mature(out->animal_id, cur_grow, out->growth, &(out->diff_mature));
	}
	uint32_t tmp_total_output = out->total_output;
	this->set_total_output(out->max_output, &out->total_output);

	sprintf(this->sqlstr, "update %s set growth = %u ,mature = %u, total_output = %u,\
			output_time = %u  where id = %u",
			this->get_table_name(userid),
			out->growth,
			out->mature_time,
			tmp_total_output,
			out->output_time,
			id
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/*
 * @brief获取动物成长值和等级
 */
int Cuser_farm_animal::get_growth_level(userid_t userid, uint32_t id,  user_fairy_get_growth_out *p_out)
{
	sprintf(this->sqlstr, "select animalid, growth, level from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	        INT_CPY_NEXT_FIELD(p_out->animal_id);
	        INT_CPY_NEXT_FIELD(p_out->growth);
	        INT_CPY_NEXT_FIELD(p_out->level);
			p_out->if_mature = 0;
    STD_QUERY_ONE_END();
}
int  Cuser_farm_animal::get_table_mature(userid_t userid, uint32_t id , uint32_t &mature)
{
	sprintf(this->sqlstr, "select mature from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
	        INT_CPY_NEXT_FIELD(mature);
    STD_QUERY_ONE_END();

}
