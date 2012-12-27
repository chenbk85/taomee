// copy follow content to player_attr.hpp
#ifndef GF_PLAYER_ATTR_HPP
#define GF_PLAYER_ATTR_HPP

#include <stdint.h>
#include <math.h>

typedef int (*gf_calc_my_fun_def)(int lv);
typedef float (*gf_calc_my_floatfun_def)(int lv);

// declare API array:
extern gf_calc_my_fun_def calc_strength[5];
extern gf_calc_my_fun_def calc_agility[5];
extern gf_calc_my_fun_def calc_body_quality[5];
extern gf_calc_my_fun_def calc_stamina[5];
extern gf_calc_my_fun_def calc_mp[5];
extern gf_calc_my_fun_def calc_hp[5];
extern uint32_t calc_exp(int lv, bool is_summon = false);
extern uint32_t calc_old_exp(int lv, bool is_summon = false);

extern void gf_set_static_log_path(const char* path);
extern int calc_lv_add_exp(int gain_exp, uint32_t& exp, uint16_t& lv, int max_lv, uint32_t role_type, uint32_t uid, bool is_summon = false);
extern uint8_t	get_cloth_duration_state(uint16_t cur_duration, uint16_t init_duration);

extern uint32_t calc_honor(uint16_t honor_lv);
extern uint16_t get_user_lv_needed_by_honor_lv(uint16_t honor_lv);

extern float hit_rates[];
extern int crit_adjs[];
inline int calc_atk(int strength){
	return (int)(strength * 1);
}

inline float calc_def_rate(int lv){
	return (lv * 2 / 2500.0 );
}

inline uint32_t calc_def_value(int lv)
{
	return lv * 1.2;
}


inline float calc_dodge_rate(int agility){
	return (agility / 1250.0);
}
/*

inline float calc_dodge_rate(int lv){
	return (26.3) / (100 * pow(lv, 1.32));
}

inline float calc_crit_rate(int lv) {
	return (26.3) / (100 * pow(lv, 1.32));
}

inline float calc_hit_rate(int lv) {
	return (25) / (100 * pow(lv, 1.32));
}

*/

int calc_weapon_add_atk(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_weapon_add_agility(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_weapon_add_strength(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_weapon_add_hit(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_clothes_add_defense(uint32_t attirelv);
int calc_clothes_add_body(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_clothes_add_hp(uint32_t uselv, uint32_t attirelv, uint32_t quality);
int calc_clothes_add_dodge(uint32_t uselv, uint32_t attirelv, uint32_t quality);

int calc_player_base_mp(uint32_t userlv, uint32_t role_type);


#endif
