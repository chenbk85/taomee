/*
 * =====================================================================================
 *
 *       Filename:  player.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/08/2011 06:03:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <algorithm>
#include<netinet/in.h>
#include <libtaomee++/random/random.hpp>
#include "../../ant/inet/pdumanip.hpp"
#include <vector>
#include "buff.h"
#include "buffmgr.h"
#include "skill.h"
#include "skillmgr.h"
#include "iteminfomgr.h"
#include "packetmgr.h"
#include "player.h"

#define MAX_BUFF_COUNT	5
#define MAX_SKILL_COUNT	5

SkillInfo::SkillInfo(int32_t idx): id(idx)
{
	Skill* skill = g_skillMgr.GetSkill(id);
	if( skill )
	{
		point = skill->GetUsePoint();
	}
	else
	{
		point = 0;
	}
}

bool SkillInfo::operator==( const SkillInfo& skill )
{
	if( skill.id == id && point != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

Player::Player():m_pSprite(NULL), m_nCreatureID(-1), m_pPartner(NULL), m_nAddExp(0), m_bActioned(false), m_bIsBoss(false), m_bIsLeave(false)
{
	m_nAttackEx 	= 0;
	m_nActSpeedEx 	= 0;
	m_nEvasionEx 	= 0;
	m_nCriticalEx 	= 0;
	m_nBlockEx 		= 0;
	m_nComboEx 		= 0;
	m_nHitEx 		= 0;
	m_nDefEx		= 0;
	m_vecSkillID.reserve(MAX_SKILL_COUNT);
	m_vecBuff.reserve(MAX_BUFF_COUNT);
	m_vecSkillID.clear();
	m_vecBuff.clear();

	m_nAttack 	= 0;
	m_nActSpeed = 0;
	m_nEvasion	= 0;
	m_nBlock	= 0;
	m_nDef		= 0;
	m_nCombo	= 0;
	m_nCritical = 0;
	m_nHit		= 0;
	m_nHP		= 0;
	m_nMaxHP	= 0;
	m_nMaxMP	= 0;
	m_nMP		= 0;
	m_nLevel	= 1;
	m_cName[0]	= '\0';
	m_nColor	= 0;
	m_nActionState = AS_WAIT;
}

Player::~Player()
{
	if( m_pPartner )
	{
		delete m_pPartner;
		m_pPartner = NULL;
	}
}

int Player::SetActionState( ACTION_STATE as, int32_t param )
{
	if( m_nActionState != AS_WAIT )
	{
		DEBUG_LOG( "Action Was Set" );
		return AFE_HAS_OPERATED;
	}

	switch( as )
	{
	case AS_SKILL:
		{
			VecSkillIDIt_t it = find( m_vecSkillID.begin(), m_vecSkillID.end(), SkillInfo(param) );
			if( it == m_vecSkillID.end() )
			{
				DEBUG_LOG("[%u] Player::SetActionState You haven't get this skill, Skill ID:[%d]", GetUserID(), param );
				return AFE_INVALID_SKILL;
			}
			Skill* skill = g_skillMgr.GetSkill( param );
			if( !skill )
			{
				DEBUG_LOG("Player::SetActionState Invalid skill ID");
				return AFE_INVALID_SKILL;
			}
			else
			{
				if( m_nMP < skill->GetConsume() )
				{
					DEBUG_LOG( "Not Enough Mana" );
					return AFE_NOT_ENOUGH_MANA;
				}
			}
			it->point -= 1;
		}
		break;
	case AS_ATTACK:
		break;
	case AS_BLOCK:
		{
			m_nBlockEx 		+= 100;
			m_nActSpeedEx 	+= 1000;
			//DEBUG_LOG( "AS_BLOCK: [%d], [%d]", m_nBlock, m_nBlockEx );
		}
		break;
	case AS_EVADE:
		{
			m_nEvasionEx	+= 100;
			m_nActSpeedEx 	+= 1000;
		}
		break;
	case AS_REST:
		{
			m_nActSpeedEx 	+= 1000;
		}
		break;
	case AS_AUTO:
		{
			return AutoAct();
		}
	default:
		{
			DEBUG_LOG( "Invalid Action Cmd" );
		}
		return AFE_INVALID_OPERATE;
	}
	m_nActionState = as;
	m_nASParam = param;

	return 0;
}

void Player::ClearActionState()
{
	switch( m_nActionState)
	{
	case AS_SKILL:
		break;
	case AS_ATTACK:
		break;
	case AS_BLOCK:
		m_nBlockEx 		-= 100;
		m_nActSpeedEx 	-= 1000;
		break;
	case AS_EVADE:
		m_nEvasionEx 	-= 100;
		m_nActSpeedEx 	-= 1000;
		break;
	case AS_REST:
		m_nActSpeedEx 	-= 1000;
		break;
	default:
		break;
	}
	m_nActionState = AS_WAIT;
	m_bActioned = false;
}

uint32_t Player::GetUserID()
{
	if( m_pSprite )
	{
		return m_pSprite->id;
	}
	else
	{
		//for monster
		return m_nCreatureID;
	}
}

int Player::Action( bool declare )
{
	//第一条数据为玩家ID以及对应操作
	uint8_t* data_buf = NULL;
	int32_t len;

	NoticeGetHead( &data_buf, len );

	//DEBUG_LOG("[%d] Act:[%d] Param:[%d]", GetUserID(), int32_t(m_nActionState), m_nASParam );

	if( data_buf )
	{
		ant::pack( data_buf, GetTeamID(), len );
		ant::pack( data_buf, int32_t(m_nActionState), len );
		ant::pack( data_buf, m_nASParam, len );
		CompletePacket( data_buf, PC_ID, len, true );
	}
	else
	{
		ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p]", data_buf );
	}

	//活着才能有操作
	if( m_nHP <= 0 || declare )
	{
		return PS_NORMAL;
	}

	switch( m_nActionState )
	{
	case AS_ATTACK:
		{
			Attack();
		}
		break;
	case AS_SKILL:
		{
			UseSkill();
		}
		break;
	case AS_BLOCK:
		{
			int mpInc = 100;
			m_nMP += mpInc;
			uint8_t* data_buf = NULL;
			int32_t len;
			uint32_t spaceLeft;
			NoticeNewPacket( &data_buf, len, spaceLeft );
			if( data_buf && spaceLeft > sizeof(int32_t)*1 + sizeof(PacketHead) )
			{
				ant::pack( data_buf, mpInc, len );
				CompletePacket( data_buf, PC_BLOCK, len );
			}
			else
			{
				ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
			}
		}
		break;
	case AS_EVADE:
		{
			int mpInc = 100;
			m_nMP += mpInc;
			uint8_t* data_buf = NULL;
			int32_t len;
			uint32_t spaceLeft;
			NoticeNewPacket( &data_buf, len, spaceLeft );
			if( data_buf && spaceLeft > sizeof(int32_t)*1 + sizeof(PacketHead) )
			{
				ant::pack( data_buf, mpInc, len );
				CompletePacket( data_buf, PC_EVAD, len );
			}
			else
			{
				ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
			}
		}
		break;
	case AS_REST:
		{
			int mpInc = 200;
			m_nMP += mpInc;
			uint8_t* data_buf = NULL;
			int32_t len;
			uint32_t spaceLeft;
			NoticeNewPacket( &data_buf, len, spaceLeft );
			if( data_buf && spaceLeft > sizeof(int32_t)*1 + sizeof(PacketHead) )
			{
				ant::pack( data_buf, mpInc, len );
				CompletePacket( data_buf, PC_REST, len );
			}
			else
			{
				ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
			}
		}
		break;
	default:
		break;
	}
	m_bActioned = true;

	ConstraintHpMp();

	if( 0 >= m_nHP )
	{
		return PS_OVER;
	}
	else if( 0 >= GetTar()->m_nHP )
	{
		return PS_WIN;
	}
	else
	{
		return PS_NORMAL;
	}
}

int Player::ExecuteBuff()
{
	//执行状态效果
	int32_t cHp = m_nHP;
	int32_t cMp = m_nMP;

	VecBuffInfoIt_t it = m_vecBuff.begin();
	Buff* bd = NULL;
	while( it != m_vecBuff.end() )
	{
		//DEBUG_LOG( "[%u] ExecuteBuff, Lasts:[%d]", GetUserID(), it->m_nRoundLasts );
		if( it->m_nRoundLasts < 0 )
		{
			it->m_nRoundLasts *= -1;
		}
		else
		{
			bd = g_buffMgr.GetBuff( it->m_nBuffID );
			if( bd )
			{
				bd->Execute( this, it->m_nRoundLasts, it->m_vecEffect );
			}
			else
			{
				//ERROR_LOG( "ExecuteBuff invalid buff idx:[%u]", it->m_nBuffID );
			}
			it->m_nRoundLasts--;
		}

		if( 0 == it->m_nRoundLasts )
		{
			RemoveBuff( it->m_nBuffID );
		}
		else
		{
			it++;
		}
	}
	ConstraintHpMp();
	cHp = m_nHP - cHp;
	cMp = m_nMP - cMp;
	//pack modify
	if( cHp != 0 || cMp != 0 )
	{

		uint8_t* data_buf = NULL;
		int32_t len;
		uint32_t spaceLeft;
		NoticeNewPacket( &data_buf, len, spaceLeft );
		if( data_buf && spaceLeft > sizeof(int32_t)*2 + sizeof(PacketHead) )
		{
			ant::pack( data_buf, cHp, len );
			ant::pack( data_buf, cMp, len );
			CompletePacket( data_buf, PC_BUFF_EFFECT, len );
			//DEBUG_LOG( "BUFF_EFFECT Hp:[%d] Mp:[%d]", cHp, cMp );
		}
		else
		{
			ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
		}
	}

	if( 0 >= m_nHP )
	{
		return PS_OVER;
	}
	else
	{
		return PS_NORMAL;
	}
}

int Player::Attack()
{
	if( !m_pActTar )
	{
		return 0;
	}

	int val;
	int nAtkCount = 1;
	//连击
	val = rand()%100;
	if( val <= m_nCombo + m_nComboEx )
	{
		nAtkCount = 2;
	}

	AttackInfo info;
	for( int i = 0; i < nAtkCount; ++i )
	{
		val = rand()%100;
		int rate = ( m_nHit + m_nHitEx ) - ( m_pActTar->m_nBlock + m_pActTar->m_nBlockEx );
		//DEBUG_LOG("BOLCK: Val:[%d] Rate:[%d] = ([%d] + [%d]) - ([%d]+[%d])", val, rate, m_nHit, m_nHitEx, m_pActTar->m_nBlock, m_pActTar->m_nBlockEx );
		//格挡
		if( val >= rate )
		{
			//sign
			info.m_isBlock = 1;
			info.m_isCritical = 0;
			info.m_nDamage = 0;
		}
		else
		{
			info.m_isBlock = 0;
			info.m_isCritical = 0;
			int nCritical = 1;
			val = rand()%100;
//			DEBUG_LOG("CRITICAL: rand()%%100=[%d], rate = [%d + %d] = [%d]", val, m_nCritical, m_nCriticalEx, m_nCritical + m_nCriticalEx );
			//暴击
			if( val <= m_nCritical + m_nCriticalEx )
			{
				info.m_isCritical = 1;
				nCritical = 2;
			}
			int damage = ( m_nAttack + m_nAttackEx ) * nCritical;

			info.m_nDamage = m_pActTar->GetDamage( damage );

			m_pActTar->IncHP( -m_pActTar->GetDamage( damage ) );
		}

		PACKET_CMD cmd = PC_ATTACK;
		if( 1 == i )
		{
			cmd = PC_COMBO;
		}

		uint8_t* data_buf = NULL;
		int32_t len;
		uint32_t spaceLeft;
		NoticeNewPacket( &data_buf, len, spaceLeft );
		if( data_buf && spaceLeft > sizeof(int32_t)*2 + sizeof(PacketHead) )
		{
			ant::pack( data_buf, info.m_isBlock, len );
			ant::pack( data_buf, info.m_isCritical, len );
			ant::pack( data_buf, info.m_nDamage, len );
			CompletePacket( data_buf, int32_t(cmd), len );
		}
		else
		{
			ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
		}

		if( cmd == PC_COMBO )
		{
			//DEBUG_LOG("[%u] COMBO: Block[%d],Critical[%d],Damage[%d]", GetUserID(), info.m_isBlock, info.m_isCritical, info.m_nDamage );
		}
		else
		{
			//DEBUG_LOG("[%u] ATTACK: Block[%d],Critical[%d],Damage[%d]", GetUserID(), info.m_isBlock, info.m_isCritical, info.m_nDamage );
		}
	}
	return 0;
}

int Player::UseSkill()
{
	uint32_t skill_id = uint32_t( m_nASParam );
	Skill * skill = g_skillMgr.GetSkill( skill_id );
	if( !skill )
	{
		return 0;
	}

	skill->OnSelect( this, m_vecAddedEffect );
	//用于测试没有MP时可以放技能的bug
	if( m_nMP < skill->GetConsume() )
	{
		ERROR_LOG( "ERROR Not Enough Mana But The Skill Still Executed" );
	}

	IncMP( -skill->GetConsume() );

	bool bCritical = false;
	if( skill->IsEvil() )
	{
		if( !m_pActTar )
		{
			return 0;
		}

		int  val;
		val = rand()%100;
		int rate = ( m_nHit + m_nHitEx ) - ( m_pActTar->m_nEvasion + m_pActTar->m_nEvasionEx );
		//闪避
		if( val >= rate )
		{
			//sign

			SkillAtkInfo info;
			info.m_isToSelf = 0;
			info.m_nSkillID = skill_id;
			info.m_isEvad = 1;
			info.m_isCritical = 0;
			info.m_nDamage = 0;
			info.m_nMpConsume = skill->GetConsume();

			uint8_t* data_buf = NULL;
			int32_t len;
			uint32_t spaceLeft;
			NoticeNewPacket( &data_buf, len, spaceLeft );
			if( data_buf && spaceLeft > sizeof(int32_t)*2 + sizeof(PacketHead) )
			{
				ant::pack( data_buf, info.m_isToSelf, len );
				ant::pack( data_buf, info.m_nSkillID, len );
				ant::pack( data_buf, info.m_isEvad, len );
				ant::pack( data_buf, info.m_isCritical, len );
				ant::pack( data_buf, info.m_nDamage, len );
				ant::pack( data_buf, info.m_nMpConsume, len );
				CompletePacket( data_buf, PC_SKILL, len );
			}
			else
			{
				ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
			}

			//DEBUG_LOG("[%u] SKILL: ToSelf[%d], SkillID[%d], Evad[%d], Critical[%d], Damage[%d] Consume[%d]",
			//	 GetUserID(), info.m_isToSelf, info.m_nSkillID, info.m_isEvad, info.m_isCritical, info.m_nDamage, info.m_nMpConsume );
			return 0;
		}

		val = rand()%100;
//		DEBUG_LOG("CRITICAL: rand()%%100=[%d], rate = [%d + %d] = [%d]", val, m_nCritical, m_nCriticalEx, m_nCritical + m_nCriticalEx );
		//暴击
		if( val <= m_nCritical + m_nCriticalEx )
		{
			bCritical = true;
		}
	}
	skill->Execute( this, bCritical, m_vecAddedEffect );
	return 0;
}

void Player::ConstraintHpMp()
{
	if( m_nHP < 0 )
	{
		m_nHP = 0;
	}
	else if( m_nHP > m_nMaxHP )
	{
		m_nHP = m_nMaxHP;
	}

	if( m_nMP < 0 )
	{
		m_nMP = 0;
	}
	else if( m_nMP > m_nMaxMP )
	{
		m_nMP = m_nMaxMP;
	}
}


void Player::AddBuff( int32_t idx, int32_t skillID )
{
	if( m_nHP <= 0 )
	{
		return;
	}
	Buff* bd = g_buffMgr.GetBuff( idx );
	if( !bd )
	{
		DEBUG_LOG("AddBuff invalid buff idx:[%u]", idx);
		return;
	}

	BuffInfo bufInfo;
	bufInfo.m_nRoundLasts = bd->GetRoundLasts();
	if( !m_bActioned )
	{
		bufInfo.m_nRoundLasts *= -1;
	}
	bufInfo.m_nBuffID = idx;

	uint8_t* data_buf = NULL;
	int32_t len;
	uint32_t spaceLeft;
	NoticeNewPacket( &data_buf, len, spaceLeft );
	if( data_buf && spaceLeft > sizeof(BuffInfo) + sizeof(PacketHead) )
	{
		ant::pack( data_buf, skillID, len );
		ant::pack( data_buf, bufInfo.m_nBuffID, len );
		ant::pack( data_buf, bd->GetRoundLasts(), len );
		CompletePacket( data_buf, PC_ADD_BUFF, len );
	}
	else
	{
		ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
	}

	//avoid overlying
	VecBuffInfoIt_t it = m_vecBuff.begin();
	while( it != m_vecBuff.end() )
	{
		if( it->m_nBuffID == bufInfo.m_nBuffID )
		{
			break;
		}
		it++;
	}
	if( it == m_vecBuff.end() )
	{
		m_vecBuff.push_back( bufInfo );
		bd->OnAdd( this, m_vecBuff.back().m_vecEffect );
//		DEBUG_LOG("+++++++++++ADD BUFF+++++++++++ ID:[%d], Round:[%d]", m_vecBuff.back().m_nBuffID, m_vecBuff.back().m_nRoundLasts );
	}
	else
	{
		it->m_nRoundLasts = bd->GetRoundLasts();
	}
}

void Player::RemoveBuff( int32_t idx )
{
	VecBuffInfoIt_t it = m_vecBuff.begin();
	bool bGetVal = false;
	while( it != m_vecBuff.end() )
	{
		if( it->m_nBuffID == idx )
		{
			bGetVal = true;
			break;
		}
		it++;
	}
	if( bGetVal )
	{
		Buff* bd = g_buffMgr.GetBuff( it->m_nBuffID );
		if( !bd )
		{
			ERROR_LOG( "RemoveBuff invalid buff idx:[%u]", it->m_nBuffID );
		}
		else
		{
			bd->OnRemove( this, it->m_vecEffect );
		}

		m_vecBuff.erase( it );

		uint8_t* data_buf = NULL;
		int32_t len;
		uint32_t spaceLeft;
		NoticeNewPacket( &data_buf, len, spaceLeft );
		if( data_buf && spaceLeft > sizeof(idx) + sizeof(PacketHead) )
		{
			ant::pack( data_buf, idx, len );
			CompletePacket( data_buf, PC_REMOVE_BUFF, len );
		}
		else
		{
			ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
		}
	}
}

int Player::SetInfo( CreatureInfo* my_info )
{
	if( !my_info )
	{
		DEBUG_LOG( "SetInfo Invalid Creature Info" );
		return -1;
	}
	m_nCreatureID		= my_info->m_nCreatureID;
	m_nLevel			= my_info->m_nLevel;
	m_bIsBoss			= my_info->m_bIsBoss;

	m_nStrength 		= my_info->m_nStr;
	m_nIntelligence		= my_info->m_nInt;
	m_nAgile			= my_info->m_nAgile;
	m_nHabitus			= my_info->m_nHabitus;

	m_nAttack			= my_info->m_nAttack;
	m_nActSpeed			= my_info->m_nActSpeed;
	m_nCritical			= my_info->m_nCritical;
	m_nCombo			= my_info->m_nCombo;
	m_nHit				= my_info->m_nHit;
	m_nEvasion			= my_info->m_nEvasion;
	m_nDef				= my_info->m_nDef;
	m_nBlock			= my_info->m_nBlock;

	m_vecSkillID.clear();
	for( int i = 0; i < my_info->m_nSkillCount; ++i )
	{
		if( my_info->m_nSkill[i] >= 0 )
		{
			m_vecSkillID.push_back( SkillInfo(my_info->m_nSkill[i]) );
		}
		else
		{
			DEBUG_LOG( "SetInfo invalid skill id:[%d]", my_info->m_nSkill[i] );
		}
	}
	ComputeAttr();
	DEBUG_LOG( "Player::SetInfo Monster OK" );
	return 0;
}

int Player::SetInfo( char* data, int32_t len, bool isPK /* = false */ )
{
	int32_t expectLen = sizeof(PlayerAttr_s);
	DEBUG_LOG( "Player::SetInfo data:[%p] len:[%d]", data, len );

	CHECK_BODY_LEN_GE( len, expectLen );

	PlayerAttr_s* player_attr = (PlayerAttr_s*)data;

	strcpy( m_cName, player_attr->m_cName );
	m_nLevel			= player_attr->m_nLevel;
	m_nStrength 		= player_attr->m_nStrength + 20;
	m_nIntelligence		= player_attr->m_nIntelligence;
	m_nAgile			= player_attr->m_nAgile;
	m_nHabitus			= player_attr->m_nHabitus;
	m_nEnergy			= player_attr->m_nEnergy;
	m_nVitality			= player_attr->m_nVitality;
	m_nEquipID			= player_attr->m_nEquip;
	m_nColor			= player_attr->m_nColor;

	int j = expectLen;

	int32_t item_id;
	DbItemInfo_s item_info;
	expectLen += sizeof(item_info) * player_attr->m_nItemCount;
	CHECK_BODY_LEN_GE( len, expectLen );
	ItemAttr* attr;
	DEBUG_LOG( "Player Item Count:[%d]", player_attr->m_nItemCount );
	for( int i = 0; i < player_attr->m_nItemCount; ++i )
	{
		ant::unpack( data, &item_info, sizeof(item_info), j );
		item_id = item_info.m_nID * g_itemInfoMgr.m_nMaxLevel + item_info.m_nLevel - 1;
		DEBUG_LOG("Item ID:[%d]", item_id );
		attr = g_itemInfoMgr.GetItemInfo( item_id );
		AddAttr( attr );
	}
	//物品附加属性
	if( m_nEquipID >= 0 )
	{
		attr = g_itemInfoMgr.GetItemInfo( m_nEquipID * g_itemInfoMgr.m_nMaxLevel );
		//DEBUG_LOG( "Curr Hab:[%d]", m_nHabitus );
		//DEBUG_LOG( "Add Equip Attr Hab:[%d]", attr->m_nAddHab );
		AddAttr( attr );
		//DEBUG_LOG( "After Hab:[%d]", m_nHabitus );
	}

	expectLen += sizeof(item_id) * player_attr->m_nStateCount;

	CHECK_BODY_LEN_GE( len, expectLen );
	DEBUG_LOG( "Get Add Item Count:[%d]", player_attr->m_nStateCount );
	for( int i = 0; i < player_attr->m_nStateCount; ++i )
	{
		ant::unpack( data, &item_id, sizeof(item_id), j );
		DEBUG_LOG( "Get Add Item ID:[%d]", item_id );
		switch( item_id )
		{
		case IAS_STR:
			{
				m_nStrength += 15;
			}
			break;
		case IAS_INT:
			{
				m_nIntelligence += 15;
			}
			break;
		case IAS_HAB:
			{
				m_nHabitus += 15;
			}
			break;
		case IAS_ALI:
			{
				m_nAgile += 15;
			}
			break;
		case IAS_EXP_INITIVATE:
			{
				//主动战斗
				if( isPK || (!isPK && 0 == m_nTeamID) )
				{
					m_nAddExp = 6;
				}
			}
			break;
		case IAS_EXP_PASSIVE:
			{
				//被动战斗
				if( m_nTeamID == 1 && !isPK )
				{
					m_nAddExp = 6;
				}
			}
			break;
		default:
			break;
		}
	}
	ComputeAttr();
	return 0;
}

