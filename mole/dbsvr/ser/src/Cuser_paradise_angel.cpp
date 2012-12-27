/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise_angel.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/16/2011 01:54:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include<algorithm>
#include<ctime>
#include "proto.h"
#include "common.h"
#include <map>
#include "msglog.h"
#include "Cuser_paradise_angel.h"

#define BASE  1353201

static stru_angel_info angel_attribute[] ={
	{1353201, 100, 0, 1, 400,  300,  2, 3, 100, 0},
	{1353202, 100, 0, 1, 600,  500,  2, 5, 100, 0},
	{1353203, 100, 0, 1, 2000, 1600, 3, 20,100, 0},
	{1353204, 100, 0, 1, 1200, 400,  6, 35,100, 1},
	{1353205, 100, 0, 1, 1200, 800,  6, 15,100, 0},
	{1353206, 100, 0, 1, 600,  400,  5,  5,100, 0},
	{1353207, 100, 0, 1, 800,  600,  4,  6, 100, 0},
	{1353208, 100, 0, 1, 2000, 1600,  6, 20,100, 0},
	{1353209, 100, 0, 1, 2400, 1800,  8, 30,100, 1},

	{1353210, 100, 0, 1, 800,  500,  12, 15,100, 1},
	{1353211, 100, 0, 1, 600,  400,   8, 10,100, 1},
	{1353212, 100, 0, 1, 1200, 900,  14, 15,100, 1},
	{1353213, 100, 0, 1, 800,  500,   8, 10,100, 0},

	{1353214, 100, 0, 1, 1800,  1400, 10,20,100, 0},
	{1353215, 100, 0, 1, 600,   400,  8, 10,100, 1},
	{1353216, 100, 0, 1, 1800,  1400, 10,30,100, 1},
	{1353217, 100, 0, 1, 2400,  2000, 12,30,100, 1},

	{1353218, 100, 0, 1, 1200,  800,  30,40,100, 0},//

	{1353219, 100, 0, 1, 600,   400,  12,25,100, 0},
	{1353220, 100, 0, 1, 1200,  600,  10,30,100, 1},
	{1353221, 100, 0, 1, 800,   600,  20,20,100, 1},
	{1353222, 100, 0, 1, 600,   400,  20,50,100, 1},

	{1353223, 100, 0, 1, 600,   400,  30,40,100, 2},
	{1353224, 100, 0, 1, 1200,  800,  15,30,100, 1},
	{1353225, 100, 0, 1, 600,   400,  20,20,100, 0},
	{1353226, 100, 0, 1, 1200,  800,  15,20,100, 1},
	{1353227, 100, 0, 1, 600,   400,  20,40,100, 0},
	{1353228, 100, 0, 1, 600,   400,  20,20,100, 1},
	{1353229, 100, 0, 1, 600,   400,  15,15,100, 0},
	{1353230, 100, 0, 1, 800,   600,  30,20,100, 1},
	{1353231, 100, 0, 1, 600,   400,  15,15,100, 1},
	{1353232, 100, 0, 1, 1200,  900,  25,35,100, 1},
	{1353233, 100, 0, 1, 600,   400,  20,20,100, 0},
	{1353234, 100, 0, 1, 600,   400,  20,20,100, 1},
	{1353235, 100, 0, 1, 1200,  900,  25,25,100, 1},
	{1353236, 100, 0, 1, 600,   400,  20,40,100, 2},
	{1353237, 100, 0, 1, 600,   400,  20,20,100, 0},
	{1353238, 100, 0, 1, 600,   400,  30,20,100, 1},
	{1353239, 100, 0, 1, 600,   400,  40,30,100, 1},
	{1353240, 100, 0, 1, 1200,  900,  30,30,100, 1},
	{1353241, 100, 0, 1, 600,   400,  30,20,100, 1},
	{1353242, 100, 0, 1, 600,   400,  30,20,100, 0},
	
	//预留
	{1353243, 100, 0, 1, 600,   400,  30,20,100, 0},
	//小秒
	{1353244, 100, 0, 1, 600,   400,  30,40,100, 3},
	{1353245, 100, 0, 1, 1000,  600,  30,30,100, 1},

	{1353246, 100, 0, 1, 1200,  800,  30,35,100, 1},
	{1353247, 100, 0, 1, 600,   400,  30,20,100, 1},
	{1353248, 100, 0, 1, 1200,  800,  30,30,100, 0},
	{1353249, 100, 0, 1, 600,   400,  30,40,100, 1},
	
	{1353250, 100, 0, 1, 800,   400,  30,20,100, 1},
	{1353251, 100, 0, 1, 800,   400,  30,20,100, 1},
	{1353252, 100, 0, 1, 800,   400,  30,20,100, 1},
	{1353253, 100, 0, 1, 800,   400,  30,20,100, 1},
	{1353254, 100, 0, 1, 1200,  800,  30,40,100, 2},
	{1353255, 100, 0, 1, 1200,  800,  30,40,100, 2},
	{1353256, 100, 0, 1, 800,   400,  30,20,100, 1},
	{1353257, 100, 0, 1, 600,   400,  20,20,100, 0},
	{1353258, 100, 0, 1, 600,   400,  20,20,100, 0},
	{1353259, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353260, 100, 0, 1, 800,   400,  30,60,100, 3},
	{1353261, 100, 0, 1, 800,   400,  30,40,100, 2},
	{1353262, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353263, 100, 0, 1, 600,   400,  30,40,100, 2},
	{1353264, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353265, 100, 0, 1, 800,   600,  30,30,100, 1},
	{1353266, 100, 0, 1, 800,   600,  30,80,100, 3},

	{1353267, 100, 0, 1, 800,   400,  30,60,100, 2},

	{1353268, 100, 0, 1, 800,   400,  30,40,100, 2},
	{1353269, 100, 0, 1, 800,   400,  30,40,100, 2},
	{1353270, 100, 0, 1, 1000,  600,  30,60,100, 2},
	{1353271, 100, 0, 1, 1200,  800,  30,80,100, 2},
	{1353272, 100, 0, 1, 600,   400,  30,60,100, 2},
	{1353273, 100, 0, 1, 800,   600,  30,80,100, 3},
	{1353274, 100, 0, 1, 800,   600,  30,80,100, 3},
	{1353275, 100, 0, 1, 800,   600,  30,80,100, 3},
	{1353276, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353277, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353278, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353279, 100, 0, 1, 1000,  600,  30,50,100, 2},
	{1353280, 100, 0, 1, 1000,  600,  30,60,100, 3},
	
	{1353281, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353282, 100, 0, 1, 1000,  600,  30,40,100, 2},
	{1353283, 100, 0, 1, 800,   400,  30,30,100, 1},
	{1353284, 100, 0, 1, 1000,  600,  30,40,100, 2},
	{1353285, 100, 0, 1, 1200,  800,  30,50,100, 2},

	{1353286, 100, 0, 1, 1000,  800,  30,40,100, 2},
	{1353287, 100, 0, 1, 1200,  800,  30,50,100, 3},
	{1353288, 100, 0, 1, 1000,  800,  30,50,100, 2},
	{1353289, 100, 0, 1, 800,   600,  30,50,100, 1},
	{1353290, 100, 0, 1, 1200,  800,  30,50,100, 3},
	{1353291, 100, 0, 1, 1000,  800,  30,50,100, 2},
	{1353292, 100, 0, 1, 1200,  800,  30,60,100, 3},
	{1353293, 100, 0, 1, 1000,  800,  30,60,100, 2},
	{1353294, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353295, 100, 0, 1, 1000,  600,  30,60,100, 2},
	{1353296, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353297, 100, 0, 1, 1200,  800,  30,90,100, 3},

	{1353298, 100, 0, 1, 1000,  600,  30,50,100, 2},
	{1353299, 100, 0, 1, 1000,  600,  30,60,100, 3},
	{1353300, 100, 0, 1, 1000,  600,  30,50,100, 2},
	{1353301, 100, 0, 1, 800,   400,  30,60,100, 3},
	{1353302, 100, 0, 1, 1000,  600,  30,90,100, 3},
	{1353303, 100, 0, 1, 1000,  600,  30,80,100, 3},
	{1353304, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353305, 100, 0, 1, 1000,  800,  30,90,100, 3},
	{1353306, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353307, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353308, 100, 0, 1, 1200,  800,  30,120,100, 4},
	{1353309, 100, 0, 1, 1000,  800,  30,90,100, 3},
	{1353310, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353311, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353312, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353313, 100, 0, 1, 1000,  600,  30,60,100, 3},
	{1353314, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353315, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353316, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353317, 100, 0, 1, 1000,  600,  30,90,100, 3},
	{1353318, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353319, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353320, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353321, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353322, 100, 0, 1, 1200,  800,  30,90,100, 3},
	{1353323, 100, 0, 1, 1200,  800,  30,90,100, 3},
};
/* 
 * 天使种子的ID号，正常状态生长率, 休眠状态生长率, 饥饿时间间隔, 成熟值，变异时成长值，
 * 消耗食物数量，签订契约增加的经验值, 变异率, 星级
 */

