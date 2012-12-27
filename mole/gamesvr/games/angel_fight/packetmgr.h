/*
 * =====================================================================================
 *
 *       Filename:  packetmgr.h
 *
 *    Description:  内部数据打包
 *
 *        Version:  1.0
 *        Created:  06/15/2011 03:11:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef PACKET_MGR_H
#define PACKET_MGR_H

#define PACKET_BUF_MAX_SIZE	2048

enum PACKET_CMD
{
	PC_BUFF_EFFECT	= 1,
	PC_ATTACK		= 2,
	PC_COMBO		= 3,
	PC_SKILL		= 4,
	PC_ADD_BUFF		= 5,
	PC_REMOVE_BUFF	= 6,
	PC_BLOCK		= 7,
	PC_EVAD	 		= 8,
	PC_REST			= 9,
	PC_ID			= 10,
};

struct AttackInfo
{
	int8_t		m_isBlock;
	int8_t		m_isCritical;
	int32_t		m_nDamage;
}__attribute__((packed));

struct SkillAtkInfo
{
	int8_t		m_isToSelf;
	int32_t		m_nSkillID;
	int8_t		m_isEvad;
	int8_t		m_isCritical;
	int32_t		m_nDamage;
	int32_t		m_nMpConsume;
}__attribute__((packed));

struct PacketHead
{
	uint32_t	m_nCmd;
	uint32_t	m_nLen;
};
/*
class PacketMgr
{
public:
	PacketMgr();
	~PacketMgr();
	void	clear();
	bool	push_back( uint8_t* buf, int32_t len );
	void	get_data( uint8_t* des, int32_t& len, int32_t maxLen );
	void	new_packet( uint8_t* buf, int32_t& len );
	void	complete_packet( uint8_t* buf, int32_t cmd, int32_t len );
private:
	uint8_t		m_pBuf[PACKET_BUF_MAX_SIZE];
	uint32_t	m_nPacketLen;
};

extern PacketMgr g_packetMgr;
*/
#endif
