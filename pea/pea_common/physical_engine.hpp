#ifndef PHYSICAL_ENGINE_HPP_
#define PHYSICAL_ENGINE_HPP_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <algorithm> 
#include "base_timer.hpp"
using namespace std;

#define PI (3.1415926)


typedef struct pos
{
	pos()
	{
		memset(this, 0, sizeof(pos));	
	}
	uint32_t x;
	uint32_t y;
	uint32_t dir;
}pos;

enum dir_enum 
{
	dir_left  = 0,	
	dir_right = 1
};

typedef struct  phy_model
{
	
	phy_model()
	{
		x_ 						 = 0.0;
		y_ 						 = 0.0;
		init_x_                   = 0.0;
		init_y_                   = 0.0;
		old_x_                   = 0.0;
		old_y_                   = 0.0;
		horizontal_speed_ 		 = 0.0;
		vertical_speed_   		 = 0.0;
		horizontal_acceleration_ = 0.0;
		vertical_acceleration_   = 0.0;
		dir_                     = dir_left;
		dir_angle_               = 0.0;
		simulate_time            = 0.0;
		simulate_flag            = 0;
	}

	uint32_t distance(phy_model* p_model)
	{
		if(p_model == this )return 0;
		return sqrt((x_ - p_model->x_)*(x_ - p_model->x_) + (y_ - p_model->y_)*(y_ - p_model->y_));
	}

	bool collision(phy_model* p_model)
	{	
		return  ( (uint32_t)abs( (int) (x_ - p_model->x_) ) < ((width_ + p_model->width_)/2) && 
			(uint32_t)abs( (int)(y_ - p_model->y_)) < (( height_ + p_model->height_)/2) ); 
	}

	bool in_area(phy_model* p_model)
	{
		return (uint32_t)abs( (int) (x_ - p_model->x_) ) < (p_model->width_)/2 &&
		(uint32_t)abs( (int)(y_ - p_model->y_)) < ((p_model->height_)/2);
	}

	void set_rectangle(uint32_t width,  uint32_t height)
	{
		width_  = width;
		height_ = height; 
	}
	void set_speed(double x_speed, double y_speed)
	{
		horizontal_speed_ = x_speed;
		vertical_speed_   = y_speed;
	}

	void add_simulate_time(uint32_t time)
	{
		simulate_time += time;
	}
	uint32_t get_simulate_time()
	{
		return simulate_time;	
	}

	void enable_simulate()
	{
		simulate_flag = 1;	
	}

	void disable_simulate()
	{
		simulate_flag = 0;	
	}

	bool check_simulate()
	{
		return simulate_flag;	
	}

	float x_;
	float y_;
	float init_x_;
	float init_y_;
	float old_x_;
	float old_y_;
	float horizontal_speed_; 		 //水平速度，左负右正
	float vertical_speed_;   		 //垂直速度，上负下正
	float horizontal_acceleration_;	 //水平加速度，左负右正 
	float vertical_acceleration_;    //垂直加速度，上负下正
	uint32_t dir_;                   //方向 0面朝左，1面朝右
	
	double   dir_angle_;             //人物相对于水平线朝向的角度0-360
	double   dir_radion_;            //人物相对于水平线朝向的弧度
	
	double   gun_angle_;             //枪口相对于人物的角度0-360
	double   gun_radion_;            //枪口相对于人物的弧度

	uint32_t width_;                 //宽度单位像素
	uint32_t height_;                //高度单位像素
	uint32_t simulate_time;          //模拟时间长度，单位毫秒
	uint32_t simulate_flag;          //模拟标志位
}phy_model;

double angle2radian(double value);
double radian2angle(double value);


class phy_container: public base_trigger_timer
{
public:
	phy_container(float v_a = 0.0, float h_a = 0.0)
	{
		horizontal_acceleration_ = h_a;
		vertical_acceleration_   = v_a;
	}
	~phy_container(){}
public:
	void set_acceleration(float v_a,  float h_a)
	{
		horizontal_acceleration_ = h_a;
		vertical_acceleration_   = v_a;
	}
public:
	void clear_phy_models();
	bool add_phy_model(phy_model* p_model);
	bool del_phy_model(phy_model* p_model);
	void simulate_models(uint32_t millisecond, phy_model* p_model);
	virtual void simulate(uint32_t millisecond);
public:
	float calc_x_by_time(phy_model* p_model, uint32_t millisecond);

	float calc_y_by_time(phy_model* p_model, uint32_t millisecond);
private:
	float horizontal_acceleration_;  //水平加速度，左负右正 
	float vertical_acceleration_;    //垂直加速度，上负下正 
private:
	std::list<phy_model*> models;	
};










#endif
