
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/dataformatter/bin_str.h>

#include "util.h"
#include "proto.h"

#include "picsrv.h"

#define PIC_SRV_DES_KEY "pIc:Svr@"
static ip_port_t pic_srv[PIC_SRV_MAX_NUM];
static int pic_srv_num = 0;

#ifdef TW_VER
#define TUYA_SRV_DES_KEY "pIc:Svr@"
#else
#define TUYA_SRV_DES_KEY "tVYa:vr@"
#endif
static ip_port_t tuya_srv[TUYA_SRV_MAX_NUM];
static int tuya_srv_num = 0;


/* @func		load picture server  config file picsrv.xml
  * @input 	cur_node, xml node pointer
  * @ret    	0 success, others error
  */
int load_picsrv_config(xmlNodePtr cur_node)
{
	pic_srv_num = 0;
	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"PicSrv")) {
			DECODE_XML_PROP_STR(pic_srv[pic_srv_num].ip, cur_node, "Ip");
			DECODE_XML_PROP_INT(pic_srv[pic_srv_num].port, cur_node, "Port");
			pic_srv[pic_srv_num].ip_int = inet_addr(pic_srv[pic_srv_num].ip);
			pic_srv_num++;
		}
		cur_node = cur_node->next;
	}
	return 0;
}

/*@ func 	choose the picture server,according to userid%pic_srv_num
  *@input		p, sprite_t
  *@ret		0 success, others error
  */
int get_pic_srv_ip_port_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

    char inbuf[8], outbuf[8];
	int index = p->id%pic_srv_num;
	int i = sizeof(protocol_t);

	(*(uint32_t*)&inbuf) =  htonl(p->id);
	(*(uint32_t*)(inbuf+4)) = htonl(get_now_tv()->tv_sec);
	des_encrypt(PIC_SRV_DES_KEY, inbuf, outbuf);
	PKG_STR(msg, pic_srv[index].ip, i, sizeof(pic_srv[index].ip));
	PKG_UINT16(msg, pic_srv[index].port, i);
	PKG_STR(msg, outbuf, i, sizeof(outbuf));

	(*(in_addr_t*)&inbuf) = pic_srv[index].ip_int;
	(*(uint32_t*)(inbuf+4)) = htonl(pic_srv[index].port);
	des_encrypt(PIC_SRV_DES_KEY, inbuf, outbuf);
	PKG_STR(msg, outbuf, i, sizeof(outbuf));

	const struct tm* tm_tmp1 = get_now_tm();
	if (tm_tmp1->tm_wday == 5){
		char tmp[33];
		uint8_t* str_sess = msg + sizeof(protocol_t) + 18;
		hex2str(str_sess, 16, tmp);
		DEBUG_LOG("GET PIC\t[%u %s]", p->id, tmp);
	}

	init_proto_head(msg, PROTO_PIC_SRV_IP_PORT, i);
	return send_to_self(p, msg, i, 1);
}

/* @func		load tuya server  config file tuya.xml
  * @input 	cur_node, xml node pointer
  * @ret    	0 success, others error
  */
int load_tuyasrv_config(xmlNodePtr cur_node)
{
	tuya_srv_num = 0;
	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar*)"TuyaSrv")) {
			DECODE_XML_PROP_STR(tuya_srv[tuya_srv_num].ip, cur_node, "Ip");
			DECODE_XML_PROP_INT(tuya_srv[tuya_srv_num].port, cur_node, "Port");
			tuya_srv[tuya_srv_num].ip_int = inet_addr(tuya_srv[tuya_srv_num].ip);
			tuya_srv_num++;
		}
		cur_node = cur_node->next;
	}
	return 0;
}

/*@ func 	choose the picture server,according to userid%pic_srv_num
  *@input		p, sprite_t
  *@ret		0 success, others error
  */
int get_tuya_srv_ip_port_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

    char inbuf[8], outbuf[8];
	int index = rand() % tuya_srv_num;
	int i = sizeof(protocol_t);

	(*(uint32_t*)&inbuf) = htonl(p->id);
	(*(uint32_t*)(inbuf+4)) = htonl(get_now_tv()->tv_sec);
	des_encrypt(TUYA_SRV_DES_KEY, inbuf, outbuf);
	PKG_STR(msg, tuya_srv[index].ip, i, sizeof(tuya_srv[index].ip));
	PKG_UINT16(msg, tuya_srv[index].port, i);
	PKG_STR(msg, outbuf, i, sizeof(outbuf));

	(*(in_addr_t*)&inbuf) = tuya_srv[index].ip_int;
	(*(uint32_t*)(inbuf+4)) = htonl(tuya_srv[index].port);
	des_encrypt(TUYA_SRV_DES_KEY, inbuf, outbuf);
	PKG_STR(msg, outbuf, i, sizeof(outbuf));

	init_proto_head(msg, PROTO_TUYA_SRV_IP_PORT, i);

	return send_to_self(p, msg, i, 1);
}

