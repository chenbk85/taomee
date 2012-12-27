#include "object.hpp"


using namespace taomee;


/*! this is the next valid ID. Each time an Object is instantiated, this value is updated */
uint32_t Object::s_next_id_ = base_guest_id;

//---------------------------------------------------

void update_all_objects()
{

}

