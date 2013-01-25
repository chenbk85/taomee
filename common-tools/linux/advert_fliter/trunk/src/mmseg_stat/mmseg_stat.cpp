#include <algorithm>
#include <stdio.h>
#include <string>
#include <mmseg/bsd_getopt.h>
#include <mmseg/SegmenterManager.h>
#include <mmseg/csr_utils.h>
#include <libtaomee/log.h>
#include <dbser/mysql_iface.h>
#include <dbser/db_macro.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include "../CChatString.h"
#include "Cbayes.h"
using namespace css;
using namespace std;

#define DESCRIPTION "聊天记录分词统计"
#define VERSION     "version 0.1 (2011.11.22)  (c) 1998-2011   Ian"

#define STD_QUERY_ONE_BEGIN_CHAT( sqlstr) {\
        uint32_t ret;\
        MYSQL_RES *res;\
        MYSQL_ROW row;\
        ret =db->exec_query_sql(sqlstr,&res);\
        if (ret==DB_SUCC){\
            int rowcount=mysql_num_rows(res);\
            if (rowcount!=1) { \
                mysql_free_result(res);      \
				break; \
            }else { \
                row= mysql_fetch_row(res); \
                int _fi  ; _fi=-1;

#define STD_QUERY_ONE_BEGIN_CHAT_EX( sqlstr) {\
        uint32_t ret;\
        MYSQL_RES *res;\
        MYSQL_ROW row;\
        ret =db->exec_query_sql(sqlstr,&res);\
        if (ret==DB_SUCC){\
            int rowcount=mysql_num_rows(res);\
            if (rowcount!=1) { \
                mysql_free_result(res);      \
            }else { \
                row= mysql_fetch_row(res); \
                int _fi  ; _fi=-1;


#define IS_AD(ad_)		(ad & 0x1)
#define IS_MANUAL(ad)	(ad & 0x2)

int segment_only_word(char* inbuf, int len, map<string, int>& msg, char* outbuf)
{
	int i = 0;
	while (inbuf[i]) {
		if (!(inbuf[i] & 0x80)) {				// 最高位为0，单字节utf8 （ascii）
			msg[string(inbuf+i, 1)] = 1;
			i ++;
		} else if ((inbuf[i] & 0xE0) == 0xC0) { // 高3位为110， 2字节utf8
			msg[string(inbuf+i, 2)] = 1;
			i += 2;
		} else if ((inbuf[i] & 0xF0) == 0xE0) { // 高4位为1110， 3字节utf8
			msg[string(inbuf+i, 3)] = 1;
			i += 3;
		} else {							// 其他utf8表示
			i ++;
		}
	}

	memcpy(outbuf, inbuf, len);
	return 0;
}

int segment(char* buffer, int length, Segmenter* seg, map<string, int>& msg, char* outbuf)
{
    seg->setBuffer((u1*)buffer, length);
    u2 len = 0, symlen = 0;
    u2 kwlen = 0, kwsymlen = 0;
	int idx = 0;
    //check 1st token.
    char txtHead[3] = {239,187,191};
    char* tok = (char*)seg->peekToken(len, symlen);
    seg->popToken(len);
    if(seg->isSentenceEnd()){
        do {
            char* kwtok = (char*)seg->peekToken(kwlen , kwsymlen,1);
            if(kwsymlen) {
				msg[string(kwtok, kwsymlen)] += 1;
				sprintf(outbuf+idx, "%*.*s|", kwsymlen, kwsymlen, kwtok);
				idx += kwsymlen + 1;
			}
        }while(kwsymlen);
    }

    if(len == 3 && memcmp(tok,txtHead,sizeof(char)*3) == 0){
    }else{
		msg[string(tok, symlen)] += 1;
		sprintf(outbuf+idx, "%*.*s|", symlen, symlen, tok);
		idx += symlen + 1;
    }
    while(1){
        len = 0;
        char* tok = (char*)seg->peekToken(len,symlen);
        if(!tok || !*tok || !len)
            break;
        seg->popToken(len);
        if(seg->isSentenceEnd()){
            do {
                char* kwtok = (char*)seg->peekToken(kwlen , kwsymlen,1);
                if(kwsymlen) {
					msg[string(kwtok, kwsymlen)] += 1;
					sprintf(outbuf+idx, "%*.*s|", kwsymlen, kwsymlen, kwtok);
					idx += kwsymlen + 1;
				}
            }while(kwsymlen);
        }

        if(*tok == '\r')
            continue;
        if(*tok == '\n'){
            printf("\n");
            continue;
        }


		msg[string(tok, symlen)] += 1;
		sprintf(outbuf+idx, "%*.*s|", symlen, symlen, tok);
		idx += symlen + 1;
        {
            const char* thesaurus_ptr = seg->thesaurus(tok, symlen);
            while(thesaurus_ptr && *thesaurus_ptr) {
                len = strlen(thesaurus_ptr);
				msg[string(thesaurus_ptr, len)] += 1;
				sprintf(outbuf+idx, "%*.*s|", len, len, thesaurus_ptr);
				idx += len;
                thesaurus_ptr += len + 1; //move next
            }
        }
    }
    return 0;
}

mysql_interface* init_db()
{
	mysql_interface* db = new mysql_interface(
			"localhost", "root", "root", 3306, "/var/run/mysqld/mysqld.sock");

	db->set_is_log_debug_sql(0);
	db->set_is_only_exec_select_sql(0);

	db->select_db("CHAT_STAT");

	char sqlstr[1024];
	sprintf (sqlstr, "create table if not exists t_msg(userid int unsigned,"
			"time int unsigned, msg varchar(1024), ad int unsigned, "
			"primary key(userid, time)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	int count;
	db->exec_update_sql(sqlstr, &count);

	sprintf (sqlstr, "create table if not exists t_wmsg_10(msgid bigint unsigned,"
			"msg varchar(1024), ad int unsigned, "
			"primary key(msgid)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	db->exec_update_sql(sqlstr, &count);

	sprintf (sqlstr, "create table if not exists t_char_10(word varchar(64), non_ad int unsigned,"
			" ad int unsigned, mutual double, primary key (word), KEY `mutual` (`mutual`))ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	db->exec_update_sql(sqlstr, &count);

	mysql_commit(&db->handle);

	return db;
}

int mmseg_chat(mysql_interface* db)
{
	// 初始化分词系统
    SegmenterManager mgr;
    mgr.init("../data/");
    Segmenter* seg = mgr.getSegmenter();

	fstream fstr;
	fstr.open("../data/infolog", fstream::in);
	if (!fstr.is_open())
		return -1;

	string msg, nouse;
	uint32_t userid, timestamp, ad;
	int count;

	char sqlstr[1024];

	while (!fstr.eof()) {
		fstr >> nouse >> userid >> nouse >> timestamp >> msg >> ad;
		//cout << userid << "\t" << timestamp << "\t" << msg << "\t" << ad << endl;
		char buf[4096];
		mysql_real_escape_string(&(db->handle), buf, msg.c_str(), msg.size());
		sprintf(sqlstr, "insert into CHAT_STAT.t_msg values(%u, %u, \"%s\", %u)",
				userid, timestamp, buf, ad);
		db->exec_update_sql(sqlstr, &count);

		if (count == 0)
			continue;

		//提交数据
		if (mysql_commit(&(db->handle))!=DB_SUCC){
			db->show_error_log("db:COMMIT:err");
		}

		map<string, int> msg_map;
		memcpy(buf, msg.c_str(), msg.size());


		char outbuf[4096] = {};
		segment(buf, strlen(buf), seg, msg_map, outbuf);
		//segment_only_word(buf, strlen(buf), msg_map, outbuf);

		map<string, int>::iterator it = msg_map.begin();
		for (; it != msg_map.end(); it ++) {
			//cout << it->first << "\t" << it->second << endl;
			uint32_t non_ad_cnt = ad ? 0 : it->second;
			uint32_t ad_cnt = ad ? it->second : 0;
			sprintf(sqlstr, "insert into CHAT_STAT.t_char_10 values(\"%s\", %u, %u, 0.0) on duplicate key update non_ad=non_ad+%u, ad=ad+%u",
					it->first.c_str(), non_ad_cnt, ad_cnt, non_ad_cnt, ad_cnt);
			db->exec_update_sql(sqlstr, &count);
			//提交数据
			if (mysql_commit(&(db->handle))!=DB_SUCC){
				db->show_error_log("db:COMMIT:err");
			}

		}	
	}
	return 0;
}

int mmseg_word(mysql_interface* db)
{
	// 初始化分词系统
    SegmenterManager mgr;
    mgr.init("../data/");
    Segmenter* seg = mgr.getSegmenter();

	fstream fstr;
	fstr.open("../data/seer2", fstream::in);
	if (!fstr.is_open())
		return -1;

	string msg;
	uint64_t msgid;
	int ad, count;

	CTrimAbnormal trim;
	//trim.init_del_word("../data/del_word");

	char sqlstr[1024];

	while (!fstr.eof()) {
		fstr >> msg >> ad;
		char inbuf[4096] = {}, outbuf[4096] = {};
		memcpy(inbuf, msg.c_str(), msg.size());
		trim.trim_abnormal_utf8(inbuf);

		if (strlen(inbuf) == 0)
			continue;

		msgid = BKDRHash(inbuf);

		sprintf(sqlstr, "select ad from CHAT_STAT.t_wmsg_10 where msgid=%lu", msgid);
		int old_ad = -1;
		STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
			INT_CPY_NEXT_FIELD(old_ad);
		STD_QUERY_ONE_END_WITHOUT_RETURN();

		cout << msgid << "\t" << msg << "\t" << ad << old_ad << endl;

		map<string, int> msg_map;
		segment(inbuf, strlen(inbuf), seg, msg_map, outbuf);
		//segment_only_word(inbuf, strlen(inbuf), msg_map, outbuf);

		if (old_ad == -1) {
			sprintf(sqlstr, "insert into CHAT_STAT.t_wmsg_10 values(%lu, \"%s\", %u)", msgid, outbuf, ad);
		} else if ((old_ad == ad) || (IS_MANUAL(old_ad) && !IS_MANUAL(ad))
				|| (!IS_MANUAL(old_ad) && !IS_MANUAL(ad) && !IS_AD(old_ad))) {
			continue;
		} else {
			sprintf(sqlstr, "update CHAT_STAT.t_wmsg_10 set ad = %d where msgid=%lu", ad, msgid);
		}

		db->exec_update_sql(sqlstr, &count);

		if (old_ad== -1 || IS_AD(ad) != IS_AD(old_ad)) {
			map<string, int>::iterator it = msg_map.begin();
			for (; it != msg_map.end(); it ++) {
				int non_ad_cnt = IS_AD(ad) ? (old_ad == -1 ? 0 : -1) : 1;
				int ad_cnt = IS_AD(ad) ? 1 : (old_ad == -1 ? 0 : -1);
				sprintf(sqlstr, "insert into CHAT_STAT.t_char_10 values(\"%s\", %u, %u, 0.0) "
						"on duplicate key update non_ad=non_ad+%d, ad=ad+%d",
						it->first.c_str(), non_ad_cnt, ad_cnt, non_ad_cnt, ad_cnt);
				db->exec_update_sql(sqlstr, &count);

			}	
		}
		//提交数据
		if (mysql_commit(&(db->handle))!=DB_SUCC){
			db->show_error_log("db:COMMIT:err");
		}
	}
	return 0;
}

int mutual_word(mysql_interface* db)
{
	char sqlstr[1024];
	uint32_t msg_ad_cnt = 0, msg_nonad_cnt = 0;

	sprintf(sqlstr, "select COUNT(*) from CHAT_STAT.t_wmsg_10 where ad&0x1=0;");
	STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
		INT_CPY_NEXT_FIELD(msg_nonad_cnt);
	STD_QUERY_ONE_END_WITHOUT_RETURN();


	sprintf(sqlstr, "select COUNT(*) from CHAT_STAT.t_wmsg_10 where ad&0x1=1;");
	STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
		INT_CPY_NEXT_FIELD(msg_ad_cnt);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	double msg_cnt = msg_ad_cnt + msg_nonad_cnt; 

	double p_msg_ad = double (msg_ad_cnt) / msg_cnt;
	double p_msg_nonad = double (msg_nonad_cnt) / msg_cnt;

	CTrimAbnormal trim;
	trim.init_del_word("../data/del_word");

	int index = 0;
	while (true) {
		char word[64] ={};
		double word_ad_cnt, word_nonad_cnt;
		sprintf(sqlstr, "select word, non_ad, ad from CHAT_STAT.t_char_10 limit %d, 1;", index);
		STD_QUERY_ONE_BEGIN_CHAT(sqlstr);
			BIN_CPY_NEXT_FIELD(word, 64);
			INT_CPY_NEXT_FIELD(word_nonad_cnt);
			INT_CPY_NEXT_FIELD(word_ad_cnt);
		STD_QUERY_ONE_END_WITHOUT_RETURN();

		if (trim.is_del_word(word)) {
			index ++;
			continue;
		}

		//if (word_nonad_cnt == 0) word_nonad_cnt = 1;
		if (word_ad_cnt < 0.01) word_ad_cnt = 0.01;
		if (word_nonad_cnt < 0.01) word_nonad_cnt = 0.01;

		if ((word_nonad_cnt < msg_nonad_cnt / 1000.0 && word_ad_cnt < msg_ad_cnt / 100.0)
			|| (word_nonad_cnt > msg_nonad_cnt / 50.0 && word_ad_cnt < msg_ad_cnt / 1000.0) ){
			index ++;
			continue;
		}

		double word_cnt = word_ad_cnt + word_nonad_cnt;
		double p_word_ad = word_ad_cnt / word_cnt;
		double p_word_nonad = word_nonad_cnt / word_cnt;

		double p_msg_ad_word_ad = word_ad_cnt / msg_cnt;
		double p_msg_ad_word_nonad = p_msg_ad - p_msg_ad_word_ad; 
		double p_msg_nonad_word_nonad = word_nonad_cnt / msg_cnt;
		double p_msg_nonad_word_ad = p_msg_nonad - p_msg_nonad_word_nonad;

		double mutual = p_msg_ad_word_ad * log(p_msg_ad_word_ad / (p_msg_ad * p_word_ad))
			+ p_msg_ad_word_nonad * log(p_msg_ad_word_nonad / (p_msg_ad * p_word_nonad))
			+ p_msg_nonad_word_ad * log(p_msg_nonad_word_ad / (p_msg_nonad * p_word_ad))
			+ p_msg_nonad_word_nonad * log(p_msg_nonad_word_nonad / (p_msg_nonad * p_word_nonad));

		cout << string(word) << "\t" << word_ad_cnt << "\t" << word_nonad_cnt << "\t" << mutual << endl;
		index ++;
	}

	cout << msg_ad_cnt << endl;
	cout << msg_nonad_cnt << endl;
	return 0;	
}

int bayes_test(mysql_interface* db)
{
	unsigned long srch,str;
	str = currentTimeMillis();

	Cbayes bayes;
	bayes.train(db);

	srch = currentTimeMillis() - str;
	cout << "训练时间：\t" << srch << "ms" << endl;

	str = currentTimeMillis();

    SegmenterManager mgr;
    mgr.init("../data/");
    Segmenter* seg = mgr.getSegmenter();

	fstream fstr;
	fstr.open("../data/umsg", fstream::in);
	if (!fstr.is_open())
		return -1;

	string msg;
	int ad;

	uint32_t all_count = 0, wrong_count = 0;

	while (!fstr.eof()) {
		fstr >> msg >> ad;
		map<string, int> word_map;
		char inbuf[4096] = {}, outbuf[4096];
		memcpy(inbuf, msg.c_str(), msg.size()); 
		segment(inbuf, strlen(inbuf), seg, word_map, outbuf);
		//segment_only_word(inbuf, strlen(inbuf), word_map, outbuf);

		all_count ++;
		ad = !IS_AD(ad);

		//int class_ad = bayes.classify(word_list);
		int class_ad = bayes.classify(word_map);
		if (ad != class_ad) {
			wrong_count ++;
		}

		cout << msg << "\t" << class_ad << "\t" << ad << endl;
	}

	cout << all_count << "\t" << wrong_count << endl;
	srch = currentTimeMillis() - str;
	cout << "分类时间：\t" << srch << "ms" << endl;

	return 0;
}

int main(int argc, char *argv[])
{
	// 初始化日志 
	log_init("../log", log_lvl_trace, 1 << 30, 100, "mmseg");


	// 初始化数据库
	mysql_interface* db = init_db();

	if (argc <= 1) {                        /* if no argument given */
	  printf("usage: %s [options]"
	                   "[-f file|-t]\n", argv[0]);
	  printf("%s\n", DESCRIPTION);
	  printf("%s\n", VERSION);
	  printf("-f       induce a full Bayes classifier "
	                  "(default: naive Bayes)\n");
	  printf("-t       distribute tuple weight for null values\n");
	  return 0;                   /* print a usage message */
	}                             /* and abort the program */

	/* --- evaluate arguments --- */
	for (int i = 1; i < argc; i++) {  /* traverse arguments */
	  char* s = argv[i];                /* get option argument */
	  int k = 0;
	  if ((*s == '-') && *++s) {  /* -- if argument is an option */
	    while (1) {               /* traverse characters */
	      switch (*s++) {         /* evaluate option */
	        case 'f': return mmseg_chat(db);
			case 't': return mutual_word(db);
			case 'm': return mmseg_word(db);
			case 'b': return bayes_test(db);
	      }                       /* set option variables */
	      if (!*s) break;         /* if at end of string, abort loop */
	    } }                       /* get option argument */
	  else {                      /* -- if argument is no option */
	    switch (k++) {            /* evaluate non-option */
	      case  0: break;
	    }                         /* note filenames */
	  }
	}


	delete db;

    return 0;
}
