#include "Cbayes.h"
#include <fstream>
#include <iostream>
#include <dbser/db_macro.h>
using namespace std;
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

static inline uint32_t BKDRHash32(const char *str)
{       
    uint64_t seed = 131; // 31 131 1313 13131 131313 etc..
    uint64_t hash = 0;
        
    while (*str)
    {   
        hash = hash * seed + (*str++);
    }   
            
    return (hash & 0x7FFFFFFFl);
}  

double Cbayes_stat::value_probability(int value)
{
	if (value != 0 && value != 1)
		return 0.0;
	return this->stat_prob_map[value];
}

double Cbayes_stat::value_probability(double sample_count, int value, bayes_attr_type attr_type)
{
	if (attr_type == bayes_attr_type_bool) {
		// 布尔型的非真即假，非法值概率为0
		map<int, uint32_t>::iterator it = this->stat_map.find(value);
		if (it == this->stat_map.end())
			return 0.0;
		return it->second / sample_count;
	} else if (attr_type == bayes_attr_type_int) {
		// 整型值未出现在统计中的归为一类
		map<int, uint32_t>::iterator it = this->stat_map.find(value);
		if (it == this->stat_map.end())
			return 1.0 - this->total_count / sample_count;

		return it->second / sample_count;
	} else if (attr_type == bayes_attr_type_range) {
		map<int, uint32_t>::iterator it = this->stat_map.begin();
		map<int, uint32_t>::reverse_iterator it_end = this->stat_map.rbegin();
		if (it->first > value || it_end->first < value)
			return 1.0 - this->total_count / sample_count;

		uint32_t count = it->second;
		for (it ++; it != this->stat_map.end(); it ++) {
			if (it->first > value)
				break;
			count = it->second;
		}

		return count / sample_count;
	}

	return 0.0;
}

void Cbayes_trainer::correct_conditional_prob(int class_id, double& bayes_class_prob)
{
	map<int, Cbayes_stat>::iterator it = this->bayes_stat_map.find(class_id);
	if (it != this->bayes_stat_map.end()) {
		Cbayes_stat& stat = it->second;
		bayes_class_prob /= stat.stat_prob_map[0];
		bayes_class_prob *= stat.stat_prob_map[1];
	}
}

double Cbayes_trainer::conditional_probability(int class_id, double sample_count, int value)
{
	map<int, Cbayes_stat>::iterator it = this->bayes_stat_map.find(class_id);
	if (it == this->bayes_stat_map.end())
		return 0.0;

	return it->second.value_probability(value);
	//return it->second.value_probability(sample_count, value, this->attr_type);
}

void Cbayes::train()
{
	std::map<int, Cbayes_trainer*>::iterator it = this->bayes_trainer_map.begin();
	for (; it != this->bayes_trainer_map.end(); it ++) {
		Cbayes_trainer* p_trainer = it->second;
		p_trainer->train();
	}
}

bool Cbayes::read_attribute(const char* attr_file)
{
	fstream fstr;
	fstr.open(attr_file, fstream::in);
	if (!fstr.is_open())
		return false;

	string flag, type;

	for (;;) {
		fstr >> flag;
		if (flag != "@attribute")
			break;

		Cbayes_trainer* p_trainer = new Cbayes_trainer();

		fstr >> type;
		if (type == "int") {
			p_trainer->attr_type = bayes_attr_type_int;
		} else if (type == "range") {
			p_trainer->attr_type = bayes_attr_type_range;
		} else {
			p_trainer->attr_type = bayes_attr_type_bool;
		}
	}

	fstr.close();

	return true;	
}

void Cbayes::init()
{
	this->read_attribute("../data/attribute");
}

int Cbayes::classify(std::map<int, int>& attr_list)
{
	int ret_class = -1;
	double max_probability = 0.0;
	map<int, uint32_t>::iterator it = this->bayes_class_map.begin();

	for (; it != this->bayes_class_map.end(); it ++) {
		double probability = this->class_probability(it->first, it->second, attr_list);
		if (probability > max_probability) {
			ret_class = it->first;
			max_probability = probability;
		}
		//cout << "prob\t" << ret_class << "\t" << probability << endl;
	}

	return ret_class;
}

int Cbayes::classify(map<string, int>& word_list)
{
	int ret_class = -1;
	double max_probability = 0.0;
	map<int, uint32_t>::iterator it = this->bayes_class_map.begin();

	for (; it != this->bayes_class_map.end(); it ++) {
		double probability = this->bayes_false_prob_map[it->first] * it->second;
		map<string, int>::iterator it_word = word_list.begin();
		for (; it_word != word_list.end(); it_word ++) {
			int wordid = BKDRHash32(it_word->first.c_str());
			map<int, Cbayes_trainer*>::iterator it_trainer = this->bayes_trainer_map.find(wordid);
			if (it_trainer != this->bayes_trainer_map.end()) {
				Cbayes_trainer* p_trainer = it_trainer->second;
				p_trainer->correct_conditional_prob(it->first, probability);
			}
		}

		if (probability >= max_probability) {
			ret_class = it->first;
			max_probability = probability;
		}
		//cout << "prob\t" << ret_class << "\t" << probability << endl;
	}

	return ret_class;
}

