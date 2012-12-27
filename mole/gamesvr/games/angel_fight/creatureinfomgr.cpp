/*
 * =====================================================================================
 *
 *       Filename:  creatureinfomgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 10:39:07 AM
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
#include "creatureinfo.h"
#include "creatureinfomgr.h"

CreatureInfoMgr g_creatureInfoMgr;


CreatureInfoMgr::CreatureInfoMgr()
{
	m_vecCreatureInfo.clear();
	m_vecCreatureInfo.reserve(10);
	CreatureInfo* info = new CreatureInfo();
	m_vecCreatureInfo.push_back(info);
	info->m_nCreatureID = m_vecCreatureInfo.size();
}

CreatureInfoMgr::~CreatureInfoMgr()
{
	VecCreatureInfoIt_t it = m_vecCreatureInfo.begin();
	while( it != m_vecCreatureInfo.end() )
	{
		delete *it;
		it++;
	}
	m_vecCreatureInfo.clear();
}

CreatureInfo* CreatureInfoMgr::GetCreatureInfo(int32_t idx)
{
	if( idx < 0 )
	{
		DEBUG_LOG( "GetCreatureInfo invalid creature id:[%d]", idx );
		return NULL;
	}
	if( uint32_t(idx) < m_vecCreatureInfo.size() )
	{
		return m_vecCreatureInfo[idx];
	}
	else
	{
		DEBUG_LOG( "GetCreatureInfo invalid creature id:[%d]", idx );
		return NULL;
	}
}

int CreatureInfoMgr::LoadCreatureInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr nodeCreature;
	int creatureCount = 0;
	char file[] = "./conf/angelfight/monsterinfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "CreatureInfoMgr::LoadCreatureInfo() Open [%s] Failed", file ), -1 );
	}

	root = xmlDocGetRootElement(doc);
	if ( !root )
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	//怪物总数
	DECODE_XML_PROP_INT( creatureCount, root, "Count" );
	if( creatureCount <= 0 )
	{
		goto exit;
	}

	m_vecCreatureInfo.clear();
	m_vecCreatureInfo.reserve(creatureCount);

	//获取子节点
	nodeCreature= root->xmlChildrenNode;
	while( nodeCreature )
	{
		CreatureInfo* pCreateure = NULL;
		if ( 0 == xmlStrcmp( nodeCreature->name, BAD_CAST"Monster" ) )
		{
			pCreateure = new CreatureInfo();
			pCreateure->m_nCreatureID = m_vecCreatureInfo.size();
			m_vecCreatureInfo.push_back(pCreateure);

			xmlChar* str;

			int ID;

			DECODE_XML_PROP_INT( ID, nodeCreature, "ID" );
			if( ID != pCreateure->m_nCreatureID )
			{
				ERROR_LOG( "Creature ID Invalid The Value is:[%d] Expect:[%u]", ID, pCreateure->m_nCreatureID );
			}

			str = xmlGetProp( nodeCreature, BAD_CAST"Level" );
			pCreateure->XmlSetLevel( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Str" );
			pCreateure->XmlSetStr( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Int" );
			pCreateure->XmlSetInt( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Hab" );
			pCreateure->XmlSetHabitus( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Ali" );
			pCreateure->XmlSetAgile( (char*)str );
			xmlFree(str);

			str = xmlGetProp( nodeCreature, BAD_CAST"Atk" );
			pCreateure->XmlSetAtk( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Aspd" );
			pCreateure->XmlSetAspd( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Evad" );
			pCreateure->XmlSetEvad( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Block" );
			pCreateure->XmlSetBlock( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Crit" );
			pCreateure->XmlSetCrit( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Hit" );
			pCreateure->XmlSetHit( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Combo" );
			pCreateure->XmlSetCombo( (char*)str );
			xmlFree(str);
			str = xmlGetProp( nodeCreature, BAD_CAST"Def" );
			pCreateure->XmlSetDef( (char*)str );
			xmlFree(str);

			str = xmlGetProp( nodeCreature, BAD_CAST"IsBoss" );
			pCreateure->XmlSetIsBoss( (char*)str );
			xmlFree(str);

			xmlNodePtr child = nodeCreature->xmlChildrenNode;
			while( child )
			{
				if( 0 == xmlStrcmp( child->name, BAD_CAST"Skill" ) )
				{
					str = xmlGetProp( child, BAD_CAST"ID" );
					pCreateure->XmlAddSkill( (char*)str );
				}
				else if( 0 == xmlStrcmp( child->name, BAD_CAST"Drop" ) )
				{
					str = xmlGetProp( child, BAD_CAST"ID" );
					pCreateure->XmlAddItem( (char*)str );
				}
				child = child->next;
			}
		}
		nodeCreature = nodeCreature->next;
	}
exit:
	xmlFreeDoc(doc);

	DEBUG_LOG( "Load Creature Info Over Creature Size Is:[%lu]", m_vecCreatureInfo.size() );
	return ret;
}

