/*
 * =====================================================================================
 *
 *       Filename:  skill.h
 *
 *    Description:  技能描述
 *
 *        Version:  1.0
 *        Created:  06/08/2011 04:14:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef SKILL_H
#define SKILL_H

#include "player.h"

typedef std::vector<int>		VecEffect_t;
typedef VecEffect_t::iterator	VecEffectIt_t;
typedef std::vector<int>		VecBuff_t;
typedef VecBuff_t::iterator		VecBuffIt_t;

class Skill
{
public:
	Skill( int32_t id );
	~Skill();
	void	OnSelect( Player* p, VecAddedEffect_t& vecEffect );
	void	Execute( Player* p, bool critical, VecAddedEffect_t& vecEffect );
	bool	IsEvil();
	int32_t	GetConsume();
	int32_t	GetUsePoint();
	int32_t GetDamage();
	VecBuff_t&	GetBuffID();

	void	XmlSetSkillID( char* val );
	void	XmlSetDamage( char* val );
	void	XmlSetConsume( char* val );
	void	XmlSetIsEvil( char* val );
	void	XmlAddBuffID( char* val );
	void	XmlAddEffectID( char* val );
	void	XmlSetUsePoint( char* val );
private:
	int32_t		m_nSkillID;
	int32_t		m_nDamage;
	int32_t		m_nConsume;
	bool		m_bIsEvil;
	int32_t		m_nUsePoint;				//使用次数
	VecBuff_t	m_vecBuff;				//技能使用后附加状态
	VecEffect_t m_vecEffect;			//技能特性,用特效描述

};

inline int32_t Skill::GetConsume()
{
	return m_nConsume;
}

inline bool Skill::IsEvil()
{
	return m_bIsEvil;
}

inline int32_t Skill::GetUsePoint()
{
	return m_nUsePoint;
}

inline int32_t Skill::GetDamage()
{
	return m_nDamage;
}

inline VecBuff_t& Skill::GetBuffID()
{
	return m_vecBuff;
}

#endif
