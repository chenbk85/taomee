#include "stage_config.hpp"
#include "utils.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "cli_proto.hpp"
#include "global_data.hpp"
using namespace taomee;

//static stage_data_ptr* data_array = NULL;
//static bool init_data_array( stage_data_mgr* mgr );
//static bool final_data_array();
static bool check_time(stage_time_data* pdata);
static int enter_stage_func_ret(player_t* p, int ret, stage_data* pdata);


bool string2time(const char* str,  int32_t& hour, int32_t& minute)
{
	if(str == NULL || strlen(str) != 4)return false;
	char temp[3] ={ 0 };
	memcpy(temp,  str, 2);
	hour = atoi(temp);
	memcpy(temp,  str+2, 2);
	minute = atoi(temp);
	if(  !(hour >=0 && hour<= 23) ||   !(minute >=0 && minute <= 59)){
		return false;
	}
	return true;
}

/*
bool init_data_array( stage_data_mgr* mgr)
{
	uint32_t max_id = mgr->get_max_stage_id();
	if(max_id == 0)return false;
	if(data_array != NULL){
		delete [] data_array;
		data_array = NULL;
	}
	data_array = new stage_data_ptr[ max_id + 1];
	for( uint32_t i =0; i < max_id + 1; i++)
	{
		data_array[i] = mgr->get_data_by_stage_id(i);
	}
	return true;
}

bool final_data_array()
{
	if( data_array != NULL)
	{
		delete [] data_array;
		data_array = NULL;
	}
	return true;
}
*/