void Player::AddAttr(ItemAttr * attr)
{
	if( !attr )
	{
		return;
	}
	if( ITEM_HAS_ADDED != attr->m_nType )
	{
		m_nStrength 		+= attr->m_nAddStr;
		m_nIntelligence		+= attr->m_nAddInt;
		m_nAgile			+= attr->m_nAddAgi;
		m_nHabitus			+= attr->m_nAddHab;
	}
	m_nAttack			+= attr->m_nAddAtk;
	m_nActSpeed			+= attr->m_nAddASD;
	m_nEvasion			+= attr->m_nAddEva;
	m_nCritical			+= attr->m_nAddCrit;
	m_nBlock			+= attr->m_nAddBlock;
	m_nHit				+= attr->m_nAddHit;
	m_nCombo			+= attr->m_nAddCombo;
	m_nDef				+= attr->m_nAddDef;
	m_nMaxHP			+= attr->m_nAddHp;
	m_nMaxMP			+= attr->m_nAddMp;

	//DEBUG_LOG( " ADD Attr Hp[%d] Mp[%d]",attr->m_nAddHp, attr->m_nAddMp );

	if( attr->m_nSkillID >= 0 )
	{
		if( m_vecSkillID.size() >= MAX_SKILL_COUNT ||
			find( m_vecSkillID.begin(), m_vecSkillID.end(), attr->m_nSkillID ) != m_vecSkillID.end() )
		{
			return;
		}
		else
		{
			m_vecSkillID.push_back( SkillInfo(attr->m_nSkillID) );
		}
	}
}

