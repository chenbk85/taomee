/*
 * =====================================================================================
 *
 *       Filename:  CChatTest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/18/2011 09:10:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */
#include "CChatTest.h"
#include <libtaomee/random/random.h>

uint32_t g_userid = 50000;

CTestUser::CTestUser() 
{ 
	userid = g_userid ++; 
}

void CNormalUser::talk(char* buf, int& len)
{
	const char* s = "abcdefghijklmopqrst \tuvwxyz0123456789!@#$%^&*()+-";
	const int slen = strlen(s);
	len = ranged_random(128, 256);
	for (int loop = 0; loop < len - 1; loop ++) {
		buf[loop] = s[rand() % slen];
	}
	buf[len - 1] = '\0';

}

void CAbnormalUser::talk(char* buf, int& len)
{
	/*
	const char *str[10] = {
		"第1句话",
		"第2句话",
		"第3句话",
		"第4句话",
		"第5句话",
		"第6句话",
		"第7句话",
		"第8句话",
		"第9句话",
		"第10句话",
	};
	*/

	const char *str[10] = {
		"第\t1句话",
		"第1%句话",
		"第1(句话",
		"第1句话)",
		"第1句话+++",
		"第---1句话",
		"第^^1句话",
		"第1&*&*句话",
		"第1句               话",
		"第1\t\t\t\t句话",
	};

	len = strlen(str[str_idx[cur_idx]]);
	memcpy(buf, str[str_idx[cur_idx]], len);
	buf[len] = '\0';
	cur_idx = (cur_idx + 1) % 10;
}

void CChatTest::talk_test(CChatCheck* p_chat_check)
{
	char buf[1024];
	int len = 0;
	/*  
	const char* trim_str = "我是\thello";
	memcpy(buf, trim_str, strlen(trim_str) + 1);
	p_chat_check->trim_abnormal(buf);
	*/

	for (int i = 0; i < 100; i ++) {
	
		int normal_user_count = rand() % 2;
		for (int loop = 0; loop < normal_user_count; loop ++) {
			CNormalUser* puser = &user[rand() % 1000];
			puser->talk(buf, len);
			if (!p_chat_check->check_msg(puser->userid, buf, len))
				DEBUG_LOG("CHECK FAIL\t[%u]", puser->userid);
		}
		for (int j = 0; j < 2; j ++) {
			nuser[cur_nuser_idx].talk(buf, len);
			if (!p_chat_check->check_msg(nuser[cur_nuser_idx].userid, buf, len))
				DEBUG_LOG("CHECK FAIL\t[%u]", nuser[cur_nuser_idx].userid);
			cur_nuser_idx = (cur_nuser_idx + 1) % 10;
		}
	}
}