bool stage_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}
	final();
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr stage_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the stage xml file is not exist"));
		return false;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the stage xml file content is empty"));
		return false;
	}

	stage_node = root->xmlChildrenNode;

	while(stage_node)
	{
        if (xmlStrcmp(stage_node->name, reinterpret_cast<const xmlChar*>("stage"))) {
            stage_node = stage_node->next;
            continue;
        }
		stage_data stage_data_elem;
        memset(&stage_data_elem, 0x00, sizeof(stage_data));
		stage_data* pdata = &stage_data_elem;
		
		
		get_xml_prop_def(pdata->stage_id_, stage_node,  "id", 0);
		get_xml_prop_def(pdata->min_lv_, stage_node,  "min_lv", 0);
		get_xml_prop_def(pdata->max_lv_, stage_node,  "max_lv", 0);
		get_xml_prop_def(pdata->pet_flag_, stage_node,  "pet", 1);	
		get_xml_prop_def(pdata->free_daily_id, stage_node,  "FreeDailyID", 0);
		get_xml_prop_def(pdata->coins, stage_node,  "Coins", 0);
		get_xml_prop_def(pdata->stage_diff, stage_node,  "StageDiff", 0);
		if( pdata->stage_diff > 11){
			throw XmlParseError(std::string("the stage xml file stage_diff > 11"));
			return false;
		}

		char fromstr[128] = "";
		time_t tmptime;
		get_xml_prop_raw_str_def(fromstr, stage_node, "From", "NULL");
		if (strcmp(fromstr, "NULL") != 0) 
		{
			if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) 
			{
				ERROR_LOG("time format invalid[%s]", fromstr);
				throw XmlParseError(std::string("error log"));
				return -1;
			}
		} 
		else 
		{
			tmptime = 0;
		}
		pdata->from = tmptime;

		memset(fromstr, 0, sizeof(fromstr));
		get_xml_prop_raw_str_def(fromstr, stage_node, "To", "NULL");
		if (strcmp(fromstr, "NULL") != 0) 
		{
			if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) 
			{
				ERROR_LOG("time format invalid[%s]", fromstr);
				throw XmlParseError(std::string("error log"));
				return -1;
			}
		} 
		else 
		{
			tmptime = 0;
		}
		pdata->to = tmptime;


		uint32_t itemarr[20] = { 0 };
		int arr_len = get_xml_prop_arr_def(itemarr, stage_node, "UnlimitedItem", 0);
		if (arr_len > 20 || arr_len % 2 != 0) {
			throw XmlParseError(std::string("unlimited item too many"));
			return -1;
		}
		TRACE_LOG("%u %u", arr_len/ 2, pdata->stage_id_);
		pdata->unlimited_item.count = arr_len / 2;
		for (uint32_t i = 0; i < pdata->unlimited_item.count; i++) {
			pdata->unlimited_item.itemarr[i].itemid = itemarr[i * 2];
			pdata->unlimited_item.itemarr[i].count = itemarr[i * 2 + 1];

			if (pdata->unlimited_item.itemarr[i].itemid != 4) {
			const GfItem* itm = items->get_item(pdata->unlimited_item.itemarr[i].itemid);
				if ( !itm ) {
					throw XmlParseError(std::string("item not exist"));
					return false;
				}
			}
			TRACE_LOG("%u %u %u %u", pdata->unlimited_item.count, i, pdata->unlimited_item.itemarr[i].itemid, pdata->unlimited_item.itemarr[i].count);
		}


		uint32_t condition_arr[20] = {0};
		arr_len = get_xml_prop_arr_def(condition_arr, stage_node, "ConditionItem", 0);
		if (arr_len > 20 || arr_len % 2 != 0) {
			throw XmlParseError(std::string("ConditionItem too many"));
			return -1;
		}
		TRACE_LOG("%u %u", arr_len/ 2, pdata->stage_id_);
		pdata->condition_item.count = arr_len / 2;
		for (uint32_t i = 0; i < pdata->condition_item.count; i++) {
			pdata->condition_item.itemarr[i].itemid = condition_arr[i * 2];
			pdata->condition_item.itemarr[i].count = condition_arr[i * 2 + 1];

			if (pdata->condition_item.itemarr[i].itemid != 4) {
				const GfItem* itm = items->get_item(pdata->condition_item.itemarr[i].itemid);
				if ( !itm ) {
					throw XmlParseError(std::string("item not exist"));
					return false;
				}
			}
			TRACE_LOG("%u %u %u %u", pdata->condition_item.count, i, 
					pdata->condition_item.itemarr[i].itemid, pdata->condition_item.itemarr[i].count);
		}

		
		get_xml_prop_def(pdata->time_limit_id, stage_node, "TimeLimit", 0); 

		if( pdata->stage_id_ == 0){
			throw XmlParseError(std::string("the stage xml file stage_id is 0"));
			return false;
		}

		xmlNodePtr limit_node = stage_node->xmlChildrenNode;
		while(limit_node)
		{
		    if (xmlStrcmp(limit_node->name, reinterpret_cast<const xmlChar*>("limit"))) {
	            limit_node = limit_node->next;
            	continue;
        	}
			stage_time_data data;
			
			get_xml_prop_def(data.year_,  limit_node, "year", -1);
			get_xml_prop_def(data.month_,  limit_node, "month", -1);
			get_xml_prop_def(data.week_,   limit_node, "week",  -1);
			get_xml_prop_def(data.day_,   limit_node, "day",  -1);
			
			char begin_time[10] = { 0 };
			char end_time[10] = { 0 };
			get_xml_prop_raw_str_def(begin_time, limit_node, "begin_hour", "");
			get_xml_prop_raw_str_def(end_time, limit_node, "end_hour", "");

			bool ret = false;

			if(strlen(begin_time) != 0){
				ret = string2time(begin_time,  data.begin_hour_, data.begin_minute_);
				if(!ret){
					throw XmlParseError(std::string("begin_hour error"));
				}
			}

			if(strlen(end_time) != 0){
				ret = string2time(end_time, data.end_hour_,  data.end_minute_);
				if(!ret){
					throw XmlParseError(std::string("end_hour error"));
				}
			}

			TRACE_LOG("%u %u %u", data.year_, data.month_, data.week_);
			get_xml_prop_def(data.enter_flag_, limit_node, "enter_flag", 0);
			
			pdata->push_stage_time_data(data);
			
			limit_node = limit_node->next;
		}

		add_data(pdata);
		stage_node = stage_node->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	//init_data_array(this);
	return true;
}

bool stage_data_mgr::final()
{
	max_stage_id_ = 0;
	//std::map<uint32_t,  stage_data_arr>::iterator pItr = stage_datas_.begin();
/*	for( ; pItr != stage_datas_.end(); ++pItr)
	{
		stage_data* pdata = pItr->second;
		if(pdata != NULL){
			delete pdata;
			pdata = NULL;
		}
	}*/
	stage_datas_.clear();
	//final_data_array();
	return true;
}



