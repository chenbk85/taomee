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

#include "fwd_decl.hpp"


/**
  * @brief hook for hooking all objects together
  */
typedef boost::intrusive::list_member_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink> > ObjectHook;


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
	Object();
	
	/**
	  * @brief default destructor
	  */
	virtual ~Object()
		{ }

	/**
	  * @brief update attribute of this object
	  */
	virtual void update(int time_elapsed)
		{ }

	/**
	  * @brief unique id of an object
	  */
	uint32_t id() const
		{ return id_; }

private:
	/*! each entity has a unique ID */
	uint32_t	id_;
	/*! this is the next valid ID. Each time an Object is instantiated, this value is updated */
	static uint32_t	s_next_id_;
};

//----------------------------------------------------

//----------------------------------------------------

/* class Object*/
inline
Object::Object()
{
	id_ = ++s_next_id_;
	if (id_ == 0) {
		s_next_id_ = base_guest_id;
		id_ = ++s_next_id_;
	}
}

/**
  * @brief type of an object list
  */
typedef std::list<Object*> ObjList;

//--------------------------------------------------

void update_all_objects();

#endif // KFBTL_OBJECT_HPP_

