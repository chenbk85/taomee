#include "mail.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "item.hpp"
#include "login.hpp"
#include <libtaomee/project/utilities.h>
#include "switch.hpp"
#include "global_data.hpp"
#include "stat_log.hpp"
#include "common_op.hpp"

using namespace std;
using namespace taomee;

//-----------------------------------------------------------
//  function implement
//-----------------------------------------------------------

//---------------------------------------------static function----------------------------------------------------------//


//format num enclosure info to string 
static bool mail_numerical_enclosure_to_str( vector<mail_numerical_enclosure> *mail_nums,  char* buf, uint32_t buflen);
//format string to num enclosure info
static bool str_to_mail_numberical_enclosure(char* buf, vector<mail_numerical_enclosure> *mail_nums);
//format item enclosure info to string
static bool mail_item_enclosure_to_str( vector<mail_item_enclosure>* mail_items, char* buf, uint32_t buflen);
//format string to item enclosure info 
static bool str_to_mail_item_enclosure(char* buf,  vector<mail_item_enclosure>* mail_items);
//format equip enclosure info to string
static bool mail_equip_enclosure_to_str( vector<mail_equip_enclosure>* mail_equips,  char* buf, uint32_t buflen);
//format string to equip enclosure info
static bool str_to_mail_equip_enclosure(char* buf,  vector<mail_equip_enclosure>* mail_equips);


/**
 *   @brief format num enclosure info to string
 *   @param  vector<mail_numerical_enclosure>*,   char* ,  uint32
 *   @return  true sucess, false otherwirse fail
**/
bool mail_numerical_enclosure_to_str( vector<mail_numerical_enclosure> *mail_nums,  char* buf, uint32_t buflen)
{
	if(mail_nums == NULL){
		return false;
	}
	memset(buf, 0, buflen);
	uint32_t i = 0, j = 0;
	for(i = 0; i < mail_nums->size(); i++)
	{
		if( buflen -j >= 4){
			j += sprintf(buf + j, "%u %u\t", (*mail_nums)[i].type, (*mail_nums)[i].number );				
		}
		else
		{
			return false;
		}
	}	
	if(j > 0){
		buf[j-1] = '\0';
	}
	return true;
}


/**
 *   @brief format string to num enclosure info
 *   @param char*,  vector<mail_numerical_enclosure>*
 *   @return  true sucess, false otherwirse fail
**/
bool str_to_mail_numberical_enclosure(char* buf, vector<mail_numerical_enclosure> *mail_nums)
{
	if(mail_nums == NULL)return false;
	mail_nums->clear();	
	char* token = NULL;
	token = strtok(  buf, "\t");
	while(token)
	{
		mail_numerical_enclosure num;
		sscanf(token, "%u %u", &num.type, &num.number);
		mail_nums->push_back(num);
		token = strtok( NULL, "\t");
	}	
	return true;
}


/**
 *   @brief format item enclosure info to string
 *   @param  vector<mail_item_enclosure>*,   char* ,  uint32
 *   @return  true sucess, false otherwirse fail
**/
bool mail_item_enclosure_to_str( vector<mail_item_enclosure>* mail_items, char* buf, uint32_t buflen)
{
	if(mail_items == NULL){
		return false;
	}
	memset(buf, 0, buflen);
	uint32_t i = 0, j = 0;
	for(i =0; i< mail_items->size(); i++)
	{
		if(buflen - j >= 4){
			TRACE_LOG("%u %u\t", (*mail_items)[i].item_id, (*mail_items)[i].item_count);
			j += sprintf(buf+j, "%u %u\t", (*mail_items)[i].item_id, (*mail_items)[i].item_count);
		}
		else
		{
			return false;
		}
	}
	if(j > 0){
		buf[j -1 ] = '\0';
	}
	return true;
}



/**
 *   @brief format string to item enclosure info
 *   @param char*,  vector<item_numerical_enclosure>*
 *   @return  true sucess, false otherwirse fail
**/
bool str_to_mail_item_enclosure(  char* buf,  vector<mail_item_enclosure>* mail_items)
{
	if(mail_items == NULL)return false;
	char* token = NULL;
	mail_items->clear();

	token = strtok(buf, "\t");
	while(token)
	{
		mail_item_enclosure item;
		sscanf(token, "%u %u", &item.item_id, &item.item_count);
		mail_items->push_back(item);
		token = strtok(NULL, "\t");
	}
	return true;
}


