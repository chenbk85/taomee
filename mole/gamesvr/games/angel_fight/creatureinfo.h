/*
 * =====================================================================================
 *
 *       Filename:  creatureinfo.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 10:13:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CREATURE_INFO_H
#define CREATURE_INFO_H

#define MAX_SKILL_COUNT		5				//最大技能数
#define MAX_ITEM_COUNT		5				//最大掉落物品数

class CreatureInfo
{
public:
	CreatureInfo();
	~CreatureInfo();
	void AddSkill( int32_t idx );
	void AddItem( int32_t idx );

	void XmlSetStr( char* val );
	void XmlSetInt( char* val );
	void XmlSetAgile( char* val );
	void XmlSetHabitus( char* val );
	void XmlSetLevel( char* val );
	void XmlAddItem( char* val );
	void XmlAddSkill( char* val );

	void XmlSetAtk( char* val );
	void XmlSetAspd( char* val );
	void XmlSetEvad( char* val );
	void XmlSetBlock( char* val );
	void XmlSetCrit( char* val );
	void XmlSetHit( char* val );
	void XmlSetCombo( char* val );
	void XmlSetDef( char* val );
	void XmlSetIsBoss( char* val );
public:
	int32_t m_nLevel;				//等级
	int32_t	m_nCreatureID;
	bool	m_bIsBoss;
	int32_t m_nStr;					//力量
	int32_t m_nInt;					//智力
	int32_t m_nAgile;				//敏捷
	int32_t m_nHabitus;				//体质

	int32_t m_nAttack;				//攻击力
	int32_t	m_nActSpeed;			//行动速度
	int32_t m_nEvasion;				//闪避率
	int32_t	m_nBlock;				//格挡率
	int32_t	m_nCritical;			//暴击率
	int32_t m_nHit;					//命中率
	int32_t	m_nCombo;				//连击
	int32_t m_nDef;					//防御

	int32_t m_nSkill[MAX_SKILL_COUNT];	//技能索引
	int32_t m_nSkillCount;
	int32_t m_nItem[MAX_ITEM_COUNT];
	int32_t m_nItemCount;
};

#endif
