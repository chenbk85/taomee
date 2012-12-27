/*
 * =====================================================================================
 *
 *       Filename:  utils.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2012 10:46:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <time.h>
extern "C" {
#include <stdint.h>
}

bool is_same_day(time_t tm_1, time_t tm_2);

int day_interval(time_t tm_1, time_t tm_2);

uint32_t get_today_last_sec();

#endif


