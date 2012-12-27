/*
 * =====================================================================================
 *
 *       Filename:  mole_avatar.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/31/2011 02:08:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee, ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "item.h"
#include "central_online.h"
#include "mole_avatar.h"

int save_avatar_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );

	uint32_t db_buf[MAX_ITEMS_WITH_BODY + 1];
	if( p->item_cnt == 0 )
	{
		uint32_t ret = SAR_EMPTY;
		int send_len = sizeof(protocol_t);
		PKG_UINT32( msg, ret, send_len );
		init_proto_head( msg, p->waitcmd, send_len );
		return send_to_self( p, msg, send_len, 1 );
	}
	db_buf[0] = p->item_cnt;
	int j;
	for( j = 0; j < p->item_cnt; ++j )
	{
		if( p->items[j] != 0 )
		{
			db_buf[1+j] = p->items[j];
		}
	}
	++j;
	return send_request_to_db( SVR_PROTO_SAVE_AVATAR, p, j * sizeof(uint32_t), db_buf, p->id );
}

int save_avatar_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	//统计保存形象的玩家
	if( ret == SAR_OK )
	{
		uint32_t msg_id;
		if( ISVIP(p->flag) )
		{
			msg_id = 0x0409BD9B;
		}
		else
		{
			msg_id = 0x0409BD9C;
		}
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	int send_len = sizeof(protocol_t);
	PKG_UINT32( msg, ret, send_len );
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

int remove_avatar_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t avatar_id;
	CHECK_BODY_LEN( len, sizeof(avatar_id) );
	int j = 0;
	UNPKG_UINT32( body, avatar_id, j );
	return send_request_to_db( SVR_PROTO_REMOVE_AVATAR, p, sizeof(avatar_id), &avatar_id, p->id );
}

int remove_avatar_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t ret;
	CHECK_BODY_LEN( len, sizeof(ret) );
	ret = *(uint32_t*)buf;
	int send_len = sizeof(protocol_t);
	PKG_UINT32( msg, ret, send_len );
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

int get_avatar_list_cmd(sprite_t* p, const uint8_t* body, int len)
{
	return send_request_to_db( SVR_PROTO_GET_AVATAR_LIST, p, 0, NULL, p->id );
}

int get_avatar_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t avatar_cnt;
	uint32_t avatar_id;
	uint32_t item_cnt;
	uint32_t item_id;
	uint32_t expect_len = sizeof( avatar_cnt );
	CHECK_BODY_LEN_GE( len, expect_len );
	int j = 0;
	UNPKG_H_UINT32( buf, avatar_cnt, j );
	int send_len = sizeof(protocol_t);
	PKG_UINT32( msg, avatar_cnt, send_len );
	int avatar_idx;

	for( avatar_idx = 0; avatar_idx < avatar_cnt; ++avatar_idx )
	{
		UNPKG_H_UINT32( buf, avatar_id, j );
		PKG_UINT32( msg, avatar_id, send_len );
		UNPKG_H_UINT32( buf, item_cnt, j );

		expect_len += sizeof(avatar_id) + sizeof(item_cnt) + item_cnt*sizeof(item_id);
		CHECK_BODY_LEN_GE( len, expect_len );
		PKG_UINT32( msg, item_cnt, send_len );
		int item_idx;
		for( item_idx = 0; item_idx < item_cnt; ++item_idx )
		{

			UNPKG_H_UINT32( buf, item_id, j );
			PKG_UINT32( msg, item_id, send_len );
		}

	}
	init_proto_head( msg, p->waitcmd, send_len );
	return send_to_self( p, msg, send_len, 1 );
}

