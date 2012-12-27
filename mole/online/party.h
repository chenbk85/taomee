
/*
 *@ mole party api
 */

#ifndef PARTY_H
#define PARTY_H


#define MAX_ONLINE_ID	1000
#define CHECK_VALID_ONLINE_ID(p_, id) \
	if((id) > MAX_ONLINE_ID) ERROR_RETURN(("bad online id\t[uid=%u]", (p_)->id), -1)

#define db_set_mole_party(p_, buf)	\
		send_request_to_db(SVR_PROTO_SET_MOLE_PARTY, (p_), 44, buf, 0)


int get_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen);
int set_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_own_mole_party_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_party_count_cmd(sprite_t* p, uint8_t* body, int bodylen);

int get_mole_party_callback(sprite_t * p,uint32_t id,char * buf,int len);
int set_mole_party_callback(sprite_t * p,uint32_t id,char * buf,int len);
int get_own_mole_party_callback(sprite_t * p,uint32_t id,char * buf,int len);
int get_party_count_callback(sprite_t * p,uint32_t id,char * buf,int len);

#endif

