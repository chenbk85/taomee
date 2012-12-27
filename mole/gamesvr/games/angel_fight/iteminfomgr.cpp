/*
 * =====================================================================================
 *
 *       Filename:  iteminfomgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 01:12:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
extern "C"
{
#include <libtaomee/log.h>
#include <libxml/tree.h>
#include "../../util.h"
}

#include "iteminfomgr.h"

ItemInfoMgr	g_itemInfoMgr;

ItemInfoMgr::ItemInfoMgr()
{

}

ItemInfoMgr::~ItemInfoMgr()
{
	MapItemInfoIt_t it = m_ItemInfo.begin();

	while( it != m_ItemInfo.end() )
	{
		delete it->second;
		it++;
	}
	m_ItemInfo.clear();
}

ItemAttr* ItemInfoMgr::GetItemInfo(int32_t idx)
{
	if( idx < 0 )
	{
		ERROR_LOG( "GetItemInfo invalid item id:[%d]", idx );
		return NULL;
	}
	MapItemInfoIt_t it = m_ItemInfo.find(idx);
	if( it != m_ItemInfo.end() )
	{
		return it->second;
	}
	else
	{
		ERROR_LOG( "GetItemInfo invalid item id:[%d]", idx );
		return NULL;
	}
}

int ItemInfoMgr::LoadItemInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr cur;
	char file[] = "./conf/angelfight/iteminfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "ItemInfoMgr::LoadItemInfo() Open [%s] Failed", file ), -1 );
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur)
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	DECODE_XML_PROP_INT( m_nMaxLevel, cur, "MaxLevel" );

	//获取子节点
	cur = cur->xmlChildrenNode;
	while (cur)
	{
		ItemAttr* pItem = NULL;
		int itemID;
		int itemLevel;
		if( 0 == xmlStrcmp(cur->name, BAD_CAST"Item") )
		{
			pItem = new ItemAttr();
			DECODE_XML_PROP_INT( itemID, cur, "ID" );
			DECODE_XML_PROP_INT_DEF( itemLevel, cur, "Level", 0 );
			int realID = itemID * m_nMaxLevel + itemLevel - 1;
			m_ItemInfo.insert( ItemInfoValue_t( realID, pItem ) );

			DECODE_XML_PROP_INT( pItem->m_nType, cur, "Type");
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddStr, 	cur, "Str", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddInt, 	cur, "Int", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddHab, 	cur, "Hab", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddAgi, 	cur, "Ali", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddAtk, 	cur, "Atk", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddASD, 	cur, "Aspd", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddEva, 	cur, "Evad", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddBlock,cur, "Block", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddCombo,cur, "Combo", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddCrit, cur, "Crit", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddHit, 	cur, "Hit", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddDef, 	cur, "Def", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddHp, 	cur, "Hp", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nAddMp, 	cur, "Mp", 0 );
			DECODE_XML_PROP_INT_DEF( pItem->m_nSkillID, cur, "Skill", -1 );
		}
		cur = cur->next;
	}
exit:
	xmlFreeDoc(doc);
	DEBUG_LOG( "Load Item Info Over Item Size Is:[%lu]", m_ItemInfo.size() );
	return ret;
}
