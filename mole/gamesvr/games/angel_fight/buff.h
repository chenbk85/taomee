/*
 * =====================================================================================
 *
 *       Filename:  buff.h
 *
 *    Description:  ¼¼ÄÜ¸½¼Ó×´Ì¬
 *
 *        Version:  1.0
 *        Created:  06/13/2011 10:29:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef BUFF_H
#define	BUFF_H
#include "player.h"

class Buff
{
public:
	Buff();
	~Buff();
	void	OnAdd( Player* p , VecAddedEffect_t& vecEffect );
	void	OnRemove( Player* p , VecAddedEffect_t& vecEffect );
	void	Execute( Player* p, int32_t roundLeft , VecAddedEffect_t& vecEffect );
	int32_t	GetRoundLasts();

	void	XmlAddEffect( char* val );
	void	XmlSetLasts( char* val );

private:
	typedef std::vector<int>		VecEffect_t;
	typedef VecEffect_t::iterator	VecEffectIt_t;

	VecEffect_t	m_vecEffect;
	uint32_t	m_nRoundLasts;
};

#endif
