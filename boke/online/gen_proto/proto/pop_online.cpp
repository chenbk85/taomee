#include "pop_online.h"

cli_buy_item_in::cli_buy_item_in(){
	this->init();
}
void cli_buy_item_in::init(){
	this->itemid=0;

}
bool  cli_buy_item_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool cli_buy_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}
	
cli_buy_item_out::cli_buy_item_out(){
	this->init();
}
void cli_buy_item_out::init(){
	this->left_xiaomee=0;

}
bool  cli_buy_item_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->left_xiaomee)) return false;
	return true;
}

bool cli_buy_item_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->left_xiaomee)) return false;
	return true;
}
	
cli_buy_item_use_gamept_in::cli_buy_item_use_gamept_in(){
	this->init();
}
void cli_buy_item_use_gamept_in::init(){
	this->itemid=0;

}
bool  cli_buy_item_use_gamept_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool cli_buy_item_use_gamept_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}
	
cli_buy_item_use_gamept_out::cli_buy_item_use_gamept_out(){
	this->init();
}
void cli_buy_item_use_gamept_out::init(){
	this->left_gamept=0;

}
bool  cli_buy_item_use_gamept_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->left_gamept)) return false;
	return true;
}

bool cli_buy_item_use_gamept_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->left_gamept)) return false;
	return true;
}
	
cli_click_stat_in::cli_click_stat_in(){
	this->init();
}
void cli_click_stat_in::init(){
	this->type=0;

}
bool  cli_click_stat_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	return true;
}

bool cli_click_stat_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	return true;
}
	
cli_cur_game_end_in::cli_cur_game_end_in(){
	this->init();
}
void cli_cur_game_end_in::init(){
	this->win_flag=0;

}
bool  cli_cur_game_end_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->win_flag)) return false;
	return true;
}

bool cli_cur_game_end_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->win_flag)) return false;
	return true;
}
	
cli_del_item_in::cli_del_item_in(){
	this->init();
}
void cli_del_item_in::init(){
	this->itemlist.clear();

}
bool  cli_del_item_in::read_from_buf(byte_array_t & ba ){

	uint32_t itemlist_count ;
	if (!ba.read_uint32( itemlist_count )) return false;
	item_t  itemlist_item;
	this->itemlist.clear();
	{for(uint32_t i=0; i<itemlist_count;i++){
		if (!itemlist_item.read_from_buf(ba)) return false;
		this->itemlist.push_back(itemlist_item);
	}}
	return true;
}

bool cli_del_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemlist.size())) return false;
	{for(uint32_t i=0; i<this->itemlist.size() ;i++){
		if (!this->itemlist[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_draw_lottery_out::cli_draw_lottery_out(){
	this->init();
}
void cli_draw_lottery_out::init(){
	this->item_get.init();

}
bool  cli_draw_lottery_out::read_from_buf(byte_array_t & ba ){
	if (!this->item_get.read_from_buf(ba)) return false;
	return true;
}

bool cli_draw_lottery_out::write_to_buf(byte_array_t & ba ){
	if (!this->item_get.write_to_buf(ba)) return false;
	return true;
}
	
cli_find_map_add_in::cli_find_map_add_in(){
	this->init();
}
void cli_find_map_add_in::init(){
	this->islandid=0;
	this->mapid=0;

}
bool  cli_find_map_add_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	return true;
}

bool cli_find_map_add_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	return true;
}
	
cli_game_opt_in::cli_game_opt_in(){
	this->init();
}
void cli_game_opt_in::init(){
	this->_gamemsg_len=0;

}
bool  cli_game_opt_in::read_from_buf(byte_array_t & ba ){

	if (!ba.read_uint32(this->_gamemsg_len )) return false;
	if (this->_gamemsg_len>9999) return false;
 	if (!ba.read_buf(this->gamemsg,this->_gamemsg_len)) return false;
	return true;
}

bool cli_game_opt_in::write_to_buf(byte_array_t & ba ){
	if (this->_gamemsg_len>9999) return false;
	if (!ba.write_uint32(this->_gamemsg_len))return false;
	if (!ba.write_buf(this->gamemsg,this->_gamemsg_len)) return false;
	return true;
}
	
cli_game_play_with_other_in::cli_game_play_with_other_in(){
	this->init();
}
void cli_game_play_with_other_in::init(){
	this->gameid=0;
	this->obj_userid=0;
	this->is_start=0;
	this->why_not_start_flag=0;

}
bool  cli_game_play_with_other_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if(!ba.read_buf(this->request_session,32)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;
	if (!ba.read_uint32(this->is_start)) return false;
	if (!ba.read_uint32(this->why_not_start_flag)) return false;
	return true;
}

bool cli_game_play_with_other_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if(!ba.write_buf(this->request_session,32)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->is_start)) return false;
	if (!ba.write_uint32(this->why_not_start_flag)) return false;
	return true;
}
	
cli_game_play_with_other_out::cli_game_play_with_other_out(){
	this->init();
}
void cli_game_play_with_other_out::init(){
	this->obj_user_stat=0;

}
bool  cli_game_play_with_other_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_user_stat)) return false;
	return true;
}

bool cli_game_play_with_other_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_user_stat)) return false;
	return true;
}
	
cli_game_request_in::cli_game_request_in(){
	this->init();
}
void cli_game_request_in::init(){
	this->gameid=0;
	this->obj_userid=0;

}
bool  cli_game_request_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_game_request_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_game_request_out::cli_game_request_out(){
	this->init();
}
void cli_game_request_out::init(){
	this->obj_user_stat=0;

}
bool  cli_game_request_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_user_stat)) return false;
	return true;
}

bool cli_game_request_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_user_stat)) return false;
	return true;
}
	
online_user_info_t::online_user_info_t(){
	this->init();
}
void online_user_info_t::init(){
	this->obj_userid=0;
	this->color=0;
	this->xiaomee=0;
	this->level=0;
	this->experience=0;
	this->x=0;
	this->y=0;
	this->medal_list.clear();
	this->game_point=0;
	this->use_clothes_list.clear();
	this->effect_list.clear();

}
bool  online_user_info_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_int32(this->x)) return false;
	if (!ba.read_int32(this->y)) return false;

	uint32_t medal_list_count ;
	if (!ba.read_uint32( medal_list_count )) return false;
	uint32_t  medal_list_item;
	this->medal_list.clear();
	{for(uint32_t i=0; i<medal_list_count;i++){
		if (!ba.read_uint32(medal_list_item)) return false;
		this->medal_list.push_back(medal_list_item);
	}}
	if (!ba.read_uint32(this->game_point)) return false;

	uint32_t use_clothes_list_count ;
	if (!ba.read_uint32( use_clothes_list_count )) return false;
	uint32_t  use_clothes_list_item;
	this->use_clothes_list.clear();
	{for(uint32_t i=0; i<use_clothes_list_count;i++){
		if (!ba.read_uint32(use_clothes_list_item)) return false;
		this->use_clothes_list.push_back(use_clothes_list_item);
	}}

	uint32_t effect_list_count ;
	if (!ba.read_uint32( effect_list_count )) return false;
	uint32_t  effect_list_item;
	this->effect_list.clear();
	{for(uint32_t i=0; i<effect_list_count;i++){
		if (!ba.read_uint32(effect_list_item)) return false;
		this->effect_list.push_back(effect_list_item);
	}}
	return true;
}

