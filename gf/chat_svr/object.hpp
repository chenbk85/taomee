/*
 * =====================================================================================
 *
 *       Filename:  object.hpp
 *
 *    Description:  basic object
 *
 *        Version:  1.0
 *        Created:  12/29/2011 04:59:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ianyang (ianyang), ianyang@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef OBJECT_HPP_
#define OBJECT_HPP_
#include <libtaomee++/memory/mempool.hpp>

enum {
	base_obj_id = 0
};

class Object : public taomee::MemPool {
	public:
		Object() {
			id_ = ++s_next_id_;
			if (id_ == 0) {
				s_next_id_ = base_obj_id;
				id_ = ++s_next_id_;
			}
		}
		virtual ~Object() {
		}
	public:
		uint32_t id() {
			return id_;
		}
	private:
		uint32_t id_;
		/* ! this is the next valid ID. Each time an Object is instantiated, this value is updated */
	   	static uint32_t	s_next_id_;
};

#endif //END OBJECT_HPP_
