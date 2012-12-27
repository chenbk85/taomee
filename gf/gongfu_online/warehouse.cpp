#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>
#include "item.hpp"
#include "warehouse.hpp"
#include "utils.hpp"

using namespace taomee;


//-----------------------------------------------------------
// function implement
//-----------------------------------------------------------

/**
  * @brief initialization player's warehouse and set the max warehouse grid count 
  * @param player_t* , max_warehouse_grid_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int init_player_warehouse(player_t* p)
{
	/*if(p ==NULL){
		return 0;
	}*/
	p->m_data_maps = new std::map<uint32_t,  warehouse_item_data>; 
	p->m_clothes_data_maps = new std::map<uint32_t, warehouse_clothes_item_data>;
	//p->m_max_grid_count = max_warehouse_grid_count;
	return 0;
}

/**
  * @brief uninitialization player's warehouse and set the max warehouse grid count 
  * @param player_t* ,
  * @return  0 sucess,  -1 otherwirse fail
 */
int final_player_warehouse(player_t* p)
{
	/*if(p ==NULL){
		return 0;
	}*/

    if(p->m_data_maps ){
		delete p->m_data_maps;
		p->m_data_maps = NULL;
	} 
	if(p->m_clothes_data_maps){
		delete p->m_clothes_data_maps;
		p->m_clothes_data_maps = NULL;
	}
    //p->m_max_grid_count = 0;
    return 0;
}

/**
  * @brief check delete item from player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */
bool check_warehouse_delete_item(player_t* p,  uint32_t item_id, int item_count)
{
	/*
	if(p == NULL || p->m_data_maps == NULL){
		return false;
	}*/
	const GfItem* pItem = items->get_item(item_id);
    if (!pItem){
            WARN_LOG("check_delete_item error :[%u]", item_id);
            return false;
	}
	map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->find(item_id); 
	if(pItr == p->m_data_maps->end()){
		return false;
	}
	if(item_count == -1){
		return true;
	}
	if(item_count > 0 &&(int) (pItr->second.item_count) >= item_count){
		return true;
	}
	return false;
}

bool check_warehouse_delete_clothes_item(player_t*p, uint32_t id, uint32_t item_id)
{
	/*if(p == NULL || p->m_clothes_data_maps == NULL) {
		return false;
	}*/
	const GfItem* pItem = items->get_item(item_id);
    if (!pItem){
		WARN_LOG("check_delete_item error :[%u]", item_id);
		return false;
    }
	map<uint32_t, warehouse_clothes_item_data>::iterator pItr = p->m_clothes_data_maps->find(id);
	if(pItr == p->m_clothes_data_maps->end()){
		return false;
	}
	if(pItr->second.attireid != item_id){
		return false;
	}
	return true;
}

/**
  * @brief check put item to player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */
bool check_warehouse_add_item(player_t *p, uint32_t item_id, int item_count)
{
	//if(p == NULL || p->m_data_maps == NULL )return false;
	const GfItem* pItem = items->get_item(item_id);
	if (!pItem){
    	WARN_LOG("check_enough_grid error :[%u]", item_id);
        return false;
    }
	//you can not put task item into warehouse
	if ( items->is_task_item(pItem->category()) && !(pItem->summon_id())){
		return false;
	}
	//you can not put skill book item into warehouse
	if( items->is_skill_book(pItem->category())){
		return false;
	}

	map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->find(item_id);
    if(pItr == p->m_data_maps->end()) {
    	return get_warehouse_cur_used_grid_count(p) < get_warehouse_max_grid_count(p);
	}
	return (pItr->second.item_count + item_count) <= pItem->max(); 	
}

bool check_warehouse_add_clothes_item(player_t*p,  uint32_t id,  uint32_t item_id)
{
	//if(p == NULL || p->m_clothes_data_maps == NULL) return false;
	const GfItem* pItem = items->get_item(item_id);
    if (!pItem){
    	WARN_LOG("check_enough_grid error :[%u]", item_id);
        return false;
    }
    //you can not put task item into warehouse
    if ( items->is_task_item(pItem->category()) ){
    	return false;
    }
	map<uint32_t,  warehouse_clothes_item_data>::iterator pItr = p->m_clothes_data_maps->find(id);
    if(pItr != p->m_clothes_data_maps->end()){
		return false;
	}
	return get_warehouse_cur_used_grid_count(p) < get_warehouse_max_grid_count(p);
}

