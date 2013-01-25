
#include "CtableRoute100x1.h"

CtableRoute100x1::CtableRoute100x1(mysql_interface* db, const char* db_name_pre,const char* table_name_pre,const char* id_name,const char *key2_name)
	:CtableRoute(db,db_name_pre,table_name_pre,id_name,key2_name)
{

}

char* CtableRoute100x1::get_table_name(uint32_t id)
{
    this->id = id;
    sprintf(this->db_name,"%s_%02d",this->db_name_pre,id % 100);
    this->db->select_db(this->db_name);
    sprintf(this->db_table_name,"%s.%s",this->db_name,this->table_name_pre);
    return this->db_table_name;
}
