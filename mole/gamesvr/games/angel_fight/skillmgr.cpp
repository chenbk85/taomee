/*
 * =====================================================================================
 *
 *       Filename:  skillmgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/13/2011 04:00:35 PM
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
#include "skill.h"
#include "skillmgr.h"

#define MAX_SKILL_NUM		64

SkillMgr g_skillMgr;

SkillMgr::SkillMgr()
{
	m_vecSkill.reserve(MAX_SKILL_NUM);
	m_vecSkill.clear();
}

SkillMgr::~SkillMgr()
{
	VecSkillIt_t it = m_vecSkill.begin();
	while( it != m_vecSkill.end() )
	{
		delete *it;
		it++;
	}
	m_vecSkill.clear();
}

Skill* SkillMgr::GetSkill( int32_t idx )
{
	if( idx < 0 )
	{
		ERROR_LOG( "GetSkill invalid skill id:[%d]", idx );
		return NULL;
	}
	if( uint32_t(idx) < m_vecSkill.size() )
	{
		return m_vecSkill[idx];
	}
	else
	{
		ERROR_LOG( "GetSkill invalid skill id:[%d]", idx );
		return NULL;
	}
}

int SkillMgr::LoadSkillInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr nodeSkill;
	int skillCount = 0;
	char file[] = "./conf/angelfight/skillinfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "SkillMgr::LoadSkillInfo() Open [%s] Failed", file ), -1 );
	}

	root = xmlDocGetRootElement(doc);
	if ( !root )
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	//技能总数
	DECODE_XML_PROP_INT( skillCount, root, "Count" );
	if( skillCount <= 0 )
	{
		goto exit;
	}

	m_vecSkill.clear();
	m_vecSkill.reserve(skillCount);

	//获取子节点
	nodeSkill= root->xmlChildrenNode;
	while( nodeSkill )
	{
		Skill* pSkill = NULL;
		if ( 0 == xmlStrcmp( nodeSkill->name, BAD_CAST"Skill" ) )
		{
			pSkill = new Skill( m_vecSkill.size() );
			m_vecSkill.push_back(pSkill);

			uint32_t ID;
			DECODE_XML_PROP_INT( ID, nodeSkill, "ID" );
			if( ID != m_vecSkill.size() - 1 )
			{
				ERROR_LOG( "Skill ID Invalid The Value is:[%d] Expect:[%lu]", ID, m_vecSkill.size() - 1 );
			}

			xmlChar* str;

			str = xmlGetProp( nodeSkill, BAD_CAST"Damage" );
			pSkill->XmlSetDamage( (char*)str );
			xmlFree(str);

			str = xmlGetProp( nodeSkill, BAD_CAST"Consume" );
			pSkill->XmlSetConsume( (char*)str );
			xmlFree(str);

			str = xmlGetProp( nodeSkill, BAD_CAST"IsEvil" );
			pSkill->XmlSetIsEvil( (char*)str );
			xmlFree(str);

			str = xmlGetProp( nodeSkill, BAD_CAST"UsePoint" );
			pSkill->XmlSetUsePoint( (char*)str );
			xmlFree(str);

			xmlNodePtr child = nodeSkill->xmlChildrenNode;
			while( child )
			{
				if( 0 == xmlStrcmp( child->name, BAD_CAST"Buff" ) )
				{
					str = xmlGetProp( child, BAD_CAST"ID" );
					pSkill->XmlAddBuffID( (char*)str );
				}
				else if( 0 == xmlStrcmp( child->name, BAD_CAST"Effect" ) )
				{
					str = xmlGetProp( child, BAD_CAST"ID" );
					pSkill->XmlAddEffectID( (char*)str );
				}
				child = child->next;
			}
		}
		nodeSkill = nodeSkill->next;
	}
exit:
	xmlFreeDoc(doc);
	DEBUG_LOG( "Load Skill Info Over Skill Size Is:[%lu]", m_vecSkill.size() );
	return ret;
}