/**
 *   @brief format equip enclosure info to string
 *   @param  vector<equip_item_enclosure>*,   char* ,  uint32
 *   @return  true sucess, false otherwirse fail
**/
bool mail_equip_enclosure_to_str( vector<mail_equip_enclosure>* mail_equips,  char* buf, uint32_t buflen)
{
	if(mail_equips == NULL){
		return false;
	}
	memset(buf, 0, buflen);
	uint32_t i =0, j = 0;
	for( i =0; i< mail_equips->size(); i++)
	{
		if(buflen - j >= 10){
			TRACE_LOG("%u %u %u %u %u\t", (*mail_equips)[i].equip_id, (*mail_equips)[i].equip_get_time, (*mail_equips)[i].equip_rank, (*mail_equips)[i].equip_duration, (*mail_equips)[i].equip_lifetime);
			j += sprintf(buf + j, "%u %u %u %u %u\t", (*mail_equips)[i].equip_id, (*mail_equips)[i].equip_get_time, (*mail_equips)[i].equip_rank, (*mail_equips)[i].equip_duration, (*mail_equips)[i].equip_lifetime);
		}
		else
		{
			return false;
		}
	}
	if( j > 0){
		buf[j - 1] = '\0';
	}
	return true;
}


/**
 *   @brief format string to equip enclosure info
 *   @param char*,  vector<equip_numerical_enclosure>*
 *   @return  true sucess, false otherwirse fail
**/
bool str_to_mail_equip_enclosure( char* buf,  vector<mail_equip_enclosure>* mail_equips)
{
	if(mail_equips == NULL){
		return false;
	}
	mail_equips->clear();
	char* token = NULL;
	token = strtok(buf, "\t");
	while(token)
	{
		mail_equip_enclosure equip;
		sscanf(token, "%u %u %u %u %u",  &equip.equip_id, &equip.equip_get_time, &equip.equip_rank, &equip.equip_duration, &equip.equip_lifetime);
		mail_equips->push_back(equip);
		token = strtok(NULL, "\t");
	}
	return true;
}

uint32_t get_daily_temple_mail_count(player_t *p, uint32_t temple_id)
{
    uint32_t beg_tm = get_today_begin_second(0);
    uint32_t end_tm = get_today_end_second(0);
    uint32_t mail_cnt = 0;
    std::map<uint32_t, mail_data>::iterator it = p->m_read_mails->begin();
    for (; it != p->m_read_mails->end(); ++it) {
        if (it->second.mail_templet == temple_id && 
            (beg_tm < it->second.mail_time && end_tm > it->second.mail_time) ) {
            mail_cnt++;
        }
    }

    it = p->m_unread_mails->begin();
    for (; it != p->m_unread_mails->end(); ++it) {
        if (it->second.mail_templet == temple_id && 
            (beg_tm < it->second.mail_time && end_tm > it->second.mail_time)) {
            mail_cnt++;
        }
    }
    return mail_cnt;
}

