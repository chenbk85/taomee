#include <string.h>
#include <limits.h>
extern "C" {
#include <libtaomee/project/stat_agent/msglog.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
}

#include "player_attr.hpp"

static char static_log_path[PATH_MAX];
//float hit_rates[] = { 0.0, 0.85, 0.8, 0.9 };
float hit_rates[] = { 0.0, 1, 1, 0.95, 0.9};
int crit_adjs[] = { 0, 3, 8, 1, 2};
static uint32_t honor_exp_arr[] = {
	25, 96, 234, 460, 795, 1260, 1875, 2661, 3639, 4830, 
	6255, 7935, 9890, 12141, 14709, 17615, 20880, 24525, 28570, 33036, 
	37944, 43315, 49170, 55530, 62415, 69846, 77844, 86430, 95625, 105450, 
	115925, 127071, 138909, 151460, 164745, 178785, 193600, 209211, 225639, 242905, 
	261030
};

static uint16_t user_lv_by_honor_lv_arr[] = {
	10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
	20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 
	40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 
	60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 
	80
};


static const int max_role_type_gf_calc=4;
static const int max_attr_type_gf_calc=9;

/*static double base_attr[3][9]={
{ 15.000000, 12.000000, 13.000000, 12.000000, 25.000000, 180.000000, 15360.000000, 10.000000, 0.520000},
{ 13.000000, 17.000000, 10.000000, 14.000000, 32.000000, 160.000000, 15360.000000, 9.000000, 0.400000},
{ 17.000000, 5.000000, 18.000000, 10.000000, 20.000000, 200.000000, 15360.000000, 11.000000, 0.720000}
};*/
int calc_hp_monkey(int body_quality)
{
	//return 180 + (body_quality - 13) * 25;
	return body_quality * 25;
}
int calc_hp_rabbit(int body_quality)
{
	//return 160 + (body_quality - 10) * 25;
	return body_quality * 25;
}
int calc_hp_panda(int body_quality)
{
	//return 200 + (body_quality - 18) * 25;
	return body_quality * 25;
}
int calc_hp_dragon(int body_quality)
{
	return body_quality * 25;
}
int calc_mp_monkey(int stimina)
{
	return 50 + (stimina - 11) * 9;
}
int calc_mp_rabbit(int stimina)
{
	return 64 + (stimina - 14) * 9;
}
int calc_mp_panda(int stimina)
{
	return 40 + (stimina - 10) * 9;
}
int calc_mp_dragon(int stimina)
{
	return 40 + (stimina - 10) * 9;
}

// copy follow content to head of this file
//get strength of monkey
int calc_strength_monkey(int lv);

//get strength of rabbit
int calc_strength_rabbit(int lv);

//get strength of panda
int calc_strength_panda(int lv);

//get strength of dragon
int calc_strength_dragon(int lv);

//get agility of monkey
int calc_agility_monkey(int lv);

//get agility of rabbit
int calc_agility_rabbit(int lv);

//get agility of panda
int calc_agility_panda(int lv);

//get agility of dragon
int calc_agility_dragon(int lv);

//get body_quality of monkey
int calc_body_quality_monkey(int lv);

//get body_quality of rabbit
int calc_body_quality_rabbit(int lv);

//get body_quality of panda
int calc_body_quality_panda(int lv);

//get body_quality of  dragon
int calc_body_quality_dragon(int lv);


//get stamina of monkey
int calc_stamina_monkey(int lv);

//get stamina of rabbit
int calc_stamina_rabbit(int lv);

//get stamina of panda
int calc_stamina_panda(int lv);

//get stamina of dragon
int calc_stamina_dragon(int lv);


//get mp of monkey
int calc_mp_monkey(int lv);

//get mp of rabbit
int calc_mp_rabbit(int lv);

//get mp of panda
int calc_mp_panda(int lv);

//get mp of dragon
int calc_mp_dragon(int lv);

//get hp of monkey
int calc_hp_monkey(int lv);

//get hp of rabbit
int calc_hp_rabbit(int lv);

//get hp of panda
int calc_hp_panda(int lv);

//get hp of dragon
int calc_hp_dragon(int lv);

//get exp of monkey
int calc_exp_monkey(int lv);

//get exp of rabbit
int calc_exp_rabbit(int lv);

//get exp of panda
int calc_exp_panda(int lv);

//get exp of dragon
int calc_exp_dragon(int lv);

