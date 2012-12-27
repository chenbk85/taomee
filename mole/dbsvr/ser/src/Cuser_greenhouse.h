/*
 * =====================================================================================
 *
 *       Filename:  Cuser_greenhouse.h
 *
 *    Description:  暖房类，它实现了向“暖房”的数据更新，此处是接口
 *
 *        Version:  1.0
 *        Created:  11/16/09 16:18:18
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Cuser_greenhouse.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef CUSER_GREENHOUSE_INCL
#define CUSER_GREENHOUSE_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_greenhouse: public CtableRoute100x10
{
public:
	Cuser_greenhouse(mysql_interface * db); 

	int insert(userid_t user_id, uint32_t animal_id, uint32_t animal_type); 
	
	int drop(userid_t user_id, uint32_t animal_id);

	int get_remain_time(userid_t user_id, uint32_t aniaml_id, uint32_t* remain_time);

	int user_exist(userid_t user_id, uint32_t* stutas);

	int user_all_animal(const userid_t user_id, user_greenhouse_user_all_animal_out_item** pplist, uint32_t* p_count);

	uint32_t user_get_baby_id(userid_t user_id, uint32_t animal_id, const bool is_vip);

	static uint32_t get_rabbit_baby_id(const bool is_vip);

	static uint32_t get_big_ear_rabbite_baby_id(const bool is_vip);

	static uint32_t married_to_remain_time(const uint32_t married_time);
private:
	int get_married_time_(userid_t user_id, uint32_t animal_id, uint32_t *p_out);

	int greenhouse_insert_(userid_t user_id, uint32_t animal_id, uint32_t animal_type);

	int greenhouse_drop_(userid_t user_id, uint32_t animal_id);

	int get_animal_type(userid_t user_id, uint32_t animal_id, uint32_t *p_out);
};

inline uint32_t Cuser_greenhouse::married_to_remain_time(const uint32_t married_time)
{
	//从配对开始到生兔子需要24小时
	const uint32_t need_time=24*60*60;
	uint32_t t=(need_time)-((uint32_t)::time(NULL)-married_time);
	return (t > need_time ? 0 : t);
}

inline uint32_t Cuser_greenhouse::get_rabbit_baby_id(const bool is_vip)
{
	const uint32_t white_rabbit=1270015;
	const uint32_t gray_rabbit=1270016;
	const uint32_t colorful_rabbit=1270017;
	int t=::rand()%10;
	if(!is_vip && t<=3)
	{
		//白兔子
		return white_rabbit;
	} else if (!is_vip) {
		//黑兔子
		return gray_rabbit;
	}

	if(is_vip && t<=2)
	{
		//白兔子
		return white_rabbit;
	}else if(is_vip && t<=8){
		//黑兔子
		return gray_rabbit;
	}else {
		//花兔子
		return colorful_rabbit;
	}
}
/*
inline uint32_t Cuser_greenhouse::get_big_ear_rabbite_baby_id(const bool is_vip)
{
	const uint32_t big_ear_white_rabbit = 1270073;
	const uint32_t big_ear_blue_rabbit = 1270072;
	const uint32_t big_ear_pink_rabbit = 1270074;
	int random = rand() % 100;
	if(is_vip){
		if(random < 50){
			return big_ear_pink_rabbit;
		}
		else if(random >= 50 && random < 75){
			return big_ear_blue_rabbit;
		}
		else{
			return  big_ear_white_rabbit;
		}
	}
	else{
		if(random < 50){
			return big_ear_blue_rabbit;
		}
		else{
			return big_ear_white_rabbit;
		}
	}
}
*/
#endif //CUSER_GREENHOUSE_INCL

