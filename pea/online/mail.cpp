#include <cstdio>

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/utils/md5.h>

extern "C"
{
#include <glib.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/mcast.h>
}

#include "proto.hpp"
#include "cli_proto.hpp"
#include "mail.hpp"
#include "player.hpp"
#include "db_mail.hpp"


using namespace taomee;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bool mail_item_enclosure_to_str( vector<mail_item_enclosure>* mail_items, char* buf, uint32_t buflen)
{
	if(mail_items == NULL){
		return false;
	}
	memset(buf, 0, buflen);
	uint32_t i = 0, j = 0;
	for(i =0; i< mail_items->size(); i++)
	{
		if(buflen - j >= 8){
			j += sprintf(buf+j, "%u %u %u %u\t", (*mail_items)[i].item_id, (*mail_items)[i].item_count, (*mail_items)[i].duration_time, (*mail_items)[i].end_time);
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

bool str_to_mail_item_enclosure(char* buf,  vector<mail_item_enclosure>* mail_items)
{
	if(mail_items == NULL)return false;
	char* token = NULL;
	mail_items->clear();

	token = strtok(buf, "\t");
	while(token)
	{
		mail_item_enclosure item;
		sscanf(token, "%u %u %u %u", &item.item_id, &item.item_count, &item.duration_time, &item.end_time);
		mail_items->push_back(item);
		token = strtok(NULL, "\t");
	}
	return true;

}

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
			j += sprintf(buf + j, "%u %u %u %u\t", (*mail_equips)[i].equip_id, (*mail_equips)[i].equip_count, (*mail_equips)[i
					].duration_time, (*mail_equips)[i].end_time);
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

bool str_to_mail_equip_enclosure(char* buf,  vector<mail_equip_enclosure>* mail_equips)
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
		sscanf(token, "%u %u %u %u",  &equip.equip_id, &equip.equip_count, &equip.duration_time, &equip.end_time);
		mail_equips->push_back(equip);
		token = strtok(NULL, "\t");
	}
	return true;

}



//---------------------------------------------------------------------------------------------//
bool init_player_mail(player_t *p)
{
	p->m_read_mails = new std::map<uint32_t, mail_data>();
	p->m_unread_mails = new std::map<uint32_t, mail_data>();
	return true;
}


bool final_player_mail(player_t *p)
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
	return true;
}

bool add_player_mail(player_t* p, mail_data& data)
{
	if( is_mail_exist(p, data.mail_id))return false;

	if(data.mail_state == mail_state_read)
	{
		(*p->m_read_mails)[data.mail_id] = data;
	}
	if(data.mail_state == mail_state_unread)
	{
		(*p->m_unread_mails)[data.mail_id] = data;
	}
	return true;
}

bool add_player_mail(player_t* p, db_mail_head_info_t& head)
{
	mail_data data;
	
	data.mail_id = head.mail_id;
	data.mail_time = head.mail_time;
	data.mail_state = head.mail_state;
	data.mail_templet = head.mail_templet;
	data.mail_type = head.mail_type;
	data.sender_id = head.sender_id;
	data.sender_role_tm = head.sender_role_tm;
	strcpy((char*)data.sender_nick, (char*)head.sender_nick);
	strcpy((char*)data.mail_title, (char*)head.mail_title);

	return add_player_mail(p, data);
}

bool is_mail_exist(player_t* p, uint32_t mail_id)
{
	if(  p->m_read_mails->find(mail_id) != p->m_read_mails->end()){
		return true;
	}
	if( p->m_unread_mails->find(mail_id) != p->m_unread_mails->end() ){
		return true;
	}
	return false;
}

bool is_mail_load_ok(player_t* p, uint32_t mail_id)
{
	mail_data* p_data = get_player_mail(p, mail_id);
	if(p_data == NULL)return false;
	return strlen(p_data->mail_content) > 0;
}

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

bool has_mail_read(player_t* p, uint32_t mail_id)
{
	mail_data *pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return false;
	}
	return pData->mail_state == mail_state_read;
}

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


//----------------------------------------------------------------//
int cli_proto_mail_head_list(DEFAULT_ARG)
{
	return send_mail_head_list(p);	
}

