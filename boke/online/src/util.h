#ifndef _UTIL_H_
#define _UTIL_H_

// C89
#include <string.h>
#include <time.h>
// C99
#include <stdint.h>
#include <math.h>
// Posix
#include <arpa/inet.h>

#include <glib.h>

#include <libxml/tree.h>
#ifdef __cplusplus
extern "C"
{
#endif

#include <libtaomee/log.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include "libtaomee/project/stat_agent/msglog.h"
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/dataformatter/bin_str.h>

#ifdef __cplusplus
}
#endif
#include <libtaomee++/proto/proto_util.h>

inline uint32_t get_period_midnight()
{
    time_t nowtime =time(NULL);
    struct tm tm_midnight;
    localtime_r(&nowtime, &tm_midnight);
    tm_midnight.tm_hour = 24;
    tm_midnight.tm_min = 0;
    tm_midnight.tm_sec = 0;
	DEBUG_LOG("get_period_midnight====%u,%u", 
			(uint32_t)mktime(&tm_midnight) ,(uint32_t) nowtime  );
    return mktime(&tm_midnight) - nowtime;
}

inline uint32_t get_day_start_seconds(uint32_t t)
{
	return t - (t + 8 * 3600) % 86400;
}

inline uint32_t get_day_second(uint32_t t)
{
	return (t + 8 * 3600) % 86400;
}

inline bool is_same_day(uint32_t t1, uint32_t t2)
{
	return get_day_start_seconds(t1) == get_day_start_seconds(t2);
}

template <typename T>
int rand_index(const T& proability_list, uint32_t base)
{
    uint32_t rval = rand() % base, total_val = 0;
    for (uint32_t index = 0; index < proability_list.size(); index ++) {
        total_val += proability_list[index].proability;
        if (rval < total_val)
            return index;
    }

    return proability_list.size();
}

inline int rand_index(int cnt, uint32_t* probability, uint32_t base)
{
    uint32_t rval = rand() % base, total_val = 0;
    for (int index = 0; index < cnt; index ++) {
        total_val += probability[index];
        if (rval < total_val)
            return index;
    }

    return cnt;
}

// 二次方程求根（注意如果方程无实数解，也返回0）
inline double quadratic(double a, double b, double c)
{
	double X = b * b - 4 * a * c;
	if (X < 0) return 0;

	X = sqrt(X);
	double x1 = (X - b) / 2 / a;
	double x2 = (-X - b) / 2 / a;
	return x2 <= 0 ? x1 : x2;
}

#include "benchapi.h"


#define SUCC 0

#define END_TASK_NODEID  99999
#define TASK_XIAOMEE_VALUE 100 
#define NICK_LEN 	16

#define GAME_REST_MIN		30
#define GAME_REST_MIN_2		45
#define GAME_REST_SEC		(GAME_REST_MIN * 60)
#define GAME_REST_SEC_2		(GAME_REST_MIN_2 * 60)
#define GAME_NORMAL_MIN		180
#define GAME_NORMAL_SEC		(GAME_NORMAL_MIN * 60)
#define GAME_MAX_MIN		300
#define GAME_MAX_SEC		(GAME_MAX_MIN * 60)

// 类属性get和set函数
#define POP_PROPERTY_READONLY(varType, varName)\
	private: varType varName;\
	public: varType get_##varName(void);

#define POP_PROPERTY_READONLY_BY_REF(varType, varName)\
	private: varType varName;\
	public: const varType& get_##varName(void);

#define POP_PROPERTY(varType, varName)\
	private: varType varName;\
	public: varType get_##varName(void);\
	public: void set_##varName(varType var);

#define POP_PROPERTY_PASS_BY_REF(varType, varName)\
	private: varType varName;\
	public: const varType& get_##varName(void);\
	public: void set_##varName(const varType& var);

// 类属性get和set函数(默认实现)
#define POP_PROPERTY_READONLY_DEFAULT(varType, varName)\
	private: varType varName;\
	public: varType get_##varName(void) { return varName; }

#define POP_PROPERTY_READONLY_BY_REF_DEFAULT(varType, varName)\
	private: varType varName;\
	public: const varType& get_##varName(void) { return varName; }

#define POP_PROPERTY_DEFAULT(varType, varName)\
	private: varType varName;\
	public: varType get_##varName(void) { return varName; } \
	public: void set##varName(varType var) { varName = var; }

#define POP_PROPERTY_PASS_BY_REF_DEFAULT(varType, varName)\
	private: varType varName;\
	public: const varType& get_##varName(void) { return varName; } \
	public: void set_##varName(const varType& var) { varName = var; }

#endif