void Player::ComputeAttr()
{
	m_nAttack 	+= m_nStrength;
	m_nActSpeed += m_nAgile;
	m_nHit		+= 100;
//	m_nBlock	+= m_nAgile/5;
//	m_nCombo	+= 70;
	m_nCritical	+= m_nIntelligence/10;
	m_nEvasion	+= m_nIntelligence/10;
//	m_nDef		+= m_nAgile/5;
	m_nMaxHP	+= 150 + m_nHabitus * 8;
	m_nHP		= m_nMaxHP;
	m_nMaxMP	+= 500;
	m_nMP		+= 0;
}

int Player::AINormal()
{
	char candidate[AS_MAX];
	memset( candidate, 1, sizeof(candidate) );
	candidate[AS_WAIT] = 0;

	//获取一个随机技能
	int skillID;
	if( m_vecSkillID.size() > 0 )
	{
		int randVal = rand() % m_vecSkillID.size();
		if( m_vecSkillID[randVal].point != 0 )
		{
			skillID = m_vecSkillID[randVal].id;

			Skill* skill = g_skillMgr.GetSkill( skillID );
			if( !skill )
			{
				candidate[AS_SKILL] = 0;
			}
			else
			{
				if( m_nMP < skill->GetConsume() )
				{
					candidate[AS_SKILL] = 0;
				}

				if( !skill->IsEvil() )
				{
					VecBuff_t& vecBuff = skill->GetBuffID();
					if( !vecBuff.empty() )
					{
						VecBuffInfoIt_t it = m_vecBuff.begin();
						while( it != m_vecBuff.end() )
						{
							if( it->m_nBuffID == vecBuff[0] )
							{
								candidate[AS_SKILL] = 0;
								break;
							}
							it++;
						}
					}
				}
			}
		}
		else
		{
			candidate[AS_SKILL] = 0;
		}
	}
	else
	{
		candidate[AS_SKILL] = 0;
	}

	//filter
	//魔法足够施放技能时，一定概率排除休息
	if( 1 == candidate[AS_SKILL] && rand()%100 < 60 )
	{
		candidate[AS_REST] = 0;
	}
	//mp充满时，排除休息
	if( m_nMaxMP == m_nMP )
	{
		candidate[AS_REST] = 0;
	}

	if( !m_pActTar )
	{
		ERROR_LOG( "AutoAct invalid Action Tar Ptr" );
		return -1;
	}

	//目标MP为空时，排除闪避
	if( 0 == m_pActTar->m_nMP )
	{
		candidate[AS_EVADE] = 0;
	}

	ACTION_STATE act;
	//当自己血量比对方高时，一定概率选择进攻
	if( m_pActTar->m_nHP < m_nHP && rand()%100 < 50 )
	{
		if( 1 == candidate[AS_SKILL] )
		{
			act = AS_SKILL;
		}
		else
		{
			act = AS_ATTACK;
		}
		goto Decide;
	}

	//对方MP满格时，一定概率直接选择闪避
	if( m_pActTar->m_nMP == m_pActTar->m_nMaxMP && rand() % 100 < 50 )
	{
		act = AS_EVADE;
		goto Decide;
	}
	//MP超过一半时，一定概率直接选择施放技能
	if( 1 == candidate[AS_SKILL] && m_nMP >= m_nMaxMP/2 && rand()%100 < 50 )
	{
		act = AS_SKILL;
		goto Decide;
	}
	//MP满格时，高概率选择进攻
	if( m_nMP >= m_nMaxMP && rand()%100 < 80 )
	{
		if( 1 == candidate[AS_SKILL] && rand()%100 < 70 )
		{
			act = AS_SKILL;
		}
		else
		{
			act = AS_ATTACK;
		}
		goto Decide;
	}

	//random decide
	{
		std::vector<ACTION_STATE> vecCandidat;
		for( int i = 0; i < int(AS_MAX); ++i )
		{
			if( 1 == candidate[i] )
			{
				vecCandidat.push_back( ACTION_STATE(i) );
			}
		}
		int val = rand() % vecCandidat.size();
		act = vecCandidat[val];
	}

Decide:
	if( 0 != SetActionState( act, skillID ) )
	{
		ERROR_LOG( "Auto Act Failed" );
	}
	return 0;
}

