#include "apothecary.hpp"
#include "cli_proto.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "item.hpp"
#include "login.hpp"
#include "buy_item_limit.hpp"
#include "global_data.hpp"
#include "stat_log.hpp"
using namespace taomee;

static void do_stat_log_learn_secondary_pro(uint32_t userid, uint32_t pro_type);
static void do_stat_log_secondary_pro_level_change(uint32_t userid, uint32_t pro_type, uint32_t level);
static void do_stat_log_player_little_game(uint32_t userid, uint32_t pro_type);
static void do_stat_log_yaoshi_level_up(uint32_t old_level, uint32_t new_level);

void do_stat_log_yaoshi_level_up(uint32_t old_level, uint32_t new_level)
{
	uint32_t msg_content[2] = { 0 };

	switch(old_level)
	{
		if(old_level == 0){
			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up1,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		else if( old_level == 1){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up1,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up2,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		else if(old_level == 2){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up2,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up3,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		else if(old_level == 3){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up3,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up4,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		else if(old_level == 4){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up4,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up5,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));		
		}
		else if(old_level == 5){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up5,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up6,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		else if(old_level == 6){
			msg_content[0] = 0;
			msg_content[1] = 1;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up6,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));

			msg_content[0] = 1;
			msg_content[1] = 0;
			msglog(statistic_logfile,  stat_log_yaoshi_level_up7,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
	}
}

void do_stat_log_player_little_game(uint32_t userid, uint32_t pro_type)
{
	uint32_t msg_content = 1;

	switch(pro_type)
	{
		case apothecary_profession_type:
		{
			msglog(statistic_logfile, stat_log_player_little_game,  get_now_tv()->tv_sec, &msg_content, sizeof(msg_content));
		}
		break;
	}	
}

void do_stat_log_learn_secondary_pro(uint32_t userid, uint32_t pro_type)
{
	switch(pro_type)	
	{	
		case apothecary_profession_type:
		{
			msglog(statistic_logfile, stat_log_learn_yaoshi_secondary_pro, get_now_tv()->tv_sec, &userid, sizeof(userid) );	
		}
		break;
	}
	TRACE_LOG("stat log: user_id = %u, pro_type = %u", userid, pro_type);
}
void do_stat_log_secondary_pro_level_change(uint32_t userid, uint32_t pro_type, uint32_t level)
{
	uint32_t buf[2] = {0};
	buf[0] = userid;
	buf[1] = level;
	switch(pro_type)
	{
		case apothecary_profession_type:
		{
			msglog(statistic_logfile, stat_log_yaoshi_secondary_pro_level_change, get_now_tv()->tv_sec, buf, sizeof(buf) );
		}
		break;	
	}
	TRACE_LOG("stat log: user_id = %u, pro_type = %u, level = %u", userid, pro_type, level);
}

/**
 *  @brief init formulatio data from xml file
 *  @param const char* 
 *  @return  true sucess,  false  otherwirse fail
**/
bool formulation_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	xmlDocPtr  doc = NULL;
    xmlNodePtr root = NULL;
    xmlNodePtr node = NULL;	
	
	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		return false;
	}
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		return false;
	}
	bool ret = true;	

	node = root->xmlChildrenNode;	

	while(node)
	{
		formulation_data data;
		if ((!xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("Activity")))) {
			get_xml_prop_def(data.id, node, "ID", 0);
		} else if ((!xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("Formulation")))) {
			get_xml_prop_def(data.id, node, "FormulationID", 0);
		} else {
			node = node->next;
			ERROR_LOG("%s", node->name);
			continue;
		}
		
		get_xml_prop_def(data.level_limit, node, "LevelLimit", 0);
		get_xml_prop_def(data.add_exp, node, "AddExp", 0);
		get_xml_prop_def(data.create_item_id, node, "ProductID", 0);		
		get_xml_prop_def(data.create_item_count, node, "ProductCount", 0);
		get_xml_prop_def(data.pro_type, node, "Profession", 0);
		get_xml_prop_def(data.need_vitality_point, node, "VitalityPoint", 0);
	
		xmlNodePtr material_node = node->xmlChildrenNode;
		while(material_node)
		{
			
			uint32_t odds = 0;
			uint32_t itemid = 0;
			uint32_t count = 0;
			get_xml_prop_def(itemid,  material_node, "ItemID", 0);
			get_xml_prop_def(count, material_node, "Cnt", 0);	
			get_xml_prop_def(odds, material_node, "Odds", 0);
			
			if ((!xmlStrcmp(material_node->name, reinterpret_cast<const xmlChar *>("RandomProduct")))) {
				random_item_data  mdata;
				mdata.item_id = itemid;
				mdata.item_count = count;
				mdata.odds = odds;
				if (mdata.item_id) {
					mdata.odds = odds;
					data.random_items.push_back(mdata);	
				}
			} else {
				material_data  mdata;
				mdata.item_id = itemid;
				mdata.item_count = count;
				if (mdata.item_id) {
					data.materials.push_back(mdata);	
				}
			} 
			material_node = material_node->next;
		}
		add_formulation_data(data);
		node = node->next;
	}	
	
	xmlFreeDoc(doc);
    xmlCleanupParser();	
	return ret;
}