int send_mail_head_list(player_t* p)
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
		pack(pkgbuf,  pItr->second.sender_nick, sizeof(pItr->second.sender_nick), idx);
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
		pack(pkgbuf,  pItr->second.sender_nick, sizeof(pItr->second.sender_nick), idx);
		pack(pkgbuf,  pItr->second.mail_title,  sizeof(pItr->second.mail_title), idx);
	}
	init_cli_proto_head(pkgbuf, p,  cli_proto_mail_head_list_cmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int cli_proto_mail_body(DEFAULT_ARG)
{
	cli_proto_mail_body_in* p_in = P_IN;

	if( !is_mail_exist(p, p_in->mail_id)){
		return 	send_error_to_player(p,  ONLINE_ERR_INVALID_MAIL_ID);
	}

	if( is_mail_load_ok(p, p_in->mail_id)){
		return send_mail_body_info(p, p_in->mail_id);	
	}
	return db_mail_body(p, p_in->mail_id);	
}

int send_mail_body_info(player_t* p, int mail_id)
{
	mail_data* pData = get_player_mail(p, mail_id);
	if(pData == NULL){
		return send_error_to_player(p,  ONLINE_ERR_INVALID_MAIL_ID);
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pData->mail_id, idx);
	pack(pkgbuf, pData->mail_content, sizeof(pData->mail_content), idx);

	uint32_t count = pData->mail_nums.size();
	pack(pkgbuf, count, idx);
	for(uint32_t i =0; i< count; i++)
	{
		mail_numerical_enclosure* num = &pData->mail_nums[i];
		pack(pkgbuf, num->type, idx);
		pack(pkgbuf, num->number, idx);
	}

	count = pData->mail_items.size();
	pack(pkgbuf, count, idx);
	for(uint32_t i =0; i< count; i++)
	{
		mail_item_enclosure* item = &pData->mail_items[i];
		pack(pkgbuf, item->item_id, idx);
		pack(pkgbuf, item->item_count, idx);
		pack(pkgbuf, item->duration_time, idx);
		pack(pkgbuf, item->end_time, idx);
	}

	count = pData->mail_equips.size();
	pack(pkgbuf, count, idx);
	for(uint32_t i =0; i< count; i++)
	{
		mail_equip_enclosure* equip = &pData->mail_equips[i];	
		pack(pkgbuf, equip->equip_id, idx);
		pack(pkgbuf, equip->equip_count, idx);
		pack(pkgbuf, equip->duration_time, idx);
		pack(pkgbuf, equip->end_time, idx);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int cli_proto_del_mail(DEFAULT_ARG)
{
	cli_proto_del_mail_in* p_in = P_IN;

	if(!is_mail_exist(p, p_in->mail_id)){
		return send_error_to_player(p, ONLINE_ERR_INVALID_MAIL_ID);
	}
	return db_del_mail(p, p_in->mail_id);
}

int send_del_mail(player_t* p, int mail_id)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, mail_id, idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}



int cli_proto_take_mail_enclosure(DEFAULT_ARG)
{
	cli_proto_take_mail_enclosure_in* p_in = P_IN;
	
	if(!is_mail_exist(p, p_in->mail_id)){
		return send_error_to_player(p, ONLINE_ERR_INVALID_MAIL_ID);
	}

	if(!is_mail_include_enclosure(p, p_in->mail_id)){
		return send_error_to_player(p, ONLINE_ERR_NO_ENCLOSURE);	
	}
	
	return db_take_mail_enclosure(p, p_in->mail_id);
}

int send_take_mail_enclosure(player_t* p, int mail_id)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, mail_id, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 1);
}

int cli_proto_send_mail(DEFAULT_ARG)
{
	cli_proto_send_mail_in* p_in = P_IN;		

	if( p_in->receive_id == p->id || !is_valid_uid(p->id))
	{
		return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);		
	}		

	if(strlen((char*)p_in->mail_title) > MAX_MAIL_TITLE_LEN){
		return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
	}

	if(strlen((char*)p_in->mail_content) > MAX_MAIL_CONTENT_LEN){
		return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
	}

	return db_send_mail(p, p_in->receive_id, p_in->receive_role_tm, p_in->mail_title, p_in->mail_content);
}


int notify_player_new_mail(player_t* p, db_mail_head_info_t* info)
{
	int idx = sizeof(cli_proto_t);		

	pack(pkgbuf,  info->mail_id, idx);
	pack(pkgbuf,  info->mail_time, idx);
	pack(pkgbuf,  info->mail_state, idx);
	pack(pkgbuf,  info->mail_templet, idx);
	pack(pkgbuf,  info->mail_type, idx);
	pack(pkgbuf,  info->sender_id, idx);
	pack(pkgbuf,  info->sender_role_tm, idx);
	pack(pkgbuf,  info->sender_nick, sizeof(info->sender_nick), idx);
	pack(pkgbuf,  info->mail_title,  sizeof(info->mail_title), idx);


	init_cli_proto_head(pkgbuf, p, cli_proto_new_mail_cmd, idx);
	return send_to_player(p, (char*)pkgbuf, idx, 0);
}