bool stage_data_mgr::add_data(stage_data* pData)
{
	if(pData == NULL)return false;
	//std::map<uint32_t, stage_data_arr>::iterator pItr = stage_datas_.find(pData->stage_id_);
//	if(pItr == stage_datas_.end()){
		stage_data_arr* p_arr = &(stage_datas_[ pData->stage_id_ ]);
		p_arr->stage_diff_data[pData->stage_diff] = *pData;
		max_stage_id_ = max_stage_id_ >= pData->stage_id_ ? max_stage_id_ : pData->stage_id_;
		return true;
//	}
//	throw XmlParseError(std::string("add_data"));
//	return false;
}


stage_data* stage_data_mgr::get_data_by_stage_id(uint32_t stage_id, uint32_t stage_diff)
{
	std::map<uint32_t, stage_data_arr>::iterator pItr = stage_datas_.find(stage_id);
	if(pItr == stage_datas_.end()){
		return NULL;
	}
	if (pItr->second.stage_diff_data[0].stage_id_) {
		return &(pItr->second.stage_diff_data[0]);
	}
	if (pItr->second.stage_diff_data[stage_diff].stage_id_) {
		return &(pItr->second.stage_diff_data[stage_diff]);
	}
	return NULL;
}

uint32_t get_stage_begin_time(uint32_t stage_id, uint32_t stage_diff)
{
    std::map<uint32_t, stage_data_arr>::iterator pItr = get_stage_data_mgr()->stage_datas_.find(stage_id);
    if(pItr == get_stage_data_mgr()->stage_datas_.end()) return 0;

    return pItr->second.stage_diff_data[stage_diff].from;
}

uint32_t get_stage_end_time(uint32_t stage_id, uint32_t stage_diff)
{
    std::map<uint32_t, stage_data_arr>::iterator pItr = get_stage_data_mgr()->stage_datas_.find(stage_id);
    if(pItr == get_stage_data_mgr()->stage_datas_.end()) return 0;

    return pItr->second.stage_diff_data[stage_diff].to;
}

uint32_t stage_data_mgr::get_max_stage_id()
{
	return max_stage_id_; 
}

