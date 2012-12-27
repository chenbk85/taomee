/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet_work.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2012 05:01:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_piglet_work.h"
#include <algorithm>
static part_material_t part_materials_map[] = {
	//螺母
	{ 1613600, 1, {{1613501, 1}, {0, 0}, {0, 0}}, 120, 0},
	//螺栓
	{ 1613601, 1, {{1613501, 1}, {0, 0}, {0, 0}}, 120, 0},
	//垫圈
	{ 1613602, 1, {{1613501, 1}, {0, 0}, {0, 0}}, 180, 0},
	//转动轴
	{ 1613603, 1, {{1613501, 15}, {0, 0}, {0, 0}}, 500, 0},
	//钢梁
	{ 1613604, 1, {{1613501, 100}, {0, 0}, {0, 0}}, 1500, 0},
	//胶水瓶
	{ 1613605, 1, {{1613501, 15}, {0, 0}, {0, 0}}, 600, 0},
	//溶剂瓶
	{ 1613606, 1, {{1613501, 15}, {0, 0}, {0, 0}}, 600, 0},
	//铁脑壳
	{ 1613607, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//铁甲片
	{ 1613608, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//铁盒子
	{ 1613609, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//粗铁条
	{ 1613610, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//集装箱
	{ 1613611, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 1000, 0},
	//角铁架
	{ 1613612, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//焊接台
	{ 1613613, 1, {{1613501, 30}, {0, 0}, {0, 0}}, 750, 0},
	//智能芯片
	{ 1613614, 1, {{1613501, 50}, {0, 0}, {0, 0}}, 1000, 0},
	//超级芯片
	{ 1613615, 1, {{1613501, 50}, {0, 0}, {0, 0}}, 1000, 0},
	//转轮
	{ 1613616, 1, {{1613502, 3}, {0, 0}, {0, 0}}, 300, 0},
	//导线
	{ 1613617, 1, {{1613502, 3}, {0, 0}, {0, 0}}, 600, 0},
	//发条陀螺
	{ 1613618, 1, {{1613501, 80}, {0, 0}, {0, 0}}, 1200, 0},
	//
	{ 1613619, 1, {{1613501, 10}, {0, 0}, {0, 0}}, 600, 0},
	//
	{ 1613620, 1, {{1613502, 5}, {0, 0}, {0, 0}}, 600, 0},
	//
	{ 1613621, 1, {{1613501, 20}, {0, 0}, {0, 0}}, 900, 0},
	//
	{ 1613622, 1, {{1613502, 10}, {0, 0}, {0, 0}}, 900, 0},
	//秒表壳
	{ 1613623, 1, {{1613501, 3}, {0, 0}, {0, 0}}, 180, 0},
	//铜喇叭
	{ 1613624, 1, {{1613502, 3}, {0, 0}, {0, 0}}, 180, 0},
	//炫彩棒
	{ 1613625, 1, {{1613501, 3}, {0, 0}, {0, 0}}, 180, 0},
	//卷尺筒
	{ 1613626, 1, {{1613501, 5}, {0, 0}, {0, 0}}, 180, 0},
	//铁丝网
	{ 1613627, 1, {{1613501, 10}, {0, 0}, {0, 0}}, 600, 0},
	//打气筒
	{ 1613628, 1, {{1613501, 10}, {0, 0}, {0, 0}}, 600, 0},
	{ 1613629, 1, {{1613501, 5}, {1613502, 5}, {0, 0}}, 600, 0},
	{ 1613630, 1, {{1613501, 5}, {1613502, 5}, {0, 0}}, 600, 0},
	{ 1613631, 1, {{1613501, 1}, {0, 0}, {0, 0}}, 60, 0},

	{ 1613632, 1, {{1613501, 3}, {0, 0}, {0, 0}}, 180, 0},
	{ 1613633, 1, {{1613501, 3}, {0, 0}, {0, 0}}, 180, 0},
	
   	//螺母
	{ 1613600, 50, {{1613501, 50}, {0, 0}, {0, 0}}, 7200+30*60, 60},
	{ 1613600, 100, {{1613501, 100}, {0, 0}, {0, 0}}, 6*3600+40*60, 100},
	//螺栓
	{ 1613601, 50, {{1613501, 50}, {0, 0}, {0, 0}}, 7200+30*60, 60},
	{ 1613601, 100, {{1613501, 100}, {0, 0}, {0, 0}}, 6*3600+40*60, 100},
	//垫圈
	{ 1613602, 50, {{1613501, 50}, {0, 0}, {0, 0}}, 3*3600+45*60, 60},
	{ 1613602, 100, {{1613501, 100}, {0, 0}, {0, 0}}, 10*3600, 100},


	{ 1613634, 1, {{1613501, 20}, {1613502, 10}, {1613503, 5}}, 7200, 0},
	{ 1613635, 1, {{1613502, 3}, {0, 0}, {0, 0}}, 900, 0},
	{ 1613636, 1, {{1613501, 5}, {0, 0}, {0, 0}}, 600, 0},
	{ 1613637, 1, {{1613501, 1}, {1613502, 2}, {0, 0}}, 900, 0},
	{ 1613638, 1, {{1613501, 10}, {1613502, 3}, {0, 0}}, 1200, 0},

	//超合金心脏
	{ 1613639, 1, {{1613501, 10}, {1613502, 10}, {1613503, 5}}, 3600, 0},
	//超合金头盔
	{ 1613640, 1, {{1613501, 10}, {1613502, 10}, {1613503, 5}}, 3600, 0},
	


};

static ore_time_t ore_tim[] = {
	{ 1613400, 40 },
	{ 1613401, 120 },
	{ 1613402, 300 },
	{ 1613403, 600 },
	{ 1613404, 60 },
	{ 1613405, 150},
	{ 1613406, 300},
	{ 1613407, 600 },
	{ 1613408, 100 },
	{ 1613409, 200 },
	{ 1613410, 300 },
	{ 1613411, 600},
};
static ore_time_t batch_ore_tim[] = {
	{ 1613400, 15*60 },
	{ 1613401, 60*60 },
	{ 1613402, 2*60*60 },
	{ 1613403, 5*60*60 },
	{ 1613404, 15*60 },
	{ 1613405, 60*60},
	{ 1613406, 2*60*60},
	{ 1613407, 5*60*60 },
	{ 1613408, 15*60 },
	{ 1613409, 60*60 },
	{ 1613410, 2*60*60 },
	{ 1613411, 5*60*60},
};

//零件id，类型, 原料id及所需原料数量的数组， 加工时间

Cuser_piglet_work::Cuser_piglet_work(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_piglet_work", "userid")
{

}

uint32_t Cuser_piglet_work::get_ore_time(uint32_t ore_id, uint32_t batch){
	if(batch == 0){
		for(uint32_t k = 0; k < sizeof(ore_tim)/sizeof(ore_time_t); ++k){
			if(ore_tim[k].ore_id == ore_id){
				return ore_tim[k].need_time;
			}
		}
		return 10;

	}
	else{
		for(uint32_t k = 0; k < sizeof(batch_ore_tim)/sizeof(ore_time_t); ++k){
			if(batch_ore_tim[k].ore_id == ore_id){
				return batch_ore_tim[k].need_time;
			}
		}
		return 5*60*60;

	}
}
int Cuser_piglet_work::get_stove_product(uint32_t ore_id, uint32_t *obj_product, uint32_t *count)
{
	if(ore_id >= 1613400 && ore_id <= 1613403){
		*obj_product = 1613501;
		if(ore_id != 1613403){
			*count = ore_id - 1613400 + 1;
		}
		else{
			*count = 5;
		}
	}	
	else if(ore_id >= 1613404 && ore_id <= 1613407){
		*obj_product =  1613502;
		if(ore_id != 1613407){
			*count = ore_id - 1613404 + 1;
		}
		else{
			*count = 5;
		}
	}
	else if(ore_id >= 1613408 && ore_id <= 1613411){
		*obj_product =  1613503;
		if(ore_id != 1613411){
			*count = ore_id - 1613408 + 1;
		}
		else{
			*count = 5;
		}
	}
	else{
		*obj_product = 0;
		*count = 0;
	}
	return 0;
}

int Cuser_piglet_work::find(uint32_t part_id, uint32_t type, part_material_t* pos)
{
	if(part_id >= 1613600 && part_id <= 1613899){
		uint32_t flag = 0;
		for(uint32_t k = 0; k < sizeof(part_materials_map)/sizeof(part_material_t); ++k){
			if(part_materials_map[k].part_id == part_id && part_materials_map[k].type == type){
				 memcpy((char*)pos, (char*)(&part_materials_map[k]), sizeof(part_material_t));
				 flag = 1;
			}
		}
		if(flag != 1){
			pos = 0;
		}
	}
	else{
		pos = 0;
	}
	return 0;	
}

int Cuser_piglet_work::add(userid_t userid, uint32_t tool_type, uint32_t tool_index,
		uint32_t tool_itemid, uint32_t work_end_time, work_piglets_t* work_piglets,
		uint32_t raw_material, uint32_t cnt)
{
	
	char mysql_piglets[mysql_str_len(sizeof(work_piglets_t))];
	memset(mysql_piglets, 0, sizeof(work_piglets_t));	
	set_mysql_string(mysql_piglets, (char*)work_piglets, sizeof(work_piglets_t));
	
	sprintf(this->sqlstr, "insert into %s values( %u, %u, %u, %u, %u, '%s', %u, 0, 0, %u)",
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index,
			tool_itemid,
			work_end_time,
			mysql_piglets,
			raw_material,
			cnt
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_piglet_work::finish_work(userid_t userid, uint32_t tool_type, uint32_t tool_index)
{
	char mysql_piglets[mysql_str_len(sizeof(user_piglet_dress))];
	memset(mysql_piglets, 0, sizeof(work_piglets_t));	
	
	sprintf(this->sqlstr, "update %s set work_time = 0, work_piglets = '%s', need_time = 0  where userid = %u \
			and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			mysql_piglets,
			userid,
			tool_type,
			tool_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
	
}

int Cuser_piglet_work::upgrade(userid_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t itemid)
{
	sprintf(this->sqlstr, "update %s set tool_itemid = %u where userid = %u and tool_type and tool_index = %u",
			this->get_table_name(userid),
			itemid,
			userid,
			tool_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_work::get_tool_level(uint32_t tool_type, uint32_t tool_itemid, uint32_t *level)
{
	*level = 1;
	uint32_t tools[] = {1613138, 1613139, 1613140, 1613141, 1613142, 1613143, 1613144, 1613145};
	uint32_t *iter = std::find(tools, tools+sizeof(tools)/sizeof(uint32_t), tool_itemid);
	if(iter == tools+sizeof(tools)/sizeof(uint32_t)){
		return 1;
	}

	if( tool_itemid == 1613138 || tool_itemid == 1613139 || tool_itemid == 1613140 || tool_itemid == 1613141){
		*level = 2;
	}
	else if(tool_itemid == 1613142 || tool_itemid == 1613143 || tool_itemid == 1613144 || tool_itemid == 1613145){
		*level = 3;
	}
	return 0;
}

int Cuser_piglet_work::get_work_info(userid_t userid, uint32_t *p_count, 
		user_get_piglet_machine_work_out_item_2 **pp_list)
{
	sprintf(this->sqlstr, "select tool_type, tool_index, tool_itemid, work_time, need_time, glue_id from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	uint32_t tool_itemid = 0;
	uint32_t now = time(0);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->tool_type);
		INT_CPY_NEXT_FIELD((*pp_list + i)->tool_index);
		INT_CPY_NEXT_FIELD(tool_itemid);
		get_tool_level((*pp_list + i)->tool_type, tool_itemid,&((*pp_list + i)->tool_level));
		INT_CPY_NEXT_FIELD((*pp_list + i)->left_time);
		if((*pp_list+i)->left_time > 0 ){
			if((*pp_list+i)->left_time > now){
				(*pp_list+i)->work_state = 1;
				(*pp_list+i)->left_time = (*pp_list+i)->left_time - now;
			}
			else{
				 (*pp_list+i)->work_state = 2;
				 (*pp_list+i)->left_time = 0;
			}
		}
		else{
			(*pp_list+i)->work_state = 0;
			(*pp_list+i)->left_time = 0;
		}
		INT_CPY_NEXT_FIELD((*pp_list + i)->total_time);
		INT_CPY_NEXT_FIELD((*pp_list + i)->glue_id);
	STD_QUERY_WHILE_END();
}

int Cuser_piglet_work::start_working(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
		uint32_t end_time, uint32_t need_time, work_piglets_t* work_piglets, uint32_t material,
		uint32_t cnt)
{
	char mysql_piglets[mysql_str_len(sizeof(work_piglets_t))];
	memset(mysql_piglets, 0, sizeof(work_piglets_t));	
	set_mysql_string(mysql_piglets, (char*)work_piglets, sizeof(work_piglets_t));
	sprintf(this->sqlstr, "update %s set work_time = if(glue_id != 0, %u + %u, %u), piglets = '%s',	\
			need_time = if(glue_id != 0, %u+%u, %u), raw_material = %u, obj_count = %u where userid = %u and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			end_time,
			need_time,
			end_time,
			mysql_piglets,
			need_time,
			need_time,
			need_time,
			material,
			cnt,
			userid,
			tool_type,
			tool_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_work::get_machine(userid_t userid, uint32_t tool_type, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and tool_type = %u",
			this->get_table_name(userid),
			userid,
			tool_type
			);	

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*count);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet_work::get_need_count(userid_t userid, uint32_t stove_index, uint32_t batch, uint32_t *need_count)
{
	uint32_t stove_itemid = 0;
	int ret = get_one_col(userid, 1, stove_index, "tool_itemid", &stove_itemid);	
	uint32_t level = 0;
	ret = get_tool_level(1, stove_itemid, &level);
	if(batch == 0){
		if(level == 1){
			*need_count = 10;
		}
		else if(level == 2){
			*need_count = 20;
		}
		else{
			*need_count = 30;
		}
	
	}
	else{
		if(level == 1){
			*need_count = 100;
		}
		else if(level == 2){
			*need_count = 200;
		}
		else{
			*need_count = 300;
		}
	}

	return SUCC;
}

int Cuser_piglet_work::get_one_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
		const char *col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and tool_type = %u and tool_index = %u",
			col,
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_work::get_two_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, const char *col_1,
                const char *col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and tool_type = %u and tool_index = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();	

}

int Cuser_piglet_work::check_tool_satisfy_material(uint32_t type, uint32_t tool, 
		uint32_t material_id,  uint32_t *flag)
{
	uint32_t level = 0;		 
	if(type == 1){//熔炉
		 get_tool_level(1, tool, &level);
		 if(level == 1){
			 if(material_id >= 1613400 && material_id <= 1613403){
				 *flag = 1;
			 }
			 else{
				 *flag = 0;
			 }
		 }
		 else if(level == 2){
			 if(material_id >= 1613400 && material_id <= 1613407){
				 *flag = 1;
			 }
			 else{
				 *flag = 0;
			 }
		 }
		 else if(level == 3){
			 if(material_id >= 1613400 && material_id <= 1613411){
				 *flag = 1;
			 }
			 else{
				 *flag = 0;
			 }
		 }
		 else{
			 *flag = 0;
		 }
	}
	else{//机床
		get_tool_level(2, tool, &level);	
		if(level == 1){
			if(material_id == 1613501){
				*flag = 1;
			}
			else{
				*flag = 0;
			}
		}
		else if(level == 2){
			if(material_id == 1613501 || material_id == 1613502)
			{
				*flag = 1;
			}
			else{
				*flag = 0;
			}
		}
		else if(level == 3){
			if(material_id == 1613503 || material_id == 1613501 || material_id == 1613502)
			{
				*flag = 1;
			}
			else{
				*flag = 0;
			}
		}
		else{
			*flag = 0;
		}
	}
	return SUCC;
}

int Cuser_piglet_work::get_piglets_datetime(userid_t userid, uint32_t tool_type, uint32_t tool_index,
		work_piglets_t* piglets, uint32_t *endtime, uint32_t* obj_part)
{
	sprintf(this->sqlstr, "select work_time, piglets, raw_material from %s where \
			userid = %u and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*endtime);
		BIN_CPY_NEXT_FIELD((char*)(piglets), sizeof(work_piglets_t));
		INT_CPY_NEXT_FIELD(*obj_part);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_work::finish_working(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
		work_piglets_t *work_piglets) 
{
	char mysql_piglets[mysql_str_len(sizeof(work_piglets_t))];
	set_mysql_string(mysql_piglets, (char*)work_piglets, sizeof(work_piglets_t));
	sprintf(this->sqlstr, "update %s set work_time = 0, piglets = '%s', raw_material = 0, glue_id = 0 where userid = %u and \
			tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			mysql_piglets,
			userid,
			tool_type,
			tool_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_work::update_one_col(userid_t userid, uint32_t tool_type, uint32_t tool_index, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			tool_type,
			tool_index
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_work::get_one_tool_info(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
		uint32_t *level, uint32_t *end_time, work_piglets_t *work_piglets)
{
	sprintf(this->sqlstr, "select work_time, piglets, tool_itemid from %s where \
			userid = %u and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index
			);

	uint32_t tool_itemid = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*end_time);
		BIN_CPY_NEXT_FIELD((char*)(work_piglets), sizeof(work_piglets_t));
		INT_CPY_NEXT_FIELD(tool_itemid);
		get_tool_level(tool_type, tool_itemid, level);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_work::get_one_another_info(userid_t userid, uint32_t tool_type, uint32_t tool_index, 
		uint32_t *level, uint32_t *work_state, uint32_t *left_time)
{
	sprintf(this->sqlstr, "select work_time, tool_itemid from %s where \
			userid = %u and tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index
			);
	uint32_t now = time(0);
	uint32_t tool_itemid = 0, end_time = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(end_time);
		INT_CPY_NEXT_FIELD(tool_itemid);
		if(end_time > 0 ){
			if(end_time > now){
				*work_state = 1;
				*left_time = end_time - now;
			}
			else{
				 *work_state = 2;
				 *left_time = 0;
			}
		}
		else{
			*work_state = 0;
			*left_time = 0;
		}

		get_tool_level(tool_type, tool_itemid, level);
	STD_QUERY_ONE_END();	
}
int Cuser_piglet_work::accelerate_machine(userid_t userid, uint32_t type, uint32_t index, uint32_t time,
		uint32_t *state)
{
	*state = 0;	
	uint32_t  work_end_time = 0;	
	int ret = get_one_col(userid, type, index, "work_time", &work_end_time);
	if(work_end_time >= time){
		ret = this->update_one_col(userid, type, index, "work_time", work_end_time - time);
		*state = 1;
	}
	return ret;
}

int Cuser_piglet_work::get_piglets_worktime(uint32_t userid, uint32_t tool_type, uint32_t tool_index,uint32_t* endtime)
{
    sprintf(this->sqlstr, "select work_time from %s where userid = %u and \
			tool_type = %u and tool_index = %u",
			this->get_table_name(userid),
			userid,
			tool_type,
			tool_index);
    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*endtime);
	STD_QUERY_ONE_END();	
}

int Cuser_piglet_work::slow_down(uint32_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t tool_id,
		uint32_t *state)
{
	uint32_t glue_id = 0;	
	if((tool_id == 1614009 && tool_index != 1) || (tool_id == 1614010 && !(tool_index == 1 || tool_index == 2))){
		*state = 0;
		return 0;
	}
	uint32_t work_time = 0;
	int ret = get_two_col(userid, tool_type, tool_index, "work_time", "glue_id", &work_time, &glue_id);
	if(ret != SUCC){
		*state = 0;
		return 0;
	}
	if(glue_id != 0){
		*state = 0;
		return 0;
	}
	if(work_time != 0){
		sprintf(this->sqlstr, "update %s set work_time = work_time + need_time, need_time = need_time*2, \
				glue_id = %u where userid = %u and tool_type = %u and tool_index = %u",
				this->get_table_name(userid),
				tool_id,
				userid,
				tool_type,
				tool_index
				);
	}
	else{
		sprintf(this->sqlstr, "update %s set glue_id = %u where userid = %u and tool_type = %u and tool_index = %u",
				this->get_table_name(userid),
				tool_id,
				userid,
				tool_type,
				tool_index
				);

	}
	*state = 1;

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_piglet_work::recover(uint32_t userid, uint32_t tool_type, uint32_t tool_index, uint32_t tool_id,
		uint32_t *state)
{
	if((tool_id == 1614012 && tool_index != 1) || (tool_id == 1614013 && !(tool_index == 1 || tool_index == 2))){
		*state = 0;
		return 0;
	}
	uint32_t glue_id = 0, need_time = 0;
	int ret = this->get_two_col(userid, tool_type, tool_index, "glue_id", "need_time", &glue_id, &need_time); 
	if(ret != SUCC){
		*state = 0;
		return 0;
	}
	if(glue_id == 0){
		*state = 0;
		return 0;	
	}
	need_time = need_time / 2;	
	uint32_t work_time = 0;
	ret = this->get_one_col(userid, tool_type, tool_index, "work_time", &work_time);
	if(work_time != 0){
		sprintf(this->sqlstr, "update %s set work_time = work_time - %u, need_time = %u, glue_id = 0 \
				where userid = %u and tool_type = %u and tool_index = %u",
				this->get_table_name(userid),
				need_time,
				need_time,
				userid,
				tool_type,
				tool_index
				);
	}
	else{
		sprintf(this->sqlstr, "update %s set glue_id = 0 where userid = %u and tool_type = %u and tool_index = %u",
				this->get_table_name(userid),
				userid,
				tool_type,
				tool_index
				);
	}
	*state = 1;
    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
	
}