bool online_user_info_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_int32(this->x)) return false;
	if (!ba.write_int32(this->y)) return false;
	if (!ba.write_uint32(this->medal_list.size())) return false;
	{for(uint32_t i=0; i<this->medal_list.size() ;i++){
		if (!ba.write_uint32(this->medal_list[i])) return false;
	}}
	if (!ba.write_uint32(this->game_point)) return false;
	if (!ba.write_uint32(this->use_clothes_list.size())) return false;
	{for(uint32_t i=0; i<this->use_clothes_list.size() ;i++){
		if (!ba.write_uint32(this->use_clothes_list[i])) return false;
	}}
	if (!ba.write_uint32(this->effect_list.size())) return false;
	{for(uint32_t i=0; i<this->effect_list.size() ;i++){
		if (!ba.write_uint32(this->effect_list[i])) return false;
	}}
	return true;
}
	
cli_get_all_user_info_from_cur_map_out::cli_get_all_user_info_from_cur_map_out(){
	this->init();
}
void cli_get_all_user_info_from_cur_map_out::init(){
	this->online_user_info_list.clear();

}
bool  cli_get_all_user_info_from_cur_map_out::read_from_buf(byte_array_t & ba ){

	uint32_t online_user_info_list_count ;
	if (!ba.read_uint32( online_user_info_list_count )) return false;
	online_user_info_t  online_user_info_list_item;
	this->online_user_info_list.clear();
	{for(uint32_t i=0; i<online_user_info_list_count;i++){
		if (!online_user_info_list_item.read_from_buf(ba)) return false;
		this->online_user_info_list.push_back(online_user_info_list_item);
	}}
	return true;
}

bool cli_get_all_user_info_from_cur_map_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_user_info_list.size())) return false;
	{for(uint32_t i=0; i<this->online_user_info_list.size() ;i++){
		if (!this->online_user_info_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_get_card_list_by_islandid_in::cli_get_card_list_by_islandid_in(){
	this->init();
}
void cli_get_card_list_by_islandid_in::init(){
	this->islandid=0;

}
bool  cli_get_card_list_by_islandid_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	return true;
}

bool cli_get_card_list_by_islandid_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	return true;
}
	
cli_get_card_list_by_islandid_out::cli_get_card_list_by_islandid_out(){
	this->init();
}
void cli_get_card_list_by_islandid_out::init(){
	this->cardid_list.clear();

}
bool  cli_get_card_list_by_islandid_out::read_from_buf(byte_array_t & ba ){

	uint32_t cardid_list_count ;
	if (!ba.read_uint32( cardid_list_count )) return false;
	item_t  cardid_list_item;
	this->cardid_list.clear();
	{for(uint32_t i=0; i<cardid_list_count;i++){
		if (!cardid_list_item.read_from_buf(ba)) return false;
		this->cardid_list.push_back(cardid_list_item);
	}}
	return true;
}

bool cli_get_card_list_by_islandid_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cardid_list.size())) return false;
	{for(uint32_t i=0; i<this->cardid_list.size() ;i++){
		if (!this->cardid_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
game_user_t::game_user_t(){
	this->init();
}
void game_user_t::init(){
	this->src_userid=0;
	this->gameid=0;
	this->userlist.clear();

}
bool  game_user_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;
	if (!ba.read_uint32(this->gameid)) return false;

	uint32_t userlist_count ;
	if (!ba.read_uint32( userlist_count )) return false;
	uint32_t  userlist_item;
	this->userlist.clear();
	{for(uint32_t i=0; i<userlist_count;i++){
		if (!ba.read_uint32(userlist_item)) return false;
		this->userlist.push_back(userlist_item);
	}}
	return true;
}

bool game_user_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->userlist.size())) return false;
	{for(uint32_t i=0; i<this->userlist.size() ;i++){
		if (!ba.write_uint32(this->userlist[i])) return false;
	}}
	return true;
}
	
cli_get_game_user_out::cli_get_game_user_out(){
	this->init();
}
void cli_get_game_user_out::init(){
	this->game_user_list.clear();

}
bool  cli_get_game_user_out::read_from_buf(byte_array_t & ba ){

	uint32_t game_user_list_count ;
	if (!ba.read_uint32( game_user_list_count )) return false;
	game_user_t  game_user_list_item;
	this->game_user_list.clear();
	{for(uint32_t i=0; i<game_user_list_count;i++){
		if (!game_user_list_item.read_from_buf(ba)) return false;
		this->game_user_list.push_back(game_user_list_item);
	}}
	return true;
}

bool cli_get_game_user_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->game_user_list.size())) return false;
	{for(uint32_t i=0; i<this->game_user_list.size() ;i++){
		if (!this->game_user_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
island_time_t::island_time_t(){
	this->init();
}
void island_time_t::init(){
	this->islandid=0;
	this->is_open=0;
	this->next_state_time=0;

}
bool  island_time_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->is_open)) return false;
	if (!ba.read_uint32(this->next_state_time)) return false;
	return true;
}

bool island_time_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->is_open)) return false;
	if (!ba.write_uint32(this->next_state_time)) return false;
	return true;
}
	
cli_get_island_time_out::cli_get_island_time_out(){
	this->init();
}
void cli_get_island_time_out::init(){
	this->svr_time=0;
	this->islandlist.clear();

}
bool  cli_get_island_time_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->svr_time)) return false;

	uint32_t islandlist_count ;
	if (!ba.read_uint32( islandlist_count )) return false;
	island_time_t  islandlist_item;
	this->islandlist.clear();
	{for(uint32_t i=0; i<islandlist_count;i++){
		if (!islandlist_item.read_from_buf(ba)) return false;
		this->islandlist.push_back(islandlist_item);
	}}
	return true;
}

bool cli_get_island_time_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->svr_time)) return false;
	if (!ba.write_uint32(this->islandlist.size())) return false;
	{for(uint32_t i=0; i<this->islandlist.size() ;i++){
		if (!this->islandlist[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_get_item_list_in::cli_get_item_list_in(){
	this->init();
}
void cli_get_item_list_in::init(){
	this->startid=0;
	this->endid=0;

}
bool  cli_get_item_list_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->startid)) return false;
	if (!ba.read_uint32(this->endid)) return false;
	return true;
}

bool cli_get_item_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->startid)) return false;
	if (!ba.write_uint32(this->endid)) return false;
	return true;
}
	
cli_get_item_list_out::cli_get_item_list_out(){
	this->init();
}
void cli_get_item_list_out::init(){
	this->item_list.clear();

}
bool  cli_get_item_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	item_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool cli_get_item_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_get_login_chest_out::cli_get_login_chest_out(){
	this->init();
}
void cli_get_login_chest_out::init(){
	this->item_get.init();

}
bool  cli_get_login_chest_out::read_from_buf(byte_array_t & ba ){
	if (!this->item_get.read_from_buf(ba)) return false;
	return true;
}

bool cli_get_login_chest_out::write_to_buf(byte_array_t & ba ){
	if (!this->item_get.write_to_buf(ba)) return false;
	return true;
}
	
cli_get_login_reward_in::cli_get_login_reward_in(){
	this->init();
}
void cli_get_login_reward_in::init(){
	this->index=0;

}
bool  cli_get_login_reward_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->index)) return false;
	return true;
}

bool cli_get_login_reward_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->index)) return false;
	return true;
}
	
cli_get_login_reward_out::cli_get_login_reward_out(){
	this->init();
}
void cli_get_login_reward_out::init(){
	this->item_get.init();

}
bool  cli_get_login_reward_out::read_from_buf(byte_array_t & ba ){
	if (!this->item_get.read_from_buf(ba)) return false;
	return true;
}

bool cli_get_login_reward_out::write_to_buf(byte_array_t & ba ){
	if (!this->item_get.write_to_buf(ba)) return false;
	return true;
}
	
cli_get_lottery_count_out::cli_get_lottery_count_out(){
	this->init();
}
void cli_get_lottery_count_out::init(){
	this->count=0;

}
bool  cli_get_lottery_count_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool cli_get_lottery_count_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
cli_get_spec_item_in::cli_get_spec_item_in(){
	this->init();
}
void cli_get_spec_item_in::init(){
	this->itemid=0;

}
bool  cli_get_spec_item_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool cli_get_spec_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}
	