//-------------------------------------------extern function-----------------------------------------------------//
/**
 *   @brief initialization player's mail 
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int init_player_mail(player_t *p)
{
	p->m_read_mails = new std::map<uint32_t, mail_data>();
	p->m_unread_mails = new std::map<uint32_t, mail_data>();
	return 0;
}


/**
 *   @brief uninitialization player's mail
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int final_player_mail(player_t *p)
{
	if(p->m_read_mails)
	{
		delete p->m_read_mails;
		p->m_read_mails = NULL;
	}
	if(p->m_unread_mails)
	{
		delete p->m_unread_mails;
		p->m_unread_mails = NULL;
	}
	return 0;
}

/**
 *   @brief send mail with item
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
void send_add_item_mail(player_t*p, uint32_t itemid, uint32_t cnt)
{
	char title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
	std::vector<mail_item_enclosure> item_vec;
	std::vector<mail_equip_enclosure> equips;

	strcpy(title,"售卖奖励!");
	strcpy(content,"您好，请查收附件!");
	TRACE_LOG("mail: %s ===== %s", title, content);
	item_vec.push_back(mail_item_enclosure(itemid, cnt));
	db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
}

/**
 *   @brief send mail list request to dbserver
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int db_mail_head_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_mail_head_list, 0, 0);
}


/**
 *   @brief callback function for mail list db request
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int db_mail_head_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	mail_head_list_rsp_t *rsp = reinterpret_cast<mail_head_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(mail_head_list_rsp_t) + rsp->count*sizeof(mail_header));
	init_player_mail_head_list(p, rsp);
	return send_player_mail_head_list(p);
}

/**
 *   @brief init player mail list
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int init_player_mail_head_list(player_t* p, mail_head_list_rsp_t* rsp)
{
	p->m_read_mails->clear();
	p->m_unread_mails->clear();
	for( uint32_t i =0; i< rsp->count; i++)
	{
		add_player_mail(p, rsp->headers[i]);	
	}
	return 0;	
}

/**
 *    @brief add mail to player from mail data
 *    @param player_t*, mail_data&
 *    @return  true sucess, false otherwirse fail
**/
bool add_player_mail(player_t* p, mail_data& data)
{
	if( is_mail_exist(p, data.mail_id)){
		return false;
	}
	if( data.mail_state == mail_state_read){
		(*(p->m_read_mails))[data.mail_id] = data;
	}
	if( data.mail_state == mail_state_unread){
		((*p->m_unread_mails))[data.mail_id] = data;
	}
	return false;
}


/**
 *    @brief add mail to player from mail header
 *    @param player_t*, mail_header&
 *    @return  true sucess, false otherwirse fail
**/
bool add_player_mail(player_t* p, mail_header& header)
{
	mail_data data;
	memcpy(&data, &header, sizeof(header));
	return add_player_mail(p, data);
}



/**
 *   @brief delete mail from player by mail id
 *   @param player_t*, uin32_t 
 *   @return  true sucess, false otherwirse fail
**/
bool delete_player_mail(player_t* p, uint32_t mail_id)
{
	std::map<uint32_t, mail_data>::iterator pItr = p->m_read_mails->find(mail_id);
	if(pItr != p->m_read_mails->end()){
		p->m_read_mails->erase(pItr);
		return true;
	}	
	pItr = p->m_unread_mails->find(mail_id);
	if(pItr != p->m_unread_mails->end()){
		p->m_unread_mails->erase(pItr);
		return true;
	}
	return false;
}

/**
 *    @brief get mail data by mail id
 *    @param player_t*, uint32_t
 *    @return  mail_data* sucess, NULL otherwirse fail
**/
mail_data* get_player_mail(player_t* p, uint32_t mail_id)
{
	std::map<uint32_t, mail_data>::iterator pItr = p->m_read_mails->find(mail_id);
	if(pItr != p->m_read_mails->end()){
		return &(pItr->second);
	}
	pItr = p->m_unread_mails->find(mail_id);
	if(pItr != p->m_unread_mails->end()){
		return &(pItr->second);
	}
	return NULL;
}

/**
 *  @brief chech the mail exist
 *  @param player_t*, uint32_t
 *  @return  true sucess, false otherwirse fail
**/
bool is_mail_exist(player_t *p, uint32_t mail_id)
{
	if(  p->m_read_mails->find(mail_id) != p->m_read_mails->end()){
		return true;
	}	
	if( p->m_unread_mails->find(mail_id) != p->m_unread_mails->end() ){
		return true;
	}
	return false;
}


/**
 *   @brief check the mail has enclosure
 *   @param player_t*, uint32_t
 *   @return  true sucess, false otherwirse fail
**/
bool is_mail_include_enclosure(player_t* p, uint32_t mail_id)
{
	mail_data *pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return false;
	}
	if(pData->mail_nums.size() == 0 && pData->mail_items.size() == 0 && pData->mail_equips.size() == 0){
		return false;
	}
	return true;
}


/**
 *   @brief clear mail enclosure
 *   @param player_t*, uint32_t
 *   @return  void
**/
void clear_mail_enclosure(player_t* p, uint32_t mail_id)
{
	mail_data *pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return ;
	}
	pData->mail_nums.clear();
	pData->mail_items.clear();
	pData->mail_equips.clear();
}


