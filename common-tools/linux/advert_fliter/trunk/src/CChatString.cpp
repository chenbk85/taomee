/*
 * =====================================================================================
 *
 *       Filename:  CChatString.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/29/2011 07:09:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include "CChatString.h"
#include <fstream>
#include <string>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

#include <libtaomee/log.h>

#ifdef __cplusplus
}
#endif


CTrimAbnormal::CTrimAbnormal()
{
	for (int c = 1; c <= 127; c ++) {
		this->add_abnormal(c);
	}

	this->add_abnormal("，");
	this->add_abnormal("。");
	this->add_abnormal("！");
	this->add_abnormal("⊙ ");
	this->add_abnormal("﹏");
	this->add_abnormal("１");
	this->add_abnormal("２");
	this->add_abnormal("３");
	this->add_abnormal("４");
	this->add_abnormal("５");
	this->add_abnormal("６");
	this->add_abnormal("７");
	this->add_abnormal("８");
	this->add_abnormal("９");
	this->add_abnormal("０");
	this->add_abnormal("·");
	this->add_abnormal("丶");
	this->add_abnormal("∩");
	this->add_abnormal("☆");
	this->add_abnormal("★");
	this->add_abnormal("▽");
	this->add_abnormal("゛");
	this->add_abnormal("┣");
	this->add_abnormal("≦");
	this->add_abnormal("≧");
	this->add_abnormal("、");
	this->add_abnormal("【");
	this->add_abnormal("”");
	this->add_abnormal("“");
	this->add_abnormal("﹏");
	this->add_abnormal("？");
	this->add_abnormal("－");
	this->add_abnormal("（");
	this->add_abnormal("）");
	this->add_abnormal("＋");
	this->add_abnormal("；");
	this->add_abnormal("～");
	this->add_abnormal("ｉ");
	this->add_abnormal("Ａ");
	this->add_abnormal("Ｂ");
	this->add_abnormal("Ｃ");
	this->add_abnormal("Ｄ");
	this->add_abnormal("Ｅ");
	this->add_abnormal("Ｆ");
	this->add_abnormal("Ｇ");
	this->add_abnormal("Ｈ");
	this->add_abnormal("Ｉ");
	this->add_abnormal("Ｊ");
	this->add_abnormal("Ｋ");
	this->add_abnormal("Ｌ");
	this->add_abnormal("Ｍ");
	this->add_abnormal("Ｏ");
	this->add_abnormal("Ｐ");
	this->add_abnormal("Ｑ");
	this->add_abnormal("Ｒ");
	this->add_abnormal("Ｓ");
	this->add_abnormal("Ｔ");
	this->add_abnormal("Ｕ");
	this->add_abnormal("Ｖ");
	this->add_abnormal("Ｗ");
	this->add_abnormal("Ｘ");
	this->add_abnormal("Ｙ");
	this->add_abnormal("Ｚ");
	this->add_abnormal("ａ");
	this->add_abnormal("ｂ");
	this->add_abnormal("ｃ");
	this->add_abnormal("ｄ");
	this->add_abnormal("ｅ");
	this->add_abnormal("ｆ");
	this->add_abnormal("ｇ");
	this->add_abnormal("ｈ");
	this->add_abnormal("ｉ");
	this->add_abnormal("ｊ");
	this->add_abnormal("ｋ");
	this->add_abnormal("ｌ");
	this->add_abnormal("ｍ");
	this->add_abnormal("ｏ");
	this->add_abnormal("ｐ");
	this->add_abnormal("ｑ");
	this->add_abnormal("ｒ");
	this->add_abnormal("ｓ");
	this->add_abnormal("ｔ");
	this->add_abnormal("ｕ");
	this->add_abnormal("ｖ");
	this->add_abnormal("ｗ");
	this->add_abnormal("ｘ");
	this->add_abnormal("ｙ");
	this->add_abnormal("ｚ");
	/*  
	for (int c = 1; c < '0'; c ++) {
		this->add_abnormal(c);
	}

	for (int c = '9' + 1; c < 'A'; c ++) {
		this->add_abnormal(c);
	}

	for (int c = 'Z' + 1; c < 'a'; c ++) {
		this->add_abnormal(c);
	}

	for (int c = 'z' + 1; c < 128; c ++) {
		this->add_abnormal(c);
	}
	*/
}