cli_get_spec_item_list_in::cli_get_spec_item_list_in(){
	this->init();
}
void cli_get_spec_item_list_in::init(){
	this->startid=0;
	this->endid=0;

}
bool  cli_get_spec_item_list_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->startid)) return false;
	if (!ba.read_uint32(this->endid)) return false;
	return true;
}

bool cli_get_spec_item_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->startid)) return false;
	if (!ba.write_uint32(this->endid)) return false;
	return true;
}
	
cli_get_spec_item_list_out::cli_get_spec_item_list_out(){
	this->init();
}
void cli_get_spec_item_list_out::init(){
	this->spec_item_list.clear();

}
bool  cli_get_spec_item_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t spec_item_list_count ;
	if (!ba.read_uint32( spec_item_list_count )) return false;
	item_day_limit_t  spec_item_list_item;
	this->spec_item_list.clear();
	{for(uint32_t i=0; i<spec_item_list_count;i++){
		if (!spec_item_list_item.read_from_buf(ba)) return false;
		this->spec_item_list.push_back(spec_item_list_item);
	}}
	return true;
}

bool cli_get_spec_item_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->spec_item_list.size())) return false;
	{for(uint32_t i=0; i<this->spec_item_list.size() ;i++){
		if (!this->spec_item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_get_spec_item_out::cli_get_spec_item_out(){
	this->init();
}
void cli_get_spec_item_out::init(){
	this->item.init();

}
bool  cli_get_spec_item_out::read_from_buf(byte_array_t & ba ){
	if (!this->item.read_from_buf(ba)) return false;
	return true;
}

bool cli_get_spec_item_out::write_to_buf(byte_array_t & ba ){
	if (!this->item.write_to_buf(ba)) return false;
	return true;
}
	
cli_get_user_game_stat_in::cli_get_user_game_stat_in(){
	this->init();
}
void cli_get_user_game_stat_in::init(){
	this->obj_userid=0;

}
bool  cli_get_user_game_stat_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_get_user_game_stat_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_get_user_game_stat_out::cli_get_user_game_stat_out(){
	this->init();
}
void cli_get_user_game_stat_out::init(){
	this->obj_userid=0;
	this->game_stat_list.clear();

}
bool  cli_get_user_game_stat_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;

	uint32_t game_stat_list_count ;
	if (!ba.read_uint32( game_stat_list_count )) return false;
	game_info_t  game_stat_list_item;
	this->game_stat_list.clear();
	{for(uint32_t i=0; i<game_stat_list_count;i++){
		if (!game_stat_list_item.read_from_buf(ba)) return false;
		this->game_stat_list.push_back(game_stat_list_item);
	}}
	return true;
}

bool cli_get_user_game_stat_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->game_stat_list.size())) return false;
	{for(uint32_t i=0; i<this->game_stat_list.size() ;i++){
		if (!this->game_stat_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_get_user_info_in::cli_get_user_info_in(){
	this->init();
}
void cli_get_user_info_in::init(){
	this->obj_userid=0;

}
bool  cli_get_user_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_get_user_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_get_user_info_out::cli_get_user_info_out(){
	this->init();
}
void cli_get_user_info_out::init(){
	this->regtime=0;
	this->color=0;
	this->left_xiaomee=0;
	this->level=0;
	this->experience=0;
	this->game_point=0;
	this->use_clothes_list.clear();
	this->effect_list.clear();
	this->user_log_list.clear();
	this->complete_islandid_list.clear();

}
bool  cli_get_user_info_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->regtime)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->left_xiaomee)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_uint32(this->game_point)) return false;

	uint32_t use_clothes_list_count ;
	if (!ba.read_uint32( use_clothes_list_count )) return false;
	uint32_t  use_clothes_list_item;
	this->use_clothes_list.clear();
	{for(uint32_t i=0; i<use_clothes_list_count;i++){
		if (!ba.read_uint32(use_clothes_list_item)) return false;
		this->use_clothes_list.push_back(use_clothes_list_item);
	}}

	uint32_t effect_list_count ;
	if (!ba.read_uint32( effect_list_count )) return false;
	uint32_t  effect_list_item;
	this->effect_list.clear();
	{for(uint32_t i=0; i<effect_list_count;i++){
		if (!ba.read_uint32(effect_list_item)) return false;
		this->effect_list.push_back(effect_list_item);
	}}

	uint32_t user_log_list_count ;
	if (!ba.read_uint32( user_log_list_count )) return false;
	user_log_t  user_log_list_item;
	this->user_log_list.clear();
	{for(uint32_t i=0; i<user_log_list_count;i++){
		if (!user_log_list_item.read_from_buf(ba)) return false;
		this->user_log_list.push_back(user_log_list_item);
	}}

	uint32_t complete_islandid_list_count ;
	if (!ba.read_uint32( complete_islandid_list_count )) return false;
	uint32_t  complete_islandid_list_item;
	this->complete_islandid_list.clear();
	{for(uint32_t i=0; i<complete_islandid_list_count;i++){
		if (!ba.read_uint32(complete_islandid_list_item)) return false;
		this->complete_islandid_list.push_back(complete_islandid_list_item);
	}}
	return true;
}

bool cli_get_user_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->regtime)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->left_xiaomee)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_uint32(this->game_point)) return false;
	if (!ba.write_uint32(this->use_clothes_list.size())) return false;
	{for(uint32_t i=0; i<this->use_clothes_list.size() ;i++){
		if (!ba.write_uint32(this->use_clothes_list[i])) return false;
	}}
	if (!ba.write_uint32(this->effect_list.size())) return false;
	{for(uint32_t i=0; i<this->effect_list.size() ;i++){
		if (!ba.write_uint32(this->effect_list[i])) return false;
	}}
	if (!ba.write_uint32(this->user_log_list.size())) return false;
	{for(uint32_t i=0; i<this->user_log_list.size() ;i++){
		if (!this->user_log_list[i].write_to_buf(ba)) return false;
	}}
	if (!ba.write_uint32(this->complete_islandid_list.size())) return false;
	{for(uint32_t i=0; i<this->complete_islandid_list.size() ;i++){
		if (!ba.write_uint32(this->complete_islandid_list[i])) return false;
	}}
	return true;
}
	
cli_get_user_island_find_map_info_in::cli_get_user_island_find_map_info_in(){
	this->init();
}
void cli_get_user_island_find_map_info_in::init(){
	this->islandid=0;

}
bool  cli_get_user_island_find_map_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	return true;
}

bool cli_get_user_island_find_map_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	return true;
}
	
cli_get_user_island_find_map_info_out::cli_get_user_island_find_map_info_out(){
	this->init();
}
void cli_get_user_island_find_map_info_out::init(){
	this->find_map_list.clear();

}
bool  cli_get_user_island_find_map_info_out::read_from_buf(byte_array_t & ba ){

	uint32_t find_map_list_count ;
	if (!ba.read_uint32( find_map_list_count )) return false;
	uint32_t  find_map_list_item;
	this->find_map_list.clear();
	{for(uint32_t i=0; i<find_map_list_count;i++){
		if (!ba.read_uint32(find_map_list_item)) return false;
		this->find_map_list.push_back(find_map_list_item);
	}}
	return true;
}

bool cli_get_user_island_find_map_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->find_map_list.size())) return false;
	{for(uint32_t i=0; i<this->find_map_list.size() ;i++){
		if (!ba.write_uint32(this->find_map_list[i])) return false;
	}}
	return true;
}
	
cli_get_user_island_task_info_in::cli_get_user_island_task_info_in(){
	this->init();
}
void cli_get_user_island_task_info_in::init(){
	this->islandid=0;

}
bool  cli_get_user_island_task_info_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	return true;
}