int err_return(int lv);
gf_calc_my_fun_def calc_strength[max_role_type_gf_calc+1]={ 
	err_return,
	calc_strength_monkey,
	calc_strength_rabbit,
	calc_strength_panda,
	calc_strength_dragon

};
gf_calc_my_fun_def calc_agility[max_role_type_gf_calc+1]={ 
	err_return,
	calc_agility_monkey,
	calc_agility_rabbit,
	calc_agility_panda,
	calc_agility_dragon
};
gf_calc_my_fun_def calc_body_quality[max_role_type_gf_calc+1]={ 
	err_return,
	calc_body_quality_monkey,
	calc_body_quality_rabbit,
	calc_body_quality_panda,
	calc_body_quality_dragon

};
gf_calc_my_fun_def calc_stamina[max_role_type_gf_calc+1]={ 
	err_return,
	calc_stamina_monkey,
	calc_stamina_rabbit,
	calc_stamina_panda,
	calc_stamina_dragon

};
gf_calc_my_fun_def calc_mp[max_role_type_gf_calc+1]={ 
	err_return,
	calc_mp_monkey,
	calc_mp_rabbit,
	calc_mp_panda,
	calc_mp_dragon
};
gf_calc_my_fun_def calc_hp[max_role_type_gf_calc+1]={ 
	err_return,
	calc_hp_monkey,
	calc_hp_rabbit,
	calc_hp_panda,
	calc_hp_dragon
};

/*
// copy follow content to player_attr.hpp
#ifndef GF_PLAYER_ATTR_HPP
#define GF_PLAYER_ATTR_HPP


typedef int (*gf_calc_my_fun_def)(int lv);
typedef float (*gf_calc_my_floatfun_def)(int lv);

// declare API array:
extern gf_calc_my_fun_def calc_strength[4];
extern gf_calc_my_fun_def calc_agility[4];
extern gf_calc_my_fun_def calc_body_quality[4];
extern gf_calc_my_fun_def calc_stamina[4];
extern gf_calc_my_fun_def calc_mp[4];
extern gf_calc_my_fun_def calc_hp[4];
extern gf_calc_my_fun_def calc_exp[4];

extern float hit_rates[];
extern int crit_adjs[];
inline int calc_atk(int strength){
	return (int)(strength * 0.7);
}
inline float calc_def_rate(int body_quality){
	return (body_quality / 2500.0);
}
inline float calc_dodge_rate(int agility){
	return (agility / 1250.0);
}
#endif
*/

int err_return(int lv)
{
	return 0;
}