/**
 *  @brief uninit formulatio data 
 *  @param const char* 
 *  @return  true sucess,  false  otherwirse fail
**/
bool formulation_data_mgr::final()
{
	m_data_maps.clear();
	return true;
}


/**
 *   @brief add formulatio data to manager
 *   @param formulation_data&
 *   @return  true sucess,  false  otherwirse fail
**/
bool  formulation_data_mgr::add_formulation_data( formulation_data& data )
{
	if( is_formulation_exist(data.id) ){
		return false;
	}
	m_data_maps[data.id] = data;
	TRACE_LOG("%u %u %u %u", data.id, data.add_exp, m_data_maps[data.id].id, m_data_maps[data.id].add_exp);
	return true;
}


/**
 *   @brief check formulatio data exist
 *   @param uint32_t
 *   @return  true sucess,  false  otherwirse fail
**/
bool  formulation_data_mgr::is_formulation_exist(uint32_t id)
{
	std::map<uint32_t, formulation_data>::iterator pItr = m_data_maps.find(id);
	if(pItr == m_data_maps.end()){
		return false;
	}
	return true;
}

/**
 *  @brief get formulation count of manager
 *  @param  null
 *  @return the count of datas
**/
uint32_t  formulation_data_mgr::get_formulation_data_count()
{
	return m_data_maps.size();	
}



/**
 *  @brief get formulation data by id
 *  @param  uint32_t
 *  @return point sucess , null otherwise fail
**/
formulation_data* formulation_data_mgr::get_formulation_data_by_id(uint32_t id)
{
	std::map<uint32_t, formulation_data>::iterator pItr = m_data_maps.find(id);
	if(pItr == m_data_maps.end()){
		return NULL;
	}
	return &(pItr->second);
}

/**
 *   @brief initialization player's secondary professtion
 *   @param player_t* 
 *   @return  true sucess,  false  otherwirse fail
**/
bool init_player_secondary_profession(player_t* p)
{
	p->m_pro_data_maps = new std::map<uint32_t, secondary_profession_data>;
	p->vitality_point = 0;
	ADD_TIMER_EVENT(p, player_auto_recover_vitality_point, 0, get_now_tv()->tv_sec + 60);
	return true;
}

/**
 *   @brief unitialization player's secondary professtion
 *   @param player_t*  
 *   @return  true sucess,  false  otherwirse fail
**/
bool final_player_secondary_profession(player_t* p)
{
	if(p->m_pro_data_maps)
	{
		delete p->m_pro_data_maps;
		p->m_pro_data_maps = NULL;
		p->vitality_point = 0;
	}
	return true;
}


/**
 *  @brief get player's secondary professtion by type
 *  @param player_t*  utin32_t 
 *  @return  point sucess,  NULL otherwirse fail
**/
secondary_profession_data*  get_player_secondary_profession_by_type(player_t* p , uint32_t type)
{
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return false;
	}
	
	std::map<uint32_t, secondary_profession_data>::iterator pItr = p->m_pro_data_maps->find(type);
	if(pItr == p->m_pro_data_maps->end()){
		return NULL;
	}
	return &(pItr->second);
}