bool cli_get_user_island_task_info_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	return true;
}
	
cli_get_user_island_task_info_out::cli_get_user_island_task_info_out(){
	this->init();
}
void cli_get_user_island_task_info_out::init(){
	this->task_nodeid_list.clear();

}
bool  cli_get_user_island_task_info_out::read_from_buf(byte_array_t & ba ){

	uint32_t task_nodeid_list_count ;
	if (!ba.read_uint32( task_nodeid_list_count )) return false;
	uint32_t  task_nodeid_list_item;
	this->task_nodeid_list.clear();
	{for(uint32_t i=0; i<task_nodeid_list_count;i++){
		if (!ba.read_uint32(task_nodeid_list_item)) return false;
		this->task_nodeid_list.push_back(task_nodeid_list_item);
	}}
	return true;
}

bool cli_get_user_island_task_info_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->task_nodeid_list.size())) return false;
	{for(uint32_t i=0; i<this->task_nodeid_list.size() ;i++){
		if (!ba.write_uint32(this->task_nodeid_list[i])) return false;
	}}
	return true;
}
	
cli_get_valid_gamept_out::cli_get_valid_gamept_out(){
	this->init();
}
void cli_get_valid_gamept_out::init(){
	this->left_gamept=0;

}
bool  cli_get_valid_gamept_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->left_gamept)) return false;
	return true;
}

bool cli_get_valid_gamept_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->left_gamept)) return false;
	return true;
}
	
cli_hang_bell_get_item_in::cli_hang_bell_get_item_in(){
	this->init();
}
void cli_hang_bell_get_item_in::init(){
	this->bell_type=0;
	this->zone_type=0;

}
bool  cli_hang_bell_get_item_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->bell_type)) return false;
	if (!ba.read_uint32(this->zone_type)) return false;
	return true;
}

bool cli_hang_bell_get_item_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->bell_type)) return false;
	if (!ba.write_uint32(this->zone_type)) return false;
	return true;
}
	
cli_hang_bell_get_item_out::cli_hang_bell_get_item_out(){
	this->init();
}
void cli_hang_bell_get_item_out::init(){
	this->item_get.init();

}
bool  cli_hang_bell_get_item_out::read_from_buf(byte_array_t & ba ){
	if (!this->item_get.read_from_buf(ba)) return false;
	return true;
}

bool cli_hang_bell_get_item_out::write_to_buf(byte_array_t & ba ){
	if (!this->item_get.write_to_buf(ba)) return false;
	return true;
}
	
cli_login_in::cli_login_in(){
	this->init();
}
void cli_login_in::init(){
	this->server_id=0;
	this->login_userid=0;

}
bool  cli_login_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->server_id)) return false;
	if (!ba.read_uint32(this->login_userid)) return false;
	if(!ba.read_buf(this->session,32)) return false;
	return true;
}

bool cli_login_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->server_id)) return false;
	if (!ba.write_uint32(this->login_userid)) return false;
	if(!ba.write_buf(this->session,32)) return false;
	return true;
}
	
cli_login_out::cli_login_out(){
	this->init();
}
void cli_login_out::init(){
	this->regflag=0;
	this->left_xiaomee=0;
	this->color=0;
	this->age=0;
	this->last_islandid=0;
	this->last_mapid=0;
	this->last_x=0;
	this->last_y=0;
	this->task_nodeid_list.clear();
	this->use_clothes_list.clear();
	this->effect_list.clear();
	this->find_map_list.clear();

}
bool  cli_login_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->regflag)) return false;
	if (!ba.read_uint32(this->left_xiaomee)) return false;
	if (!ba.read_uint32(this->color)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->last_islandid)) return false;
	if (!ba.read_uint32(this->last_mapid)) return false;
	if (!ba.read_uint32(this->last_x)) return false;
	if (!ba.read_uint32(this->last_y)) return false;

	uint32_t task_nodeid_list_count ;
	if (!ba.read_uint32( task_nodeid_list_count )) return false;
	uint32_t  task_nodeid_list_item;
	this->task_nodeid_list.clear();
	{for(uint32_t i=0; i<task_nodeid_list_count;i++){
		if (!ba.read_uint32(task_nodeid_list_item)) return false;
		this->task_nodeid_list.push_back(task_nodeid_list_item);
	}}

	uint32_t use_clothes_list_count ;
	if (!ba.read_uint32( use_clothes_list_count )) return false;
	uint32_t  use_clothes_list_item;
	this->use_clothes_list.clear();
	{for(uint32_t i=0; i<use_clothes_list_count;i++){
		if (!ba.read_uint32(use_clothes_list_item)) return false;
		this->use_clothes_list.push_back(use_clothes_list_item);
	}}

	uint32_t effect_list_count ;
	if (!ba.read_uint32( effect_list_count )) return false;
	uint32_t  effect_list_item;
	this->effect_list.clear();
	{for(uint32_t i=0; i<effect_list_count;i++){
		if (!ba.read_uint32(effect_list_item)) return false;
		this->effect_list.push_back(effect_list_item);
	}}

	uint32_t find_map_list_count ;
	if (!ba.read_uint32( find_map_list_count )) return false;
	uint32_t  find_map_list_item;
	this->find_map_list.clear();
	{for(uint32_t i=0; i<find_map_list_count;i++){
		if (!ba.read_uint32(find_map_list_item)) return false;
		this->find_map_list.push_back(find_map_list_item);
	}}
	return true;
}

bool cli_login_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->regflag)) return false;
	if (!ba.write_uint32(this->left_xiaomee)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->last_islandid)) return false;
	if (!ba.write_uint32(this->last_mapid)) return false;
	if (!ba.write_uint32(this->last_x)) return false;
	if (!ba.write_uint32(this->last_y)) return false;
	if (!ba.write_uint32(this->task_nodeid_list.size())) return false;
	{for(uint32_t i=0; i<this->task_nodeid_list.size() ;i++){
		if (!ba.write_uint32(this->task_nodeid_list[i])) return false;
	}}
	if (!ba.write_uint32(this->use_clothes_list.size())) return false;
	{for(uint32_t i=0; i<this->use_clothes_list.size() ;i++){
		if (!ba.write_uint32(this->use_clothes_list[i])) return false;
	}}
	if (!ba.write_uint32(this->effect_list.size())) return false;
	{for(uint32_t i=0; i<this->effect_list.size() ;i++){
		if (!ba.write_uint32(this->effect_list[i])) return false;
	}}
	if (!ba.write_uint32(this->find_map_list.size())) return false;
	{for(uint32_t i=0; i<this->find_map_list.size() ;i++){
		if (!ba.write_uint32(this->find_map_list[i])) return false;
	}}
	return true;
}
	
cli_noti_effect_used_out::cli_noti_effect_used_out(){
	this->init();
}
void cli_noti_effect_used_out::init(){
	this->src_userid=0;
	this->itemid=0;
	this->unset_itemid=0;

}
bool  cli_noti_effect_used_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->unset_itemid)) return false;
	return true;
}

bool cli_noti_effect_used_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->unset_itemid)) return false;
	return true;
}
	
cli_noti_first_enter_island_out::cli_noti_first_enter_island_out(){
	this->init();
}
void cli_noti_first_enter_island_out::init(){
	this->islandid=0;
	this->xiaomee=0;
	this->is_newid=0;

}
bool  cli_noti_first_enter_island_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->xiaomee)) return false;
	if (!ba.read_uint32(this->is_newid)) return false;
	return true;
}

bool cli_noti_first_enter_island_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->xiaomee)) return false;
	if (!ba.write_uint32(this->is_newid)) return false;
	return true;
}
	
user_game_win_t::user_game_win_t(){
	this->init();
}
void user_game_win_t::init(){
	this->obj_userid=0;
	this->win_flag=0;
	this->get_point=0;

}
bool  user_game_win_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	if (!ba.read_uint32(this->win_flag)) return false;
	if (!ba.read_int32(this->get_point)) return false;
	return true;
}

