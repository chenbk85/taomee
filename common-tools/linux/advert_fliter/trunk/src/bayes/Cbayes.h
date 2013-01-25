#ifndef CBAYES_H_
#define CBAYES_H_

#include <map>
#include <string>
#include <stdint.h>

enum bayes_attr_type {
	bayes_attr_type_bool,	// 布尔型特征值（如关键字是否存在）
	bayes_attr_type_int,	// 整型特征值
	bayes_attr_type_range,	// 连续区间特征值
};

class Cbayes_stat {
	uint32_t total_count;
	std::map<int, uint32_t> stat_map;

public:
	double value_probability(double sample_count, int value, bayes_attr_type attr_type);
};

class Cbayes_trainer {

	// 不同类的特征值统计 
	std::map<int, Cbayes_stat> bayes_stat_map;
public:
	bayes_attr_type	attr_type;

	virtual void train() { }
	double conditional_probability(int class_id, double sample_count, int value);
};


class Cbayes {
	// 统计类与样本数量的对应关系
	std::map<int, uint32_t> bayes_class_map;
	// 不同特征值的学习器
	std::map<int, Cbayes_trainer*> bayes_trainer_map;
	// 计算某个统计类的概率
	double class_probability(int class_id, uint32_t sample_count, std::map<int, int>& attr_list);
public:
	bool read_attribute(const char * attr_file);
	void train();
	int classify(std::map<int, int>& attr_list);
	void init();
	~Cbayes();
};

#endif
