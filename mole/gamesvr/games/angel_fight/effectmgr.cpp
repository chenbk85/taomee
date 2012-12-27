/*
 * =====================================================================================
 *
 *       Filename:  effectmgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/13/2011 01:35:36 PM
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

#include <stdlib.h>
#include <vector>
#include "condition.h"
#include "conditionhp.h"
#include "conditionrand.h"
#include "effect.h"
#include "effectmodhp.h"
#include "effectmodaspd.h"
#include "effectmodatk.h"
#include "effectmodcrit.h"
#include "effectmoddef.h"
#include "effectmodhit.h"
#include "effectmodmp.h"
#include "effectmodevad.h"
#include "effectmodblock.h"
#include "effectmgr.h"

EffectMgr g_effectMgr;

EffectMgr::EffectMgr()
{

}

EffectMgr::~EffectMgr()
{
	VecEffectIt_t it = m_vecEffect.begin();
	while( it != m_vecEffect.end() )
	{
		delete *it;
		it++;
	}
	m_vecEffect.clear();
}

Effect*	EffectMgr::GetEffect( int32_t	 idx )
{
	if( idx < 0 )
	{
		ERROR_LOG( "GetEffect invalid effect id:[%d]", idx );
		return NULL;
	}
	if( uint32_t(idx) < m_vecEffect.size() )
	{
		return m_vecEffect[idx];
	}
	else
	{
		ERROR_LOG( "GetEffect invalid effect id:[%d]", idx );
		return NULL;
	}
}

int EffectMgr::LoadEffectInfo()
{
	int ret = 0;
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr nodeEffect;
	int effectCount = 0;
	char file[] = "./conf/angelfight/effectinfo.xml";

	doc = xmlParseFile(file);
	if (!doc)
	{
		ERROR_RETURN( ( "EffectMgr::LoadEffectInfo() Open [%s] Failed", file ), -1 );
	}

	root = xmlDocGetRootElement(doc);
	if ( !root )
	{
		ERROR_LOG("xmlDocGetRootElement error");
		ret = -1;
		goto exit;
	}
	//状态总数
	DECODE_XML_PROP_INT( effectCount, root, "Count" );
	if( effectCount <= 0 )
	{
		goto exit;
	}

	m_vecEffect.clear();
	m_vecEffect.reserve(effectCount);

	//获取子节点
	nodeEffect= root->xmlChildrenNode;
	Effect* pEffect;
	while( nodeEffect )
	{
		if ( 0 == xmlStrcmp( nodeEffect->name, BAD_CAST"Effect" ) )
		{
			xmlChar* str;

			str = xmlGetProp( nodeEffect, BAD_CAST"Type" );
			if( str )
			{
				if( 0 == xmlStrcmp( str, BAD_CAST"HP" ) )
				{
					pEffect = new EModHp();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"DEF" ) )
				{
					pEffect = new EModDef();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"HIT" ) )
				{
					pEffect = new EModHit();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"CRIT" ) )
				{
					pEffect = new EModCrit();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"ATK" ) )
				{
					pEffect = new EModAtk();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"ASPD" ) )
				{
					pEffect = new EModAspd();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"MP" ) )
				{
					pEffect = new EModMp();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"EVAD" ) )
				{
					pEffect = new EModEvad();
				}
				else if( 0 == xmlStrcmp( str, BAD_CAST"BLOCK" ) )
				{
					pEffect = new EModBlock();
				}
				else
				{
					xmlFree(str);
					nodeEffect = nodeEffect->next;
					continue;
				}
			}
			xmlFree(str);

			if( !pEffect )
			{
				DEBUG_LOG( "Malloc Effect Object Failed" );
				nodeEffect = nodeEffect->next;
				continue;
			}

			m_vecEffect.push_back( pEffect );

			uint32_t ID;
			DECODE_XML_PROP_INT( ID, nodeEffect, "ID" );
			if( ID != m_vecEffect.size() - 1 )
			{
				ERROR_LOG( "Effect ID Invalid The Value is:[%d] Expect:[%lu]", ID, m_vecEffect.size() - 1 );
			}

			str = xmlGetProp( nodeEffect, BAD_CAST"Value" );
			pEffect->XmlSetEffect( (char*)str );
			xmlFree(str);

			xmlNodePtr child = nodeEffect->xmlChildrenNode;
			Condition* pCond = NULL;
			while( child )
			{
				if( 0 == xmlStrcmp( child->name, BAD_CAST"Condition" ) )
				{
					str = xmlGetProp( child, BAD_CAST"Type" );
					if( str )
					{
						if( 0 == xmlStrcmp( str, BAD_CAST"HP" ) )
						{
							pCond = new CondHP();
						}
						else if( 0 == xmlStrcmp( str, BAD_CAST"RAND" ) )
						{
							pCond = new CondRand();
						}
						else
						{
							child = child->next;
							xmlFree(str);
							continue;
						}
					}
					xmlFree(str);
					if( !pCond )
					{
						DEBUG_LOG( "Malloc Condition Object Failed" );
						child = child->next;
						continue;
					}
					pEffect->AddCondition( pCond );

					str = xmlGetProp( child, BAD_CAST"Except" );
					pCond->XmlSetExpect( (char*)str );
					xmlFree(str);
				}
				child = child->next;
			}
		}
		nodeEffect = nodeEffect->next;
	}
exit:
	xmlFreeDoc(doc);
	DEBUG_LOG( "Load Effect Info Over Effect Size Is:[%lu]", m_vecEffect.size() );
	return ret;
}

