#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "proto.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "db_mail.hpp"
#include "login.hpp"

static void mail_enclosure_to_parse_item(mail_data* data, std::vector<parse_item>& items);

void mail_enclosure_to_parse_item(mail_data* data, std::vector<parse_item>& items)
{
	for(uint32_t i =0; i< data->mail_items.size(); i++)
	{
		mail_item_enclosure* m = &data->mail_items[i];

		parse_item tm;
		tm.item_id = m->item_id;
		tm.item_count = m->item_count;
		if(m->duration_time)
		{
			tm.expire_time = time(NULL) + m->duration_time;	
		}
		if(m->end_time)
		{
			tm.expire_time = m->end_time;	
		}
		items.push_back(tm);
	}

	for(uint32_t i =0; i< data->mail_equips.size(); i++)
	{
		mail_equip_enclosure* m = &data->mail_equips[i];

		parse_item tm;
		tm.item_id = m->equip_id;
		tm.item_count = m->equip_count;
		if(m->duration_time)
		{
			tm.expire_time = time(NULL) + m->duration_time;
		}
		if(m->end_time)
		{
			tm.expire_time = m->end_time;
		}
		items.push_back(tm);
	}
}

/*------------------------------------------------------------*/

int db_mail_head_list(player_t* p)
{
	db_proto_mail_head_list_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;

	return send_to_db(p, db_proto_mail_head_list_cmd, &in);
}

int db_proto_mail_head_list_callback(DEFAULT_ARG)
{
	db_proto_mail_head_list_out* p_in = P_IN;
	
	for(uint32_t i =0; i< p_in->head_infos.size(); i++)
	{
		db_mail_head_info_t* head_info = &p_in->head_infos[i];
		add_player_mail(p, *head_info);	
	}

    p->set_module(MODULE_MAIL);

	return process_login(p);
}


int db_mail_body(player_t* p, uint32_t mail_id)
{
	db_proto_mail_body_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;
	in.mail_id = mail_id;

	return send_to_db(p, db_proto_mail_body_cmd, &in);
}


int db_proto_mail_body_callback(DEFAULT_ARG)
{
	db_proto_mail_body_out* p_in = P_IN;

	mail_data* p_data =  get_player_mail(p, p_in->mail_id);

	if(p_data == NULL){
		return send_error_to_player(p, ONLINE_ERR_INVALID_MAIL_ID);	
	}

	strcpy(p_data->mail_content, (char*)p_in->mail_content);
	str_to_mail_numberical_enclosure( (char*)p_in->numerical_enclosure,  &(p_data->mail_nums));
	str_to_mail_item_enclosure( (char*)p_in->item_enclosure, &(p_data->mail_items));
	str_to_mail_equip_enclosure( (char*)p_in->equip_enclosure, &(p_data->mail_equips));
	
	p_data->mail_state = mail_state_read;

	return send_mail_body_info(p, p_in->mail_id);	
}

int db_del_mail(player_t* p, uint32_t mail_id)
{
	db_proto_del_mail_in in;
	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;
	in.mail_id = mail_id;

	return send_to_db(p, db_proto_del_mail_cmd, &in);
}

int db_proto_del_mail_callback(DEFAULT_ARG)
{
	db_proto_del_mail_out* p_in = P_IN;
	
	mail_data* p_data =  get_player_mail(p, p_in->mail_id);

	if(p_data == NULL){
		return send_error_to_player(p, ONLINE_ERR_INVALID_MAIL_ID);
	}

	delete_player_mail(p, p_in->mail_id);

	return send_del_mail(p, p_in->mail_id);
}


int db_take_mail_enclosure(player_t* p, uint32_t mail_id)
{
	mail_data* p_data =  get_player_mail(p, mail_id);
	
	if(p_data == NULL)return 0;
	
	db_proto_take_mail_enclosure_in in;

	in.db_user_id.role_tm = p->role_tm;
	in.db_user_id.server_id = p->server_id;
	in.db_user_id.user_id = p->id;
	in.mail_id = mail_id;

	for(uint32_t i = 0; i< p_data->mail_nums.size(); i++)
	{
		numerical_enclosure_info_t num;
		num.type = p_data->mail_nums[i].type;
		num.number = p_data->mail_nums[i].number;
		in.add_values.push_back(num);
	}

	std::vector<parse_item> items;
	mail_enclosure_to_parse_item(p_data, items);

	bool ret = parse_db_add_item_request(p, items, in.add_items);
	if(!ret)
	{
		return send_error_to_player(p, ONLINE_ERR_BAG_FULL);
	}

	return send_to_db(p, db_proto_take_mail_enclosure_cmd, &in);
}

int db_proto_take_mail_enclosure_callback(DEFAULT_ARG)
{
	db_proto_take_mail_enclosure_out* p_in = P_IN;		
	
	mail_data* p_data =  get_player_mail(p, p_in->mail_id);

	if(p_data == NULL){
		return send_error_to_player(p, ONLINE_ERR_INVALID_MAIL_ID);
	}

	clear_mail_enclosure(p, p_in->mail_id);

	send_take_mail_enclosure(p, p_in->mail_id);

	for(uint32_t i=0; i< p_in->add_items.size(); i++)
	{
		db_add_item_reply_t* reply_t = &p_in->add_items[i];
		p->bag->add_item_by_grid(reply_t);
		notify_player_add_item(p, reply_t);
	}
	return 0;
}

int db_send_mail(player_t* p, uint32_t receive_id, uint32_t receive_role_tm, uint8_t* title, uint8_t* content)
{
	if(strlen((char*)title) > MAX_MAIL_TITLE_LEN){
		return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);	
	}

	if(strlen((char*)title) > MAX_MAIL_CONTENT_LEN){
		return send_error_to_player(p, ONLINE_ERR_INVALID_PARA);
	}
	
	db_proto_send_mail_in in;
	
	in.db_user_id.role_tm = p->role_tm;
	    in.db_user_id.server_id = p->server_id;
		    in.db_user_id.user_id = p->id;
	in.receive_id = receive_id;
	in.receive_role_tm = receive_role_tm;
	strcpy((char*)in.sender_nick, (char*)p->nick);
	strcpy((char*)in.mail_title, (char*)title);
	strcpy((char*)in.mail_content, (char*)content);
	
	return send_to_db(p, db_proto_send_mail_cmd, &in);
}

int db_proto_send_mail_callback(DEFAULT_ARG)
{
	db_proto_send_mail_out* p_in = P_IN;
	send_error_to_player(p, ONLINE_ERR_NOERROR);	

	player_t* target = get_player(p_in->recevie_id);
	if(target)
	{
		notify_player_new_mail(target, &p_in->mail_head);	
	}
	return 0;
}