int Cbayes::classify(vector<string>& word_list)
{
	map<int, int> attr_list;
	for (uint32_t i = 0; i < word_list.size(); i ++) {
		uint32_t wordid = BKDRHash32(word_list[i].c_str());
		attr_list.insert(pair<int, int>(wordid, 1));
	}


	map<int, Cbayes_trainer*>::iterator it = this->bayes_trainer_map.begin();
	for (; it != this->bayes_trainer_map.end(); it ++) {
		attr_list.insert(pair<int, int>(it->first, 0));
	}

	return this->classify(attr_list);
}

double Cbayes::class_probability(int class_id, uint32_t sample_count, map<int, int>& attr_list)
{
	double probability = 1.0;
	map<int, Cbayes_trainer*>::iterator it = this->bayes_trainer_map.begin();
	for (; it != this->bayes_trainer_map.end(); it ++) {
		map<int, int>::iterator it_attr = attr_list.find(it->first);
		if (it_attr == attr_list.end()) {
			cout << "attr miss\t" << it->first << endl;
			return 0.0;
		}

		Cbayes_trainer* p_trainer = it->second;
		probability *= p_trainer->conditional_probability(class_id, sample_count, it_attr->second);
	}
	return probability;
}

Cbayes::~Cbayes()
{
	map<int, Cbayes_trainer*>::iterator it = this->bayes_trainer_map.begin();
	for (; it != this->bayes_trainer_map.end(); it ++) {
		delete it->second;
	}

}

void Cbayes_trainer::train(int classid, uint32_t word_cnt, uint32_t class_cnt)
{
	Cbayes_stat stat;
	stat.total_count = class_cnt;
	stat.stat_map.insert(pair<int, uint32_t>(1, word_cnt));	
	stat.stat_map.insert(pair<int, uint32_t>(0, class_cnt - word_cnt));	

	double true_prob = (double)word_cnt / (double)class_cnt;
	double false_prob = 1 - true_prob;
	stat.stat_prob_map.insert(pair<int, double>(1, true_prob));	
	stat.stat_prob_map.insert(pair<int, double>(0, false_prob));	
	//cout << "类" << classid << "\t" << word_cnt << "\t" << class_cnt << endl;
	this->bayes_stat_map.insert(pair<int, Cbayes_stat>(classid, stat));
}

int Cbayes::train(mysql_interface* db)
{
	char sqlstr[1024];
	uint32_t msg_ad_cnt = 0, msg_nonad_cnt = 0;

	sprintf(sqlstr, "select COUNT(*) from CHAT_STAT.t_umsg where ad=0;");
	STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
		INT_CPY_NEXT_FIELD(msg_nonad_cnt);
	STD_QUERY_ONE_END_WITHOUT_RETURN();


	sprintf(sqlstr, "select COUNT(*) from CHAT_STAT.t_umsg where ad=1;");
	STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
		INT_CPY_NEXT_FIELD(msg_ad_cnt);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	this->bayes_class_map.insert(pair<int, uint32_t>(0, msg_ad_cnt));
	this->bayes_class_map.insert(pair<int, uint32_t>(1, msg_nonad_cnt));

	fstream fstr;
	fstr.open("../data/bayes_word", fstream::in);
	if (!fstr.is_open())
		return -1;
	string word;

	double false_prob_ad = 1.0;
	double false_prob_nonad = 1.0;

	while (!fstr.eof()) {
		fstr >> word;
		sprintf(sqlstr, "select ad,non_ad from CHAT_STAT.t_word where word=\"%s\";", word.c_str());
		uint32_t word_ad_cnt = 0, word_nonad_cnt = 0;
		STD_QUERY_ONE_BEGIN_CHAT_EX(sqlstr);
			INT_CPY_NEXT_FIELD(word_ad_cnt);
			INT_CPY_NEXT_FIELD(word_nonad_cnt);
		STD_QUERY_ONE_END_WITHOUT_RETURN();

		//cout << word << "\t" << word_ad_cnt << "\t" << word_nonad_cnt << endl;

		//if (word_ad_cnt == 0 && word_nonad_cnt == 0)
		//	continue;
		
		if (word_ad_cnt < 0.01) word_ad_cnt = 0.01;
		if (word_nonad_cnt < 0.01) word_nonad_cnt = 0.01;

		if ((word_nonad_cnt < msg_nonad_cnt / 1000.0 && word_ad_cnt < msg_ad_cnt / 100.0)
			|| (word_nonad_cnt > msg_nonad_cnt / 50.0 && word_ad_cnt < msg_ad_cnt / 1000.0) ){
			continue;
		}

		Cbayes_trainer* p_trainer = new Cbayes_trainer();
		p_trainer->attr_type = bayes_attr_type_bool;	
		uint32_t wordid = BKDRHash32(word.c_str());
		this->bayes_trainer_map.insert(pair<int, Cbayes_trainer*>(wordid, p_trainer));
		p_trainer->train(0, word_ad_cnt, msg_ad_cnt);
		false_prob_ad *= double(msg_ad_cnt - word_ad_cnt) / double (msg_ad_cnt);
		p_trainer->train(1, word_nonad_cnt, msg_nonad_cnt);
		false_prob_nonad *= double(msg_nonad_cnt - word_nonad_cnt) / double (msg_nonad_cnt);
	}

	this->bayes_false_prob_map.insert(pair<int, double>(0, false_prob_ad));
	this->bayes_false_prob_map.insert(pair<int, double>(1, false_prob_nonad));
	cout << "false prob\t" << false_prob_ad << "\t" << false_prob_nonad << "\t" << msg_ad_cnt << "\t" << msg_nonad_cnt << endl;
	return 0;
}
