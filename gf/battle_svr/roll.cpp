#include "roll.hpp"
#include "fwd_decl.hpp"
#include "utils.hpp"
#include "battle.hpp"
#include "battle_impl.hpp"
#include <kf/item_impl.hpp>
#include "item.hpp"

/**
 *   @brief  add all player in map to data
 *   @param  map_t* 
 *   @return  void
**/

void battle_roll_data::add_map_player(map_t* map)
{
	PlayerSet::iterator pItr = map->players.begin();
	for(; pItr != map->players.end(); ++pItr)
	{
		add_player_id( (*pItr)->id);
	}
}

/**
 *   @brief  check the roll is finish and the delete from data
 *   @param  void 
 *   @return  true
**/

bool battle_roll_data::check_delete()
{
	if(  roll_datas.size() == 0)
	{
		if( btl->m_prepare_roll_datas.size() > 0)
		{
			battle_roll_data* data = btl->m_prepare_roll_datas.front();
			btl->add_roll_data(data);
			btl->m_prepare_roll_datas.pop_front();
		}	
		ev_mgr.remove_events(*this);
		btl->del_roll_data(this);
	}
	return true;
}

/**
 *   @brief  timer roll result when the timer is in time this function will called
 *   @param  void 
 *   @return  0
**/

int battle_roll_data::timer_roll_result()
{
	////////////自动帮忙roll点////////////////////
	for( uint32_t i =0; i <roll_datas.size(); i++)
	{
		for( uint32_t j = 0; j < MAX_ROLL_PLAYER_COUNT; j++)
		{
			if( player_ids[j] == 0)continue;
			if( roll_datas[i].roll_points[j] == 0){
				int32_t roll_point = 0;
				uint32_t roll_id = roll_datas[i].roll_id;
				player_roll_point(roll_datas[i].roll_id, player_ids[j], roll_point);
				btl->notify_team_roll_point(roll_id, player_ids[j], roll_point);
				if(is_roll_finish(roll_id))
				{
					check_delete();
				}
			}

		}
	}
	return 0;
}

/**
 *   @brief  player roll result 
 *   @param  uint32_t, uint32_t
 *   @return  0
**/

int battle_roll_data::player_roll_result(uint32_t roll_id, uint32_t player_id)
{	
	Player* p = btl->get_player_in_btl(player_id);
	/////////////////////////////////////////////////////////	
	std::vector<roll_data>::iterator pItr = roll_datas.begin();
	for(; pItr != roll_datas.end(); ++pItr){
		if( pItr->roll_id == roll_id){
			db_pick_roll_item(p, *pItr);			
			pItr = roll_datas.erase(pItr);
			break;
		}
	}
	return 0;
}

/**
 *    @brief  player refuse roll result
 *    @param  uint32_t
 *    @return  0
**/

int battle_roll_data::player_refuse_roll_result(uint32_t roll_id)
{
	//////////所有玩家拒绝在roll_id局中拒绝roll点/////////////////
	//物品回收？？？
	/////////////////////////////////////////////////////////////	
	std::vector<roll_data>::iterator pItr = roll_datas.begin();
	for(; pItr != roll_datas.end(); ++pItr){
		if( pItr->roll_id == roll_id){
			pItr = roll_datas.erase(pItr);
			break;
		}
	}
	return 0;
}

/**
 *   @brief set player roll point
 *   @param Player*, uint32_t, uint32_t , uint32_t, int32_t&
 *   @return  true sucess,  false otherwirse fail
**/

bool set_player_roll_point(Player* p, uint32_t battle_id, uint32_t roll_id, int32_t roll_flag, int32_t& roll_point)
{
	Battle* btl = p->btl;
	if(btl->id() != battle_id){
		return false;
	}

	battle_roll_data *pdata = btl->get_battle_roll_data_by_roll_id(roll_id);
	if(pdata == NULL){
		return false;
	}
	
	bool ret = pdata->player_roll_point(roll_id, p->id, roll_point, roll_flag);
	if(ret == false){
		return false;
	}
	ret = pdata->is_roll_finish(roll_id);
	return pdata->check_delete();
}


/**
 *   @brief create battle roll data
 *   @param  Battle*
 *   @return  battle_roll_data*
**/

battle_roll_data* create_battle_roll_data( Battle* btl )
{
	battle_roll_data* pData = new battle_roll_data();
	pData->set_battle(btl);
	return pData;
}