static  user_paradise_angel_rand_info angle_variate_array[] =
{
	//爱莲宝宝
	{1353201, 1353402, 0, 2, {{0, 50, 1353401, 2, 1},{50, 100, 1353402, 0, 0}}},
	//尼尼莫莫瓜
	{1353202, 1353404, 0, 2, {{0, 40, 1353403, 3, 1},{40, 100, 1353404, 0, 0}}},
	//斯达球
	{1353203, 1353406, 0, 2, {{0, 20, 1353405, 3, 1},{20, 100, 1353406, 0, 0}}},
	//金球萤火
	{1353204, 1353417, 0, 2, {{0, 30, 1353416, 2, 1},{30, 100, 1353415, 1, 1}}},
	//蝌蚪君
	{1353205, 1353419, 0, 2, {{0, 20, 1353418, 1, 1},{20, 100, 1353419, 0, 0}}},
	//雪兔星
	{1353206, 1353421, 0, 2, {{0, 30, 1353414, 2, 1},{30, 100, 1353421, 0, 0}}},
	//彩羽蛋
	{1353207, 1353420, 0, 2, {{0, 40, 1353407, 1, 1},{40, 100, 1353420, 0, 0}}},
	//糕糕
	{1353208, 1353423, 0, 2, {{0, 20, 1353422, 1, 1},{20, 100, 1353423, 0, 0}}},
	//呐果子
	{1353209, 1353425, 0, 2, {{0, 20, 1353424, 1, 1},{20, 100, 1353425, 0, 0}}},
	//豆豆果
	{1353210, 1353426, 0, 2, {{0, 70, 1353417, 1, 1},{70, 100, 1353426, 0, 0}}},
	//摆摆种子
	{1353211, 1353428, 0, 2, {{0, 20, 1353427, 1, 1},{20, 100, 1353428, 0, 0}}},
	//小星星
	{1353212, 1353430, 0, 2, {{0, 40, 1353429, 1, 1},{40, 100, 1353430, 0, 0}}},
	//晶晶球
	{1353213, 1353432, 0, 2, {{0, 20, 1353431, 1, 1},{20, 100, 1353432, 0, 0}}},
	//刺宝宝
	{1353214, 1354034, 0, 2, {{0, 40, 1354033, 2, 1},{40, 100, 1354034, 0, 0}}},
	//仙人种子
	{1353215, 1354036, 0, 2, {{0, 20, 1354035, 1, 1},{20, 100, 1354036, 0, 0}}},
	//呱仔
	{1353216, 1354038, 0, 2, {{0, 30, 1354037, 1, 1},{30, 100, 1354038, 0, 0}}},
	//小薇
	{1353217, 1354040, 0, 2, {{0, 40, 1354039, 1, 1},{40, 100, 1354040, 0, 0}}},

	//可爱桶宝宝
	{1353218, 1354097, 1, 1, {{0, 100, 1354097, 1, 1}}},

	//阿桶
	{1353219, 1354042, 0, 2, {{0, 20, 1354041, 1, 1},{20, 100, 1354042, 0, 0}}},
	//水泡泡
	{1353220, 1354044, 0, 2, {{0, 40, 1354043, 1, 1},{40, 100, 1354044, 0, 0}}},
	//紫球球
	{1353221, 1354046, 0, 2, {{0, 20, 1354045, 1, 1},{20, 100, 1354046, 0, 0}}},
	//红珠
	{1353222, 1354048, 0, 2, {{0, 70, 1354047, 3, 1},{70, 100, 1354048, 0, 0}}},
	//樱果子
	{1353223, 1354050, 0, 2, {{0, 30, 1354049, 1, 1},{30, 100, 1354050, 0, 0}}},
	//小灰灰
	{1353224, 1354052, 0, 2, {{0, 20, 1354051, 4, 1},{20, 100, 1354052, 0, 0}}},
	//绵绵
	{1353225, 1354054, 0, 2, {{0, 30, 1354053, 4, 1},{30, 100, 1354054, 0, 0}}},
	//蛾尔
	{1353226, 1354056, 0, 2, {{0, 40, 1354055, 4, 1},{40, 100, 1354056, 0, 0}}},
	//海王苗苗
	{1353227, 1354058, 0, 2, {{0, 30, 1354057, 4, 1},{30, 100, 1354058, 0, 0}}},
	//岩灵
	{1353228, 1354060, 0, 2, {{0, 20, 1354059, 1, 1},{20, 100, 1354060, 0, 0}}},
	//妙妙
	{1353229, 1354062, 0, 2, {{0, 20, 1354061, 1, 1},{20, 100, 1354062, 0, 0}}},
	//奶酪果果
	{1353230, 1354064, 0, 2, {{0, 20, 1354063, 1, 1},{20, 100, 1354064, 0, 0}}},
	//小飞
	{1353231, 1354066, 0, 2, {{0, 30, 1354065, 1, 1},{30, 100, 1354066, 0, 0}}},
	//猛犸斯达
	{1353232, 1354068, 0, 2, {{0, 30, 1354067, 3, 1},{30, 100, 1354068, 0, 0}}},
	//赤金子
	{1353233, 1354070, 0, 2, {{0, 30, 1354069, 1, 1},{30, 100, 1354070, 0, 0}}},
	//小剑
	{1353234, 1354072, 0, 2, {{0, 30, 1354071, 5, 1},{30, 100, 1354072, 0, 0}}},
	//璀璨晶球
	{1353235, 1354074, 0, 2, {{0, 20, 1354073, 1, 1},{20, 100, 1354074, 0, 0}}},
	//蛇小苗
	{1353236, 1354076, 0, 2, {{0, 70, 1354075, 5, 1},{70, 100, 1354076, 0, 0}}},
	//皇之爱莲
	{1353237, 1354078, 0, 2, {{0, 30, 1354077, 3, 1},{30, 100, 1354078, 0, 0}}},
	//宝岩
	{1353238, 1354080, 0, 2, {{0, 20, 1354079, 1, 1},{20, 100, 1354080, 0, 0}}},
	//涂鸦蝠
	{1353239, 1354082, 0, 2, {{0, 10, 1354081, 1, 1},{10, 100, 1354082, 0, 0}}},
	//力士火苗
	{1353240, 1354084, 0, 2, {{0, 30, 1354083, 1, 1},{30, 100, 1354084, 0, 0}}},
	//小香炉
	{1353241, 1354086, 0, 2, {{0, 20, 1354085, 1, 1},{20, 100, 1354086, 0, 0}}},
	//宝石星
	{1353242, 1354088, 0, 2, {{0, 20, 1354087, 1, 1},{20, 100, 1354088, 0, 0}}},
	
	//预留
	{1353243, 1354088, 0, 2, {{0, 20, 1354087, 1, 1},{20, 100, 1354088, 0, 0}}},

	//小秒
	{1353244, 1354090, 3, 2, {{0, 30, 1354089, 5, 1},{30, 100, 1354090, 3, 1}}},
	//钻石豆
	{1353245, 1354092, 0, 2, {{0, 30, 1354091, 1, 1},{30, 100, 1354092, 0, 0}}},
	//钱宝宝
	{1353246, 1354094, 0, 2, {{0, 40, 1354093, 1, 1},{40, 100, 1354094, 0, 0}}},
	//白玉螺
	{1353247, 1354096, 0, 2, {{0, 20, 1354095, 1, 1},{20, 100, 1354096, 0, 0}}},
	//小企
	{1353248, 1354099, 0, 2, {{0, 20, 1354098, 1, 1},{20, 100, 1354099, 0, 0}}},
	//小葫芦
	{1353249, 1354101, 0, 2, {{0, 30, 1354100, 1, 1},{30, 100, 1354101, 0, 0}}},


	//玫红球球
	{1353250, 1354105, 0, 2, {{0, 30, 1354104, 1, 1},{30, 100, 1354105, 0, 0}}},
	//黄球球
	{1353251, 1354107, 0, 2, {{0, 30, 1354106, 1, 1},{30, 100, 1354107, 0, 0}}},
	//紫球球
	{1353252, 1354109, 0, 2, {{0, 30, 1354108, 1, 1},{30, 100, 1354109, 0, 0}}},
	//粉球球
	{1353253, 1354111, 0, 2, {{0, 30, 1354110, 1, 1},{30, 100, 1354111, 0, 0}}},
	//红球球
	{1353254, 1354113, 0, 2, {{0, 20, 1354112, 1, 1},{20, 100, 1354113, 0, 0}}},
	//橘球球
	{1353255, 1354115, 0, 2, {{0, 20, 1354114, 1, 1},{20, 100, 1354115, 0, 0}}},
	//蓝球球
	{1353256, 1354117, 0, 2, {{0, 30, 1354116, 1, 1},{30, 100, 1354117, 0, 0}}},
	//粽宝宝
	{1353257, 1354119, 0, 2, {{0, 30, 1354118, 1, 1},{30, 100, 1354119, 0, 0}}},
	//小黄帽
	{1353258, 1354121, 0, 2, {{0, 40, 1354120, 1, 1},{40, 100, 1354121, 0, 0}}},
	//冰冰球
	{1353259, 1354123, 0, 2, {{0, 20, 1354122, 1, 1},{20, 100, 1354123, 0, 0}}},
	//驼宝
	{1353260, 1354103, 0, 2, {{0, 70, 1354102, 3, 1},{70, 100, 1354103, 0, 0}}},
	//阿星球
	{1353261, 1354125, 0, 2, {{0, 20, 1354124, 1, 1},{20, 100, 1354125, 0, 0}}},
	//糯米团团
	{1353262, 1354127, 0, 2, {{0, 20, 1354126, 1, 1},{20, 100, 1354127, 0, 0}}},
	//仔仔不倒翁
	{1353263, 1354129, 0, 2, {{0, 30, 1354128, 1, 1},{30, 100, 1354129, 0, 0}}},
	//小小浆果
	{1353264, 1354131, 0, 2, {{0, 20, 1354130, 1, 1},{20, 100, 1354131, 0, 0}}},
	//小夫子
	{1353265, 1354133, 0, 2, {{0, 30, 1354132, 1, 1},{30, 100, 1354133, 0, 0}}},
	//伯爵之子
	{1353266, 1354135, 3, 2, {{0, 60, 1354134, 3, 1},{60, 100, 1354135, 3, 1}}},

	//熊小白
	{1353267, 1354161, 1, 1, {{0, 100, 1354161, 1, 1}}},
	
	//粉小芽
	{1353268, 1354137, 0, 2, {{0, 55, 1354136, 1, 1},{55, 100, 1354137, 0, 0}}},
	//橙小芽
	{1353269, 1354139, 0, 2, {{0, 55, 1354138, 1, 1},{55, 100, 1354139, 0, 0}}},
	//雾团团
	{1353270, 1354141, 0, 2, {{0, 70, 1354140, 1, 1},{70, 100, 1354141, 0, 0}}},
	//喵小盒
	{1353271, 1354143, 0, 2, {{0, 80, 1354142, 1, 1},{80, 100, 1354143, 0, 0}}},
	//皮皮芦
	{1353272, 1354145, 0, 2, {{0, 70, 1354144, 1, 1},{70, 100, 1354145, 0, 0}}},
	//金蟾子
	{1353273, 1354147, 0, 2, {{0, 80, 1354146, 3, 1},{80, 100, 1354147, 0, 0}}},
	//雷之龙灵
	{1353274, 1354149, 0, 2, {{0, 80, 1354148, 3, 1},{80, 100, 1354149, 0, 0}}},
	//火之龙灵
	{1353275, 1354151, 0, 2, {{0, 80, 1354150, 3, 1},{80, 100, 1354151, 0, 0}}},
	//梦泡泡
	{1353276, 1354153, 0, 2, {{0, 40, 1354152, 1, 1},{40, 100, 1354153, 0, 0}}},
	//棉宝宝
	{1353277, 1354155, 0, 2, {{0, 40, 1354154, 1, 1},{40, 100, 1354155, 0, 0}}},
	//翠儿蛋
	{1353278, 1354157, 0, 2, {{0, 30, 1354156, 1, 1},{30, 100, 1354157, 0, 0}}},
	//虎宝
	{1353279, 1354158, 2, 1, {{0, 100, 1354158, 2, 1}}},
	//虎小乖
	{1353280, 1354160, 0, 2, {{0, 45, 1354159, 3, 1},{45, 100, 1354160, 0, 0}}},
	//红星娃
	{1353281, 1354164, 0, 2, {{0, 40, 1354163, 5, 1},{40, 100, 1354164, 0, 0}}},
	//蜂蛹
	{1353282, 1354166, 0, 2, {{0, 30, 1354165, 5, 1},{30, 100, 1354166, 0, 0}}},
	//葫芦子
	{1353283, 1354168, 0, 2, {{0, 35, 1354167, 5, 1},{35, 100, 1354168, 0, 0}}},
	//吧吧雏
	{1353284, 1354170, 0, 2, {{0, 35, 1354169, 3, 1},{35, 100, 1354170, 0, 0}}},
	//太子宝
	{1353285, 1354172, 0, 2, {{0, 30, 1354171, 3, 1},{30, 100, 1354172, 0, 0}}},
	//刺团
	{1353286, 1354174, 0, 2, {{0, 40, 1354173, 3, 1},{40, 100, 1354174, 0, 0}}},
	//花蟹苞
	{1353287, 1354176, 0, 2, {{0, 30, 1354175, 5, 1},{30, 100, 1354176, 0, 0}}},
	//鼠小弟
	{1353288, 1354178, 0, 2, {{0, 30, 1354177, 3, 1},{30, 100, 1354178, 0, 0}}},
	//磁力蛋
	{1353289, 1354180, 0, 2, {{0, 45, 1354179, 5, 1},{45, 100, 1354180, 0, 0}}},
	//猫核儿
	{1353290, 1354182, 0, 2, {{0, 35, 1354181, 5, 1},{35, 100, 1354182, 0, 0}}},
	//雏翎
	{1353291, 1354184, 0, 2, {{0, 25, 1354183, 3, 1},{25, 100, 1354184, 0, 0}}},
	//辛巴
	{1353292, 1354186, 0, 2, {{0, 40, 1354185, 3, 1},{40, 100, 1354186, 0, 0}}},
	//小月
	{1353293, 1354188, 0, 2, {{0, 35, 1354187, 5, 1},{35, 100, 1354188, 0, 0}}},
	//斑比
	{1353294, 1354190, 0, 2, {{0, 45, 1354189, 3, 1},{45, 100, 1354190, 0, 0}}},
	//多利
	{1353295, 1354192, 0, 2, {{0, 40, 1354191, 5, 1},{40, 100, 1354192, 0, 0}}},
	//圈圈
	{1353296, 1354194, 0, 2, {{0, 45, 1354193, 5, 1},{45, 100, 1354194, 0, 0}}},
	//帕丘
	{1353297, 1354196, 0, 2, {{0, 45, 1354195, 5, 1},{45, 100, 1354196, 0, 0}}},

	//莉莉露 
	{1353298, 1354198, 0, 2, {{0, 40, 1354197, 5, 1},{40, 100, 1354198, 0, 0}}},
	//小角 
	{1353299, 1354200, 0, 2, {{0, 45, 1354199, 3, 1},{45, 100, 1354200, 0, 0}}},
	//璐璐 
	{1353300, 1354202, 0, 2, {{0, 65, 1354201, 5, 1},{65, 100, 1354202, 0, 0}}},
	//佩鲁 
	{1353301, 1354204, 0, 2, {{0, 25, 1354203, 5, 1},{25, 100, 1354204, 0, 0}}},
	//小羽 
	{1353302, 1354206, 0, 2, {{0, 65, 1354205, 3, 1},{65, 100, 1354206, 0, 0}}},
	//果果子
	{1353303, 1354208, 0, 2, {{0, 75, 1354207, 5, 1},{75, 100, 1354208, 0, 0}}},
	//笑笑蛛
	{1353304, 1354210, 0, 2, {{0, 75, 1354209, 3, 1},{75, 100, 1354210, 0, 0}}},
	//鲁鲁
	{1353305, 1354212, 0, 2, {{0, 45, 1354211, 1, 1},{45, 100, 1354212, 0, 0}}},
	//斯诺
	{1353306, 1354214, 0, 2, {{0, 50, 1354213, 1, 1},{50, 100, 1354214, 0, 0}}},
	//苏苏
	{1353307, 1354216, 0, 2, {{0, 55, 1354215, 1, 1},{55, 100, 1354216, 0, 0}}},
	//小狸
	{1353308, 1354217, 3, 1, {{0, 100, 1354217, 3, 1}}},
	//小瓶瓶
	{1353309, 1354219, 0, 2, {{0, 65, 1354218, 1, 1},{65, 100, 1354219, 0, 0}}},
	//叮叮
	{1353310, 1354221, 0, 2, {{0, 60, 1354220, 1, 1},{60, 100, 1354221, 0, 0}}},
	//天使龙蛋
	{1353311, 1354225, 0, 2, {{0, 55, 1354224, 3, 1},{55, 100, 1354225, 0, 0}}},
	//雪宝宝
	{1353312, 1354223, 0, 2, {{0, 60, 1354222, 1, 1},{60, 100, 1354223, 0, 0}}},
	//双生鱼泡
	{1353313, 1354227, 0, 2, {{0, 60, 1354226, 5, 1},{60, 100, 1354227, 0, 0}}},
	//魔书
	{1353314, 1354230, 0, 2, {{0, 45, 1354229, 1, 1},{45, 100, 1354230, 0, 0}}},
	//机械咩咩
	{1353315, 1354232, 0, 2, {{0, 60, 1354231, 5, 1},{60, 100, 1354232, 0, 0}}},
	//吼姆
	{1353316, 1354234, 0, 2, {{0, 60, 1354233, 1, 1},{60, 100, 1354234, 0, 0}}},
	//毛青虫
	{1353317, 1354236, 0, 2, {{0, 50, 1354235, 5, 1},{50, 100, 1354236, 0, 0}}},
	//华冠海豚
	{1353318, 1354237, 3, 1, {{0, 100, 1354237, 3, 1}}},
	//小哞哞
	{1353319, 1354239, 0, 2, {{0, 50, 1354238, 3, 1},{50, 100, 1354239, 0, 0}}},
	//机械龙宝宝
	{1353320, 1354241, 0, 2, {{0, 40, 1354240, 1, 1},{40, 100, 1354241, 0, 0}}},
	//淑女露丝
	{1353321, 1354243, 0, 2, {{0, 50, 1354242, 1, 1},{50, 100, 1354243, 0, 0}}},
	//双子天使
	{1353322, 1354245, 0, 2, {{0, 50, 1354244, 5, 1},{50, 100, 1354245, 0, 0}}},
	//巨蟹天使
	{1353323, 1354247, 0, 2, {{0, 50, 1354246, 5, 1},{50, 100, 1354247, 0, 0}}},


};
/*
 * 天使id号，默认成年天使，天使属性，  能够变异的个数， 起始数值，终止数值， 变异后的id号，
 * 变异后天使属性, 变异是否成功 ......
 */

