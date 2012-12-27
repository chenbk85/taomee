/*
 * =====================================================================================
 *
 *       Filename:  iteminfomgr.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 01:11:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef ITEM_INFO_MGR_H
#define ITEM_INFO_MGR_H

#include <map>
#include "itemattr.h"

class ItemInfoMgr
{
public:
	ItemInfoMgr();
	~ItemInfoMgr();

	int			LoadItemInfo();
	ItemAttr* 	GetItemInfo( int32_t idx );
private:

	typedef std::map< int, ItemAttr* >	MapItemInfo_t;
	typedef MapItemInfo_t::iterator		MapItemInfoIt_t;
	typedef MapItemInfo_t::value_type	ItemInfoValue_t;
	MapItemInfo_t	m_ItemInfo;
public:
	int32_t			m_nMaxLevel;		//道具的最高等级
};

extern ItemInfoMgr	g_itemInfoMgr;
#endif

