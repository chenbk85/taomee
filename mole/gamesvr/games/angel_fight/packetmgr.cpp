/*
 * =====================================================================================
 *
 *       Filename:  packetmgr.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/15/2011 03:18:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
/*
extern "C"
{
#include <libtaomee/log.h>
}

#include "../../ant/inet/pdumanip.hpp"

#include <algorithm>
#include "packetmgr.h"


PacketMgr	g_packetMgr;

PacketMgr::PacketMgr()
{

}

PacketMgr::~PacketMgr()
{

}

void PacketMgr::clear()
{
	m_nPacketLen = 0;
}

bool PacketMgr::push_back( uint8_t* buf, int32_t len )
{
	if( !buf )
	{
		ERROR_LOG( "PacketMgr::push_back Invalid buf ptr" );
		return false;
	}

	if( m_nPacketLen + len < PACKET_BUF_MAX_SIZE )
	{
		memcpy( m_pBuf + m_nPacketLen, buf, len );
		m_nPacketLen += len;
		return true;
	}
	else
	{
		return false;
	}
}

void PacketMgr::get_data( uint8_t* des, int32_t& len, int32_t maxlen )
{
	if( int32_t(m_nPacketLen) + len < maxlen )
	{
		memcpy( des+len, m_pBuf, m_nPacketLen );
		len += m_nPacketLen;
	}
	else
	{
		ERROR_LOG( "PacketMgr::get_data buf length out of range" );
	}
}

void PacketMgr::new_packet(uint8_t * buf,int32_t& len)
{
	len = sizeof(PacketHead);
}

void PacketMgr::complete_packet( uint8_t * buf, int32_t cmd, int32_t len )
{
	PacketHead* p = (PacketHead*)buf;
	int j;
	j = 0;
	ant::pack( &p->m_nCmd, cmd, j );
	j = 0;
	ant::pack( &p->m_nLen, len, j );
}

*/