/**
  * @brief check the item count in player's warehouse 
  * @param player_t* 
  * @return true ok, otherwise false
 */
bool check_warehouse_item_count(player_t* p, uint32_t item_id, int item_count)
{
	//if(p == NULL || p->m_data_maps == NULL )return false;
	const GfItem* pItem = items->get_item(item_id);
    if (!pItem){
 	       WARN_LOG("check_warehouse_item_count error :[%u]", item_id);
	       return false;
 	}
	map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->find(item_id);
	if(pItr != p->m_data_maps->end() && (int)pItr->second.item_count >= item_count && item_count > 0){
		return true;
	}
	return false;
}

/**
  * @brief add item to player's warehouse 
  * @param player_t* , item_id, item_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int add_warehouse_item(player_t *p, uint32_t item_id,  int item_count)
{
	map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->find(item_id);
	if(pItr == p->m_data_maps->end()){
		warehouse_item_data data;
		data.item_id = item_id;
		data.item_count = item_count;
		(*(p->m_data_maps))[item_id] = data;
	
	}
	else
	{
		pItr->second.item_count += item_count;
	}

	return 0;
}

int add_warehouse_clothes_item(player_t* p,  uint32_t id, uint32_t attireid, uint32_t get_time, uint32_t attire_rank, uint32_t duration, uint32_t attire_lv, uint32_t end_time)
{
	warehouse_clothes_item_data data;
	data.id = id;
	data.attireid = attireid;
	data.get_time = get_time;
	data.attire_rank = attire_rank;
	data.duration  = duration;
	data.attire_lv = attire_lv;
	data.end_time = end_time;
	(*(p->m_clothes_data_maps))[id] = data;
	return 0;
}

/**
  * @brief delete item from player's warehouse 
  * @param player_t* , item_id, item_count
  * @return  0 sucess,  -1 otherwirse fail
 */
int del_warehouse_item(player_t* p, uint32_t item_id,  int item_count)
{
	if( !check_warehouse_delete_item(p, item_id, item_count) )return -1;
	map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->find(item_id);	
	if(pItr == p->m_data_maps->end()){
		return -1;
	}
	if( item_count == -1 || item_count ==(int) (pItr->second.item_count)){
		p->m_data_maps->erase(pItr);
	}
	if(item_count < (int)(pItr->second.item_count)){
		pItr->second.item_count -= item_count;
	}
	return 0;
}

int del_warehouse_clothes_item(player_t* p, uint32_t id, uint32_t item_id)
{
	if( !check_warehouse_delete_clothes_item(p, id, item_id) ) {
		return -1;
	}
	map<uint32_t, warehouse_clothes_item_data>::iterator pItr = p->m_clothes_data_maps->find(id);
	if(pItr == p->m_clothes_data_maps->end()) {
		return -1;
	}
	p->m_clothes_data_maps->erase(pItr);
	return 0;
}

/**
  * @brief increase player's warehouse grid count 
  * @param player_t* , grid_count 
  * @return  0 sucess,  -1 otherwirse fail
 */
int  increase_warehouse_grid_count(player_t* p, uint32_t grid_count)
{
	//if(p == NULL)return 0;
	//p->m_max_grid_count += grid_count;
	return 0;
}

/**
  * @brief get count of player's warehouse grid 
  * @param player_t* 
  * @return  value be more than or equal to 0
 */
int  get_warehouse_max_grid_count(player_t* p)
{
	if(!is_vip_player(p)){
		return DEFAULT_WAREHOUSE_GRID_COUNT;
	}
	return DEFAULT_WAREHOUSE_GRID_COUNT + p->extern_warehouse_grid_count;
}

/**
  * @brief get current userd count of player's warehouse grid 
  * @param player_t* 
  * @return  value be more than or equal to 0
 */
int  get_warehouse_cur_used_grid_count(player_t* p)
{
	//if(p == NULL || p->m_data_maps == NULL || p->m_clothes_data_maps == NULL) return 0;
	return p->m_data_maps->size() + p->m_clothes_data_maps->size();
}