uint32_t check_player_enter_stage(stage_data_mgr* mgr, player_t* p,  uint32_t stage_id, uint32_t stage_diff, bool is_free)
{
	if (stage_diff > 11) {
		return cli_err_system_error;
	}
	if( (stage_id >= 0 && stage_id <= mgr->get_max_stage_id() ) )
	{
		stage_data* pdata = mgr->get_data_by_stage_id(stage_id, stage_diff);
		if(pdata != NULL)
		{	
			if (pdata->stage_diff && stage_diff && pdata->stage_diff != stage_diff) {
				return 0;
			}

			if (pdata->from && get_now_tv()->tv_sec < pdata->from) {
				return cli_err_swap_action_not_in_time;
			}
			if (pdata->to && get_now_tv()->tv_sec > pdata->to) {
				return cli_err_swap_action_not_in_time;
			}
		
			//检查级别
			bool ret = pdata->check_level(p);
			if(!ret) {
				return cli_err_less_lv;
			}
			//检查宠物
			if( !pdata->allow_pet() && p->fight_summon && p->fight_summon->call_flag == 1) {
				return cli_err_shilian_stage_limit_take_summon;	
			}

			if (is_free) {
				if (pdata->free_daily_id) {
					int i = pdata->free_daily_id - 1;
					if ((i < 0) || (static_cast<uint32_t>(i) >= max_restr_count) || 
						(g_all_restr[i].id != pdata->free_daily_id)) {
						return cli_err_limit_time_act;
					}
					restriction_t* p_restr = &g_all_restr[i];
					if (p->daily_restriction_count_list[i] >= p_restr->toplimit) {
						return cli_err_limit_time_act;
					}
				}
				//检查时间
				if( pdata->time_datas_.size() == 0) {
					return enter_stage_func_ret(p, 0, pdata);
				};
				for( uint32_t i = 0; i< pdata->time_datas_.size(); i++)
				{
					stage_time_data* p_time_data = &(pdata->time_datas_[i]);
					ret = check_time(p_time_data);
					if(ret){
						return enter_stage_func_ret(p, 0, pdata);
					}
				}
				uint32_t ret_code = 0;
				switch (stage_id) {
					case 909:
						ret_code = cli_err_stage_limit_time_act_909;
						break;
					case 925:
						ret_code = cli_err_stage_limit_time_act_925;
						break;
					case 926:
						ret_code = cli_err_stage_limit_time_act_926;
						break;
					case 927:
					case 928:
					case 929:	
						ret_code = cli_err_stage_limit_time_act_927;
						break;
                    case 935:
                    case 936:
                    case 937:
                    case 938:
                        ret_code = cli_err_stage_limit_time_act_935;
                        break;
					case 939:	
					case 941:
						ret_code = cli_err_stage_limit_time_act_939;
						break;
					default:
						ret_code = cli_err_stage_limit_time_act_909;
						break;
				}
				return ret_code;
			} else {
				if (stage_id == 930) {
					//检查时间
					if( pdata->time_datas_.size() == 0)return 0;
					for( uint32_t i = 0; i< pdata->time_datas_.size(); i++)
					{
						stage_time_data* p_time_data = &(pdata->time_datas_[i]);
						ret = check_time(p_time_data);
						if(ret){
							if (pdata->unlimited_item.count) {
								for (uint32_t i = 0; i < pdata->unlimited_item.count; i++) {
									if (pdata->unlimited_item.itemarr[i].itemid == 4) {
									//reduce fumo points 
										if (p->fumo_points_total < pdata->unlimited_item.itemarr[i].count) {
											return cli_err_no_item;
										}
									} else {
									
										if (p->my_packs->get_item_cnt(pdata->unlimited_item.itemarr[i].itemid) < 
											pdata->unlimited_item.itemarr[i].count) {
											return cli_err_no_item;
										}
										//p->my_packs->del_item(p, pdata->unlimited_item, 1);

										const GfItem* itm = items->get_item(pdata->unlimited_item.itemarr[i].itemid);
										if ( !itm ) {
											return cli_err_no_item;
										}
									}
								}
								return enter_stage_func_ret(p, 0, pdata);
							}
							return cli_err_no_item;
						}
					}
					uint32_t ret_code = 0;
					switch (stage_id) {
						case 909:
							ret_code = cli_err_stage_limit_time_act_909;
							break;
						case 925:
							ret_code = cli_err_stage_limit_time_act_925;
							break;
						case 926:
							ret_code = cli_err_stage_limit_time_act_926;
							break;
						case 927:
						case 928:
						case 929:	
						case 930:
							ret_code = cli_err_stage_limit_time_act_927;
							break;
						case 939:
						case 941:
							ret_code = cli_err_stage_limit_time_act_939;
							break;
						default:
							ret_code = cli_err_stage_limit_time_act_909;
							break;
					}
					return ret_code;
				} else {
					if (pdata->unlimited_item.count) {
						for (uint32_t i = 0; i < pdata->unlimited_item.count; i++) {
							if (pdata->unlimited_item.itemarr[i].itemid == 4) {
							//reduce fumo points 
								if (p->fumo_points_total < pdata->unlimited_item.itemarr[i].count) {
									return cli_err_no_item;
								}
							} else {
								if (p->my_packs->get_item_cnt(pdata->unlimited_item.itemarr[i].itemid) < 
									pdata->unlimited_item.itemarr[i].count) {
									return cli_err_no_item;
								}
								//p->my_packs->del_item(p, pdata->unlimited_item, 1);

								const GfItem* itm = items->get_item(pdata->unlimited_item.itemarr[i].itemid);
								if ( !itm ) {
									return cli_err_no_item;
								}
							}
						}
						return enter_stage_func_ret(p, 0, pdata);
					}

					if (pdata->condition_item.count) {
						for (uint32_t i = 0; i < pdata->condition_item.count; i++) {
							if (p->my_packs->get_item_cnt(pdata->condition_item.itemarr[i].itemid) < 
									pdata->condition_item.itemarr[i].count) {
								return cli_err_no_item;
							}
							//p->my_packs->del_item(p, pdata->unlimited_item, 1);

							const GfItem* itm = items->get_item(pdata->condition_item.itemarr[i].itemid);
							if ( !itm ) {
								return cli_err_no_item;
							}

						}
					}

				}
				
			}
			
			return 1;
		}	
	}
	return 0;
}


uint32_t check_and_reduce_coins(player_t *p, uint32_t coin)
{
	if ( !p || coin == 0) {
		return 0;
	}
	if (p->coins < coin) {
		return cli_err_no_enough_coins;
	}
	p->coins -= coin;
	db_set_role_base_info(p);
	return 0;
}

