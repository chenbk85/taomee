#include "central_online.h"
#include "dbproxy.h"
#include "proto.h"
#include "util.h"

#include "sand_pool.h"


int get_time_mail_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    return  send_request_to_db(SVR_PROTO_USER_GET_TIME_MAIL, p, 0, NULL, p->id);
}

int get_time_mail_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(sand_pool_mail_t));

	const sand_pool_mail_t* last_mail = (void*)buf;
	CHECK_BODY_LEN(len, sizeof(sand_pool_mail_t) + last_mail->msglen);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, last_mail->flag, i);
	PKG_UINT32(msg, last_mail->sent_tm, i);
	PKG_UINT16(msg, last_mail->msglen, i);
	PKG_STR(msg, last_mail->msg, i, last_mail->msglen);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int send_time_mail_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    send_mail_buff_t *p_mail = (void *)p->session;
	int i = 0;
	CHECK_BODY_LEN_GE(bodylen, 2);
	UNPKG_UINT16(body, p_mail->msglen, i);
	DEBUG_LOG("p_mail->msglen:%d ", p_mail->msglen);

	if (p_mail->msglen > 150)
	{
	    DEBUG_LOG("p_mail->msglen:%d ", p_mail->msglen);
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	CHECK_BODY_LEN(bodylen, p_mail->msglen + 2);
	UNPKG_STR(body, p_mail->msg, i, p_mail->msglen);
	p_mail->msg[p_mail->msglen - 1] = '\0';

	CHECK_DIRTYWORD(p, p_mail->msg);

    return  send_request_to_db(SVR_PROTO_USER_SEND_TIME_MAIL, p, p_mail->msglen + 2, p_mail, p->id);
}

int send_time_mail_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B444, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_time_item_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    return  send_request_to_db(SVR_PROTO_USER_GET_TIME_ITEM, p, 0, NULL, p->id);
}

int get_time_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	int j = 0;
	uint32_t itemid = 0;
	uint32_t flag  = 0;
 	UNPKG_H_UINT32(buf,itemid,j);
 	UNPKG_H_UINT32(buf,flag,j);

    if (itemid != 0)
    {
 	    db_exchange_single_item_op(p, 202, itemid, 1, 0);
 	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, i);
	PKG_UINT32(msg, flag, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int send_time_item_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    uint32_t itemid = 0;
	if ( unpkg_uint32(body, bodylen, &itemid) == -1) {
		return -1;
	}

    if (itemid < 1351110 || itemid > 1351120)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    return  send_request_to_db(SVR_PROTO_USER_SEND_TIME_ITEM, p, 4, &itemid, p->id);
}

int send_time_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B443, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int get_time_mail_item_count_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    return  send_request_to_db(SVR_PROTO_USER_GET_MAIL_ITEM_COUNT, p, 0, NULL, p->id);
}

int get_time_mail_item_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B445, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

    uint32_t mail_cnt = 0;
    uint32_t item_cnt = 0;
    int j = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_H_UINT32(buf,mail_cnt,j);
	UNPKG_H_UINT32(buf,item_cnt,j);

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, mail_cnt, i);
	PKG_UINT32(msg, item_cnt, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}