/**
 *    @brief check mail has been read
 *    @param player_t*, uint32_t
 *    @return  true sucess, false otherwirse fail
**/
bool has_mail_read(player_t* p, uint32_t mail_id)
{
	mail_data *pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return false;
	}
	return pData->mail_state == mail_state_read;
}

/**
 *   @brief process mail body cmd from client
 *   @param player_t*,
 *   @return  0 sucess, -1 otherwirse fail
**/
int mail_body_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mail_id = 0;
	unpack(body, mail_id, idx);
	mail_data* pData = get_player_mail(p, mail_id);
	if( pData == NULL ){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	return db_mail_body(p, mail_id);
}


/**
 *   @brief send mail body db request to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_mail_body(player_t* p, uint32_t mail_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, mail_id, idx);
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_mail_body, dbpkgbuf, idx);
}


/**
 *   @brief db callback function for sending mail body db request
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_mail_body_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{	
	CHECK_DBERR(p, ret);
	mail_body_rsp_t *rsp = reinterpret_cast<mail_body_rsp_t*>(body);
	mail_data* pData = get_player_mail(p, rsp->mail_id);
	if(pData == NULL)
	{
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	strcpy(pData->mail_content, rsp->mail_body);
	str_to_mail_numberical_enclosure( rsp->mail_numerical_enclosure,  &(pData->mail_nums));	
	str_to_mail_item_enclosure(rsp->mail_item_enclosure, &(pData->mail_items));
	str_to_mail_equip_enclosure(rsp->mail_equip_enclosure, &(pData->mail_equips));
	pData->mail_state = mail_state_read;
	return send_mail_body_info(p, rsp->mail_id);
}



/**
 *   @brief send mail body info to client
 *   @param player_t*, uint32_t
 *   @return 0 sucess, -1 otherwirse fail
**/
int send_mail_body_info(player_t*p, uint32_t mail_id)
{
	mail_data* pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pData->mail_id, idx);
	pack(pkgbuf, pData->mail_content, sizeof(pData->mail_content), idx);
	
	uint32_t num_count = pData->mail_nums.size(); 
	uint32_t item_count = pData->mail_items.size();
	
	//这个地方吧数值附件和物品附件合并
	uint32_t i   = 0;
	pack(pkgbuf, num_count + item_count, idx);
	for(i = 0; i < num_count; i++)
	{
		pack(pkgbuf, pData->mail_nums[i].type, idx);
		pack(pkgbuf, pData->mail_nums[i].number, idx);
	}
	
	for(i = 0; i< item_count; i++)
	{
		pack(pkgbuf, pData->mail_items[i].item_id, idx);
		pack(pkgbuf, pData->mail_items[i].item_count, idx);
	}

	uint32_t equip_count = pData->mail_equips.size();
	pack(pkgbuf, equip_count, idx);
	for(i = 0; i< equip_count; i++)
	{
		pack(pkgbuf, pData->mail_equips[i].equip_id, idx);
		//pack(pkgbuf, pData->mail_equips[i].equip_get_time, idx);
		//pack(pkgbuf, pData->mail_equips[i].equip_rank, idx);
		//pack(pkgbuf, pData->mail_equips[i].equip_duration, idx);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
 *   @brief process delete mail cmd from client
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int delete_mail_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mail_id = 0;
	unpack(body, mail_id, idx);
	if(! is_mail_exist(p, mail_id) ){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	return db_delete_mail(p, mail_id);
}

/**
    *    @brief send delete mail db request to dbserver
	*    @param player_t*, uint32_t
	*    @return  0 sucess, -1 otherwirse fail     
**/
int db_delete_mail(player_t* p, uint32_t mail_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, mail_id, idx);
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_delete_mail, dbpkgbuf, idx);
}


/**
 *    @brief db callback function for sending delete mail db request
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_delete_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	delete_mail_rsp_t *rsp = reinterpret_cast<delete_mail_rsp_t*>(body);
	if( !is_mail_exist(p, rsp->mail_id) )
	{
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	delete_player_mail(p, rsp->mail_id);
	
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->mail_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
	
	/*
	该对象封装了对内存的PACK和UNPACK操作，做了边界检查，防止内存MEMCPY溢出
	int idx = sizeof(cli_proto_t);
	net_stream.clear();
	net_stream.seek(idx);
	net_stream.put_uint32(rsp->mail_id);
	init_cli_proto_head((void*)net_stream.get_buf(),  p, p->waitcmd, net_stream.get_cur_buf_len());
	return send_to_player(p, (void*)net_stream.get_buf(), net_stream.get_cur_buf_len(), 1);
	*/
}

