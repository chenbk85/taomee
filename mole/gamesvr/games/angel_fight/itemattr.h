/*
 * =====================================================================================
 *
 *       Filename:  itemattr.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 11:37:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef	ITEM_ATTR_H
#define	ITEM_ATTR_H

#define ITEM_HAS_ADDED		1

class ItemAttr
{
public:
	int32_t m_nType;				//1--被动技能  0--天使(带技能的武器)
	int32_t m_nAddStr;				//力量
	int32_t m_nAddInt;				//智力
	int32_t m_nAddAgi;				//敏捷
	int32_t m_nAddHab;				//体质

	int32_t m_nAddAtk;				//攻击力
	int32_t	m_nAddASD;				//攻击速度
	int32_t m_nAddEva;				//闪避率
	int32_t	m_nAddCrit;				//暴击率
	int32_t m_nAddBlock;			//格挡
	int32_t m_nAddHit;				//命中
	int32_t	m_nAddCombo;			//连击
	int32_t m_nAddDef;				//防御
	int32_t m_nAddHp;
	int32_t m_nAddMp;
	int32_t m_nSkillID;				//技能索引
};

#endif

