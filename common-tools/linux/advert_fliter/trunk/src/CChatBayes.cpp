#include "CChatBayes.h"
#include <fstream>
#include <iostream>
#include <libtaomee/log.h>
#include "CChatString.h"
using namespace std;

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

void Cbayes_trainer::correct_conditional_prob(int class_id, double& bayes_class_prob)
{
	map<int, Cbayes_stat>::iterator it = this->bayes_stat_map.find(class_id);
	if (it != this->bayes_stat_map.end()) {
		Cbayes_stat& stat = it->second;
		bayes_class_prob /= stat.stat_prob_map[0];
		bayes_class_prob *= stat.stat_prob_map[1];
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
	this->bayes_stat_map.insert(pair<int, Cbayes_stat>(classid, stat));
}
double Cbayes_trainer::conditional_probability(int class_id, double sample_count, int value)
{
	map<int, Cbayes_stat>::iterator it = this->bayes_stat_map.find(class_id);
	if (it == this->bayes_stat_map.end())
		return 0.0;

	return it->second.value_probability(value);
	//return it->second.value_probability(sample_count, value, this->attr_type);
}
		
int Cbayes::classify(uint64_t msgid, const char* msg, int len)
{
	map<uint64_t, int>::iterator it_cache = this->bayes_cache_map.find(msgid);
	if (it_cache != this->bayes_cache_map.end()) {
		TRACE_LOG("CACHE CLASS\t[%lu %d]", msgid, it_cache->second);
		return it_cache->second;
	}

	set<string> word_list;
	this->segment(msg, len, word_list);

	int ret_class = -1;
	double max_probability = 0.0;
	map<int, Cbayes_class>::iterator it = this->bayes_class_map.begin();

	for (; it != this->bayes_class_map.end(); it ++) {
		Cbayes_class& bayes_class = it->second;
		double probability = bayes_class.false_prob * bayes_class.sample_count;
		for (set<string>::iterator it_word = word_list.begin(); it_word != word_list.end(); it_word++) {
			int wordid = BKDRHash32(it_word->c_str());
			map<int, Cbayes_trainer*>::iterator it_trainer = this->bayes_trainer_map.find(wordid);
			if (it_trainer != this->bayes_trainer_map.end()) {
				Cbayes_trainer* p_trainer = it_trainer->second;
				p_trainer->correct_conditional_prob(it->first, probability);
			}
		}
		
		//INFO_LOG("PROB\t[%f %f %d]", probability, max_probability, it->first);
		if (probability > max_probability) {
			ret_class = it->first;
			max_probability = probability;
		}
	}

	if (this->bayes_cache_map.size() > 1024 * 1024) {
		this->bayes_cache_map.erase(this->bayes_cache_map.begin());
	}
	this->bayes_cache_map.insert(pair<uint64_t, int>(msgid, ret_class));

	return ret_class;
}

Cbayes::~Cbayes()
{
	map<int, Cbayes_trainer*>::iterator it = this->bayes_trainer_map.begin();
	for (; it != this->bayes_trainer_map.end(); it ++) {
		delete it->second;
	}
}

int Cbayes::init_bayes_cache(const char* file)
{
	// 打开统计文件
	fstream fstr;
	fstr.open(file, fstream::in);
	if (!fstr.is_open())
		return -1;

	while (!fstr.eof()) {
		string msg;
	   	int bayes_class;
		fstr >> msg >> bayes_class;
		if (bayes_class <= 1) {
			continue;
		}

		bayes_class %= 2;
		uint64_t msgid = BKDRHash(msg.c_str());
		this->bayes_cache_map.insert(pair<uint64_t, int>(msgid, !bayes_class));
	}
	return 0;
}

// 统计数据保存在文件中
int Cbayes::train(const char* file)
{
	// 打开统计文件
	fstream fstr;
	fstr.open(file, fstream::in);
	if (!fstr.is_open())
		return -1;

	// 读取样本数量
	uint32_t msg_ad_cnt = 0, msg_nonad_cnt = 0;
	fstr >> msg_ad_cnt >> msg_nonad_cnt;

	string word;
	double word_ad_cnt = 0, word_nonad_cnt = 0;

	// 计算所有布尔属性不出现的概率
	// 对于属性很多但出现的属性很少时能大大提高计算效率
	double false_prob_ad = 1.0;
	double false_prob_nonad = 1.0;

	while (!fstr.eof()) {
		// 读取属性,并进行初始化
		fstr >> word >> word_ad_cnt >> word_nonad_cnt;

		Cbayes_trainer* p_trainer = new Cbayes_trainer();
		p_trainer->attr_type = bayes_attr_type_bool;	
		uint32_t wordid = BKDRHash32(word.c_str());
		this->bayes_trainer_map.insert(pair<int, Cbayes_trainer*>(wordid, p_trainer));
		p_trainer->train(0, word_ad_cnt, msg_ad_cnt);
		false_prob_ad *= double(msg_ad_cnt - word_ad_cnt) / double (msg_ad_cnt);
		p_trainer->train(1, word_nonad_cnt, msg_nonad_cnt);
		false_prob_nonad *= double(msg_nonad_cnt - word_nonad_cnt) / double (msg_nonad_cnt);
	}

	// 初始化广告样本类
	Cbayes_class bayes_class;
	bayes_class.sample_count = msg_ad_cnt;
	bayes_class.false_prob = false_prob_ad;
	this->bayes_class_map.insert(pair<int, Cbayes_class>(0, bayes_class));
	bayes_class.sample_count = msg_nonad_cnt;
	bayes_class.false_prob = false_prob_nonad;
	this->bayes_class_map.insert(pair<int, Cbayes_class>(1, bayes_class));

	return 0;
}

// 统计数据保存在数据库中
int Cbayes::train()
{
	//string word;
	//double word_ad_cnt = 0, word_nonad_cnt = 0;

	// 计算所有布尔属性不出现的概率
	// 对于属性很多但出现的属性很少时能大大提高计算效率
	//double false_prob_ad = 1.0;
	//double false_prob_nonad = 1.0;

	//while (!fstr.eof()) {
		// 读取属性,并进行初始化
		/*
		fstr >> word >> word_ad_cnt >> word_nonad_cnt;

		Cbayes_trainer* p_trainer = new Cbayes_trainer();
		p_trainer->attr_type = bayes_attr_type_bool;	
		uint32_t wordid = BKDRHash32(word.c_str());
		this->bayes_trainer_map.insert(pair<int, Cbayes_trainer*>(wordid, p_trainer));
		p_trainer->train(0, word_ad_cnt, msg_ad_cnt);
		false_prob_ad *= double(msg_ad_cnt - word_ad_cnt) / double (msg_ad_cnt);
		p_trainer->train(1, word_nonad_cnt, msg_nonad_cnt);
		false_prob_nonad *= double(msg_nonad_cnt - word_nonad_cnt) / double (msg_nonad_cnt);
		*/
	//}

	// 初始化广告样本类
	/*
	Cbayes_class bayes_class;
	bayes_class.sample_count = msg_ad_cnt;
	bayes_class.false_prob = false_prob_ad;
	this->bayes_class_map.insert(pair<int, Cbayes_class>(0, bayes_class));
	bayes_class.sample_count = msg_nonad_cnt;
	bayes_class.false_prob = false_prob_nonad;
	this->bayes_class_map.insert(pair<int, Cbayes_class>(1, bayes_class));
	*/

	return 0;
}

void Cbayes::segment(const char* inbuf, int length, std::set<std::string>& word_list)
{
	memset(this->seg_buff, 0, sizeof(this->seg_buff));
	memcpy(this->seg_buff, inbuf, length);
    seg->setBuffer((u1*)this->seg_buff, length);
    u2 len = 0, symlen = 0;
    u2 kwlen = 0, kwsymlen = 0;
    //check 1st token.
    char txtHead[3] = {239,187,191};
    char* tok = (char*)seg->peekToken(len, symlen);
    seg->popToken(len);
    if(seg->isSentenceEnd()){
        do {
            char* kwtok = (char*)seg->peekToken(kwlen , kwsymlen,1);
            if(kwsymlen) {
				word_list.insert(string(kwtok, kwsymlen));
			}
        }while(kwsymlen);
    }

    if(len == 3 && memcmp(tok,txtHead,sizeof(char)*3) == 0){
    }else{
		word_list.insert(string(tok, symlen));
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
					word_list.insert(string(kwtok, kwsymlen));
				}
            }while(kwsymlen);
        }

        if(*tok == '\r' || *tok == '\n')
            continue;

		word_list.insert(string(tok, symlen));
        {
            const char* thesaurus_ptr = seg->thesaurus(tok, symlen);
            while(thesaurus_ptr && *thesaurus_ptr) {
                len = strlen(thesaurus_ptr);
				word_list.insert(string(thesaurus_ptr, len));
                thesaurus_ptr += len + 1; //move next
            }
        }
    }
}

int Cbayes::init_segment(const char* dir)
{
    seg_mgr.init(dir);
    seg = seg_mgr.getSegmenter();
	return 0;
}