bool user_game_win_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->win_flag)) return false;
	if (!ba.write_int32(this->get_point)) return false;
	return true;
}
	
cli_noti_game_end_out::cli_noti_game_end_out(){
	this->init();
}
void cli_noti_game_end_out::init(){
	this->user_win_list.clear();

}
bool  cli_noti_game_end_out::read_from_buf(byte_array_t & ba ){

	uint32_t user_win_list_count ;
	if (!ba.read_uint32( user_win_list_count )) return false;
	user_game_win_t  user_win_list_item;
	this->user_win_list.clear();
	{for(uint32_t i=0; i<user_win_list_count;i++){
		if (!user_win_list_item.read_from_buf(ba)) return false;
		this->user_win_list.push_back(user_win_list_item);
	}}
	return true;
}

bool cli_noti_game_end_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->user_win_list.size())) return false;
	{for(uint32_t i=0; i<this->user_win_list.size() ;i++){
		if (!this->user_win_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_noti_game_is_start_out::cli_noti_game_is_start_out(){
	this->init();
}
void cli_noti_game_is_start_out::init(){
	this->gameid=0;
	this->is_start=0;
	this->why_not_start_flag=0;
	this->obj_userid=0;

}
bool  cli_noti_game_is_start_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->is_start)) return false;
	if (!ba.read_uint32(this->why_not_start_flag)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_noti_game_is_start_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->is_start)) return false;
	if (!ba.write_uint32(this->why_not_start_flag)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_noti_game_opt_out::cli_noti_game_opt_out(){
	this->init();
}
void cli_noti_game_opt_out::init(){
	this->src_userid=0;
	this->_gamemsg_len=0;

}
bool  cli_noti_game_opt_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;

	if (!ba.read_uint32(this->_gamemsg_len )) return false;
	if (this->_gamemsg_len>9999) return false;
 	if (!ba.read_buf(this->gamemsg,this->_gamemsg_len)) return false;
	return true;
}

bool cli_noti_game_opt_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if (this->_gamemsg_len>9999) return false;
	if (!ba.write_uint32(this->_gamemsg_len))return false;
	if (!ba.write_buf(this->gamemsg,this->_gamemsg_len)) return false;
	return true;
}
	
cli_noti_game_request_out::cli_noti_game_request_out(){
	this->init();
}
void cli_noti_game_request_out::init(){
	this->gameid=0;
	this->src_userid=0;

}
bool  cli_noti_game_request_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->src_userid)) return false;
	if(!ba.read_buf(this->request_session,32)) return false;
	return true;
}

bool cli_noti_game_request_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->src_userid)) return false;
	if(!ba.write_buf(this->request_session,32)) return false;
	return true;
}
	
game_seat_t::game_seat_t(){
	this->init();
}
void game_seat_t::init(){
	this->gameid=0;
	this->side=0;
	this->userid=0;

}
bool  game_seat_t::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;
	if (!ba.read_uint32(this->side)) return false;
	if (!ba.read_uint32(this->userid)) return false;
	return true;
}

bool game_seat_t::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->side)) return false;
	if (!ba.write_uint32(this->userid)) return false;
	return true;
}
	
cli_noti_game_seat_out::cli_noti_game_seat_out(){
	this->init();
}
void cli_noti_game_seat_out::init(){
	this->gamelist.clear();

}
bool  cli_noti_game_seat_out::read_from_buf(byte_array_t & ba ){

	uint32_t gamelist_count ;
	if (!ba.read_uint32( gamelist_count )) return false;
	game_seat_t  gamelist_item;
	this->gamelist.clear();
	{for(uint32_t i=0; i<gamelist_count;i++){
		if (!gamelist_item.read_from_buf(ba)) return false;
		this->gamelist.push_back(gamelist_item);
	}}
	return true;
}

bool cli_noti_game_seat_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gamelist.size())) return false;
	{for(uint32_t i=0; i<this->gamelist.size() ;i++){
		if (!this->gamelist[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_noti_game_start_out::cli_noti_game_start_out(){
	this->init();
}
void cli_noti_game_start_out::init(){
	this->gameid=0;
	this->userid_list.clear();
	this->start_userid=0;
	this->step_timeout=0;

}
bool  cli_noti_game_start_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->gameid)) return false;

	uint32_t userid_list_count ;
	if (!ba.read_uint32( userid_list_count )) return false;
	uint32_t  userid_list_item;
	this->userid_list.clear();
	{for(uint32_t i=0; i<userid_list_count;i++){
		if (!ba.read_uint32(userid_list_item)) return false;
		this->userid_list.push_back(userid_list_item);
	}}
	if (!ba.read_uint32(this->start_userid)) return false;
	if (!ba.read_uint32(this->step_timeout)) return false;
	return true;
}

bool cli_noti_game_start_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->gameid)) return false;
	if (!ba.write_uint32(this->userid_list.size())) return false;
	{for(uint32_t i=0; i<this->userid_list.size() ;i++){
		if (!ba.write_uint32(this->userid_list[i])) return false;
	}}
	if (!ba.write_uint32(this->start_userid)) return false;
	if (!ba.write_uint32(this->step_timeout)) return false;
	return true;
}
	
cli_noti_game_user_left_game_out::cli_noti_game_user_left_game_out(){
	this->init();
}
void cli_noti_game_user_left_game_out::init(){
	this->obj_userid=0;

}
bool  cli_noti_game_user_left_game_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_noti_game_user_left_game_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_noti_game_user_out::cli_noti_game_user_out(){
	this->init();
}
void cli_noti_game_user_out::init(){
	this->state=0;
	this->game_user.init();

}
bool  cli_noti_game_user_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->state)) return false;
	if (!this->game_user.read_from_buf(ba)) return false;
	return true;
}

bool cli_noti_game_user_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->state)) return false;
	if (!this->game_user.write_to_buf(ba)) return false;
	return true;
}
	
cli_noti_get_invitation_out::cli_noti_get_invitation_out(){
	this->init();
}
void cli_noti_get_invitation_out::init(){
	this->itemid=0;

}
bool  cli_noti_get_invitation_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool cli_noti_get_invitation_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}
	
cli_noti_get_item_list_out::cli_noti_get_item_list_out(){
	this->init();
}
void cli_noti_get_item_list_out::init(){
	this->item_list.clear();

}
bool  cli_noti_get_item_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	item_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool cli_noti_get_item_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_noti_island_complete_out::cli_noti_island_complete_out(){
	this->init();
}
void cli_noti_island_complete_out::init(){
	this->userid=0;
	this->islandid=0;

}
bool  cli_noti_island_complete_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->islandid)) return false;
	return true;
}

bool cli_noti_island_complete_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->islandid)) return false;
	return true;
}
	
cli_noti_leave_island_out::cli_noti_leave_island_out(){
	this->init();
}
void cli_noti_leave_island_out::init(){
	this->cur_island=0;
	this->cur_mapid=0;
	this->islandid=0;
	this->mapid=0;

}
bool  cli_noti_leave_island_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->cur_island)) return false;
	if (!ba.read_uint32(this->cur_mapid)) return false;
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	return true;
}

bool cli_noti_leave_island_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->cur_island)) return false;
	if (!ba.write_uint32(this->cur_mapid)) return false;
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	return true;
}
	
cli_noti_one_user_info_out::cli_noti_one_user_info_out(){
	this->init();
}
void cli_noti_one_user_info_out::init(){
	this->user_info.init();

}
bool  cli_noti_one_user_info_out::read_from_buf(byte_array_t & ba ){
	if (!this->user_info.read_from_buf(ba)) return false;
	return true;
}