/**
 *   @brief check player's secondary professtion exist
 *   @param player_t*  uint32_t 
 *   @return  true exist,  not exist false
**/
bool is_player_secondary_profession_exist(player_t*p, uint32_t type)
{
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return false;			    
	}
	return  p->m_pro_data_maps->find(type) != p->m_pro_data_maps->end();
}

/**
 *  @brief add player's secondary professtion 
 *  @param player_t*  uint32_t 
 *  @return  true sucess, false otherwise fail
**/
bool add_player_secondary_profession(player_t* p, uint32_t type)
{
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return false;			    
	}
	
	if(is_player_secondary_profession_exist(p, type))return false;
	secondary_profession_data data;
	data.set_profession_data(1, 0, (profession_type)type);
	(*(p->m_pro_data_maps))[type] = data;
	return true;
}

/**
 *  @brief del player's secondary professtion 
 *  @param player_t*  uint32_t 
 *  @return  true sucess, false otherwise fail
**/
bool del_player_secondary_professoin(player_t* p, uint32_t type)
{
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return false;			    
	}
	std::map<uint32_t, secondary_profession_data>::iterator pItr = p->m_pro_data_maps->find(type);	
	if( pItr != p->m_pro_data_maps->end())
	{
		p->m_pro_data_maps->erase(pItr);
	}
	return true;
}


/**
 *  @brief set player's secondary professtion exp and calc the level from exp
 *  @param player_t* , uint32_t, uint32_t
 *  @return  true sucess, false otherwise fail
**/
bool set_player_secondary_professoin_exp(player_t*p, uint32_t type, uint32_t exp)
{
	if( !( type > begin_profession_type && type < end_profession_type) ){
	    return false;        		    
	}
	secondary_profession_data*	pData =	get_player_secondary_profession_by_type(p, type);
	if(pData == NULL){
		return false;
	}
	pData->set_profession_exp(exp);
	return true;
}

/**
 *  @brief add player's secondary professtion exp and calc the level from exp
 *  @param player_t* , uint32_t, uint32_t
 *  @return  true sucess, false otherwise fail
**/
bool add_player_secondary_professoin_exp(player_t*p, uint32_t type, uint32_t exp,  uint32_t* fact_add_exp)
{
	*fact_add_exp = 0;
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return false;    
	}	
	secondary_profession_data*  pData = get_player_secondary_profession_by_type(p, type);
	if(pData == NULL){
		return false;
	}
	uint32_t old_level = pData->m_level;
	pData->add_profession_exp(exp, *fact_add_exp);
	if (old_level != pData->m_level) {
		do_stat_log_universal_interface_2(0x09810001, 0, p->id, pData->m_level);
	}

    //title
    if (pData->m_level == 7 && type == apothecary_profession_type) {
        do_special_title_logic(p, 1002);

        secondary_profession_data*  pNewData = get_player_secondary_profession_by_type(p,
            iron_man_profession_type);
        if (pNewData && pNewData->m_level == 6) {
            do_special_title_logic(p, 1004);
        }
    }
    if (pData->m_level == 6 && type == iron_man_profession_type) {
        do_special_title_logic(p, 1003);

        secondary_profession_data*  pNewData = get_player_secondary_profession_by_type(p,
            apothecary_profession_type);
        if (pNewData && pNewData->m_level == 7) {
            do_special_title_logic(p, 1004);
        }
    }
    
    //level up
	/*
	if(pData->m_level > old_level){
		do_stat_log_secondary_pro_level_change(p->id, type, pData->m_level);
		do_stat_log_yaoshi_level_up(old_level, pData->m_level);
		//检查成就是否达成
		for( uint32_t i = 41; i<= 46; i++ )
		{
			if( is_player_achievement_data_exist(p, i)) continue;
			if( handles[i].id == 0 || handles[i].p_check_func == NULL || handles[i].p_do_func == NULL)continue;
			if( handles[i].p_check_func(p, i)){
				handles[i].p_do_func(p, i);
			}
		}
	}*/
	return true;
}