int Player::AIBoss()
{
	if( !m_pActTar )
	{
		ERROR_LOG( "AutoAct invalid Action Tar Ptr" );
		return -1;
	}

	uint8_t candidate[AS_MAX];
	memset( candidate, 1, sizeof(candidate) );

	candidate[AS_WAIT] = 0;
	enum SKILL_POLICY
	{
		SP_EFFICIENCY,		//高效
		SP_POWER,			//高伤害
	};

	SKILL_POLICY policy;
	if( m_nMP > m_nMaxMP/2 )
	{
		policy = SP_POWER;
	}
	else
	{
		policy = SP_EFFICIENCY;
	}
	//选择一个技能
	int skillID = -1;
	float skillValue = 0;
	VecSkillIDIt_t it = m_vecSkillID.begin();
	while( it != m_vecSkillID.end() )
	{
		//技能可以使用
		if( it->point != 0 )
		{
			Skill* skill = g_skillMgr.GetSkill( it->id );
			//技能存在 并且MP足够施放技能
			if( skill && m_nMP >= skill->GetConsume() )
			{
				//攻击技能
				if( skill->IsEvil() )
				{
					//伤害可以致命
					if( m_pActTar->GetDamage( skill->GetDamage() + m_nAttack + m_nAttackEx ) == m_pActTar->m_nHP )
					{
						skillID = it->id;
						break;
					}
					else
					{
						float tmpValue = 0;
						switch( policy )
						{
						case SP_EFFICIENCY:
							{
								tmpValue = float( skill->GetDamage() + m_nAttack + m_nAttackEx ) / skill->GetConsume();
							}
							break;
						case SP_POWER:
							{
								tmpValue = skill->GetDamage() + m_nAttack + m_nAttackEx;
							}
							break;
						default:
							break;
						}
						if( tmpValue > skillValue )
						{
							skillValue = tmpValue;
							skillID = it->id;
						}
					}
				}
				//辅助技能
				else
				{
					VecBuff_t& vecBuff = skill->GetBuffID();
					bool bBreak = false;
					if( !vecBuff.empty() )
					{
						VecBuffInfoIt_t itBuff = m_vecBuff.begin();
						while( itBuff != m_vecBuff.end() )
						{
							if( itBuff->m_nBuffID == vecBuff[0] )
							{
								bBreak = true;
								break;
							}
							itBuff++;
						}
					}
					//buff效果不易判定价值，故做随机处理
					if( !bBreak && rand()%100 < 30 )
					{
						skillID = it->id;
						break;
					}
				}
			}
		}
		it++;
	}
	if( skillID < 0 )
	{
		candidate[AS_SKILL] = 0;
	}

	//filter
	//魔法足够施放技能时，一定概率排除休息
	if( 1 == candidate[AS_SKILL] && rand()%100 < 80 )
	{
		candidate[AS_REST] = 0;
	}
	//mp充满时，排除休息
	if( m_nMaxMP - m_nMP < 200 )
	{
		candidate[AS_REST] = 0;
	}

	if( !m_pActTar )
	{
		ERROR_LOG( "AutoAct invalid Action Tar Ptr" );
		return -1;
	}

	//目标MP为空时，排除闪避
	if( 0 == m_pActTar->m_nMP )
	{
		candidate[AS_EVADE] = 0;
	}

	ACTION_STATE act;
	//当自己血量比对方高时，一定概率直接选择进攻
	if( m_pActTar->m_nHP < m_nHP && rand()%100 < 50 )
	{
		if( 1 == candidate[AS_SKILL] && rand()%100 < 50 )
		{
			act = AS_SKILL;
		}
		else
		{
			act = AS_ATTACK;
		}
		goto Decide;
	}
	//MP超过一半时，一定概率直接选择施放技能
	if( 1 == candidate[AS_SKILL] && m_nMP >= m_nMaxMP/2 && rand()%100 < 50 )
	{
		act = AS_SKILL;
		goto Decide;
	}
	//MP满格时，高概率选择进攻
	if( m_nMP >= m_nMaxMP && rand()%100 < 80 )
	{
		if( 1 == candidate[AS_SKILL] && rand()%100 < 70 )
		{
			act = AS_SKILL;
		}
		else
		{
			act = AS_ATTACK;
		}
		goto Decide;
	}

	//对方MP满格时，一定概率直接选择闪避
	if( m_pActTar->m_nMP == m_pActTar->m_nMaxMP && rand() % 100 < 50 )
	{
		act = AS_EVADE;
		goto Decide;
	}

	//random decide
	{
		std::vector<ACTION_STATE> vecCandidat;
		for( int i = 0; i < int(AS_MAX); ++i )
		{
			if( 1 == candidate[i] )
			{
				vecCandidat.push_back( ACTION_STATE(i) );
			}
		}
		int val = rand() % vecCandidat.size();
		act = vecCandidat[val];
	}

Decide:
	if( 0 != SetActionState( act, skillID ) )
	{
		ERROR_LOG( "Auto Act Failed" );
	}
	return 0;
}