bool cli_noti_one_user_info_out::write_to_buf(byte_array_t & ba ){
	if (!this->user_info.write_to_buf(ba)) return false;
	return true;
}
	
cli_noti_set_color_out::cli_noti_set_color_out(){
	this->init();
}
void cli_noti_set_color_out::init(){
	this->userid=0;
	this->color=0;

}
bool  cli_noti_set_color_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->color)) return false;
	return true;
}

bool cli_noti_set_color_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->color)) return false;
	return true;
}
	
cli_noti_show_out::cli_noti_show_out(){
	this->init();
}
void cli_noti_show_out::init(){
	this->src_userid=0;
	this->expression_id=0;
	this->obj_userid=0;

}
bool  cli_noti_show_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;
	if (!ba.read_uint32(this->expression_id)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_noti_show_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if (!ba.write_uint32(this->expression_id)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_noti_svr_time_out::cli_noti_svr_time_out(){
	this->init();
}
void cli_noti_svr_time_out::init(){
	this->timestamp=0;

}
bool  cli_noti_svr_time_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->timestamp)) return false;
	return true;
}

bool cli_noti_svr_time_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->timestamp)) return false;
	return true;
}
	
cli_noti_talk_out::cli_noti_talk_out(){
	this->init();
}
void cli_noti_talk_out::init(){
	this->src_userid=0;
	this->obj_userid=0;
	this->_msg_len=0;

}
bool  cli_noti_talk_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;
	if(!ba.read_buf(this->src_nick,16)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;

	if (!ba.read_uint32(this->_msg_len )) return false;
	if (this->_msg_len>9999) return false;
 	if (!ba.read_buf(this->msg,this->_msg_len)) return false;
	return true;
}

bool cli_noti_talk_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if(!ba.write_buf(this->src_nick,16)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (this->_msg_len>9999) return false;
	if (!ba.write_uint32(this->_msg_len))return false;
	if (!ba.write_buf(this->msg,this->_msg_len)) return false;
	return true;
}
	
cli_noti_user_left_map_out::cli_noti_user_left_map_out(){
	this->init();
}
void cli_noti_user_left_map_out::init(){
	this->obj_userid=0;

}
bool  cli_noti_user_left_map_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_noti_user_left_map_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_noti_user_level_up_out::cli_noti_user_level_up_out(){
	this->init();
}
void cli_noti_user_level_up_out::init(){
	this->src_userid=0;
	this->level=0;
	this->experience=0;
	this->addexp=0;
	this->is_level_up=0;
	this->itemid=0;

}
bool  cli_noti_user_level_up_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->src_userid)) return false;
	if (!ba.read_uint32(this->level)) return false;
	if (!ba.read_uint32(this->experience)) return false;
	if (!ba.read_uint32(this->addexp)) return false;
	if (!ba.read_uint32(this->is_level_up)) return false;
	if (!ba.read_uint32(this->itemid)) return false;
	return true;
}

bool cli_noti_user_level_up_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->src_userid)) return false;
	if (!ba.write_uint32(this->level)) return false;
	if (!ba.write_uint32(this->experience)) return false;
	if (!ba.write_uint32(this->addexp)) return false;
	if (!ba.write_uint32(this->is_level_up)) return false;
	if (!ba.write_uint32(this->itemid)) return false;
	return true;
}
	
cli_noti_user_move_out::cli_noti_user_move_out(){
	this->init();
}
void cli_noti_user_move_out::init(){
	this->obj_userid=0;
	this->start_x=0;
	this->start_y=0;
	this->mouse_x=0;
	this->mouse_y=0;
	this->type=0;

}
bool  cli_noti_user_move_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;
	if (!ba.read_uint32(this->start_x)) return false;
	if (!ba.read_uint32(this->start_y)) return false;
	if (!ba.read_uint32(this->mouse_x)) return false;
	if (!ba.read_uint32(this->mouse_y)) return false;
	if (!ba.read_uint32(this->type)) return false;
	return true;
}

bool cli_noti_user_move_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->start_x)) return false;
	if (!ba.write_uint32(this->start_y)) return false;
	if (!ba.write_uint32(this->mouse_x)) return false;
	if (!ba.write_uint32(this->mouse_y)) return false;
	if (!ba.write_uint32(this->type)) return false;
	return true;
}
	
cli_noti_user_online_time_out::cli_noti_user_online_time_out(){
	this->init();
}
void cli_noti_user_online_time_out::init(){
	this->type=0;
	this->time=0;

}
bool  cli_noti_user_online_time_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if (!ba.read_uint32(this->time)) return false;
	return true;
}

bool cli_noti_user_online_time_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if (!ba.write_uint32(this->time)) return false;
	return true;
}
	
cli_noti_user_use_clothes_list_out::cli_noti_user_use_clothes_list_out(){
	this->init();
}
void cli_noti_user_use_clothes_list_out::init(){
	this->obj_userid=0;
	this->use_clothes_list.clear();

}
bool  cli_noti_user_use_clothes_list_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;

	uint32_t use_clothes_list_count ;
	if (!ba.read_uint32( use_clothes_list_count )) return false;
	uint32_t  use_clothes_list_item;
	this->use_clothes_list.clear();
	{for(uint32_t i=0; i<use_clothes_list_count;i++){
		if (!ba.read_uint32(use_clothes_list_item)) return false;
		this->use_clothes_list.push_back(use_clothes_list_item);
	}}
	return true;
}

bool cli_noti_user_use_clothes_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (!ba.write_uint32(this->use_clothes_list.size())) return false;
	{for(uint32_t i=0; i<this->use_clothes_list.size() ;i++){
		if (!ba.write_uint32(this->use_clothes_list[i])) return false;
	}}
	return true;
}
	
cli_post_msg_in::cli_post_msg_in(){
	this->init();
}
void cli_post_msg_in::init(){
	this->type=0;
	this->_msg_len=0;

}
bool  cli_post_msg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->type)) return false;
	if(!ba.read_buf(this->title,60)) return false;

	if (!ba.read_uint32(this->_msg_len )) return false;
	if (this->_msg_len>4096) return false;
 	if (!ba.read_buf(this->msg,this->_msg_len)) return false;
	return true;
}

bool cli_post_msg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->type)) return false;
	if(!ba.write_buf(this->title,60)) return false;
	if (this->_msg_len>4096) return false;
	if (!ba.write_uint32(this->_msg_len))return false;
	if (!ba.write_buf(this->msg,this->_msg_len)) return false;
	return true;
}
	
cli_proto_header_in::cli_proto_header_in(){
	this->init();
}
void cli_proto_header_in::init(){
	this->proto_length=0;
	this->cmdid=0;
	this->timestamp=0;
	this->seq=0;
	this->result=0;
	this->useSever=0;

}
bool  cli_proto_header_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->proto_length)) return false;
	if (!ba.read_uint16(this->cmdid)) return false;
	if (!ba.read_uint32(this->timestamp)) return false;
	if (!ba.read_uint32(this->seq)) return false;
	if (!ba.read_uint16(this->result)) return false;
	if (!ba.read_uint16(this->useSever)) return false;
	return true;
}

bool cli_proto_header_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->proto_length)) return false;
	if (!ba.write_uint16(this->cmdid)) return false;
	if (!ba.write_uint32(this->timestamp)) return false;
	if (!ba.write_uint32(this->seq)) return false;
	if (!ba.write_uint16(this->result)) return false;
	if (!ba.write_uint16(this->useSever)) return false;
	return true;
}
	