/**
 *   @brief  destroy battle roll data
 *   @param  battle_roll_data*
 *   @return void
**/

void  destroy_battle_roll_data(battle_roll_data* pData)
{
	delete pData;
}

/**
 *   @brief player roll item cmd
 *   @param Player*
 *   @return int 
**/

int player_roll_item_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	CHECK_VAL_EQ(bodylen, (uint32_t)sizeof(player_roll_item_t));
	player_roll_item_t* pRoll = reinterpret_cast<player_roll_item_t*>(body);	
	int32_t roll_point = 0;	
	bool ret = set_player_roll_point(p, pRoll->battle_id, pRoll->roll_id, pRoll->roll_flag, roll_point);
	if( !ret){
		return send_header_to_player(p, p->waitcmd, cli_err_roll_item_fail, 1); 
	}
	Battle *btl = p->btl;
	//notify the other team player execpt myself and not clear waitcmd
	btl->notify_team_roll_point(pRoll->roll_id, p->id, roll_point, p->id);
	//notify myself and clear waitcmd
	return send_roll_point(p, pRoll->roll_id, roll_point);
}

/**
 *   @brief  send roll point
 *   @param  Player*, uint32_t, uint32_t 
 *   @return int
**/

int send_roll_point(Player* p, uint32_t roll_id,  uint32_t roll_point)
{
	int idx = 0;
	idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, p->id, idx);
	pack_h(pkgbuf, roll_id, idx);
	pack_h(pkgbuf, roll_point, idx);

	init_btl_proto_head(pkgbuf, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 *   @brief send db request to roll item
 *   @param  Player*,  roll_data&
 *   @return  int
**/

int db_pick_roll_item(Player* p,  roll_data& data)
{
	uint32_t itmid = data.item_id;
	const GfItem* itm = items->get_item(itmid);
    if(itm == NULL)return 0;
	int idx = 0;

	pack_h(dbpkgbuf, data.roll_id, idx);	
	pack_h(dbpkgbuf, itmid, idx);
	pack_h(dbpkgbuf, itm->db_category(), idx);
	pack_h(dbpkgbuf, itm->max(), idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(itm->duration * clothes_duration_ratio), idx);
	pack_h(dbpkgbuf, p->max_bag_grid_count,  idx);
	uint32_t pos = get_unique_item_bitpos(itmid);
	TRACE_LOG("get_unique_item_bitpos%u:%u", pos, itmid);
	if (pos && !test_bit_on(p->unique_item_bit, pos)) {
		uint8_t bit_tmp[sizeof(p->unique_item_bit)]= "";
		memcpy(bit_tmp, p->unique_item_bit, sizeof(p->unique_item_bit));
		taomee::set_bit_on(bit_tmp, pos);
		pack_h(dbpkgbuf, static_cast<uint32_t>(sizeof(bit_tmp)), idx);
		pack(dbpkgbuf, bit_tmp, sizeof(bit_tmp), idx);
	} else {
		pack_h(dbpkgbuf, 0, idx);
	}
	return send_request_to_db(p, p->id, p->role_tm, dbproto_player_pick_roll_item, dbpkgbuf, idx);
}

/**
 *    @brief db player pick roll item callback
 *    @param  Battle*
 *    @return  battle_roll_data*
**/

int db_player_pick_roll_item_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if(ret == 1017 || ret == 1002 || ret == 1003){
		return send_header_to_player(p,  btl_player_get_roll_item, cli_err_base_dberr + ret, 0);
	}
	player_pick_roll_item_rsp_t* rsp = reinterpret_cast<player_pick_roll_item_rsp_t*>(body);	
	int sucess = 1;
	if(rsp->err_code != 0)
	{
		p->btl->drop_item_to_team(p->cur_map, rsp->item_id, p->pos().x(), p->pos().y());
		sucess = 0;
	}
	uint32_t pos = get_unique_item_bitpos(rsp->item_id);
	if(pos)
	{
		if (!test_bit_on(p->unique_item_bit, pos)){
			 taomee::set_bit_on(p->unique_item_bit, pos);
		}
	}
	p->btl->notify_team_get_item(rsp->roll_id, p->id, rsp->item_id, rsp->unique_id, sucess);
	return 0;
}