/**
 *   @brief learn player's secondary professtion 
 *   @param player_t* , uint8_t, uint32_t
 *   @return  true sucess, false otherwise fail
**/
int learn_secondary_pro_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t type = 0;
	unpack(body, type, idx);
	if( !( type > begin_profession_type && type < end_profession_type) ){
		return send_header_to_player(p, p->waitcmd,  cli_err_secondary_pro_invalid_pro_type, 1);	
	}
	//check the player has learned the profession
	if( is_player_secondary_profession_exist(p, type) ){
		return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_exist, 1);	
	}
	//check money 
	if( p->coins < LEARN_SECONDARY_PRO_COSTS){
		return send_header_to_player(p, p->waitcmd,  cli_err_secondary_pro_not_enough_money , 1);
	}
	//check player level
	if( p->lv <  LEARN_SECONDARY_PRO_LEVEL_LIMIT){
		return send_header_to_player(p, p->waitcmd,  cli_err_secondary_pro_player_low_level, 1);
	}	


	KDEBUG_LOG(p->id, "LEARN PRO\t player_id:%u, pro_type:%u", p->id, type);
	//send db request to dbserver
	return learn_secondary_pro_db_request(p, type, LEARN_SECONDARY_PRO_COSTS);
}


/**
 *    @brief send learn secondary professtion db request to dbserver
 *    @param player_t* , uint32_t, uint32_t
 *    @return  0 sucess, unzero otherwise 
**/
int learn_secondary_pro_db_request(player_t* p,  uint32_t type, uint32_t money)
{
	int idx = 0;
	pack_h(dbpkgbuf, type, idx);
	pack_h(dbpkgbuf, money, idx);
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_learn_secondary_pro, dbpkgbuf, idx);
}


/**
 *    @brief  learn secondary professtion db callback function
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise 
**/
int db_learn_secondary_pro_callback( player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	learn_secondary_pro_rsp_t *rsp = reinterpret_cast<learn_secondary_pro_rsp_t*>(body);
	do_stat_log_coin_add_reduce(p, rsp->left_coin, channel_string_sec_pro);

	p->coins = rsp->left_coin;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->left_coin, idx);
	pack(pkgbuf, rsp->type, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	
	add_player_secondary_profession(p, rsp->type);
	//msglog info learn pro
	do_stat_log_learn_secondary_pro(p->id, rsp->type);
	//msglog info pro level
	do_stat_log_secondary_pro_level_change(p->id, rsp->type, 1);
	//msglog info level up
	do_stat_log_yaoshi_level_up(0, 1);	
	do_stat_log_universal_interface_1(0x09810001, p->role_type, 1);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 *    @brief  fuse one formulation
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise 
**/
int secondary_pro_fuse_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t formulation_id = 0;
	uint32_t count = 0;
	uint32_t err_code = 0;
	unpack(body, formulation_id, idx);
	unpack(body, count, idx);
	formulation_data* pData = get_formulation_data_mgr()->get_formulation_data_by_id(formulation_id);	
	if(pData == NULL){
			return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_invalid_formulation_id, 1);	
	}
	secondary_profession_data* pProData = get_player_secondary_profession_by_type(p, pData->pro_type);
	if(pProData == NULL){
			return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_not_exist, 1);
	}

	if( pProData->get_profession_level() < pData->level_limit){
			return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_low_level, 1);
	}
	if( !check_secondary_pro_fuse_condition(p, pData, count, &err_code) )
	{
			return send_header_to_player(p, p->waitcmd, err_code, 1);
	}
	KDEBUG_LOG(p->id, "PRO FUSE\t player_id:%u formution_id:%u count:%u", p->id, formulation_id, count);
	return  secondary_pro_fuse_db_request(p, pData, count, pProData->m_pCalc->get_max_exp()); 	
}

