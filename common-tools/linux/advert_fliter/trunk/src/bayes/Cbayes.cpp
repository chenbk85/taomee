#include <Cbayes.h>
#include <fstream>
#include <iostream>
using namespace std;

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

double Cbayes_trainer::conditional_probability(int class_id, double sample_count, int value)
{
	map<int, Cbayes_stat>::iterator it = this->bayes_stat_map.find(class_id);
	if (it == this->bayes_stat_map.end())
		return 0.0;

	return it->second.value_probability(sample_count, value, this->attr_type);
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
	}

	return ret_class;
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