void CTrimAbnormal::add_abnormal(const char* s)
{
	uint8_t s0 = s[0];
	if ((s0 & 0xF0) == 0xF0)	// 不管特殊字符
		return;

	if ((s0 & 0xF0) == 0xE0) {
		uint32_t val = 0;
		memcpy(&val, s, 3);
		this->abnormal_set_3.insert(val);
	} else if ((s0 & 0xE0) == 0xC0) {
		this->abnormal_set_2.insert(*(uint16_t *)s);
	} else if (!(s0 & 0x80)) {
		add_abnormal(s[0]);
	}
}

void CTrimAbnormal::add_abnormal(const char c)
{
	this->abnormal_set_1.insert(c);
}

bool CTrimAbnormal::is_del_word(const char* word)
{
	return this->del_word_set.find(BKDRHash(word)) != this->del_word_set.end();
}

void CTrimAbnormal::init_del_word(const char* file)
{
    fstream fstr;
    fstr.open(file, fstream::in);
    if (!fstr.is_open())
        return;

	while (!fstr.eof()) {
		string word;
		fstr >> word;
		this->del_word_set.insert(BKDRHash(word.c_str()));
	}
}

void CTrimAbnormal::show_abnormal()
{
	for(std::set<uint8_t>::iterator it = abnormal_set_1.begin(); it != abnormal_set_1.end(); it ++) {
		DEBUG_LOG("ABNORMAL 1 VAL\t[%u]", *it);
	}

	for(std::set<uint16_t>::iterator it = abnormal_set_2.begin(); it != abnormal_set_2.end(); it ++) {
		DEBUG_LOG("ABNORMAL 2 VAL\t[%u]", *it);
	}

	for(std::set<uint32_t>::iterator it = abnormal_set_3.begin(); it != abnormal_set_3.end(); it ++) {
		DEBUG_LOG("ABNORMAL 3 VAL\t[%u]", *it);
	}
}

void CTrimAbnormal::trim_abnormal_utf8(char* s)
{
	//show_abnormal();
	int i = 0, j = -1;
	while (s[i]) {
		if (!(s[i] & 0x80)) {				// 最高位为0，单字节utf8 （ascii）
			if (abnormal_set_1.find(s[i]) != abnormal_set_1.end()) {
				i ++;
			} else if (j + 1 < i) {
				j ++; s[j] = s[i]; i ++;
			} else {
				i ++; j ++;
			}
		} else if ((s[i] & 0xE0) == 0xC0) { // 高3位为110， 2字节utf8
			uint16_t val = *(uint16_t *)s;
			if (abnormal_set_2.find(val) == abnormal_set_2.end()) {
				if (j + 1 < i) {
					s[j+1] = s[i];
					s[j+2] = s[i + 1];
				}
				j += 2;
			}

			i += 2;
		} else if ((s[i] & 0xF0) == 0xE0) { // 高4位为1110， 3字节utf8
			uint32_t val = 0;
			memcpy(&val, s + i, 3);

			if (abnormal_set_3.find(val) == abnormal_set_3.end()) {
				if (j + 1 < i) {
					s[j+1] = s[i];
					s[j+2] = s[i+1];
					s[j+3] = s[i+2];
				}
				j += 3;
			}

			i += 3;
		} else {							// 其他utf8表示
			i ++;
		}
	}

	s[++j] = '\0';
	//DEBUG_LOG("AFTER TRIM\t[%s]", s);
}