/**
 *    @brief  check fuse one formulation condition
 *    @param player_t* 
 *    @return true sucess, false otherwise fail 
**/
bool check_secondary_pro_fuse_condition(player_t*p, formulation_data* pData, uint32_t count,  uint32_t* err_code)
{
	*err_code = 0;
	//check has enough materials
	for(uint32_t i =0; i< pData->materials.size(); i++)
	{
		if( !p->my_packs->check_bag_item_count(pData->materials[i].item_id, pData->materials[i].item_count) )
		{
			*err_code = cli_err_secondary_pro_item_count_err;
			return false;
		}
	}
	int min =  pData->create_item_count < count ? pData->create_item_count : count;
	//check has enough grid to put item
	if( !p->my_packs->check_enough_bag_item_grid(p, pData->create_item_id, min))
	{
		*err_code = cli_err_secondary_pro_bag_full;	
		return false;
	}
	//check has enough vitality_point 
	if( p->vitality_point < pData->need_vitality_point)
	{
		*err_code = cli_err_secondary_pro_not_enough_point;
		return false;
	}
	return true;
}

/**
 *    @brief  send fuse one formulation db request to db server
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise 
**/
int secondary_pro_fuse_db_request(player_t* p, formulation_data* pData, uint32_t count, uint32_t max_exp_limit)
{
	int idx = 0;
	uint32_t min = pData->create_item_count < count ? pData->create_item_count : count; 
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	pack_h(dbpkgbuf, max_exp_limit, idx);
	pack_h(dbpkgbuf, pData->pro_type, idx);
	//寒假双倍经验
	pack_h(dbpkgbuf, pData->add_exp * 2, idx);
	pack_h(dbpkgbuf, pData->id, idx);
	//pack_h(dbpkgbuf, pData->create_item_id, idx);
	//pack_h(dbpkgbuf, min, idx);
	
	uint32_t material_count = pData->materials.size();
	uint32_t random_count = pData->random_items.size();
	
	pack_h(dbpkgbuf, material_count, idx);
	int random_idx = idx;
	pack_h(dbpkgbuf, 0, idx);
	for(uint32_t i=0; i< material_count; i++)
	{
		pack_h(dbpkgbuf,  pData->materials[i].item_id, idx);
		pack_h(dbpkgbuf,  pData->materials[i].item_count, idx);
	}

	uint32_t random_out_cnt = 0;
	for(uint32_t i=0; i< random_count; i++)
	{
		uint32_t rd_num = random() % 100;
		if (rd_num < pData->random_items[i].odds) {
			pack_h(dbpkgbuf,  pData->random_items[i].item_id, idx);
			pack_h(dbpkgbuf,  pData->random_items[i].item_count, idx);
			random_out_cnt++;
		}
	}
	pack_h(dbpkgbuf, pData->create_item_id, idx);
	pack_h(dbpkgbuf, min, idx);
	random_out_cnt++;
	pack_h(dbpkgbuf, random_out_cnt, random_idx);
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_secondary_pro_fuse, dbpkgbuf, idx);
}

int db_set_player_vitality_point(player_t* p, uint32_t point)
{
	int idx = 0;
	pack_h(dbpkgbuf, point, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_role_vitality_point, dbpkgbuf, idx);
}


int db_set_player_second_pro_exp(player_t* p, uint32_t pro_type, uint32_t pro_exp)
{
	int idx = 0;
	pack_h(dbpkgbuf,  pro_type, idx);
	pack_h(dbpkgbuf,  pro_exp, idx);
	return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_second_pro_exp, dbpkgbuf, idx);
}

/**
 *    @brief  fuse one formulation condtion callback
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise  
**/
int db_secondary_pro_fuse_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	secondary_pro_fuse_rsp_t *rsp = reinterpret_cast<secondary_pro_fuse_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(secondary_pro_fuse_rsp_t) + (rsp->material_count + rsp->random_count)*sizeof(db_material_item_t));	


	do_stat_log_player_little_game(p->id, rsp->pro_type);


	 //add exp for secondary profession
	 uint32_t fact_add_exp = 0;
	 add_player_secondary_professoin_exp(p, rsp->pro_type, rsp->add_exp, &fact_add_exp);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->pro_type, idx);
	pack(pkgbuf, rsp->id, idx);
	pack(pkgbuf, fact_add_exp, idx);