int Player::AutoAct()
{
	if( m_bIsBoss )
	{
		return AIBoss();
	}
	else
	{
		return AINormal();
	}
}

void Player::AddPartner(Player * p)
{
	if( p )
	{
		Player** next = &m_pPartner;
		while( *next )
		{
			next = &((*next)->m_pPartner);
		}
		*next = p;
		p->m_pActTar = m_pActTar;
		p->m_nTeamID = m_nTeamID;
	}
}

Player* Player::GetValidPartner()
{
	Player* p = this;
	while( p )
	{
		if( p->m_nHP > 0 )
		{
			return p;
		}
		else
		{
			p = p->m_pPartner;
		}
	}
	return NULL;
}

int32_t Player::GetTeamSize()
{
	int32_t size = 1;
	Player* p = m_pPartner;

	while( p )
	{
		++size;
		p = p->m_pPartner;
	}
	return size;
}

void Player::GetNoticeInfo(  uint8_t* des, int32_t& len, int32_t maxLen)
{
	if( !des )
	{
		return;
	}

	if( m_nNBSize + len > maxLen )
	{
		DEBUG_LOG( "Des Buff Size out of range Value:[%u], Max;[%u]", m_nNBSize + len, maxLen );
	}

	memcpy( des + len, m_noticeBuff, m_nNBSize );
	len += m_nNBSize;
}