cli_reg_in::cli_reg_in(){
	this->init();
}
void cli_reg_in::init(){
	this->channelid=0;
	this->age=0;
	this->color=0;
	this->item_list.clear();

}
bool  cli_reg_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->channelid)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	if (!ba.read_uint32(this->age)) return false;
	if (!ba.read_uint32(this->color)) return false;

	uint32_t item_list_count ;
	if (!ba.read_uint32( item_list_count )) return false;
	item_t  item_list_item;
	this->item_list.clear();
	{for(uint32_t i=0; i<item_list_count;i++){
		if (!item_list_item.read_from_buf(ba)) return false;
		this->item_list.push_back(item_list_item);
	}}
	return true;
}

bool cli_reg_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->channelid)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	if (!ba.write_uint32(this->age)) return false;
	if (!ba.write_uint32(this->color)) return false;
	if (!ba.write_uint32(this->item_list.size())) return false;
	{for(uint32_t i=0; i<this->item_list.size() ;i++){
		if (!this->item_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
cli_reg_out::cli_reg_out(){
	this->init();
}
void cli_reg_out::init(){
	this->left_xiaomee=0;

}
bool  cli_reg_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->left_xiaomee)) return false;
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool cli_reg_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->left_xiaomee)) return false;
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}
	
cli_set_busy_state_in::cli_set_busy_state_in(){
	this->init();
}
void cli_set_busy_state_in::init(){
	this->state=0;

}
bool  cli_set_busy_state_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->state)) return false;
	return true;
}

bool cli_set_busy_state_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->state)) return false;
	return true;
}
	
cli_set_color_in::cli_set_color_in(){
	this->init();
}
void cli_set_color_in::init(){
	this->color=0;

}
bool  cli_set_color_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->color)) return false;
	return true;
}

bool cli_set_color_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->color)) return false;
	return true;
}
	
cli_set_effect_used_in::cli_set_effect_used_in(){
	this->init();
}
void cli_set_effect_used_in::init(){
	this->itemid=0;
	this->type=0;

}
bool  cli_set_effect_used_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->itemid)) return false;
	if (!ba.read_uint32(this->type)) return false;
	return true;
}

bool cli_set_effect_used_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid)) return false;
	if (!ba.write_uint32(this->type)) return false;
	return true;
}
	
cli_set_item_used_list_in::cli_set_item_used_list_in(){
	this->init();
}
void cli_set_item_used_list_in::init(){
	this->itemid_list.clear();

}
bool  cli_set_item_used_list_in::read_from_buf(byte_array_t & ba ){

	uint32_t itemid_list_count ;
	if (!ba.read_uint32( itemid_list_count )) return false;
	uint32_t  itemid_list_item;
	this->itemid_list.clear();
	{for(uint32_t i=0; i<itemid_list_count;i++){
		if (!ba.read_uint32(itemid_list_item)) return false;
		this->itemid_list.push_back(itemid_list_item);
	}}
	return true;
}

bool cli_set_item_used_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->itemid_list.size())) return false;
	{for(uint32_t i=0; i<this->itemid_list.size() ;i++){
		if (!ba.write_uint32(this->itemid_list[i])) return false;
	}}
	return true;
}
	
cli_set_nick_in::cli_set_nick_in(){
	this->init();
}
void cli_set_nick_in::init(){

}
bool  cli_set_nick_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->nick,16)) return false;
	return true;
}

bool cli_set_nick_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->nick,16)) return false;
	return true;
}
	
cli_show_in::cli_show_in(){
	this->init();
}
void cli_show_in::init(){
	this->expression_id=0;
	this->obj_userid=0;

}
bool  cli_show_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->expression_id)) return false;
	if (!ba.read_uint32(this->obj_userid)) return false;
	return true;
}

bool cli_show_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->expression_id)) return false;
	if (!ba.write_uint32(this->obj_userid)) return false;
	return true;
}
	
cli_talk_in::cli_talk_in(){
	this->init();
}
void cli_talk_in::init(){
	this->obj_userid=0;
	this->_msg_len=0;

}
bool  cli_talk_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->obj_userid)) return false;

	if (!ba.read_uint32(this->_msg_len )) return false;
	if (this->_msg_len>9999) return false;
 	if (!ba.read_buf(this->msg,this->_msg_len)) return false;
	return true;
}

bool cli_talk_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->obj_userid)) return false;
	if (this->_msg_len>9999) return false;
	if (!ba.write_uint32(this->_msg_len))return false;
	if (!ba.write_buf(this->msg,this->_msg_len)) return false;
	return true;
}
	
cli_talk_npc_in::cli_talk_npc_in(){
	this->init();
}
void cli_talk_npc_in::init(){
	this->npc_id=0;

}
bool  cli_talk_npc_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->npc_id)) return false;
	return true;
}

bool cli_talk_npc_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->npc_id)) return false;
	return true;
}
	
cli_task_complete_node_in::cli_task_complete_node_in(){
	this->init();
}
void cli_task_complete_node_in::init(){
	this->islandid=0;
	this->task_nodeid=0;

}
bool  cli_task_complete_node_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->task_nodeid)) return false;
	return true;
}

bool cli_task_complete_node_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->task_nodeid)) return false;
	return true;
}
	
cli_task_complete_node_out::cli_task_complete_node_out(){
	this->init();
}
void cli_task_complete_node_out::init(){
	this->task_nodeid_list.clear();

}
bool  cli_task_complete_node_out::read_from_buf(byte_array_t & ba ){

	uint32_t task_nodeid_list_count ;
	if (!ba.read_uint32( task_nodeid_list_count )) return false;
	uint32_t  task_nodeid_list_item;
	this->task_nodeid_list.clear();
	{for(uint32_t i=0; i<task_nodeid_list_count;i++){
		if (!ba.read_uint32(task_nodeid_list_item)) return false;
		this->task_nodeid_list.push_back(task_nodeid_list_item);
	}}
	return true;
}

bool cli_task_complete_node_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->task_nodeid_list.size())) return false;
	{for(uint32_t i=0; i<this->task_nodeid_list.size() ;i++){
		if (!ba.write_uint32(this->task_nodeid_list[i])) return false;
	}}
	return true;
}
	
cli_task_del_node_in::cli_task_del_node_in(){
	this->init();
}
void cli_task_del_node_in::init(){
	this->islandid=0;
	this->task_nodeid=0;

}
bool  cli_task_del_node_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->task_nodeid)) return false;
	return true;
}

bool cli_task_del_node_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->task_nodeid)) return false;
	return true;
}
	
cli_user_move_in::cli_user_move_in(){
	this->init();
}
void cli_user_move_in::init(){
	this->start_x=0;
	this->start_y=0;
	this->mouse_x=0;
	this->mouse_y=0;
	this->type=0;

}
bool  cli_user_move_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->start_x)) return false;
	if (!ba.read_uint32(this->start_y)) return false;
	if (!ba.read_uint32(this->mouse_x)) return false;
	if (!ba.read_uint32(this->mouse_y)) return false;
	if (!ba.read_uint32(this->type)) return false;
	return true;
}

bool cli_user_move_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->start_x)) return false;
	if (!ba.write_uint32(this->start_y)) return false;
	if (!ba.write_uint32(this->mouse_x)) return false;
	if (!ba.write_uint32(this->mouse_y)) return false;
	if (!ba.write_uint32(this->type)) return false;
	return true;
}
	
cli_walk_in::cli_walk_in(){
	this->init();
}
void cli_walk_in::init(){
	this->islandid=0;
	this->mapid=0;
	this->x=0;
	this->y=0;

}
bool  cli_walk_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->islandid)) return false;
	if (!ba.read_uint32(this->mapid)) return false;
	if (!ba.read_uint32(this->x)) return false;
	if (!ba.read_uint32(this->y)) return false;
	return true;
}

bool cli_walk_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->islandid)) return false;
	if (!ba.write_uint32(this->mapid)) return false;
	if (!ba.write_uint32(this->x)) return false;
	if (!ba.write_uint32(this->y)) return false;
	return true;
}
	