/**
 *    @brief process take mail enclosure cmd from client
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int take_mail_enclosure_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t mail_id = 0;
	unpack(body, mail_id, idx);
	if( !is_mail_exist(p, mail_id) ){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	if( !is_mail_include_enclosure(p, mail_id)){
		return send_header_to_player(p, p->waitcmd, cli_err_mail_no_enclosure, 1);
	}
	KDEBUG_LOG(p->id, "take mail\t player_id:%u mail_id:%u", p->id, mail_id);
	return db_take_mail_enclosure(p, mail_id);
}


/**
 *   @brief send take mail enclosure db request to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_take_mail_enclosure(player_t* p, uint32_t mail_id)
{
	int idx = 0;
    mail_data* pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	uint32_t count1 = pData->mail_nums.size();
	uint32_t count2 = pData->mail_items.size();
	uint32_t count3 = pData->mail_equips.size();

	uint32_t i     = 0;
	pack_h(dbpkgbuf, mail_id, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	pack_h(dbpkgbuf, count1, idx);
	pack_h(dbpkgbuf, count2, idx);
	pack_h(dbpkgbuf, count3, idx);
	for(i = 0; i < count1; i++)
	{
		pack_h(dbpkgbuf, pData->mail_nums[i].type, idx);
		pack_h(dbpkgbuf, pData->mail_nums[i].number, idx);
	}
	for(i = 0; i< count2; i++)
	{
		pack_h(dbpkgbuf, pData->mail_items[i].item_id, idx);
		pack_h(dbpkgbuf, pData->mail_items[i].item_count, idx);
		const GfItem* pItem = items->get_item( pData->mail_items[i].item_id);
		uint32_t max_count = 0;
		if(pItem != NULL){
			max_count = pItem->max();
		}
		pack_h(dbpkgbuf, max_count, idx);
	}

	for(i = 0; i< count3; i++)
	{
		pack_h(dbpkgbuf, pData->mail_equips[i].equip_id, idx);
		pack_h(dbpkgbuf, pData->mail_equips[i].equip_get_time, idx);
		pack_h(dbpkgbuf, pData->mail_equips[i].equip_rank, idx);
		pack_h(dbpkgbuf, pData->mail_equips[i].equip_duration, idx);
		pack_h(dbpkgbuf, pData->mail_equips[i].equip_lifetime, idx);
        /*TRACE_LOG("send mail:[%u %u %u %u]",pData->mail_equips[i].equip_id, pData->mail_equips[i].equip_get_time,
            pData->mail_equips[i].equip_duration, pData->mail_equips[i].equip_lifetime); */
	}
    return send_request_to_db(p, p->id, p->role_tm,  dbproto_take_mail_enclosure, dbpkgbuf, idx);	
}