/**
  * @brief initialization player's warehouse item data
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int  init_player_warehouse_list(player_t* p, get_warehouse_item_list_rsp_t* rsp)
{
	//if(p == NULL || p->m_data_maps == NULL || rsp == NULL)return -1;
	p->m_data_maps->clear();
	for(uint32_t i = 0; i< rsp->item_cnt; i++)
	{
		add_warehouse_item(p, rsp->item[i].item_id,  rsp->item[i].count);		
	}
	return 0;
}

/**
  * @brief callback function for getting warehouse itemlist
  * @param player_t* 
  * @return 0 sucess, unzero otherwise 
 */
int db_get_warehouse_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	CHECK_DBERR(p, ret);
	get_warehouse_item_list_rsp_t* rsp = reinterpret_cast<get_warehouse_item_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_warehouse_item_list_rsp_t) + rsp->item_cnt * sizeof(db_item_elem_t));
	init_player_warehouse_list(p, rsp);
	return send_warehouse_item_list_info(p);	
}

/**
  * @brief send warehouse item list to player;
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int send_warehouse_item_list_info(player_t* p)
{
	//if(p == NULL || p->m_data_maps == NULL)return -1;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, get_warehouse_max_grid_count(p), idx);
	uint32_t item_count = p->m_data_maps->size();
	pack(pkgbuf, item_count, idx);	

	std::map<uint32_t,  warehouse_item_data>::iterator pItr = p->m_data_maps->begin();
	for(; pItr != p->m_data_maps->end(); ++pItr)
	{
		pack(pkgbuf, pItr->second.item_id, idx);
		pack(pkgbuf, pItr->second.item_count, idx);	
	}	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return  send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief send db request for moving item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_warehouse_to_bag_db_request(player_t* p, uint32_t item_id,  int count, int coin)
{
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, count, idx);
	pack_h(dbpkgbuf, coin, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_move_item_warehouse2bag, dbpkgbuf, idx);		
}

/**
  * @brief send db request for moving item from bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_bag_to_warehouse_db_request(player_t* p, uint32_t item_id,  int count, int coin)
{
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);  
	pack_h(dbpkgbuf, count, idx);
	pack_h(dbpkgbuf, coin, idx);
	pack_h(dbpkgbuf, get_warehouse_max_grid_count(p), idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_move_item_bag2warehouse, dbpkgbuf, idx);
}

/**
  * @brief callback function for moving item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_move_item_warehouse2bag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	//if(p == NULL || p->m_data_maps == NULL)return -1;
	move_item_rsp_t *rsp = reinterpret_cast<move_item_rsp_t*>(body);
	
 	p->coins = rsp->left_coin;
	
	int idx = sizeof(cli_proto_t);
 	pack(pkgbuf, rsp->left_coin, idx);
 	pack(pkgbuf, rsp->item_id, idx);
 	pack(pkgbuf, rsp->cnt, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	//delete item from warehouse
	ret = del_warehouse_item(p, rsp->item_id, rsp->cnt);
	if(ret != 0){
		WARN_LOG("del_warehouse_item error [%u,%u,%u]", p->id, rsp->item_id, rsp->cnt);
	}
	//add item to bag
	ret = p->my_packs->add_item(p, rsp->item_id, rsp->cnt, 0, false);
	if(ret != 0){
		WARN_LOG("add_item error [%u,%u,%u]", p->id, rsp->item_id, rsp->cnt);
	}
	//TRACE_LOG("db_move_item_warehouse2bag_callback player_id:%u, item_id:%u, item_count:%u, left_coin:%u",
	//p->id, rsp->item_id, rsp->cnt, rsp->left_coin);
	//notify client
    return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback function for moving item from  bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_move_item_bag2warehouse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	move_item_rsp_t *rsp = reinterpret_cast<move_item_rsp_t*>(body);
		
    p->coins = rsp->left_coin;
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->left_coin, idx);
    pack(pkgbuf, rsp->item_id, idx);
    pack(pkgbuf, rsp->cnt, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	//add item to warehouse
	ret = add_warehouse_item(p,  rsp->item_id, rsp->cnt);
	if(ret != 0){
		WARN_LOG("add_warehouse_item error [%u,%u,%u]", p->id, rsp->item_id, rsp->cnt);
	}
	//delete item from bag
	ret = p->my_packs->del_item(p, rsp->item_id, rsp->cnt, 0, false);
	if(ret != 0){
		WARN_LOG("del_item error [%u,%u,%u]", p->id, rsp->item_id, rsp->cnt);
	}
	//notify client
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief move item from warehouse to bag
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_warehouse_to_bag_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id = 0, item_count = 0;
  	unpack(body, item_id, idx);
  	unpack(body, item_count, idx);
	const GfItem* pItem = items->get_item(item_id);
	if (p->trade_grp) {
		ERROR_LOG("%u in trade cannot move_item_warehouse_to_bag", p->id);
		return -1;
	}
	if(pItem == NULL ||  items->is_clothes(pItem->category())){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	
	bool ret = check_warehouse_item_count(p, item_id, item_count);
	if(!ret){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	ret = p->my_packs->check_enough_bag_item_grid(p, item_id, item_count);	
	if(!ret){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_item_bag_full , 1);
	}
	if(p->coins < MOVE_ITEM_W2B_CONSUME){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_not_enough_money, 1);
	}
	//TRACE_LOG("move_item_warehouse_to_bag_cmd player_id:%u, item_id:%u, item_count:%u", p->id, item_id, item_count);
	KDEBUG_LOG(p->id, "ITEM W2B\t player_id:%u item_id:%u count:%u", p->id, item_id, item_count);
	return  move_item_warehouse_to_bag_db_request(p, item_id, item_count, MOVE_ITEM_W2B_CONSUME);
}

/**
  * @brief move item from bag to warehouse
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int move_item_bag_to_warehouse_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id = 0, item_count = 0;
	unpack(body, item_id, idx);
	unpack(body, item_count, idx);
	const GfItem* pItem = items->get_item(item_id);
	if (pItem && pItem->summon_id()) {
	} else {
		if(pItem == NULL || items->is_clothes(pItem->category()) || pItem->is_item_shop_sale()){
			return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
		}
	}

	if (p->trade_grp) {
		ERROR_LOG("%u in trade cannot move_item_bag_to_warehouse", p->id);
		return -1;
	}
	bool ret = p->my_packs->check_bag_item_count(item_id, item_count);
	if(!ret){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	ret = check_warehouse_add_item(p,  item_id, item_count);		
	if(!ret){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	if(p->coins < MOVE_ITEM_B2W_CONSUME){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_not_enough_money , 1);
	}

	KDEBUG_LOG(p->id, "ITEM B2W\t player_id:%u item_id:%u count:%u", p->id, item_id, item_count);
	return move_item_bag_to_warehouse_db_request(p, item_id, item_count, MOVE_ITEM_B2W_CONSUME);
}

/**
  * @brief get warehouse item list
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int get_warehouse_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_warehouse_item_list(p);		 
}

/**
  * @brief send db request for getting warehouse item list
  * @param player_t* 
  * @return 0 sucess, -1 otherwise 
 */
