#ifndef OBJECT_HPP_
#define OBJECT_HPP_


extern "C" 
{
#include <stdint.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
}

#include <list>
#include <boost/intrusive/list.hpp>
#include <boost/pool/object_pool.hpp>
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "map_data.hpp"

typedef boost::intrusive::list_member_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink> > ObjectHook;

class Object : public taomee::MemPool 
{
public:
	Object()
	{
		id_ = ++s_next_id_;
		if( id_ == 0)
		{
			s_next_id_ = base_guest_id;
			id_ = ++s_next_id_;
		}
		dead_ = false;
		type_ = 0;
	}
	virtual ~Object(){}

	void set_model(phy_model& model)
	{
		model_ = model;	
	}

	uint32_t id() const
	{
		return id_;	
	}

	uint32_t type() const
	{
		return type_;	
	}

	void set_type(uint32_t type)
	{
		type_ = type;	
	}

	bool is_dead() const
	{
		return dead_;	
	}

	void set_dead(bool dead)
	{
		dead_ = dead;	
	}

	uint32_t distance(Object* p_obj)
	{
		return model_.distance(&(p_obj->model_));
	}

	virtual bool collision(Object* p_obj)
	{
		return model_.collision(&(p_obj->model_));	
	}

	bool in_area(Object* p_obj)
	{
		return model_.in_area(&(p_obj->model_));	
	}

	bool collision(map_data* data, void* para)
	{
		if( collision_area(data, this, para))
		{
			return true;	
		}
		return false;
	}

	virtual bool process_collision(Object* p_obj)
	{
		if( before_collision(p_obj) && (collision(p_obj) || line_collision(p_obj)))
		{
			return after_collision(p_obj);	
		}
		return false;
	}

	virtual bool before_collision(Object* p_obj)
	{
		return true;
	}

	virtual bool after_collision(Object* p_obj)
	{
		return true;
	}
	virtual bool line_collision(Object* p_obj)
	{
		return false;	
	}

	virtual bool process_collision(map_data* data, void* para)
	{
		if( before_collision(data, para) && (line_collision(data, para) || collision(data, para) ))
		{
			return after_collision(data, para);
		}
		return false;
	}

	virtual bool before_collision( map_data* data, void* para)
	{
		return true;	
	}

	virtual bool after_collision( map_data*  data, void* para)
	{
		return true;	
	}

	virtual bool line_collision( map_data*  data, void* para)
	{
		return false;	
	}
	
	float get_x()
	{
		return model_.x_;	
	}

	float get_y()
	{
		return model_.y_;	
	}

	float get_old_x()
	{
		return model_.old_x_;	
	}

	float get_old_y()
	{
		return model_.old_y_;	
	}

	float get_init_x()
	{
		return model_.init_x_;	
	}

	float get_init_y()
	{
		return model_.init_y_;	
	}

	uint32_t get_width()
	{
		return model_.width_;	
	}

	uint32_t get_height()
	{
		return model_.height_;	
	}

	uint32_t get_dir()
	{
		return model_.dir_;	
	}

	uint32_t get_angle()
	{
		return model_.dir_angle_;		
	}

	uint32_t get_gun_angle()
	{
		return model_.gun_angle_;	
	}

	void set_x(uint32_t x)
	{
		model_.x_ = x;
		model_.init_x_ = x;
		model_.old_x_ = x;
	}

	void redress_x(uint32_t x)
	{
		model_.x_ = x;	
	}

	void redress_y(uint32_t y)
	{
		model_.y_ = y;
	}

	void set_y(uint32_t y)
	{
		model_.y_ = y;	
		model_.init_y_ = y;
		model_.old_y_ = y;
	}

	void set_dir(uint32_t dir)
	{
		model_.dir_ = dir;	
	}

	void set_angle(uint32_t angle)
	{
		model_.dir_angle_ = angle;	
		model_.dir_radion_ = angle2radian(angle);
	}

	void set_rectangle(uint32_t width, uint32_t height)
	{
		model_.set_rectangle(width, height);	
	}
	
	void set_speed(double x_speed, double y_speed)
	{
		model_.set_speed(x_speed, y_speed);	
	}

	void set_gun_angle(uint32_t angle)
	{
		model_.gun_angle_ = angle;
		model_.gun_radion_ = angle2radian(angle);
	}
	
	phy_model*  get_phy_model()
	{
		return &model_;	
	}

	void enable_simulate()
	{
		return model_.enable_simulate();	
	}

	void disable_simulate()
	{
		return model_.disable_simulate();	
	}

	uint32_t get_simulate_time()
	{
		return model_.get_simulate_time();	
	}
	virtual void update(uint32_t millisecond){}
private:
	uint32_t 	id_;
	uint32_t 	type_;
	bool     	dead_;
	phy_model	model_;
	static uint32_t s_next_id_;
};


void update_all_objects();



#endif