/**
 *    @brief db callback funtion for sending take mail enclosure db request
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_take_mail_enclosure_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t mail_id = 0;
	uint32_t mail_nums_count = 0;
	uint32_t mail_items_count = 0;
	uint32_t mail_equips_count = 0;
	int idx = 0;
	int cli_idx = 0;
	uint32_t i = 0;

	cli_idx = sizeof(cli_proto_t);

	unpack_h(body, mail_id, idx);
	pack(pkgbuf, mail_id, cli_idx);

	mail_data* pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return send_header_to_player(p, p->waitcmd,  cli_err_mail_id_not_exist, 1);
	}
	unpack_h(body, mail_nums_count, idx);
	int  cur_cli_idx = cli_idx;
	pack(pkgbuf, mail_nums_count, cli_idx);

	for(i =0; i< mail_nums_count; i++)
	{
		uint32_t type = 0;
		uint32_t number = 0;
		unpack_h(body, type, idx);
		unpack_h(body, number, idx);

		pack(pkgbuf, type, cli_idx);
		pack(pkgbuf, number, cli_idx);

		if(type == 1)//功夫豆
		{
			p->coins += number;
			do_stat_log_coin_add_reduce(p, number, (uint32_t)0, channel_string_mail);
		}
		else if(type == 2)//EXP
		{
			p->exp += number;	
		}
		else if(type == 3)//技能点
		{
			p->skill_point += number;
		}
		else if(type == 4)//伏魔点
		{
			p->fumo_points_total += number;
			do_stat_log_fumo_add_reduce(number, (uint32_t)0);
		}
	}


	unpack_h(body, mail_items_count, idx);
	pack(pkgbuf, mail_items_count + mail_nums_count, cur_cli_idx);

	for(i = 0; i< mail_items_count; i++)
	{
		uint32_t item_id = 0;
        uint32_t item_count = 0;
		unpack_h(body, item_id, idx);
		unpack_h(body, item_count, idx);

		pack(pkgbuf, item_id, cli_idx);
		pack(pkgbuf, item_count, cli_idx);	

		ret = p->my_packs->add_item(p, item_id, item_count, channel_string_mail);	
	}

	unpack_h(body, mail_equips_count, idx);
	pack(pkgbuf, mail_equips_count, cli_idx);
	for(i =0; i< mail_equips_count; i++)	
	{
		uint32_t attire_id = 0;
		uint32_t attire_get_time = 0;
		uint32_t attire_duration = 0;
		uint32_t attire_rank = 0;
		uint32_t attire_index = 0;
		uint32_t attire_endtime = 0;
		unpack_h(body, attire_id, idx);
		unpack_h(body, attire_rank, idx);
		unpack_h(body, attire_get_time, idx);
		unpack_h(body, attire_duration, idx);
		unpack_h(body, attire_endtime, idx);
		unpack_h(body, attire_index, idx);
		ret = p->my_packs->add_clothes(p, attire_id, attire_index, attire_duration, channel_string_mail,attire_get_time,attire_endtime,0);
		pack(pkgbuf, attire_index, cli_idx);
		pack(pkgbuf, attire_id,  cli_idx);
		pack(pkgbuf, attire_get_time,  cli_idx);
		pack(pkgbuf, attire_endtime,  cli_idx);
		//pack(pkgbuf, attire_duration, cli_idx);
	}

	clear_mail_enclosure(p, mail_id);	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, cli_idx);	
	return send_to_player(p, pkgbuf, cli_idx, 1);
}

/**
 *   @brief process send mail cmd from client
 *   @param player_t* 
 *   @return  0 sucess, -1 otherwirse fail
**/
int send_mail_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t mail_templet = 0;
	uint32_t mail_receive_id = 0;
	char     mail_title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char     mail_content[MAX_MAIL_CONTENT_LEN + 1] = {0};
	int idx = 0;

	unpack(body, mail_receive_id, idx);
	unpack(body, mail_templet, idx);
	unpack(body, mail_title, MAX_MAIL_TITLE_LEN, idx);
	unpack(body, mail_content, MAX_MAIL_CONTENT_LEN, idx);

	//check dirty word
	CHECK_DIRTYWORD(p, mail_title);
	CHECK_DIRTYWORD(p, mail_content);

	//check money
	if(p->coins < MAX_MAIL_PAY_FOR){
		return send_header_to_player(p, p->waitcmd, cli_err_not_enough_money, 1);
	}	
	//check uid
	if(p->id == mail_receive_id || !is_valid_uid(mail_receive_id)){
		return send_header_to_player(p, p->waitcmd,  cli_err_invalid_receive_id , 1);
	}

	return db_send_mail(p, mail_title, sizeof(mail_title), mail_content, sizeof(mail_content), mail_templet, mail_receive_id);
}