/** 
* @fn get strength of monkey
* @brief  API
*/
int calc_strength_monkey(int lv)
{
	static int strength_monkey_arr[] = {
			 14, 17, 20, 23, 26, 29, 32, 35, 38, 41,
			 44, 47, 50, 53, 56, 59, 62, 65, 68, 71,
			 74, 77, 80, 83, 86, 89, 92, 95, 98, 101,
			 104, 107, 110, 113, 116, 119, 122, 125, 128, 131,
			 134, 137, 140, 143, 146, 149, 152, 155, 158, 161,
			 164, 167, 170, 173, 176, 179, 182, 185, 188, 191,
			 194, 197, 200, 203, 206, 209, 212, 215, 218, 221,
			 224, 227, 230, 233, 236, 239, 242, 245, 248, 251,
			 254, 257, 260, 263, 266, 269, 272, 275, 278, 281,
			 284, 287, 290, 293, 296, 299, 302, 305, 308, 311
	};
	if (lv >= 1 && lv <= 100) {
		return strength_monkey_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get strength of rabbit
* @brief  API
*/
int calc_strength_rabbit(int lv)
{
	static int strength_rabbit_arr[] = {
			 21, 24, 27, 30, 33, 36, 39, 42, 45, 48,
			 51, 54, 57, 60, 63, 66, 69, 72, 75, 78,
			 81, 84, 87, 90, 93, 96, 99, 102, 105, 108,
			 111, 114, 117, 120, 123, 126, 129, 132, 135, 138,
			 141, 144, 147, 150, 153, 156, 159, 162, 165, 168,
			 171, 174, 177, 180, 183, 186, 189, 192, 195, 198,
			 201, 204, 207, 210, 213, 216, 219, 222, 225, 228,
			 231, 234, 237, 240, 243, 246, 249, 252, 255, 258,
			 261, 264, 267, 270, 273, 276, 279, 282, 285, 288,
			 291, 294, 297, 300, 303, 306, 309, 312, 315, 318
	};
	if (lv >= 1 && lv <= 100) {
		return strength_rabbit_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get strength of panda
* @brief  API
*/
int calc_strength_panda(int lv)
{
	static int strength_panda_arr[] = {
			 17, 19, 21, 23, 25, 27, 29, 31, 33, 35,
			 37, 39, 41, 43, 45, 47, 49, 51, 53, 55,
			 57, 59, 61, 63, 65, 67, 69, 71, 73, 75,
			 77, 79, 81, 83, 85, 87, 89, 91, 93, 95,
			 97, 99, 101, 103, 105, 107, 109, 111, 113, 115,
			 117, 119, 121, 123, 125, 127, 129, 131, 133, 135,
			 137, 139, 141, 143, 145, 147, 149, 151, 153, 155,
			 157, 159, 161, 163, 165, 167, 169, 171, 173, 175,
			 177, 179, 181, 183, 185, 187, 189, 191, 193, 195,
			 197, 199, 201, 203, 205, 207, 209, 211, 213, 215
	};
	if (lv >= 1 && lv <= 100) {
		return strength_panda_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get strength of dragon 
* @brief  API
*/
int calc_strength_dragon(int lv)
{
	static int strength_dragon_arr[] = {
		10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
		30,	32, 34, 36, 38, 40, 42, 44, 46, 48, 
		50,	52, 54, 56, 58, 60, 62, 64, 66, 68, 
		70,	72, 74, 76, 78, 80, 82, 84, 86, 88, 
		90,	92, 94, 96, 98, 100, 102, 104, 106, 108, 
		110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 
		130, 132, 134, 136, 138, 140, 142, 144, 146, 148,
		150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 
		170, 172, 174, 176, 178, 180, 182, 184, 186, 188,
		190, 192, 194, 196, 198, 200, 202, 204, 206, 208
	};
	if (lv >= 1 && lv <= 100) {
		return strength_dragon_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get agility of monkey
* @brief  API
*/
int calc_agility_monkey(int lv)
{
	static int agility_monkey_arr[] = {
			 14, 17, 20, 23, 26, 29, 32, 35, 38, 41,
			 44, 47, 50, 53, 56, 59, 62, 65, 68, 71,
			 74, 77, 80, 83, 86, 89, 92, 95, 98, 101,
			 104, 107, 110, 113, 116, 119, 122, 125, 128, 131,
			 134, 137, 140, 143, 146, 149, 152, 155, 158, 161,
			 164, 167, 170, 173, 176, 179, 182, 185, 188, 191,
			 194, 197, 200, 203, 206, 209, 212, 215, 218, 221,
			 224, 227, 230, 233, 236, 239, 242, 245, 248, 251,
			 254, 257, 260, 263, 266, 269, 272, 275, 278, 281,
			 284, 287, 290, 293, 296, 299, 302, 305, 308, 311
	};
	if (lv >= 1 && lv <= 100) {
		return agility_monkey_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get agility of rabbit
* @brief  API
*/
int calc_agility_rabbit(int lv)
{
	static int agility_rabbit_arr[] = {
			 14, 18, 22, 26, 30, 34, 38, 42, 46, 50,
			 54, 58, 62, 66, 70, 74, 78, 82, 86, 90,
			 94, 98, 102, 106, 110, 114, 118, 122, 126, 130,
			 134, 138, 142, 146, 150, 154, 158, 162, 166, 170,
			 174, 178, 182, 186, 190, 194, 198, 202, 206, 210,
			 214, 218, 222, 226, 230, 234, 238, 242, 246, 250,
			 254, 258, 262, 266, 270, 274, 278, 282, 286, 290,
			 294, 298, 302, 306, 310, 314, 318, 322, 326, 330,
			 334, 338, 342, 346, 350, 354, 358, 362, 366, 370,
			 374, 378, 382, 386, 390, 394, 398, 402, 406, 410
	};
	if (lv >= 1 && lv <= 100) {
		return agility_rabbit_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get agility of panda
* @brief  API
*/
int calc_agility_panda(int lv)
{
	static int agility_panda_arr[] = {
			 11, 14, 17, 20, 23, 26, 29, 32, 35, 38,
			 41, 44, 47, 50, 53, 56, 59, 62, 65, 68,
			 71, 74, 77, 80, 83, 86, 89, 92, 95, 98,
			 101, 104, 107, 110, 113, 116, 119, 122, 125, 128,
			 131, 134, 137, 140, 143, 146, 149, 152, 155, 158,
			 161, 164, 167, 170, 173, 176, 179, 182, 185, 188,
			 191, 194, 197, 200, 203, 206, 209, 212, 215, 218,
			 221, 224, 227, 230, 233, 236, 239, 242, 245, 248,
			 251, 254, 257, 260, 263, 266, 269, 272, 275, 278,
			 281, 284, 287, 290, 293, 296, 299, 302, 305, 308
	};
	if (lv >= 1 && lv <= 100) {
		return agility_panda_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get agility of dragon 
* @brief  API
*/
int calc_agility_dragon(int lv)
{
	static int agility_dragon_arr[] = {
			 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 
			 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 
			 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 
			 108,111, 114, 117, 120, 123, 126, 129, 132, 135, 
			 138,141, 144, 147, 150, 153, 156, 159, 162, 165, 
			 168,171, 174, 177, 180, 183, 186, 189, 192, 195, 
			 198,201, 204, 207, 210, 213, 216, 219, 222, 225, 
			 228,231, 234, 237, 240, 243, 246, 249, 252, 255,
			 258,261, 264, 267, 270, 273, 276, 279, 282, 285, 
			 288,291, 294, 297, 300, 303, 306, 309, 312, 315,

	};
	if (lv >= 1 && lv <= 100) {
		return agility_dragon_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get body_quality of monkey
* @brief  API
*/
int calc_body_quality_monkey(int lv)
{
	static int body_quality_monkey_arr[] = {
			 14, 17, 20, 23, 26, 29, 32, 35, 38, 41,
			 44, 47, 50, 53, 56, 59, 62, 65, 68, 71,
			 74, 77, 80, 83, 86, 89, 92, 95, 98, 101,
			 104, 107, 110, 113, 116, 119, 122, 125, 128, 131,
			 134, 137, 140, 143, 146, 149, 152, 155, 158, 161,
			 164, 167, 170, 173, 176, 179, 182, 185, 188, 191,
			 194, 197, 200, 203, 206, 209, 212, 215, 218, 221,
			 224, 227, 230, 233, 236, 239, 242, 245, 248, 251,
			 254, 257, 260, 263, 266, 269, 272, 275, 278, 281,
			 284, 287, 290, 293, 296, 299, 302, 305, 308, 311
	};
	if (lv >= 1 && lv <= 100) {
		return body_quality_monkey_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get body_quality of rabbit
* @brief  API
*/
int calc_body_quality_rabbit(int lv)
{
	static int body_quality_rabbit_arr[] = {
			 7, 10, 13, 16, 19, 22, 25, 28, 31, 34,
			 37, 40, 43, 46, 49, 52, 55, 58, 61, 64,
			 67, 70, 73, 76, 79, 82, 85, 88, 91, 94,
			 97, 100, 103, 106, 109, 112, 115, 118, 121, 124,
			 127, 130, 133, 136, 139, 142, 145, 148, 151, 154,
			 157, 160, 163, 166, 169, 172, 175, 178, 181, 184,
			 187, 190, 193, 196, 199, 202, 205, 208, 211, 214,
			 217, 220, 223, 226, 229, 232, 235, 238, 241, 244,
			 247, 250, 253, 256, 259, 262, 265, 268, 271, 274,
			 277, 280, 283, 286, 289, 292, 295, 298, 301, 304
	};
	if (lv >= 1 && lv <= 100) {
		return body_quality_rabbit_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get body_quality of panda
* @brief  API
*/
int calc_body_quality_panda(int lv)
{
	static int body_quality_panda_arr[] = {
			 21, 26, 31, 36, 41, 46, 51, 56, 61, 66,
			 71, 76, 81, 86, 91, 96, 101, 106, 111, 116,
			 121, 126, 131, 136, 141, 146, 151, 156, 161, 166,
			 171, 176, 181, 186, 191, 196, 201, 206, 211, 216,
			 221, 226, 231, 236, 241, 246, 251, 256, 261, 266,
			 271, 276, 281, 286, 291, 296, 301, 306, 311, 316,
			 321, 326, 331, 336, 341, 346, 351, 356, 361, 366,
			 371, 376, 381, 386, 391, 396, 401, 406, 411, 416,
			 421, 426, 431, 436, 441, 446, 451, 456, 461, 466,
			 471, 476, 481, 486, 491, 496, 501, 506, 511, 516
	};
	if (lv >= 1 && lv <= 100) {
		return body_quality_panda_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get body_quality of dragon 
* @brief  API
*/
int calc_body_quality_dragon(int lv)
{
	static int body_quality_dragon_arr[] = {
		 10, 13, 16, 19, 22, 25, 28, 31, 34, 37,
		 40, 43, 46, 49, 52, 55, 58, 61, 64, 67,
		 70, 73, 76, 79, 82, 85, 88, 91, 94, 97,
		 100, 103, 106, 109, 112, 115, 118, 121, 124, 127,
		 130, 133, 136, 139, 142, 145, 148, 151, 154, 157,
		 160, 163, 166, 169, 172, 175, 178, 181, 184, 187,
		 190, 193, 196, 199, 202, 205, 208, 211, 214, 217,
		 220, 223, 226, 229, 232, 235, 238, 241, 244, 247,
		 250, 253, 256, 259, 262, 265, 268, 271, 274, 277,
		 280, 283, 286, 289, 292, 295, 298, 301, 304, 307,
	
	};
	if (lv >= 1 && lv <= 100) {
		return body_quality_dragon_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get stamina of monkey
* @brief  API
*/
int calc_stamina_monkey(int lv)
{
	static int stamina_monkey_arr[] = {
			 14, 17, 20, 23, 26, 29, 32, 35, 38, 41,
			 44, 47, 50, 53, 56, 59, 62, 65, 68, 71,
			 74, 77, 80, 83, 86, 89, 92, 95, 98, 101,
			 104, 107, 110, 113, 116, 119, 122, 125, 128, 131,
			 134, 137, 140, 143, 146, 149, 152, 155, 158, 161,
			 164, 167, 170, 173, 176, 179, 182, 185, 188, 191,
			 194, 197, 200, 203, 206, 209, 212, 215, 218, 221,
			 224, 227, 230, 233, 236, 239, 242, 245, 248, 251,
			 254, 257, 260, 263, 266, 269, 272, 275, 278, 281,
			 284, 287, 290, 293, 296, 299, 302, 305, 308, 311
	};
	if (lv >= 1 && lv <= 100) {
		return stamina_monkey_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get stamina of rabbit
* @brief  API
*/
int calc_stamina_rabbit(int lv)
{
	static int stamina_rabbit_arr[] = {
			 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
			 34, 36, 38, 40, 42, 44, 46, 48, 50, 52,
			 54, 56, 58, 60, 62, 64, 66, 68, 70, 72,
			 74, 76, 78, 80, 82, 84, 86, 88, 90, 92,
			 94, 96, 98, 100, 102, 104, 106, 108, 110, 112,
			 114, 116, 118, 120, 122, 124, 126, 128, 130, 132,
			 134, 136, 138, 140, 142, 144, 146, 148, 150, 152,
			 154, 156, 158, 160, 162, 164, 166, 168, 170, 172,
			 174, 176, 178, 180, 182, 184, 186, 188, 190, 192,
			 194, 196, 198, 200, 202, 204, 206, 208, 210, 212
	};
	if (lv >= 1 && lv <= 100) {
		return stamina_rabbit_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get stamina of panda
* @brief  API
*/
int calc_stamina_panda(int lv)
{
	static int stamina_panda_arr[] = {
			 7, 9, 11, 13, 15, 17, 19, 21, 23, 25,
			 27, 29, 31, 33, 35, 37, 39, 41, 43, 45,
			 47, 49, 51, 53, 55, 57, 59, 61, 63, 65,
			 67, 69, 71, 73, 75, 77, 79, 81, 83, 85,
			 87, 89, 91, 93, 95, 97, 99, 101, 103, 105,
			 107, 109, 111, 113, 115, 117, 119, 121, 123, 125,
			 127, 129, 131, 133, 135, 137, 139, 141, 143, 145,
			 147, 149, 151, 153, 155, 157, 159, 161, 163, 165,
			 167, 169, 171, 173, 175, 177, 179, 181, 183, 185,
			 187, 189, 191, 193, 195, 197, 199, 201, 203, 205
	};
	if (lv >= 1 && lv <= 100) {
		return stamina_panda_arr[lv - 1];
	}
	return 0;
}
/** 
* @fn get stamina of  dragon
* @brief  API
*/
int calc_stamina_dragon(int lv)
{
	static int stamina_dragon_arr[] = {
	     18, 22, 26, 30, 34, 38, 42, 46, 50, 54,
		 58, 62, 66, 70, 74, 78, 82, 86, 90, 94,
		 98, 102, 106, 110, 114, 118, 122, 126, 130, 134,
		 138, 142, 146, 150, 154, 158, 162, 166, 170, 174,
		 178, 182, 186, 190, 194, 198, 202, 206, 210, 214,
		 218, 222, 226, 230, 234, 238, 242, 246, 250, 254,
		 258, 262, 266, 270, 274, 278, 282, 286, 290, 294,
		 298, 302, 306, 310, 314, 318, 322, 326, 330, 334,
		 338, 342, 346, 350, 354, 358, 362, 366, 370, 374,
		 378, 382, 386, 390, 394, 398, 402, 408, 412, 414

	};
	if (lv >= 1 && lv <= 100) {
		return stamina_dragon_arr[lv - 1];
	}
	return 0;
}


/** 
* @fn get exp of monkey
* @brief  API
*/
uint32_t calc_exp(int lv, bool is_summon)
{
#ifdef VERSION_KAIXIN
	static uint32_t exp_arr[] = {
			0, 2000, 4310, 7060, 10280, 13880, 18255, 23845, 30595, 38575, 
			46115, 54995, 65287, 76807, 90007, 102247, 116247, 133407, 153511, 176565, 
			206205, 244739, 292299, 351939, 428193, 522940, 639340, 786540, 968274, 1191154, 
			1421101, 1676343, 1959661, 2274144, 2623220, 3010695, 3440792, 3918199, 4448122, 5036336, 
			5689253, 6413991, 7218451, 8111401, 9102575, 10202779, 11424005, 12779566, 14284239, 15954426, 
			17808332, 19866170, 22150370, 24685831, 27500193, 30624134, 34091710, 37940718, 42213118, 46955481, 
			52941757, 59550606, 66846775, 74901745, 83794433, 93611960, 104450509, 116416268, 129626466, 144210525, 
			160311325, 178086609, 197710522, 219375323, 243293262, 269698668, 298850235, 331033565, 366563962, 405789520, 
			449094536, 496903274, 549684121, 607954175, 672284315, 743304790, 821711394, 908272285, 1003835509, 1109337308, 
			1225811294, 1354398574, 1496358932, 1653083167, 1826106722, 2017124727, 2228008604, 2460824405, 2717853049, 3001612673
	};

	static uint32_t summon_exp_arr[] = {
			0, 5208, 11712, 19680, 29232, 40560, 53808, 69120, 86688, 106680, 
			129240, 154608, 182928, 214416, 249264, 287712, 330389, 377761, 430344, 488711, 
			553498, 625411, 705236, 793840, 892192, 1001362, 1122541, 1257049, 1406353, 1572081, 
			1756039, 1960232, 2186887, 2438473, 2717734, 3027714, 3371792, 3753718, 4177656, 4648227, 
			5170561, 5750351, 6393919, 7108279, 7901219, 8781382, 9758363, 10842812, 12046550, 13382699, 
			14865825, 16512095, 18339454, 20367823, 22619313, 25118466, 27892526, 30971733, 34389653, 38183543, 
			42353405, 46710911, 51264504, 56023010, 60995648, 66192054, 71622299, 77296905, 83226869, 89423680, 
			95899348, 102666422, 109738013, 117127826, 124850181, 132920042, 141353046, 150165536, 159374588, 168998047, 
			179054561, 189563619, 200545585, 212021739, 224014319, 236546566, 249642764, 263328291, 277629667, 292574604, 
			308192064, 324512310, 341566966, 359389082, 378013194, 397475390, 417813385, 439066590, 461276189, 484485220
	};

#else
	static uint32_t exp_arr[] = {
			0, 2000, 4310, 7060, 10280, 13880, 18255, 23845, 30595, 38575, 
			46115, 54995, 65287, 76807, 90007, 102247, 116247, 133407, 153511, 176565, 
			206205, 244739, 292299, 351939, 428193, 522940, 639340, 786540, 968274, 1191154, 
			1491394, 1811314, 2151634, 2513314, 2897074, 3303874, 3734674, 4190194, 4671394, 5179474, 
			5715154, 6279634, 6873874, 7499074, 8156194, 8846434, 9570754, 10330594, 11127154, 11961874, 
			12835954, 13750594, 14707474, 15707794, 16753234, 17844994, 18984994, 20174674, 21415714, 22709794, 
			24058834, 25464514, 26928514, 28452994, 30039874, 32420194, 35990674, 41346394, 49379974, 61430344, 
			74430344, 88430344, 103430344, 119430344, 136430344, 154430344, 173430344, 193430344, 214430344, 236430344,	
			3152261989, 3152261989, 3152261989, 3152261989, 3152261989, 3152261989, 3152261989, 3152261989, 3152261989, 99013920,
			103134720, 107398080, 111808320, 116369760, 121086960, 125964480, 131006880, 136218960, 141605760, 147172320
	};

	
	static uint32_t summon_exp_arr[] = {
			 0, 4800, 11712, 21120, 33408, 48960, 68160, 91392, 119040, 151488, 
			 189312, 232896, 282432, 338496, 401664, 472128, 550464, 637056, 732480, 837120, 951360, 
			 1075776, 1210944, 1357248, 1515264, 1685568, 1868736, 2065344, 2275776, 2500800, 2740992, 
			 2996928, 3269184, 3558528, 3865536, 4190976, 4535616, 4900032, 5284992, 5691456, 6120000, 
			 6571584, 7046976, 7547136, 8072832, 8625024, 9204480, 9812352, 10449600, 11117376, 11816640, 
			 12548352, 13313856, 14114112, 14950464, 15823872, 16735872, 17687616, 18680448, 19715712, 20794944, 
			 21919488, 23090688, 24310272, 25579776, 26900928, 28275264, 29704512, 31190400, 32734848, 34339584, 
			 36006720, 37737984, 39535680, 41401728, 43338240, 45347520, 47431872, 49593600, 51835008, 54158592, 
			 56567040, 59062656, 61648320, 64326720, 67100736, 69973056, 72946944, 76025280, 79211136, 82507776, 
			 85918464, 89446656, 93095808, 96869568, 100771584, 104805504, 108975168, 113284608, 117737856
	};
#endif

	
	if (lv >= 1 && lv <= 100) {
		if (!is_summon) {
			return exp_arr[lv - 1];
		} else {
			return summon_exp_arr[lv - 1];
		}
	}
	return 0;
}

/** 
* @fn get exp of monkey
* @brief  API
*/
uint32_t calc_old_exp(int lv, bool is_summon)
{
#ifdef VERSION_KAIXIN
	static uint32_t exp_arr[] = {
			0, 6510, 14640, 24600, 36540, 50700, 67260, 86400, 108360, 133350, 
			161550, 193260, 228660, 268020, 311580, 359640, 412987, 472201, 537930, 610888, 
			691872, 781764, 881544, 992300, 1115240, 1251702, 1403176, 1571311, 1757942, 1965102, 
			2195049, 2450291, 2733609, 3048092, 3397168, 3784643, 4214740, 4692147, 5222070, 5810284, 
			6463201, 7187939, 7992399, 8885349, 9876523, 10976727, 12197953, 13553514, 15058187, 16728374, 
			18582281, 20640118, 22924318, 25459779, 28274141, 31398082, 34865658, 38714666, 42987066, 47729429, 
			52941757, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000
	};

	static uint32_t summon_exp_arr[] = {
			0, 5208, 11712, 19680, 29232, 40560, 53808, 69120, 86688, 106680, 
			129240, 154608, 182928, 214416, 249264, 287712, 330389, 377761, 430344, 488711, 
			553498, 625411, 705236, 793840, 892192, 1001362, 1122541, 1257049, 1406353, 1572081, 
			1756039, 1960232, 2186887, 2438473, 2717734, 3027714, 3371792, 3753718, 4177656, 4648227, 
			5170561, 5750351, 6393919, 7108279, 7901219, 8781382, 9758363, 10842812, 12046550, 13382699, 
			14865825, 16512095, 18339454, 20367823, 22619313, 25118466, 27892526, 30971733, 34389653, 38183543, 
			42353405,  4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000,
			4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000, 4000000000
	};

#else
	static uint32_t exp_arr[] = {
			0, 6000, 14640, 26400, 41760, 61200, 85200, 114240, 148800, 189360,
			236640, 291120, 353040, 423120, 502080, 590160, 688080, 796320, 915600, 1046400,
			1189200, 1344720, 1513680, 1696560, 1894080, 2106960, 2335920, 2581680, 2844720, 3126000,
			3426240, 3746160, 4086480, 4448160, 4831920, 5238720, 5669520, 6125040, 6606240, 7114320,
			7650000, 8214480, 8808720, 9433920, 10091040, 10781280, 11505600, 12265440, 13062000, 13896720,
			14770800, 15685440, 16642320, 17642640, 18688080, 19779840, 20919840, 22109520, 23350560, 24644640,
			25993680, 27399360, 28863360, 30387840, 31974720, 33626160, 35344080, 37130640, 38988000, 40918560,
			42924480, 45008400, 47172480, 49419600, 51752160, 54172800, 56684400, 59289840, 61992000, 64793760,
			67698240, 70708800, 73828320, 77060400, 80408400, 83875920, 87466320, 91183680, 95031600, 99013920,
			103134720, 107398080, 111808320, 116369760, 121086960, 125964480, 131006880, 136218960, 141605760, 147172320
	};

	
	static uint32_t summon_exp_arr[] = {
			 0, 4800, 11712, 21120, 33408, 48960, 68160, 91392, 119040, 151488, 
			 189312, 232896, 282432, 338496, 401664, 472128, 550464, 637056, 732480, 837120, 951360, 
			 1075776, 1210944, 1357248, 1515264, 1685568, 1868736, 2065344, 2275776, 2500800, 2740992, 
			 2996928, 3269184, 3558528, 3865536, 4190976, 4535616, 4900032, 5284992, 5691456, 6120000, 
			 6571584, 7046976, 7547136, 8072832, 8625024, 9204480, 9812352, 10449600, 11117376, 11816640, 
			 12548352, 13313856, 14114112, 14950464, 15823872, 16735872, 17687616, 18680448, 19715712, 20794944, 
			 21919488, 23090688, 24310272, 25579776, 26900928, 28275264, 29704512, 31190400, 32734848, 34339584, 
			 36006720, 37737984, 39535680, 41401728, 43338240, 45347520, 47431872, 49593600, 51835008, 54158592, 
			 56567040, 59062656, 61648320, 64326720, 67100736, 69973056, 72946944, 76025280, 79211136, 82507776, 
			 85918464, 89446656, 93095808, 96869568, 100771584, 104805504, 108975168, 113284608, 117737856
	};
#endif

	
	if (lv >= 1 && lv <= 100) {
		if (!is_summon) {
			return exp_arr[lv - 1];
		} else {
			return summon_exp_arr[lv - 1];
		}
	}
	return 0;
}


uint32_t calc_honor(uint16_t honor_lv)
{
	return honor_exp_arr[honor_lv - 1];
}

uint16_t get_user_lv_needed_by_honor_lv(uint16_t honor_lv)
{
	return user_lv_by_honor_lv_arr[honor_lv - 1];
}

/** 
* @fn calc player's lv after add exp
* @brief  API
*/
int calc_lv_add_exp(int gain_exp, uint32_t& exp, uint16_t& lv, int max_lv, uint32_t role_type, uint32_t uid, bool is_summon)
{
 	 int lv_up  = 0;
 	 int nextlv = lv + 1;
	 uint32_t  total_exp = exp + gain_exp;

	 if (nextlv > max_lv) {
	 	return 0;
	 }
	 if ( total_exp >= calc_exp(max_lv, is_summon)) {
		 exp = calc_exp(max_lv, is_summon);
		 lv_up = max_lv - lv;
	 } else {
		 exp = total_exp;
		 while ( total_exp >= calc_exp(nextlv, is_summon)) {
			 lv_up ++;
			 nextlv++;
		 }
	 }

  if (lv_up) {
      lv += lv_up;
	  uint32_t flag1[2] = { 0, 1 };
	  uint32_t flag2[2] = {	1, 0 };
	  int ret = 0;
#define STAT_LOG_MSG_ID_BEGIN 0x09010301
	  if (!is_summon && lv != 1) {
	  	
	  	  uint32_t flag3[2] = {uid, lv};
#ifdef VERSION_KAIXIN
		  msglog(static_log_path, 0x0F500003 + role_type, get_now_tv()->tv_sec, flag3, sizeof(flag3));
#else
		  msglog(static_log_path, 0x09010400 + role_type, get_now_tv()->tv_sec, flag3, sizeof(flag3));
	  
		  if (lv < 31) {
			uint32_t msg_id = STAT_LOG_MSG_ID_BEGIN + lv - 1;
			ret = msglog(static_log_path, msg_id - 1, get_now_tv()->tv_sec, &flag1, sizeof(flag1));
			ret += msglog(static_log_path, msg_id, get_now_tv()->tv_sec, &flag2, sizeof(flag2));

		  } else if (lv > 30 && lv < 100 && !((lv - 1) % 5)){
			uint32_t msg_id = STAT_LOG_MSG_ID_BEGIN + 30 + (lv - 30 - 1)/5;
            ret = msglog(static_log_path, msg_id - 1, get_now_tv()->tv_sec, &flag1, sizeof(flag1));
			ret += msglog(static_log_path, msg_id, get_now_tv()->tv_sec, &flag2, sizeof(flag2));

		  } else if (lv == 100) {
			  uint32_t msg_id = STAT_LOG_MSG_ID_BEGIN + 30 + (lv - 30)/5;
              ret = msglog(static_log_path, msg_id - 1, get_now_tv()->tv_sec, &flag1, sizeof(flag1));
              ret += msglog(static_log_path, msg_id, get_now_tv()->tv_sec, &flag2, sizeof(flag2));
		  }
#endif	
	  }
#if 0
      if (((lv != 1 && (lv - 1) % 10 == 0) || lv == 100) && static_log_path[0] != 0x0) {
	      	uint32_t flag[2];
	      	flag[0] = 0;
	      	flag[1] = 1;
	      	int ret = msglog(static_log_path, 0x09010301 + lv / 10 - 1, get_now_tv()->tv_sec, &flag, sizeof(flag));
	      	flag[0] = 1;
	      	flag[1] = 0;
	      	ret += msglog(static_log_path, 0x09010301 + lv / 10, get_now_tv()->tv_sec, &flag, sizeof(flag));
	      	DEBUG_LOG("REPORT LVUP %s : %d [%d]", static_log_path, get_now_tv()->tv_sec, ret);
      }
#endif
  }
  
  return lv_up;
}

/** 
* @fn set static_log_path
* @brief  API
*/
void gf_set_static_log_path(const char* path)
{
	if (strlen(path) >= PATH_MAX) {
		ERROR_LOG("static path len too long %u max:%u", (uint32_t)strlen(path), (uint32_t)PATH_MAX);
		return;
	}
	strcpy(static_log_path, path);
}

/** 
* @fn calc player's clothes duration state
* @brief  API
*/
uint8_t	get_cloth_duration_state(uint16_t cur_duration, uint16_t init_duration)
{
		float duration_rate = static_cast<float>(cur_duration) / static_cast<float>(init_duration);
    uint8_t duration_state = 0;
    if (duration_rate >= 0.5) {
        duration_state = 0;
    } else if (duration_rate >= 0.3) {
        duration_state = 50;
    } else if (duration_rate >= 0.2) {
        duration_state = 70;
    } else if (duration_rate > 0.00001) {
        duration_state = 80;
    } else {
        duration_state = 100;
    }
    
    return duration_state;
}
int calc_player_base_mp(uint32_t lv, uint32_t role_type)
{
	uint32_t base_stamin = 0;
	switch (role_type) {
	 case 1:
		 base_stamin = calc_stamina_monkey(lv);
		 break;
	 case 2:
		 base_stamin = calc_stamina_rabbit(lv);
		 break;
	 case 3:
		 base_stamin = calc_stamina_panda(lv);
		 break;
	 case 4:
		 base_stamin = calc_stamina_dragon(lv);
		 break;
	 default:
		 break;
	}
	return calc_mp[role_type](base_stamin);
}
