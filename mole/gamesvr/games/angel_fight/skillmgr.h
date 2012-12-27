/*
 * =====================================================================================
 *
 *       Filename:  skillmgr.h
 *
 *    Description:  技能管理器
 *
 *        Version:  1.0
 *        Created:  06/13/2011 03:57:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef SKILL_MGR_H
#define SKILL_MGR_H

class Skill;

class SkillMgr
{
public:
	SkillMgr();
	~SkillMgr();
	Skill*	GetSkill( int32_t idx );
	int		LoadSkillInfo();
private:
	typedef std::vector<Skill*>		VecSkill_t;
	typedef VecSkill_t::iterator	VecSkillIt_t;

	VecSkill_t	m_vecSkill;
};

extern SkillMgr g_skillMgr;
#endif