Cuser_paradise_angel::Cuser_paradise_angel(mysql_interface *db):
	CtableRoute100x10(db, "USER","t_user_angel", "userid")
{

	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_paradise_angel::get_increase_exp(uint32_t angelid, uint32_t *exp_inc)
{
	*exp_inc = angel_attribute[angelid - BASE].exp_inc;
	return SUCC;
}
bool  Cuser_paradise_angel::if_mature(uint32_t angelid, uint32_t growth)
{
	if(growth >= angel_attribute[angelid - BASE].mature){
		return true;
	}
	else{
		return false;
	}	
}

int Cuser_paradise_angel::insert(uint32_t userid, uint32_t angel_id, uint32_t position,
		uint32_t layer)
{
	uint32_t index = angel_id - BASE;
	if(index < 0 || index >= sizeof(angel_attribute)/sizeof(stru_angel_info)){
		return ANGEL_SEED_ID_NOT_EXSIT_ERR;
	} 
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, %u, %u, 0, 1, 0, 0, %u, %u, %u, 0)",
			this->get_table_name(userid),
			userid,
			angel_id,
			position,
			(uint32_t)time(0),
			(uint32_t)time(0),
			angel_attribute[index].variate_rate,
			layer
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_paradise_angel::set_pos(userid_t userid, uint32_t id, uint32_t position)
{
	sprintf(this->sqlstr, "update %s set postion = %u where userid =%u and id = %u",
			this->get_table_name(userid),
			position,
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_angel::cal_list(uint32_t userid, user_get_paradise_out_item *p_out_item,
		uint32_t *nimbus, uint32_t *unsuccess_count, uint32_t count_1, uint32_t count_2)
{
	uint32_t now = time(0);
	uint32_t index = p_out_item->angelid - BASE;
	if(index < 0 || index > sizeof(angel_attribute)/sizeof(stru_angel_info)){
		return ANGEL_SEED_ID_NOT_EXSIT_ERR;
	}
	uint32_t time_for_nims = 0;
	get_last_eattime(userid, p_out_item->id, time_for_nims);
	//uint32_t tmp_nimbus = *nimbus;
	uint32_t pass_time_hour = (now - time_for_nims)/(3600 * angel_attribute[index].hungery_interval);
	//计算pass_time_hour消耗灵气
	uint32_t one_angel_consume = angel_attribute[index].food_num * pass_time_hour;	
	if( pass_time_hour != 0){//pass_time_hour消耗灵气 != 0
		if( *nimbus < one_angel_consume){//当前灵气 < pass_time_hour消耗灵气 
			//当前灵气 = 0
			*nimbus =0;
			 p_out_item->state= 0;
		}else{
			//当前灵气 -= pass_time_hour消耗灵气 
			 *nimbus -= one_angel_consume;
			 p_out_item->state = 1; //不休眠
		}
		time_for_nims += pass_time_hour * 3600 * angel_attribute[index].hungery_interval;
		update_eattime(userid, p_out_item->id, time_for_nims);
	}else{//不要更新灵气
			;
	}
	//更新灵气时间
	if(p_out_item->state == 0){
		if(*nimbus >=  angel_attribute[index].food_num){
			p_out_item->state = 1;
		}
	}
	else{
		uint32_t tmp = (now - p_out_item->last_caltime) * angel_attribute[index].growth_rate / 3600; 
		p_out_item->growth += tmp;
		p_out_item->last_caltime  = p_out_item->last_caltime + tmp * 3600/angel_attribute[index].growth_rate;
	}
	if(p_out_item->growth > angel_attribute[index].mature){
        p_out_item->growth = angel_attribute[index].mature;
    }
	p_out_item->diff_mature =  cal_diff_mature(index, angel_attribute[index].mature, p_out_item->growth);	

	uint32_t variate_flag = 0;
	if(p_out_item->growth >= angel_attribute[index].variate_value)	{
		uint32_t variate_id = 0;
		uint32_t index = p_out_item->angelid - BASE;
		get_angel_variate_id(userid, p_out_item->id, &variate_id);
		if(variate_id == 0){
			angel_variate(userid, index, p_out_item->rate, &variate_flag, &(p_out_item->variate_id), unsuccess_count, count_1, count_2, p_out_item->angelid);
			return set_angel_attribute_growth(userid, p_out_item->id, p_out_item->growth, 
				p_out_item->last_caltime, p_out_item->state, p_out_item->variate_id, variate_flag);
		}
	}
	return set_angel_attribute_growth_not_include_variate(userid, p_out_item->id, p_out_item->growth,
		p_out_item->last_caltime, p_out_item->state);

}

int Cuser_paradise_angel::get_variate_specify(uint32_t userid,  uint32_t &count, uint32_t angelid)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and variate_id = %u ",
			this->get_table_name(userid),
			userid,
			angelid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();	

}
int Cuser_paradise_angel::angel_variate(userid_t userid, uint32_t index, uint32_t rate_inc, uint32_t *variate_flag, 
		uint32_t *variate_id, uint32_t *unsuccess_count, uint32_t count_1, uint32_t count_2, uint32_t angelid)
{
	uint32_t random = rand() % 100;
	uint32_t rate = angel_attribute[index].variate_rate;

   DEBUG_LOG("========angelid: %u , count_1：%u, count_2: %u", angelid, count_1, count_2);
	if(angelid == 1353266){
	
	   DEBUG_LOG("========count_1：%u, count_2: %u", count_1, count_2);
		if(count_1 != 0 && count_2 == 0){
			  *variate_id = 1354135;
			  *variate_flag = 3;
			   *unsuccess_count = 0;
			   return SUCC;
		}
		else if(count_1 == 0 && count_2 != 0){
			*variate_id = 1354134;
			 *variate_flag = 3; 
			 *unsuccess_count = 0;
			 return SUCC;
		}	
		else if(count_1 == 0 && count_2 == 0){
			uint32_t count_3 = 0, count_4 = 0;
			 get_variate_specify(userid,  count_3, 1354134);			
			 get_variate_specify(userid,  count_4, 1354135);			
			 if(count_3 != 0 && count_4 == 0){
				  *variate_id = 1354135;
				  *variate_flag = 3; 
				   *unsuccess_count = 0;
				  return SUCC;
			 }
			 else if(count_3 == 0 && count_4 != 0){
				  *variate_id = 1354134;
                  *variate_flag = 3;
                   *unsuccess_count = 0;
                  return SUCC;
			 }
		}
	}	
	
	if(random < rate){//开始变异
		//下面是人为干预变异
		if(*unsuccess_count >= 4){
			for(uint32_t i =0; i < angle_variate_array[index].count; ++i ){
				uint32_t temp = angle_variate_array[index].variate_rand[i].variate_flag;
				if(temp != 0){
					 *variate_id = angle_variate_array[index].variate_rand[i].variate_id;
					 *variate_flag =temp;
				     *unsuccess_count = 0;
					 return SUCC;
				}//if
			}//for
		}

		//下面使用自动概率变异	
		uint32_t variate_rate = rand() % 100;
		uint32_t k = 0;
		for(; k < angle_variate_array[index].count; ++k ){
			uint32_t start = 0, end = 0;
			if(angle_variate_array[index].variate_rand[k].is_success == 1){
				start = 0;
				end = angle_variate_array[index].variate_rand[k].rand_end + rate_inc;
			}
			else{
				start = angle_variate_array[index].variate_rand[k].rand_start + rate_inc;	
				end = 100;
			}
			if( variate_rate >= start && variate_rate < end){
				*variate_id = angle_variate_array[index].variate_rand[k].variate_id;
				*variate_flag = angle_variate_array[index].variate_rand[k].variate_flag;
				DEBUG_LOG("-----------variate_id: %u, varitate_flag: %u, index:%u, k: %u", *variate_id,  *variate_flag,index,k);
				break;
			}
		}//for
		uint32_t attribute = angle_variate_array[index].variate_rand[k].variate_flag;
		if(attribute == 1 || attribute == 3){
			*unsuccess_count = 0;
		}
		else{
			*unsuccess_count += 1;
		}

	}//if
	else{
		*variate_id = angle_variate_array[index].default_angel_id;
		*variate_flag = angle_variate_array[index].type;
		*unsuccess_count += 1;
	}//else

	return SUCC;
}

int Cuser_paradise_angel::cal_growth(uint32_t index , uint32_t &growth, uint32_t last_caltime,
	   	uint32_t hungry_time )
{
	growth = growth * 3600;
	growth += (hungry_time - last_caltime) * angel_attribute[index].growth_rate;
	growth = growth / 3600;
	
	if(growth > angel_attribute[index].mature){
		growth = angel_attribute[index].mature;
	}
	return SUCC;
}

int Cuser_paradise_angel::set_angel_attribute_growth(uint32_t userid,uint32_t id, uint32_t growth,
		uint32_t last_caltime, uint32_t state, uint32_t variate_id, uint32_t variate_flag)
{
	sprintf(this->sqlstr, "update %s set growth = %u, last_caltime = %u, state = %u  \
			, variate_id = %u , variate_flag = %u where userid = %u and id = %u",
			this->get_table_name(userid),
			growth,
			last_caltime,
			state,
			variate_id,
			variate_flag,
			userid,
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_angel::update_only_state(uint32_t userid, uint32_t id, uint32_t state)
{
	sprintf(this->sqlstr, "update %s set state = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			state,
			userid,
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise_angel::update_eattime(uint32_t userid, uint32_t id, uint32_t eattime)
{
	sprintf(this->sqlstr, "update %s set eattime = %u  where userid = %u and id = %u",
			this->get_table_name(userid),
			eattime,
			userid,
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise_angel::set_angel_attribute_growth_not_include_variate(uint32_t userid, uint32_t id,
		uint32_t growth, uint32_t last_caltime, uint32_t state)
{
	 sprintf(this->sqlstr, "update %s set growth = %u, last_caltime = %u, state = %u  \
            where userid = %u and id = %u",
            this->get_table_name(userid),
            growth,
            last_caltime,
            state,
            userid,
            id
            );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_angel::get_all(uint32_t userid, user_get_paradise_out_item **pp_list, 
		uint32_t *count, uint32_t layer)
{
	sprintf(this->sqlstr, "select id, angelid, position, growth, state, last_caltime, variate_id, variate_rate from \
			%s where userid = %u and layer = %u ",
			this->get_table_name(userid),
			userid,
			layer
		   );
	uint32_t index = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->angelid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->position);
		INT_CPY_NEXT_FIELD((*pp_list + i)->growth);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->last_caltime);
		INT_CPY_NEXT_FIELD((*pp_list + i)->variate_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->rate);
		index = (*pp_list + i)->angelid - BASE;
		(*pp_list + i)->diff_mature = cal_diff_mature(index, angel_attribute[index].mature, (*pp_list + i)->growth);
	STD_QUERY_WHILE_END();	

}

uint32_t Cuser_paradise_angel::cal_diff_mature(uint32_t index, uint32_t mature, uint32_t growth)
{
	uint32_t real_value = growth > mature? mature:growth;
	return (mature- real_value) * 60 / angel_attribute[index].growth_rate;

}
bool Cuser_paradise_angel::is_mature(uint32_t angelid,  uint32_t growth)
{
	uint32_t index = angelid - BASE;
	return growth >= angel_attribute[index].mature ? true : false;

}
int Cuser_paradise_angel::get_angel_by_position(uint32_t userid, uint32_t position, user_change_angel_pos_out *out)
{
	sprintf(this->sqlstr, "select id, angelid, position, growth, state, last_caltime, variate_id, variate_rate from \
			%s where userid = %u and position = %u ",
			this->get_table_name(userid),
			userid,
			position
		   );
	uint32_t id = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->id);
		INT_CPY_NEXT_FIELD(out->angelid);
		INT_CPY_NEXT_FIELD(out->position);
		INT_CPY_NEXT_FIELD(out->growth);
		INT_CPY_NEXT_FIELD(out->state);
		INT_CPY_NEXT_FIELD(out->last_caltime);
		INT_CPY_NEXT_FIELD(out->variate_id);
		INT_CPY_NEXT_FIELD(out->rate);
		id = out->angelid;
		out->diff_mature = (angel_attribute[id - BASE].mature - out->growth) * 60 / 
								angel_attribute[id - BASE].growth_rate;
	STD_QUERY_ONE_END();	
	
}
int Cuser_paradise_angel::get_last_eattime(uint32_t userid, uint32_t id, uint32_t &last_eattime)
{
	sprintf(this->sqlstr, "select eattime from %s  where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(last_eattime);
	STD_QUERY_ONE_END();	
}
int Cuser_paradise_angel::get_two_column_by_id(uint32_t userid, uint32_t id, char *column1, 
		char* column2, uint32_t *out_1, uint32_t *out_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and id = %u ",
			column1,
			column2,
			this->get_table_name(userid),
			userid,
			id
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*out_2);
		INT_CPY_NEXT_FIELD(*out_1);
	STD_QUERY_ONE_END();	
	
}

int Cuser_paradise_angel::get_angelid_growth_variate(uint32_t userid, uint32_t id, uint32_t *angelid, 
		uint32_t *growth, uint32_t *variate_id, uint32_t *variate_flag)
{
	sprintf(this->sqlstr, "select angelid, growth, variate_id ,variate_flag from \
			%s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*angelid);
		INT_CPY_NEXT_FIELD(*growth);
		INT_CPY_NEXT_FIELD(*variate_id);
		INT_CPY_NEXT_FIELD(*variate_flag);
	STD_QUERY_ONE_END();	

}

int Cuser_paradise_angel::get_angel_variate_id(uint32_t userid, uint32_t id, uint32_t *variate_id)
{
	sprintf(this->sqlstr, "select variate_id from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*variate_id);
	STD_QUERY_ONE_END();	

}

int Cuser_paradise_angel::get_all_angel_pos(userid_t userid, uint32_t *pos, uint32_t *count, uint32_t layer)
{
	sprintf(this->sqlstr, "select position from %s where userid = %u and layer = %u ",
			this->get_table_name(userid),
			userid,
			layer
			);
	uint32_t *pplist = 0;

	STD_QUERY_WHILE_BEGIN(this->sqlstr, &pplist, count);
		INT_CPY_NEXT_FIELD(*(pos + i));
	STD_QUERY_WHILE_END_WITHOUT_RETURN();	
	if(pplist != 0){
		free(pplist);
	}
	return SUCC;

}

int Cuser_paradise_angel::free_angel(uint32_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_paradise_angel::get_angelid_by_index(uint32_t userid, uint32_t id, uint32_t &angelid)
{
	sprintf(this->sqlstr, "select angelid from %s where userid = %u and id = %u",
			this->get_table_name(userid),
			userid,
			id
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(angelid);
	STD_QUERY_ONE_END();	

}

int Cuser_paradise_angel::get_angel_info_by_position(uint32_t userid, uint32_t position, 
		  paradise_angel_item *out)
{
	sprintf(this->sqlstr, "select id, angelid, growth, state, variate_id, last_caltime, variate_rate \
			from %s where userid = %u and position = %u",
			this->get_table_name(userid),
			userid,
			position
		   );
	uint32_t id = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->id);
		INT_CPY_NEXT_FIELD(out->angelid);
		INT_CPY_NEXT_FIELD(out->growth);
		INT_CPY_NEXT_FIELD(out->state);
		INT_CPY_NEXT_FIELD(out->variate_id);
		INT_CPY_NEXT_FIELD(out->last_caltime);
		INT_CPY_NEXT_FIELD(out->rate);
		id = out->angelid;
		out->diff_mature = (angel_attribute[id - BASE].mature - out->growth) * 60 / 
								angel_attribute[id - BASE].growth_rate;
	STD_QUERY_ONE_END();	

}
int Cuser_paradise_angel::speed_up(uint32_t userid, uint32_t up_growth, paradise_angel_item *out,
		uint32_t *count, uint32_t count_1, uint32_t count_2)
{
	uint32_t index = out->angelid- BASE;
	uint32_t res = (up_growth * angel_attribute[index].growth_rate) / 3600; 		
	uint32_t growth = out->growth + res; 
	if( growth > angel_attribute[index].mature){
		out->growth = angel_attribute[index].mature;
	}
	else{
		out->growth= growth;
	}
	out->diff_mature = cal_diff_mature(index, angel_attribute[index].mature, out->growth);
	uint32_t variate_flag = 0;
	if(out->growth >=  angel_attribute[index].variate_value){
		if(out->variate_id == 0){
			angel_variate(userid, index, out->rate, &variate_flag,  &(out->variate_id), count, count_1, count_2, out->angelid);
			update_growth_variate(userid, out->id, variate_flag, out->variate_id);
		}
	}
	int ret = update_growth(userid, out->id,  out->growth);

	return ret;
}

int Cuser_paradise_angel:: update_growth(uint32_t userid, uint32_t id,  uint32_t up_growth)
{
	sprintf(this->sqlstr, "update %s set growth = %u where \
			userid =%u and id = %u",
		this->get_table_name(userid),
		up_growth,
		userid,
		id
		);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Cuser_paradise_angel:: update_growth_variate(uint32_t userid, uint32_t id, 
		uint32_t variate_flag, uint32_t variate_id)
{
	sprintf(this->sqlstr, "update %s set variate_flag = %u, variate_id = %u where \
		userid =%u and id = %u",
	this->get_table_name(userid),
	variate_flag,
	variate_id,
	userid,
	id
	);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_paradise_angel::speed_variate_rate(userid_t userid, uint32_t id, uint32_t angelid,
		uint32_t toolid, uint32_t *rate_inc)
{
	if(angelid == 1353204 || angelid == 1353244 || angelid == 1353266){
		return THE_ANGEL_ALREADY_VARIATE_SUCCESS_ERR;
	}

	uint32_t index = angelid- BASE;
	if( toolid == 1353105 && angel_attribute[index].star > 1){
		return THE_TOOL_CANNOT_USED_FOR_THIS_ANGEL_ERR;
	}	
	
	if( (toolid == 1353109 || toolid == 1353110) && angel_attribute[index].star > 3){
		return THE_TOOL_CANNOT_USED_FOR_THIS_ANGEL_ERR;
	}	
	//uint32_t size = angle_variate_array[index].count;
	std::map<uint32_t, uint32_t> t_map;
	t_map[1353103] = 5;
	t_map[1353104] = 15;
	t_map[1353105] = 100;
	t_map[1353109] = 100;
	t_map[1353110] = 100;

	if(angle_variate_array[index].variate_rand[0].rand_end + *rate_inc >= 100){
			return THE_ANGEL_ALREADY_VARIATE_SUCCESS_ERR;
	}
	uint32_t statistics[] = {userid, 1};
	if( toolid == 1353103){

		msglog(this->msglog_file, 0x406FAAC, time(NULL), statistics, sizeof(statistics));
	}
	else if( toolid == 1353104){

		msglog(this->msglog_file, 0x406FAAD, time(NULL), statistics, sizeof(statistics));
	}
	else if( toolid == 1353105){

		msglog(this->msglog_file, 0x406FAAE, time(NULL), statistics, sizeof(statistics));
	}

	uint32_t tmp_sum = angle_variate_array[index].variate_rand[0].rand_end + *rate_inc;
	if(tmp_sum + t_map[toolid] > 100){
		*rate_inc = 100 - angle_variate_array[index].variate_rand[0].rand_end;
	}
	else{
		*rate_inc += t_map[toolid];
	}
	sprintf(this->sqlstr, "update %s set variate_rate = %u where \
		userid =%u and id = %u",
	this->get_table_name(userid),
	*rate_inc,
	userid,
	id
	);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}
int Cuser_paradise_angel::get_pos_limit(uint32_t userid, uint32_t **pp_list, uint32_t *count)
{
	sprintf(this->sqlstr,"select id from %s where userid = %u order by desc",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD(*(*pp_list + i));
	STD_QUERY_WHILE_END();	

}