int db_get_warehouse_item_list(player_t* p)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_warehouse_item_list, 0, 0);
}


/*---------------------------------------------------------------------*/

int get_warehouse_clothes_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_warehouse_clothes_item_list(p);
}

int db_get_warehouse_clothes_item_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_warehouse_clothes_item_list, 0, 0);
}


int db_get_warehouse_clothes_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_warehouse_clothes_item_list_rsp_t* rsp = reinterpret_cast<get_warehouse_clothes_item_list_rsp_t*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(get_warehouse_clothes_item_list_rsp_t) + rsp->item_cnt * sizeof( db_warehouse_clothes_item_elem_t));
    init_player_warehouse_clothes_list(p, rsp);
    return send_warehouse_clothes_item_list_info(p);
}


int send_warehouse_clothes_item_list_info(player_t* p)
{
	//if(p == NULL || p->m_clothes_data_maps == NULL)return -1;
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, get_warehouse_max_grid_count(p), idx);
    uint32_t item_count = p->m_clothes_data_maps->size();
    pack(pkgbuf, item_count, idx);

    std::map<uint32_t,  warehouse_clothes_item_data>::iterator pItr = p->m_clothes_data_maps->begin();
    for(; pItr != p->m_clothes_data_maps->end(); ++pItr)
    {
        pack(pkgbuf, pItr->second.attireid, idx);
        pack(pkgbuf, pItr->second.id, idx);
		pack(pkgbuf, pItr->second.duration, idx);
		//pack(pkgbuf, pItr->second.end_time, idx);
		pack(pkgbuf, pItr->second.attire_lv, idx);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return  send_to_player(p, pkgbuf, idx, 1);
}

