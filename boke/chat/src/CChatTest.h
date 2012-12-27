/*
 * =====================================================================================
 *
 *       Filename:  CChatTest.h
 *
 *    Description:  Test chat check: 1000 user normal, 10 user cheat
 *
 *        Version:  1.0
 *        Created:  10/18/2011 09:06:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */
#ifndef CCHAT_TEST_H_
#define CCHAT_TEST_H_

#include "CChatCheck.h"

class CTestUser {
public:
	uint32_t userid;
	CTestUser();
};

class CNormalUser : public CTestUser {
public:
	void talk(char* buf, int& len);
};

class CAbnormalUser : public CTestUser {
	int str_idx[10];
	int cur_idx;
public:
	void talk(char* buf, int& len);
	CAbnormalUser()
	{
		cur_idx = 0;
		for (int loop = 0; loop < 10; loop ++) {
			str_idx[loop] = loop;
		}

		for (int loop = 0; loop < 10; loop ++) {
			int idx = rand() % 10;
			str_idx[idx] = loop;
			str_idx[loop] = idx;
		}
	}
};

class CChatTest {
	CNormalUser user[1000];
	CAbnormalUser nuser[10];
	int cur_nuser_idx;
public:
	void talk_test(CChatCheck* p_chat_check);
	CChatTest() : cur_nuser_idx(0) {}
};

#endif
