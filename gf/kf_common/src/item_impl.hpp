#ifndef ITEM_IMPL_HPP_
#define ITEM_IMPL_HPP_

#include <vector>
//#include <libtaomee++/conf_parser/item_manager.hpp>
#include "item_manager.hpp"

//--------------------------------------------------------------------------------
// item class
//--------------------------------------------------------------------------------
const int max_cat  = 30; 
const int max_elem = 100000;



/**
 * @brief GfItemManager
 */
class GfItemManager : public ItemManager<GfItem, max_cat, max_elem> {
public:
	/**
	 * @brief constructor
	 */
	GfItemManager(const char* file) : ItemManager<GfItem, max_cat, max_elem>(file) { }
	/**
	 * @brief constructor
	 */
	GfItemManager(const char* file1, const char* file2, const char* file3) 
		: ItemManager<GfItem, max_cat, max_elem>(file1, file2, file3) { }

};

#endif //ITEM_IMPL_HPP_