/**
 *   @brief send  new mail db request to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_send_mail(player_t* p, const char* title, uint32_t title_len,  const char* content, uint32_t content_len, uint32_t mail_templet, uint32_t player_id)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->id, idx);
    pack_h(dbpkgbuf, player_id, idx);
	pack_h(dbpkgbuf, mail_templet, idx);
	pack_h(dbpkgbuf, MAX_MAIL_COUNT, idx);
	pack(dbpkgbuf, title, title_len, idx);
	pack(dbpkgbuf, content, content_len, idx);
    return send_request_to_db(p, player_id, 0,  dbproto_send_mail, dbpkgbuf, idx);	
}



/**
 *   @brief db callback funtion for sending new mail 
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_send_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = 0;
	send_mail_rsp_t *rsp = reinterpret_cast<send_mail_rsp_t*>(body); 
	//notify the receiver new mail
	mail_header header;
	header.mail_id = rsp->mail_id;
	header.mail_time = rsp->mail_time;
	header.mail_state = rsp->mail_state;
	header.mail_templet = rsp->mail_templet;
	header.mail_type = rsp->mail_type;
	header.sender_id = rsp->sender_id;
	header.sender_role_tm = 0;
	strcpy(header.mail_title, rsp->mail_title);

	notify_user_new_mail(p, &header, rsp->receive_id);

	//reduce money no callback
	db_reduce_money(p, MAX_MAIL_PAY_FOR);	
	p->coins -= MAX_MAIL_PAY_FOR;

	do_stat_log_coin_add_reduce(p, (uint32_t)0, MAX_MAIL_PAY_FOR);
	//send to sender sucess
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->mail_id, idx);
	pack(pkgbuf, MAX_MAIL_PAY_FOR, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 *   @brief send new mail header info to client 
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int send_player_new_mail(player_t* p, mail_header* header)
{
	int idx = 0;
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, header->mail_id, idx);
	pack(pkgbuf, header->mail_time, idx);
	pack(pkgbuf, header->mail_state, idx);
	pack(pkgbuf, header->mail_templet, idx);
	pack(pkgbuf, header->mail_type, idx);
	pack(pkgbuf, header->sender_id, idx);
	pack(pkgbuf, header->sender_role_tm, idx);
	pack(pkgbuf, header->mail_title, sizeof(header->mail_title), idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_new_mail, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}


/**
 *   @brief send reduce money to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_reduce_money(player_t*p, uint32_t money, bool callback)
{
	int idx = 0;
	pack_h(dbpkgbuf, money, idx);
	if(callback){
		return send_request_to_db(p,  p->id,  p->role_tm, dbproto_reduce_money, dbpkgbuf, idx);
	}
	return send_request_to_db( NULL,  p->id,  p->role_tm, dbproto_reduce_money, dbpkgbuf, idx);
}


int db_send_system_mail(player_t*p,  const char* title, uint32_t title_len, const char* content, uint32_t content_len, uint32_t mail_templet, vector<mail_numerical_enclosure>* nums,  vector<mail_item_enclosure>* items, vector<mail_equip_enclosure>* equips, uint32_t uid, uint32_t roletm)
{
	int idx = 0;
	char num_enclosure[1024] = { 0 };
	char item_enclosure[1024] = { 0 };
	char equip_enclosure[1024] = { 0 };



	pack_h(dbpkgbuf, 0, idx);
    if (p) {
        pack_h(dbpkgbuf, p->id, idx);
    } else {
        pack_h(dbpkgbuf, uid, idx);
    }
	pack_h(dbpkgbuf, mail_templet, idx);
	pack_h(dbpkgbuf, MAX_MAIL_COUNT, idx);
	pack(dbpkgbuf, title, title_len, idx);
	pack(dbpkgbuf, content, content_len, idx);

	TRACE_LOG("%lu %lu", items ? items->size() : 0, (equips ? equips->size() : 0));
	if( nums != NULL && nums->size() >0 && nums->size() <= MAX_NUM_ENCLOSURE_COUNT){
			mail_numerical_enclosure_to_str(nums, num_enclosure, sizeof(num_enclosure));
	}
	if( items != NULL && items->size() > 0 && items->size() <= MAX_ITEM_ENCLOSURE_COUNT){
			mail_item_enclosure_to_str(items, item_enclosure, sizeof(item_enclosure));
	}
	if( equips != NULL && equips->size() > 0 && equips->size() <= MAX_EQUIP_ENCLOSURE_COUNT){
			mail_equip_enclosure_to_str(equips, equip_enclosure, sizeof(equip_enclosure));
	}

	pack(dbpkgbuf, num_enclosure, sizeof(num_enclosure), idx);
	pack(dbpkgbuf, item_enclosure, sizeof(item_enclosure), idx);
	pack(dbpkgbuf, equip_enclosure, sizeof(equip_enclosure), idx);

    if (p) {
        return send_request_to_db(p, p->id, p->role_tm,  dbproto_send_system_mail, dbpkgbuf, idx);
    } else {
        return send_request_to_db(NULL, uid, roletm,  dbproto_send_system_mail, dbpkgbuf, idx);
    }
}

int db_send_system_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	send_mail_rsp_t *rsp = reinterpret_cast<send_mail_rsp_t*>(body);
	mail_header header;
	header.mail_id = rsp->mail_id;
	header.mail_time = rsp->mail_time;
	header.mail_state = rsp->mail_state;
	header.mail_templet = rsp->mail_templet;
	header.mail_type = rsp->mail_type;
	header.sender_id = rsp->sender_id;
	header.sender_role_tm = 0;
	strcpy(header.mail_title, rsp->mail_title);
	add_player_mail(p, header);
	return send_player_new_mail(p, &header);
}

int mail_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_mail_head_list(p);
}

int send_player_mail_head_list(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	uint32_t mail_list_count = p->m_read_mails->size() + p->m_unread_mails->size();
	std::map<uint32_t, mail_data>::iterator pItr = p->m_read_mails->begin();

	pack(pkgbuf,  mail_list_count, idx);
	for(; pItr != p->m_read_mails->end(); ++pItr )
	{
		pack(pkgbuf,  pItr->second.mail_id, idx);
		pack(pkgbuf,  pItr->second.mail_time, idx);
		pack(pkgbuf,  pItr->second.mail_state, idx);
		pack(pkgbuf,  pItr->second.mail_templet, idx);
		pack(pkgbuf,  pItr->second.mail_type, idx);
		pack(pkgbuf,  pItr->second.sender_id, idx);
		pack(pkgbuf,  pItr->second.sender_role_tm, idx);
		pack(pkgbuf,  pItr->second.mail_title,  sizeof(pItr->second.mail_title), idx);
	}

	pItr = p->m_unread_mails->begin();
	for(; pItr != p->m_unread_mails->end(); ++pItr)
	{
		pack(pkgbuf,  pItr->second.mail_id, idx);
		pack(pkgbuf,  pItr->second.mail_time, idx);
		pack(pkgbuf,  pItr->second.mail_state, idx);
		pack(pkgbuf,  pItr->second.mail_templet, idx);
		pack(pkgbuf,  pItr->second.mail_type, idx);
		pack(pkgbuf,  pItr->second.sender_id, idx);
		pack(pkgbuf,  pItr->second.sender_role_tm, idx);
		pack(pkgbuf,  pItr->second.mail_title,  sizeof(pItr->second.mail_title), idx);
	}
	init_cli_proto_head(pkgbuf, p,  cli_proto_mail_list, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int login_send_mail_action(player_t* p)
{
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};

	if (get_now_active_data_by_active_id(12) && !get_swap_action_times(p, 1363)) { 
		memcpy(title, char_content[44].content, MAX_MAIL_TITLE_LEN);
		memcpy(content, char_content[45].content, MAX_MAIL_CONTENT_LEN);
		if (p) {
			db_send_system_mail(0, title, sizeof(title), content, sizeof(content), 999, 0, NULL, 0, p->id, p->role_tm);
			add_swap_action_times(p, 1363);
		}
	}
	return 0;
}

int mail_active_event(player_t* p, uint32_t title_idx, uint32_t content_idx, uint32_t mail_temp, uint32_t item_id, uint32_t cnt, uint32_t uid, uint32_t roletm)
{
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};

    std::vector<mail_item_enclosure> mail_item;
    if (item_id) {
        mail_item.push_back(mail_item_enclosure(item_id, cnt));
    }

    memcpy(title, char_content[title_idx].content, MAX_MAIL_TITLE_LEN);
    memcpy(content, char_content[content_idx].content, MAX_MAIL_CONTENT_LEN);
    if (p) {
        db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_temp, 0, mail_item.size() ? &mail_item : 0, 0);
    }else {
        db_send_system_mail(0, title, sizeof(title), content, sizeof(content), mail_temp, 0, mail_item.size() ? &mail_item : 0, 0, uid, roletm);
    }
    return 0;
}

