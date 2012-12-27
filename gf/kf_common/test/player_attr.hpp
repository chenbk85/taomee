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
extern int calc_exp(int lv);

extern float hit_rates[];
extern int crit_adjs[];
inline int calc_atk(int strength){
	return (int)(strength * 0.7);
}
inline float calc_def_rate(int body_quality){
	return (body_quality / 2500.0 );
}
inline float calc_dodge_rate(int agility){
	return (agility / 1250.0);
}
#endif