int enter_stage_func_ret(player_t* p, int ret, stage_data* pdata)
{
	if (!ret && pdata) {
		return check_and_reduce_coins(p, pdata->coins);
	}
	return ret;
}

bool check_time(stage_time_data* pdata)
{
	time_t cur_time = time(NULL);		
	struct tm  *ptime = localtime(&cur_time);


	if(pdata->month_ != -1)
	{
		if( pdata->year_ == ptime->tm_year + 1900 && pdata->enter_flag_ == 0)return false;
		if( pdata->year_ != ptime->tm_year + 1900 && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->month_ != -1)
	{
		if( pdata->month_ == ptime->tm_mon  && pdata->enter_flag_ == 0)return false;	
		if( pdata->month_ != ptime->tm_mon  && pdata->enter_flag_ == 1)return false;
	}	

	if( pdata->week_ != -1)
	{
		if(pdata->week_ == ptime->tm_wday && pdata->enter_flag_ == 0)return false;
		if(pdata->week_ != ptime->tm_wday && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->day_ != -1)
	{
		if(pdata->day_ == ptime->tm_mday && pdata->enter_flag_ == 0)return false;
		if(pdata->day_ != ptime->tm_mday && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->begin_hour_ != -1 && pdata->begin_minute_ != -1)
	{
		if(pdata->begin_hour_* 100 + pdata->begin_minute_  <= ptime->tm_hour*100 + ptime->tm_min && pdata->enter_flag_ ==0)return false;
		if(pdata->begin_hour_* 100 + pdata->begin_minute_ > ptime->tm_hour*100 + ptime->tm_min && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->begin_hour_ != -1 && pdata->begin_minute_ == -1)
	{
		if( pdata->begin_minute_ <= ptime->tm_min && pdata->enter_flag_ == 0)return false;
		if( pdata->begin_minute_ >  ptime->tm_min && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->begin_hour_ == -1 && pdata->begin_minute_ != -1)
	{
		if( pdata->begin_hour_ <= ptime->tm_hour && pdata->enter_flag_ == 0)return false;
		if( pdata->begin_hour_ > ptime->tm_min && pdata->enter_flag_ == 1)return false;
	}

	if( pdata->end_hour_ != -1 && pdata->end_minute_ != -1)
	{
		if(pdata->end_hour_*100 + pdata->end_minute_ >= ptime->tm_hour*100 + ptime->tm_min && pdata->enter_flag_ == 0)return false;
		if(pdata->end_hour_*100 +pdata->end_minute_ < ptime->tm_hour*100 + ptime->tm_min && pdata->enter_flag_ == 1)return false;

	}

	if( pdata->end_hour_ != -1 && pdata->end_minute_ == -1)
	{
		if( pdata->end_hour_ >= ptime->tm_hour && pdata->enter_flag_ == 0)return false;
		if( pdata->end_hour_ < ptime->tm_hour && pdata->enter_flag_ == 1)return true;
	}	

	if( pdata->end_hour_ == -1 && pdata->end_minute_ != -1)
	{
		if( pdata->end_minute_ >= ptime->tm_min && pdata->enter_flag_ == 0)return false;
		if( pdata->end_hour_ < ptime->tm_hour && pdata->enter_flag_ == 1)return false;
	}
	return true;
}

int  get_stage_errcode(uint32_t stage_id)
{
	switch(stage_id)
	{
		case 909:
		{
			return cli_err_stage_limit_time_act_909;
		}
		break;

		case 916: 
		{
			return cli_err_stage_limit_take_summon;
		}
		break;

		case 921:
		{
			return cli_err_shilian_stage_limit_take_summon;
		}
		break;

		case 801:
		{
			return	cli_err_fumo_stage_limit_take_summon; 
		}
		break;
		default:
		{
			return cli_err_limit_time_act;
		}
		break;
	}
	return 0;
}


int check_player_enter_stage_time(stage_data_mgr * mgr,  uint32_t stage_id)
{
	stage_data * data = mgr->get_data_by_stage_id(stage_id);
	if (data && data->time_limit_id) {
		if (!get_now_active_data_by_active_id(data->time_limit_id)) {
			return cli_err_swap_action_not_in_time;
		}
	}
	return 0;
}