//	pack(pkgbuf, rsp->create_item_id, idx);
//	pack(pkgbuf, rsp->create_item_count, idx);
	pack(pkgbuf, rsp->material_count, idx);
	pack(pkgbuf, rsp->random_count, idx);
	//delete material
	for(uint32_t i =0; i< rsp->material_count; i++)
	{
		p->my_packs->del_item(p, rsp->items[i].item_id, rsp->items[i].item_count, channel_string_sec_pro);
		pack(pkgbuf, rsp->items[i].item_id, idx);
		pack(pkgbuf, rsp->items[i].item_count, idx);
	}
	db_random_add_item_t* p_random_add = (db_random_add_item_t*)(rsp->items + rsp->material_count);
	for(uint32_t i =0; i< rsp->random_count; i++)
	{
		p->my_packs->add_item(p, p_random_add[i].item_id, p_random_add[i].item_count, channel_string_sec_pro);
		pack(pkgbuf, p_random_add[i].item_id, idx);
		pack(pkgbuf, p_random_add[i].item_count, idx);
	}
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);
	//modify vitality_point
	formulation_data* pData = get_formulation_data_mgr()->get_formulation_data_by_id(rsp->id);
	if(pData && pData->need_vitality_point > 0)
	{
		dec_player_vitality_point(p, pData->need_vitality_point);			
	}
	return 0;
}


/**
 *    @brief init player profession list callback
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise  
**/
int init_player_secondary_pro_list(player_t*p, get_secondary_pro_list_rsp_t* rsp)
{
	for(uint32_t i=0; i< rsp->pro_count; i++)
	{
		add_player_secondary_profession(p,  rsp->pros[i].pro);
		set_player_secondary_professoin_exp(p,  rsp->pros[i].pro,  rsp->pros[i].pro_exp);
	}
	return 0;
}


/**
 *    @brief send get profession list to dbserver
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise  
**/
int db_get_secondary_pro_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_secondary_pro_list, 0, 0);
}


/**
 *    @brief db get profession list callback
 *    @param player_t* 
 *    @return 0 sucess, unzero otherwise  
**/
int db_get_secondary_pro_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_secondary_pro_list_rsp_t* rsp = reinterpret_cast<get_secondary_pro_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_secondary_pro_list_rsp_t) + rsp->pro_count*sizeof(secondary_pro_info_t));
	init_player_secondary_pro_list(p, rsp);
	if ( p->waitcmd == cli_proto_login ){
		db_get_buy_item_limit_list(p);
	}
	return 0;
}

int get_player_max_vitality_point(player_t* p)
{
	return 50 + (p->lv-1) * 20;
}

int player_auto_recover_vitality_point(void* owner, void* data)
{
	player_t* p = reinterpret_cast<player_t*>(owner);
	add_player_vitality_point(p, 2);	
	ADD_TIMER_EVENT(p, player_auto_recover_vitality_point, 0, get_now_tv()->tv_sec + 60);
	return 0;
}

int notify_player_vitality_point_change(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->vitality_point, idx);
	pack(pkgbuf, get_player_max_vitality_point(p), idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_vitality_point_change, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

int add_player_vitality_point(player_t* p, uint32_t point)
{	
	uint32_t max_point = get_player_max_vitality_point(p);
	if(p->vitality_point >= max_point)return 0;

	p->vitality_point += point;
	if(p->vitality_point > max_point)
	{
		p->vitality_point = max_point;	
	}
	db_set_player_vitality_point(p, p->vitality_point);
	return notify_player_vitality_point_change(p);
}

int dec_player_vitality_point(player_t* p, uint32_t point)
{
	if(p->vitality_point < point)return 0;
	p->vitality_point -= point;
	db_set_player_vitality_point(p, p->vitality_point);
	return notify_player_vitality_point_change(p);
}

int notify_player_second_pro_exp_lv_change(player_t* p, uint32_t pro_type)
{
	secondary_profession_data* p_data = get_player_secondary_profession_by_type(p, pro_type);
	if(p_data == NULL)return 0;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pro_type, idx);
	pack(pkgbuf, p_data->get_profession_exp(), idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_second_pro_exp_lv_change, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}



