/*
 * =====================================================================================
 *
 *       Filename:  buffmgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/13/2011 04:45:26 PM
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

#include <vector>
#include "buff.h"
#include "buffmgr.h"

BuffMgr g_buffMgr;

BuffMgr::BuffMgr()
{
}

BuffMgr::~BuffMgr()
{
	VecBuffDefIt_t it = m_vecBuffDef.begin();
	while( it != m_vecBuffDef.end() )
	{
		delete *it;
		it++;
	}
	m_vecBuffDef.clear();
}

Buff* BuffMgr::GetBuff( int32_t idx )
{
	if( idx < 0 )
	{
		ERROR_LOG( "GetBuff invalid buff id:[%d]", idx );
		return NULL;
	}
	if( uint32_t(idx) < m_vecBuffDef.size() )
	{
		return m_vecBuffDef[idx];
	}
	else
	{
		ERROR_LOG( "GetBuff invalid buff id:[%d]", idx );
		return NULL;
	}
}

int BuffMgr::LoadBuffInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr nodeBuff;
	int buffCount = 0;
	char file[] = "./conf/angelfight/buffinfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "BuffMgr::LoadBuffInfo() Open [%s] Failed", file ), -1 );
	}

	root = xmlDocGetRootElement(doc);
	if ( !root )
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	//状态总数
	DECODE_XML_PROP_INT( buffCount, root, "Count" );
	if( buffCount <= 0 )
	{
		goto exit;
	}

	m_vecBuffDef.clear();
	m_vecBuffDef.reserve(buffCount);

	//获取子节点
	nodeBuff= root->xmlChildrenNode;
	Buff* pBuff;
	while( nodeBuff )
	{
		if ( 0 == xmlStrcmp( nodeBuff->name, BAD_CAST"Buff" ) )
		{
			pBuff = new Buff();

			m_vecBuffDef.push_back(pBuff);
			xmlChar* str;

			uint32_t ID;

			DECODE_XML_PROP_INT( ID, nodeBuff, "ID" );
			if( ID != m_vecBuffDef.size() - 1 )
			{
				ERROR_LOG( "Buff ID Invalid The Value is:[%d] Expect:[%lu]", ID, m_vecBuffDef.size() - 1 );
			}

			str = xmlGetProp( nodeBuff, BAD_CAST"Lasts" );
			pBuff->XmlSetLasts( (char*)str );
			xmlFree(str);

			xmlNodePtr child = nodeBuff->xmlChildrenNode;
			while( child )
			{
				if( 0 == xmlStrcmp( child->name, BAD_CAST"Effect" ) )
				{
					str = xmlGetProp( child, BAD_CAST"ID" );
					pBuff->XmlAddEffect( (char*)str );
				}
				child = child->next;
			}
		}
		nodeBuff = nodeBuff->next;
	}
exit:
	xmlFreeDoc(doc);
	DEBUG_LOG( "Load Buff Info Over Buff Size Is:[%lu]", m_vecBuffDef.size() );
	return ret;
}
