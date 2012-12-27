#include <algorithm>
#include "db_error.h"
#include "Cgf_ap_toplist.h"

Cgf_ap_toplist::Cgf_ap_toplist(mysql_interface * db )
		:Ctable(db, "GF_OTHER", "t_gf_top_achieve_point")
{

}

int Cgf_ap_toplist::get_ap_toplist(uint32_t begin_index, uint32_t end_index, gf_get_ap_toplist_out_element** pdata, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select * from %s order by achieve_point desc, last_update_tm asc limit %u, %u", get_table_name(), begin_index - 1, end_index - begin_index + 1);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pdata, count);
        INT_CPY_NEXT_FIELD( (*pdata+i)->userid );
		INT_CPY_NEXT_FIELD( (*pdata+i)->role_regtime );
		BIN_CPY_NEXT_FIELD( (*pdata+i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD( (*pdata+i)->ap_point );
		INT_CPY_NEXT_FIELD( (*pdata+i)->last_update_tm );
	STD_QUERY_WHILE_END();
}
