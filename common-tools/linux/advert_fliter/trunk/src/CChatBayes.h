#ifndef CCHATBAYES_H_
#define CCHATBAYES_H_

#include <map>
#include <set>
#include <string>
#include <stdint.h>
#include <mmseg/bsd_getopt.h>
#include <mmseg/SegmenterManager.h>
#include <mmseg/csr_utils.h>
using namespace css;

enum bayes_attr_type {
	bayes_attr_type_bool,	// 布尔型特征值（如关键字是否存在）
	bayes_attr_type_int,	// 整型特征值
	bayes_attr_type_range,	// 连续区间特征值
};

class Cbayes_stat {
public:
	uint32_t total_count;
	std::map<int, uint32_t> stat_map;
	std::map<int, double> stat_prob_map;

public:
	// 只作用于bool type
	double value_probability(int value);
};

class Cbayes_trainer {

	// 不同类的特征值统计 
	std::map<int, Cbayes_stat> bayes_stat_map;
public:
	bayes_attr_type	attr_type;

	virtual void train(int classid, uint32_t word_cnt, uint32_t class_cnt);
	double conditional_probability(int class_id, double sample_count, int value);
	void correct_conditional_prob(int class_id, double& bayes_class_prob);
};

struct Cbayes_class {
	uint32_t		sample_count;	// 样本数量
	double			false_prob;		// 所有布尔型属性不出现时的概率
};

class Cbayes {
	// 统计类与样本数量的对应关系
	std::map<int, Cbayes_class> bayes_class_map;
	// 不同特征值的学习器
	std::map<int, Cbayes_trainer*> bayes_trainer_map;

	std::map<uint64_t, int> bayes_cache_map;
	SegmenterManager seg_mgr;
    Segmenter* seg;

	char	seg_buff[4096];

private:
	void segment(const char* inbuf, int len, std::set<std::string>& word_list);
public:
	int init_segment(const char* dir);
	int train(const char* file);
	int train();
	int classify(uint64_t msgid, const char* msg, int length);
	int init_bayes_cache(const char* file);
	~Cbayes();
};

#endif
