/*
 * =====================================================================================
 *
 *       Filename:  levelmgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/23/2011 04:12:53 PM
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
#include "levelinfo.h"
#include "levelmgr.h"

LevelMgr g_levelMgr;

LevelMgr::LevelMgr()
{

}

LevelMgr::~LevelMgr()
{

}

void LevelMgr::GetCreatureIDByLevelID(int32_t idx,int * creatureID,int& count)
{
	if( !creatureID || idx < 0 )
	{
		count = 0;
	}

	if( idx < (int32_t)m_vecLevel.size() )
	{
		m_vecLevel[idx]->GetRandMonsterID( creatureID, count );
	}
}

int LevelMgr::GetExpByLevelID(int idx,bool isWin)
{
	if( idx >= 0 && idx < (int32_t)m_vecLevel.size() )
	{
		if( isWin )
		{
			return m_vecLevel[idx]->m_nExpWin;
		}
		else
		{
			return m_vecLevel[idx]->m_nExpLose;
		}
	}
	return 0;
}

LevelInfo* LevelMgr::GetLevelInfo( int idx )
{
	if( idx >= 0 && idx < (int32_t)m_vecLevel.size() )
	{
		return m_vecLevel[idx];
	}
	else
	{
		DEBUG_LOG( "LevelMgr::GetLevelInfo Invalid Level ID:[%d]", idx );
		return NULL;
	}
}

bool LevelMgr::CheckValidID( int idx )
{
	if( idx >= 0 && idx < (int32_t)m_vecLevel.size() && idx != LEVEL_ID_PVP && idx != LEVEL_ID_PK )
	{
		return true;
	}
	else
	{
		return false;
	}
}

int LevelMgr::LoadLevelInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	char file[] = "./conf/angelfight/levelinfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "LevelMgr::LoadLevelInfo() Open [%s] Failed", file ), -1 );
	}

	root = xmlDocGetRootElement(doc);
	if (!root)
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	int count;
	DECODE_XML_PROP_INT( count, root, "Count");
	if( count <= 0 )
	{
		goto exit;
	}

	m_vecLevel.clear();
	m_vecLevel.reserve( count );
	xmlNodePtr nodeMap;
	nodeMap = root->xmlChildrenNode;
	while( nodeMap )
	{
		if ( 0 == xmlStrcmp( nodeMap->name, BAD_CAST"Map" ) )
		{
			xmlNodePtr nodeLevel;
			//获取子节点
			nodeLevel = nodeMap->xmlChildrenNode;
			LevelInfo* pLevel;
			while (nodeLevel)
			{
				if ( 0 == xmlStrcmp( nodeLevel->name, BAD_CAST"Level" ) )
				{
					pLevel = new LevelInfo( m_vecLevel.size() );
					m_vecLevel.push_back( pLevel );

					uint32_t ID;
					DECODE_XML_PROP_INT( ID, nodeLevel, "ID" );
					if( ID != m_vecLevel.size() - 1 )
					{
						ERROR_LOG( "Level ID Invalid The Value is:[%d] Expect:[%lu]", ID, m_vecLevel.size() - 1 );
					}

					xmlChar* str;

					str = xmlGetProp( nodeLevel, BAD_CAST"MCount" );
					pLevel->XmlSetCreatureCount( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"ExpLose" );
					pLevel->XmlSetExpLose( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"ExpWin" );
					pLevel->XmlSetExpWin( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"CPoint" );
					pLevel->XmlSetCPoint( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"CondLevelID" );
					pLevel->XmlSetCondLvlID( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"CondPLevel" );
					pLevel->XmlSetCondPLevel( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"Consume" );
					pLevel->XmlSetConsume( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"Type" );
					pLevel->XmlSetType( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"TimeBegin" );
					pLevel->XmlSetTimeBegin( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"TimeEnd" );
					pLevel->XmlSetTimeEnd( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"Key" );
					pLevel->XmlSetKey( (char*)str );
					xmlFree(str);

					str = xmlGetProp( nodeLevel, BAD_CAST"Card" );
					pLevel->XmlSetCard( (char*)str );
					xmlFree(str);

					xmlNodePtr child;
					child = nodeLevel->xmlChildrenNode;
					while( child )
					{
						if ( 0 == xmlStrcmp( child->name, BAD_CAST"Monster" ) )
						{
							str = xmlGetProp( child, BAD_CAST"ID" );
							pLevel->XmlAddMonsterID( (char*)str );
							xmlFree(str);
						}
						child = child->next;
					}
				}
				nodeLevel = nodeLevel->next;
			}
		}
		nodeMap = nodeMap->next;
	}
exit:
	xmlFreeDoc(doc);
	DEBUG_LOG( "Load Level Info Over Level Size Is:[%lu]", m_vecLevel.size() );
	return ret;
}

int LevelMgr::GetCPoint(int idx)
{
	if( idx >= 0 && idx < (int)m_vecLevel.size() )
	{
		return m_vecLevel[idx]->m_nCompletePoint;
	}

	return 0;
}
