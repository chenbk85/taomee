#ifndef KFBTL_OBJECT_HPP_
#define KFBTL_OBJECT_HPP_

/**
 *============================================================
 *  @file      object.hpp
 *  @brief    base class of all objects
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
#include <stdint.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
}

#include <list>

#include <boost/intrusive/list.hpp>
#include <boost/pool/object_pool.hpp>

#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>

#include "vector/vector.hpp"
//#include "mempool/mempool.hpp"

#include "fwd_decl.hpp"
//#include "app_log.hpp"
/**
  * @brief hook for hooking all objects together
  */
typedef boost::intrusive::list_member_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink> > ObjectHook;




/**
  * @brief class of all objects' shape
  */
class Cube {
public:
	Cube(const Vector3D& center = Vector3D(), int x = 0, int y = 0, int height = 0);

	const Vector3D& center() const
		{ return center_; }

	inline void set_center(const Vector3D& center)
		{ center_ = center; }
	
	inline void set_xy(int x, int y)
		{ len_x_ = x; len_y_ = y; }

	inline void set_height(int height)
		{ height_ = height;}

	inline bool collision(const Cube& reg);

	inline bool check_distance(const Cube& reg, uint32_t distance_cmp);

	inline bool check_sector(const Cube& reg,  uint32_t radius, uint32_t angle,  uint32_t dir);
public:
	enum cube_dir
	{
		left_dir = 1,
		right_dir,
		up_dir,
		down_dir
	};
private:
	Vector3D center_;
	int len_x_;
	int len_y_;
	int height_;
};

/**
  * @brief base class of all objects
  */
// class Object : public MemPool<Object> {
class Object : public taomee::MemPool {
public:
	/**
	  * @brief default constructor
	  * @param pos position of an object
	  * @param objtype type of an object
	  */
	Object(const Vector3D& pos = Vector3D(), uint32_t objtype = 0);
	
	/**
	  * @brief default destructor
	  */
	virtual ~Object()
		{ }

	/**
	  * @brief unique id of an object
	  */
	uint32_t id() const
		{ return id_; }
	
	/**
	  * @brief return current position of an object
	  */
	const Vector3D& pos() const
		{ return region_.center(); }

	const Vector3D& old_pos() const
		{
			return old_region_.center();
		}
	/**
	  * @brief return true if dead, false otherwise
	  */
	bool is_dead() const
		{ return dead_; }

	/**
	  * @brief return the type of object
	  */
	uint32_t type() const
		{ return type_; }

	/**
	  * @brief set an object dead
	  */
	void set_type(uint32_t type)
		{ type_ = type; }

	/**
	  * @brief set current position of an object to 'v'
	  */
 	void  set_pos(const Vector3D& v)
	{ 
		TRACE_LOG("%u SET_POS x = %u, y = %u",  id(), v.x(), v.y());
		old_region_ = region_;
		region_.set_center(v);
	}

	void set_old_pos(const Vector3D& v)
	{
		old_region_.set_center(v);
	}
	/**
	  * @brief set random offset position of an object
	  */
	void set_pos_offset(const Vector3D& v, uint32_t offset_x, uint32_t offset_y)
		{
			Vector3D offset;
			offset.set_x( rand() % (2 * offset_x) - offset_x );
			offset.set_y( rand() % (2 * offset_y) - offset_y );
			region_.set_center( v + offset );
		}

	/**
	  * @brief set current region of the object
	  */
	void set_region(int x, int y, int height = 0)
		{ 
			region_.set_xy(x, y); 
			region_.set_height(height);
		}

	/**
	  * @brief set an object dead
	  */
	void set_dead(bool dead = true)
		{ dead_ = dead; }

	/**
	  * @brief test one object conflict another object
	  */
	bool collision(const Object* obj)
		{ return this->region_.collision(obj->region_); }
	
	
	bool check_distance(const Object* obj, uint32_t distance_cmp)
	{
		return region_.check_distance(obj->region_, distance_cmp);
	}

	bool check_sector(const Object* obj,  uint32_t radius,  uint32_t angle, uint32_t dir)
	{
		return region_.check_sector(obj->region_, radius, angle, dir);
	}
	/**
	  * @brief update attribute of this object
	  */
	virtual void update(int time_elapsed)
		{ }

private:
	/*! each entity has a unique ID */
	uint32_t	id_;
	/*! every entity has a type associated with it (health, troll, ammo etc) */
	uint32_t	type_;
	/*! for testing if an object is dead */
	bool		dead_;
	/*! current region of an object */
	Cube		region_;
	Cube        old_region_;
	/*! this is the next valid ID. Each time an Object is instantiated, this value is updated */
	static uint32_t	s_next_id_;
};

//----------------------------------------------------

/* class Cube*/
inline
Cube::Cube(const Vector3D& center , int x, int y, int height) 
{
	center_ = center;
	len_x_ = x;
	len_y_ = y;
	height_ = height;
}

inline bool
Cube::collision(const Cube& reg)
{
	return ( (abs(reg.center_.x() - this->center_.x()) < (reg.len_x_ + this->len_x_)/2) 
			 && (abs(reg.center_.y() - this->center_.y()) < (reg.len_y_ + this->len_y_)/2) );
}

inline bool
Cube::check_distance(const Cube& reg,  uint32_t distance_cmp)
{
	uint32_t x = abs(center_.x() - reg.center_.x());
	uint32_t y = abs(center_.y() - reg.center_.y());
	uint32_t distance =  sqrt(x*x + y*y);
	return distance <= distance_cmp;
}

inline bool 
Cube::check_sector(const Cube& reg,  uint32_t radius,  uint32_t angle, uint32_t dir)
{
	if( !(angle >0 && angle < 180) )return false;	
	if( !(dir == Cube::left_dir || dir == Cube::right_dir ) )return false;

	uint32_t x = abs(center_.x() - reg.center_.x());
	uint32_t y = abs(center_.y() - reg.center_.y());
	uint32_t distance =  sqrt(x*x + y*y);
	if(distance > radius)return false;
	
	float slope = (float)(y) / (float)(x);
	//角度转弧度
	float half_radian = angle * 3.1415926 / 360.0;
	float skill_slope = tan(half_radian);

	if( dir == Cube::left_dir &&  reg.center_.x() < center_.x() )
	{
		if( slope <= skill_slope){
			return true;
		}
	}

	if( dir == Cube::right_dir && reg.center_.x() > center_.x())
	{
		if( slope <= skill_slope ){
			return true;
		}
	}

	return false;
}

//----------------------------------------------------

/* class Object*/
inline
Object::Object(const Vector3D& pos, uint32_t objtype)
{
	id_ = ++s_next_id_;
	if (id_ == 0) {
		s_next_id_ = base_guest_id;
		id_ = ++s_next_id_;
	}

	dead_  = false;
	type_  = objtype;

	region_.set_center(pos);
}

/**
  * @brief type of an object list
  */
typedef std::list<Object*> ObjList;

//--------------------------------------------------

void update_all_objects();

#endif // KFBTL_OBJECT_HPP_

