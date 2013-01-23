#include "physical_engine.hpp"
extern "C"
{
#include <libtaomee/tlog/tlog.h>	
}
using namespace taomee;

#define E   (2.718281828)

double angle2radian(double value)
{
	return value * PI / 180.0;	
}
 
double radian2angle(double value)
{
	return value * 180.0 / PI;
}


bool phy_container::add_phy_model(phy_model* p_model)
{
	if(p_model == NULL)return false;
	std::list<phy_model*>::iterator pItr = find(models.begin(),  models.end(), p_model);
	if(pItr != models.end())return false;
	models.push_back(p_model);
	return true;
}


bool phy_container::del_phy_model(phy_model* p_model)
{
	if(p_model == NULL)return false;
	std::list<phy_model*>::iterator pItr = std::find(models.begin(),  models.end(), p_model);
	if(pItr == models.end())return false;
	models.erase(pItr);
	return true;
}

void phy_container::clear_phy_models()
{
	models.clear();	
}

float phy_container::calc_x_by_time(phy_model* p_model, uint32_t millisecond)
{
	double b = 0.05;
	double m = 2.0;

	double second = (float)millisecond/ 33.0;
	//double v_a = vertical_acceleration_ + p_model->vertical_acceleration_;
	double coeff = 1 - pow(E,  (0.0 - b * second / m));

	double txl = m* p_model->horizontal_speed_ / b;
	//double tyl = pow(m, 2) / v_a / pow(b, 2) + m* p_model->vertical_speed_ / b;

	return p_model->init_x_ + txl * coeff;
}

float phy_container::calc_y_by_time(phy_model* p_model, uint32_t millisecond)
{
	double b = 0.05;
	double m = 2.0;

	double second = (float)millisecond/ 33.0;
	double v_a = vertical_acceleration_ + p_model->vertical_acceleration_;
	double coeff = 1 - pow(E,  (0.0 - b * second / m));

	//double txl = m* p_model->horizontal_speed_ / b;
	double tyl = pow(m, 2) / v_a / pow(b, 2) + m* p_model->vertical_speed_ / b;
	return p_model->init_y_ - (tyl * coeff - m * v_a * second/ b);
}


void phy_container::simulate_models(uint32_t millisecond, phy_model* p_model)
{
	// 阻尼系数
	double b = 0.05;
	// 质量
	double m = 2.0;

	p_model->add_simulate_time(millisecond);

	double second = (float)(p_model->get_simulate_time()) / 33.0;

	ERROR_TLOG("----------------millisecond = %u, second = %f--------------", millisecond, second);

	//垂直加速度，上负下正
	double v_a = vertical_acceleration_ + p_model->vertical_acceleration_;
	//水平加速度，左负右正 
	// double h_a = horizontal_acceleration_ + p_model->horizontal_acceleration_;

    double coeff = 1 - pow(E,  (0.0 - b * second / m));
    double txl = m* p_model->horizontal_speed_ / b;
	double tyl = pow(m, 2) / v_a / pow(b, 2) + m* p_model->vertical_speed_ / b;

	p_model->old_x_ = p_model->x_;
	p_model->old_y_ = p_model->y_;
	p_model->x_ = p_model->init_x_ + txl * coeff;
	p_model->y_ = p_model->init_y_ - (tyl * coeff - m * v_a * second/ b);
	
	//ERROR_TLOG("--------coeff = %f, txl = %f, tyl = %f v_a = %f------------", coeff, txl, tyl, v_a);
}

void phy_container::simulate(uint32_t millisecond)
{
	std::list<phy_model*>::iterator pItr = models.begin();
	for(;  pItr != models.end(); ++pItr)
	{
		phy_model* p_model = *pItr;
		if(!p_model->check_simulate())continue;
		simulate_models(millisecond, p_model);
	}
}