cli_work_get_lottery_out::cli_work_get_lottery_out(){
	this->init();
}
void cli_work_get_lottery_out::init(){
	this->get_count=0;

}
bool  cli_work_get_lottery_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->get_count)) return false;
	return true;
}

bool cli_work_get_lottery_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->get_count)) return false;
	return true;
}
	
login_activate_game_in::login_activate_game_in(){
	this->init();
}
void login_activate_game_in::init(){
	this->which_game=0;

}
bool  login_activate_game_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->which_game)) return false;
	if(!ba.read_buf(this->active_code,10)) return false;
	if(!ba.read_buf(this->imgid,16)) return false;
	if(!ba.read_buf(this->verif_code,6)) return false;
	return true;
}

bool login_activate_game_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->which_game)) return false;
	if(!ba.write_buf(this->active_code,10)) return false;
	if(!ba.write_buf(this->imgid,16)) return false;
	if(!ba.write_buf(this->verif_code,6)) return false;
	return true;
}
	
login_activate_game_out::login_activate_game_out(){
	this->init();
}
void login_activate_game_out::init(){
	this->flag=0;
	this->_img_len=0;

}
bool  login_activate_game_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if(!ba.read_buf(this->img_id,16)) return false;

	if (!ba.read_uint32(this->_img_len )) return false;
	if (this->_img_len>1000) return false;
 	if (!ba.read_buf(this->img,this->_img_len)) return false;
	return true;
}

bool login_activate_game_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if(!ba.write_buf(this->img_id,16)) return false;
	if (this->_img_len>1000) return false;
	if (!ba.write_uint32(this->_img_len))return false;
	if (!ba.write_buf(this->img,this->_img_len)) return false;
	return true;
}
	
login_check_game_activate_in::login_check_game_activate_in(){
	this->init();
}
void login_check_game_activate_in::init(){
	this->which_game=0;

}
bool  login_check_game_activate_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->which_game)) return false;
	return true;
}

bool login_check_game_activate_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->which_game)) return false;
	return true;
}
	
login_check_game_activate_out::login_check_game_activate_out(){
	this->init();
}
void login_check_game_activate_out::init(){
	this->flag=0;

}
bool  login_check_game_activate_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	return true;
}

bool login_check_game_activate_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	return true;
}
	
login_get_ranged_svr_list_in::login_get_ranged_svr_list_in(){
	this->init();
}
void login_get_ranged_svr_list_in::init(){
	this->startid=0;
	this->endid=0;

}
bool  login_get_ranged_svr_list_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->startid)) return false;
	if (!ba.read_uint32(this->endid)) return false;
	return true;
}

bool login_get_ranged_svr_list_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->startid)) return false;
	if (!ba.write_uint32(this->endid)) return false;
	return true;
}
	
login_get_ranged_svr_list_out::login_get_ranged_svr_list_out(){
	this->init();
}
void login_get_ranged_svr_list_out::init(){
	this->online_list.clear();

}
bool  login_get_ranged_svr_list_out::read_from_buf(byte_array_t & ba ){

	uint32_t online_list_count ;
	if (!ba.read_uint32( online_list_count )) return false;
	online_item_t  online_list_item;
	this->online_list.clear();
	{for(uint32_t i=0; i<online_list_count;i++){
		if (!online_list_item.read_from_buf(ba)) return false;
		this->online_list.push_back(online_list_item);
	}}
	return true;
}

bool login_get_ranged_svr_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->online_list.size())) return false;
	{for(uint32_t i=0; i<this->online_list.size() ;i++){
		if (!this->online_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
login_get_recommeded_list_in::login_get_recommeded_list_in(){
	this->init();
}
void login_get_recommeded_list_in::init(){

}
bool  login_get_recommeded_list_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->session,32)) return false;
	return true;
}

bool login_get_recommeded_list_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->session,32)) return false;
	return true;
}
	
login_get_recommeded_list_out::login_get_recommeded_list_out(){
	this->init();
}
void login_get_recommeded_list_out::init(){
	this->max_online_id=0;
	this->vip=0;
	this->online_list.clear();

}
bool  login_get_recommeded_list_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->max_online_id)) return false;
	if (!ba.read_uint32(this->vip)) return false;

	uint32_t online_list_count ;
	if (!ba.read_uint32( online_list_count )) return false;
	online_item_t  online_list_item;
	this->online_list.clear();
	{for(uint32_t i=0; i<online_list_count;i++){
		if (!online_list_item.read_from_buf(ba)) return false;
		this->online_list.push_back(online_list_item);
	}}
	return true;
}

bool login_get_recommeded_list_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->max_online_id)) return false;
	if (!ba.write_uint32(this->vip)) return false;
	if (!ba.write_uint32(this->online_list.size())) return false;
	{for(uint32_t i=0; i<this->online_list.size() ;i++){
		if (!this->online_list[i].write_to_buf(ba)) return false;
	}}
	return true;
}
	
login_login_in::login_login_in(){
	this->init();
}
void login_login_in::init(){
	this->channelid=0;
	this->which_game=0;
	this->user_ip=0;

}
bool  login_login_in::read_from_buf(byte_array_t & ba ){
	if(!ba.read_buf(this->passwd_md5_two,32)) return false;
	if (!ba.read_uint32(this->channelid)) return false;
	if (!ba.read_uint32(this->which_game)) return false;
	if (!ba.read_uint32(this->user_ip)) return false;
	if(!ba.read_buf(this->img_id,16)) return false;
	if(!ba.read_buf(this->verif_code,6)) return false;
	return true;
}

bool login_login_in::write_to_buf(byte_array_t & ba ){
	if(!ba.write_buf(this->passwd_md5_two,32)) return false;
	if (!ba.write_uint32(this->channelid)) return false;
	if (!ba.write_uint32(this->which_game)) return false;
	if (!ba.write_uint32(this->user_ip)) return false;
	if(!ba.write_buf(this->img_id,16)) return false;
	if(!ba.write_buf(this->verif_code,6)) return false;
	return true;
}
	
login_login_out::login_login_out(){
	this->init();
}
void login_login_out::init(){
	this->verif_flag=0;
	this->create_role_flag=0;

}
bool  login_login_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->verif_flag)) return false;
	if(!ba.read_buf(this->session,16)) return false;
	if (!ba.read_uint32(this->create_role_flag)) return false;
	return true;
}

bool login_login_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->verif_flag)) return false;
	if(!ba.write_buf(this->session,16)) return false;
	if (!ba.write_uint32(this->create_role_flag)) return false;
	return true;
}
	
login_refresh_img_in::login_refresh_img_in(){
	this->init();
}
void login_refresh_img_in::init(){
	this->flag=0;

}
bool  login_refresh_img_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	return true;
}

bool login_refresh_img_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	return true;
}
	
login_refresh_img_out::login_refresh_img_out(){
	this->init();
}
void login_refresh_img_out::init(){
	this->flag=0;
	this->_img_len=0;

}
bool  login_refresh_img_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->flag)) return false;
	if(!ba.read_buf(this->imgid,16)) return false;

	if (!ba.read_uint32(this->_img_len )) return false;
	if (this->_img_len>1000) return false;
 	if (!ba.read_buf(this->img,this->_img_len)) return false;
	return true;
}

bool login_refresh_img_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->flag)) return false;
	if(!ba.write_buf(this->imgid,16)) return false;
	if (this->_img_len>1000) return false;
	if (!ba.write_uint32(this->_img_len))return false;
	if (!ba.write_buf(this->img,this->_img_len)) return false;
	return true;
}
	
noti_cli_leave_out::noti_cli_leave_out(){
	this->init();
}
void noti_cli_leave_out::init(){
	this->reason=0;

}
bool  noti_cli_leave_out::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->reason)) return false;
	return true;
}

bool noti_cli_leave_out::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->reason)) return false;
	return true;
}
	