void Player::NoticeBuffClear()
{
	//预留一个玩家操作头数据
	m_nNBSize = sizeof(int32_t)*3 + sizeof(PacketHead);
}

void Player::NoticeGetHead( uint8_t** buf, int32_t& len )
{
	*buf = m_noticeBuff;
	len = sizeof(PacketHead);
}

void Player::NoticeNewPacket(uint8_t**  buf, int32_t& len, uint32_t& spaceLeft )
{
	//此处返回一个数据缓冲区指针，但是外部并不知道缓冲区大小，如果数据量过大会导致溢出，需要修改
	*buf = m_noticeBuff + m_nNBSize;
	len = sizeof(PacketHead);
	spaceLeft = ACT_MSG_MAX_SIZE - m_nNBSize;
}

void Player::CompletePacket( uint8_t* buf, int32_t cmd, int32_t len, bool head )
{
	if( len + m_nNBSize < ACT_MSG_MAX_SIZE || head )
	{
		PacketHead* p = (PacketHead*)buf;
		//数据需要转化为网络字节
		int j;
		j = 0;
		ant::pack( &(p->m_nCmd), cmd, j );
		j = 0;
		ant::pack( &(p->m_nLen), len, j );

		if( !head )
		{
			m_nNBSize += len;
		}
	}
	else
	{
		DEBUG_LOG( "Player::CompletePacket  Buff Size Out Of Range Value:[%u]", m_nNBSize + len );
	}
}