int init_player_warehouse_clothes_list(player_t* p, get_warehouse_clothes_item_list_rsp_t* rsp)
{
	//if(p == NULL || p->m_clothes_data_maps == NULL || rsp == NULL)return -1;
    p->m_clothes_data_maps->clear();
    for(uint32_t i = 0; i< rsp->item_cnt; i++)
    {
        add_warehouse_clothes_item(p, rsp->item[i].id, rsp->item[i].attireid, rsp->item[i].get_time, rsp->item[i].attire_rank, rsp->item[i].duration, rsp->item[i].attire_lv, rsp->item[i].end_time);
    }
    return 0;
}


int move_clothes_item_bag_to_warehouse_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id = 0, id = 0;
	unpack(body, item_id, idx);
	unpack(body, id, idx);
	const GfItem* pItem = items->get_item(item_id);
	if(pItem == NULL ||  !items->is_clothes(pItem->category()) || pItem->is_item_shop_sale() 
        || pItem->lifetime() != 0 || pItem->quality_lv > 4){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	//check this item is exist in bag
	if( !p->my_packs->is_clother_item_exist(id, item_id) ){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	//then check having enough grid for moving item from bag to warehouse
	int ret = check_warehouse_add_clothes_item(p, id, item_id);
	if(!ret){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_warehouse_full, 1);
	}	
	//check money
	if(p->coins < MOVE_ITEM_B2W_CONSUME){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_not_enough_money, 1);
	}
	//TRACE_LOG("move_clothes_item_bag_to_warehouse_cmd player_id:%u, id:%u, item_id:%u, spend_coin:%u",
		//p->id, id, item_id, MOVE_ITEM_B2W_CONSUME);	
	KDEBUG_LOG(p->id, "CLOTHES B2W\t player_id:%u id:%u item_id:%u", p->id, id, item_id);
	return move_clothes_item_bag_to_warehouse_db_request(p, item_id, id, MOVE_ITEM_B2W_CONSUME);
}


int move_clothes_item_bag_to_warehouse_db_request(player_t* p, uint32_t item_id,  int id, int coin)
{
    int idx = 0;
    pack_h(dbpkgbuf, item_id, idx);
    pack_h(dbpkgbuf, id, idx);
    pack_h(dbpkgbuf, coin, idx);
	pack_h(dbpkgbuf, get_warehouse_max_grid_count(p), idx);
    return send_request_to_db(p, p->id, p->role_tm,  dbproto_move_clothes_item_bag2warehouse, dbpkgbuf, idx);	
}

int db_move_clothes_item_bag2warehouse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	//if(p == NULL || p->m_clothes_data_maps == NULL || p->m_data_maps == NULL)return -1;
	CHECK_DBERR(p, ret);
	move_clothes_item_rsp_t *rsp = reinterpret_cast< move_clothes_item_rsp_t* >(body);
	p->coins = rsp->left_coin;
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->old_id, idx);
    pack(pkgbuf, rsp->left_coin, idx);
    pack(pkgbuf, rsp->item_id, idx);
    pack(pkgbuf, rsp->id, idx);
	pack(pkgbuf, rsp->duration, idx);
	//pack(pkgbuf, rsp->end_time, idx);
	pack(pkgbuf, rsp->attire_lv, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	//delete item from clothes bag 
	ret = p->my_packs->del_clothes(rsp->old_id, 0);
	if(ret != 0){
		WARN_LOG("del_clothes error [player_id:%u,id:%u]", p->id, rsp->old_id);
	}
	//add item to warehouse
	ret  = add_warehouse_clothes_item(p, rsp->id, rsp->item_id, rsp->get_time, rsp->attire_rank, rsp->duration, 
			rsp->attire_lv, rsp->end_time);
	if(ret != 0){
		WARN_LOG("add_warehouse_clothes_item error [player_id:%u,id:%u, item_id:%u]", p->id, rsp->id, rsp->item_id);
	}
	//TRACE_LOG("db_move_clothes_item_bag2warehouse_callback player_id:%u,old_id:%u id:%u, item_id:%u, left_coin:%u",
	//	p->id, rsp->old_id, rsp->id, rsp->item_id, rsp->left_coin);	
	//notify client
	return send_to_player(p, pkgbuf, idx, 1);		
}






