/*
 * =====================================================================================
 *
 *       Filename:  BuffMgr.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/13/2011 04:44:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, EricLee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef BUFF_MGR_H
#define BUFF_MGR_H

class BuffMgr
{
public:
	BuffMgr();
	~BuffMgr();
	Buff*	GetBuff( int32_t idx );
	int		LoadBuffInfo();
private:

	typedef std::vector<Buff*> 		VecBuffDef_t;
	typedef VecBuffDef_t::iterator	VecBuffDefIt_t;

	VecBuffDef_t		m_vecBuffDef;

};

extern BuffMgr	g_buffMgr;


#endif
