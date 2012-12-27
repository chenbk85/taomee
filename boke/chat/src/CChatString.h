/*
 * =====================================================================================
 *
 *       Filename:  CChatString.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/29/2011 07:09:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include <stdint.h>
#include <string.h>
#include <set>
// BKDR Hash 
static inline uint64_t BKDRHash(const char *str)
{
	uint64_t seed = 131; // 31 131 1313 13131 131313 etc..
	uint64_t hash = 0;
 
	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFFFFFFFFFFl);
}


class CTrimAbnormal {
public:
	CTrimAbnormal();
	void trim_abnormal_utf8(char* s);
	bool is_del_word(const char*);
	void init_del_word(const char *);
private:
	std::set<uint8_t> abnormal_set_1;
	std::set<uint16_t> abnormal_set_2;
	std::set<uint32_t> abnormal_set_3;

	std::set<uint64_t> del_word_set;

	void add_abnormal(const char* s);
	void add_abnormal(const char c);
	void show_abnormal();
};