int move_clothes_item_warehouse_to_bag_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
    uint32_t item_id = 0, id = 0;
    unpack(body, item_id, idx);
    unpack(body, id, idx);
    const GfItem* pItem = items->get_item(item_id);
    if(pItem == NULL || !items->is_clothes(pItem->category()) ){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	//check item exist in warehouse
	if(! check_warehouse_delete_clothes_item(p, id, item_id)){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_can_not_move_item, 1);
	}
	//check remain grid count in clothes bag
	if( p->my_packs->get_remain_clothes_bag_grid(p) < 1 ){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_clothes_bag_full, 1);
	}	

	if(p->coins < MOVE_ITEM_W2B_CONSUME){
		return send_header_to_player(p, p->waitcmd, cli_err_warehouse_not_enough_money, 1);
	}
	//TRACE_LOG("move_clothes_item_warehouse_to_bag_cmd player_id:%u, id:%u, item_id:%u, spend_coin:%u", p->id, id, item_id, MOVE_ITEM_W2B_CONSUME);
	KDEBUG_LOG(p->id, "CLOTHES W2B\t player_id:%u id:%u item_id:%u", p->id, id, item_id);
	return move_clothes_item_warehouse_to_bag_db_request(p, item_id, id, MOVE_ITEM_W2B_CONSUME);
}

int move_clothes_item_warehouse_to_bag_db_request(player_t* p, uint32_t item_id,  int id, int coin)
{
    int idx = 0;
    pack_h(dbpkgbuf, item_id, idx);
    pack_h(dbpkgbuf, id, idx);
    pack_h(dbpkgbuf, coin, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
    return send_request_to_db(p, p->id, p->role_tm,  dbproto_move_clothes_item_warehouse2bag, dbpkgbuf, idx);
}

int db_move_clothes_item_warehouse2bag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	//if(p == NULL || p->m_clothes_data_maps == NULL || p->m_data_maps == NULL)return -1;
	CHECK_DBERR(p, ret);

    move_clothes_item_rsp_t *rsp = reinterpret_cast< move_clothes_item_rsp_t* >(body);
    p->coins = rsp->left_coin;

    int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->old_id, idx);
    pack(pkgbuf, rsp->left_coin, idx);
    pack(pkgbuf, rsp->item_id, idx);
    pack(pkgbuf, rsp->id, idx);
    pack(pkgbuf, rsp->duration, idx);
	//pack(pkgbuf, rsp->end_time, idx);
	pack(pkgbuf, rsp->attire_lv, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	//delete from warehoues
	ret = del_warehouse_clothes_item(p, rsp->old_id, rsp->item_id);
	if( ret != 0){
		WARN_LOG("del_warehouse_clothes_item error [player_id:%u,id:%u, item_id:%u]", p->id, rsp->old_id, rsp->item_id);
	}	
	//add to clothes bag
	ret = p->my_packs->add_clothes(p, rsp->item_id, rsp->id, rsp->duration, 0, rsp->get_time, rsp->end_time, rsp->attire_lv);
	if( ret != 0){
		WARN_LOG("add_clothes error [player_id:%u, id:%u, item_id:%u", p->id, rsp->id, rsp->item_id);
	}
	//notify client
	//TRACE_LOG("db_move_clothes_item_warehouse2bag_callback player_id:%u,old_id:%u id:%u, item_id:%u, left_coin:%u", 
		//p->id, rsp->old_id, rsp->id, rsp->item_id, rsp->left_coin );
    return send_to_player(p, pkgbuf, idx, 1);	
}
