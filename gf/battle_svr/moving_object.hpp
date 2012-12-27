#ifndef KFBTL_MOVING_OBJECT_HPP_
#define KFBTL_MOVING_OBJECT_HPP_

/**
 *============================================================
 *  @file      moving_object.hpp
 *  @brief    base class of all moving objects
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#include "object.hpp"

/**
  * @brief base class of all moving objects
  */
//class MovingObject : public Object, public MemPool<MovingObject> {
class MovingObject : public Object {
public:
	/**
	  * @brief default ctor
	  */
	MovingObject(const Vector3D& pos = Vector3D(), const Vector3D& velocity = Vector3D());
	/**
	  * @brief default dtor
	  */
	virtual ~MovingObject()
		{ }

	/**
	  * @brief set velocity of this object, unit: pixel/sec, for example: 90pixel/sec
	  */
	void set_velocity(const Vector3D& v)
		{ velocity_ = v; }

	/**
	  * @brief return velocity of this moving object
	  */
	const Vector3D& velocity() const
		{ return velocity_; }
	/**
	  * @brief return facing direction of this moving object
	  */
//	const Vector3D facing() const
//		{ Vector3D v = velocity(); v.normalize(); return v; }

public:
//	USING_MEMPOOL_OPERATORS(MovingObject);

private:
	/*! velocity of a moving object */
	Vector3D	velocity_;
};

//-----------------------------------------------------------------------------------------------
// Public Methods
inline
MovingObject::MovingObject(const Vector3D& pos, const Vector3D& velocity)
				: Object(pos), velocity_(velocity)
{
}

#endif // KFBTL_MOVING_OBJECT_HPP_

