#include <algorithm>
#include "db_error.h"
#include "Cgf_warehouse.h"

using namespace std;

Cgf_warehouse::Cgf_warehouse(mysql_interface * db )
        :CtableRoute( db,"GF" ,"t_gf_warehouse","userid")
{

}


int  Cgf_warehouse::get_warehouse_item_list(userid_t userid, uint32_t role_regtime, gf_get_warehouse_item_list_out_item** pData, uint32_t *count)
{
    if(pData == NULL || count == NULL)return 1001;
    GEN_SQLSTR(sqlstr,"select item,item_count  from %s where userid=%u ",get_table_name(userid), userid);
    STD_QUERY_WHILE_BEGIN(this->sqlstr, pData, count);
        INT_CPY_NEXT_FIELD( (*pData+i)->itemid );
        INT_CPY_NEXT_FIELD( (*pData+i)->count );
    STD_QUERY_WHILE_END();
}

int Cgf_warehouse::get_warehouse_used_count(userid_t userid, uint32_t* count)
{
	*count = 0;
	GEN_SQLSTR(sqlstr, "select count(*) from %s where userid = %u ", get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, 0);
	        INT_CPY_NEXT_FIELD( *count );
	STD_QUERY_ONE_END();
}

int Cgf_warehouse::get_warehouse_item_count(userid_t userid, uint32_t item_id, uint32_t* count)
{
	*count = 0;
	GEN_SQLSTR(sqlstr, "select item_count from %s where userid = %u and item = %u limit 1", get_table_name(userid), userid,item_id);	
    STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ITEM_NOFIND_ERR);
        INT_CPY_NEXT_FIELD( *count );
    STD_QUERY_ONE_END();
}

int Cgf_warehouse::get_used_space_cnt(userid_t userid, uint32_t* cnt)
{
    GEN_SQLSTR(this->sqlstr, "select count(*) from %s where userid=%u;",
        this->get_table_name(userid), userid);
    STD_QUERY_ONE_BEGIN(sqlstr, GF_ITEM_NOFIND_ERR);
        INT_CPY_NEXT_FIELD (*cnt);
    STD_QUERY_ONE_END();
}

int Cgf_warehouse::insert_item2warehouse(userid_t userid, uint32_t item_id, uint32_t count)
{
    uint32_t used_num = 0;
    int ret = get_used_space_cnt(userid, &used_num);
    if (ret != SUCC){
        return ret;
    }
    if (used_num >= WAREHOUSE_SPACE_MAX) {
        return GF_ITEM_KIND_MAX_ERR;
    }
    GEN_SQLSTR(sqlstr, "insert into %s (userid, role_regtime, mail_time, item, item_count) values(%u,%u,%u,%u,%u)", 
        get_table_name(userid), userid, 0, (uint32_t)time(NULL),  item_id, count);
    return this->exec_insert_sql (this->sqlstr, GF_ITEM_EXISTED_ERR );
}

int Cgf_warehouse::add_item2warehouse(userid_t userid, uint32_t item_id, uint32_t count, uint32_t limit)
{
    if (count > limit) {
        return VALUE_OUT_OF_RANGE_ERR;
    }
    uint32_t db_num = 0;
    int ret = this->get_warehouse_item_count(userid, item_id, &db_num);
    if (ret != SUCC && ret != GF_ITEM_NOFIND_ERR) {
        return ret;
    } else if (ret == GF_ITEM_NOFIND_ERR) {
        return this->insert_item2warehouse(userid, item_id, count);
    } else if (db_num + count > limit) {
        return VALUE_OUT_OF_RANGE_ERR;
    } else {
        GEN_SQLSTR(this->sqlstr,"update %s set count=count+%u where userid=%u and item=%u;",
            this->get_table_name(userid), count, userid, item_id);
        return this->exec_update_sql (this->sqlstr, GF_ITEM_NOFIND_ERR);
    }
}
int Cgf_warehouse::delete_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id,  uint32_t item_count)
{
    uint32_t cur_count = 0;
    int ret =  get_warehouse_item_count(userid, item_id, &cur_count);
    if (ret != SUCC) {
        return ret;
    }
    if (item_count >= cur_count) {
        GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and item=%u",
            get_table_name(userid), userid, item_id);
    } else {
        GEN_SQLSTR(sqlstr, "update %s set item_count=%u where userid=%u and item=%u",
            get_table_name(userid), cur_count - item_count, userid, item_id);
    }
    return exec_update_list_sql(sqlstr,SUCC);
}

int Cgf_warehouse::add_warehouse_item(userid_t userid, uint32_t role_regtime, uint32_t item_id,  uint32_t item_count)
{
    uint32_t cur_count = 0;
    int ret = get_warehouse_item_count(userid, item_id, &cur_count);
    if (ret != SUCC && ret != GF_ITEM_NOFIND_ERR) {
        return ret;
    }
    else if(ret == GF_ITEM_NOFIND_ERR)//insert
    {   
        GEN_SQLSTR(sqlstr, "insert into %s (userid, role_regtime, mail_time, item, item_count) values(%u,%u,%u,%u,%u)", 
            get_table_name(userid), userid, role_regtime, (uint32_t)time(NULL),  item_id, item_count);
    }       
    else//update    
    {       
        GEN_SQLSTR(sqlstr, "update %s set item_count = %u where userid = %u and item = %u",
            get_table_name(userid), item_count+ cur_count, userid, item_id);
    }
    return exec_update_list_sql(sqlstr,SUCC);
}
