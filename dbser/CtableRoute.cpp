/*
 * =====================================================================================
 *
 *       Filename:  CtableRoute.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "CtableRoute10.h"

#include <string>
#include <vector>


#define IS_NUM_FIELD(f) ((f)->flags & NUM_FLAG)


void str_addhex(std::string &to, char*from,int from_len  )
{
	char hex_buf[3];
	for (int i=0;i<from_len;i++){
		sprintf(hex_buf, "%02X",*((unsigned char *)&from[i]) );
		to+=hex_buf;
	}
}

CtableRoute::CtableRoute(mysql_interface * db,const  char * db_name_pre,  
	const 	char * table_name_pre,const  char* id_name ,const char* key2_name ) 
	:Ctable(db,"","")	
{ 
    strncpy(this->db_name_pre, db_name_pre,sizeof(this->db_name_pre));
	strncpy (this->table_name_pre,table_name_pre,sizeof(this->table_name_pre ) );
	strncpy (this->id_name,id_name,sizeof(this->id_name));
	strncpy (this->key2_name,key2_name,sizeof(this->key2_name));
}

char * CtableRoute::get_table_name(uint32_t id)
{

	this->id=id;
	sprintf (this->db_name,"%s_%02d", this->db_name_pre,id%100 );
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s_%02d.%s_%02d",
	this->db_name_pre,id%100, this->table_name_pre,(id/100)%100);
	return this->db_table_name;
}

int CtableRoute::id_is_existed(uint32_t id, uint32_t key2, bool * existed)
{

	sprintf( this->sqlstr, "select 1  from %s where %s=%u and %s=%u ", 
		 this->get_table_name(id),this->id_name, id,this->key2_name,key2 );
		*existed=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		*existed=true;
	STD_QUERY_ONE_END();
}


int CtableRoute::id_is_existed(uint32_t id, bool * existed)
{
	sprintf (this->sqlstr,"select  1 from %s where %s=%d ", 
			this->get_table_name(id), this->id_name,id);
		*existed=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		*existed=true;
	STD_QUERY_ONE_END();
}


int CtableRoute::get_int_value(userid_t userid ,const char * field_type ,  uint32_t * p_value)
{
	sprintf( this->sqlstr, "select  %s from %s where %s=%u ", 
		field_type , this->get_table_name(userid),this->id_name, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value );
	STD_QUERY_ONE_END();
}

int CtableRoute::set_int_value(userid_t userid ,const char * field_type , uint32_t  value)
{
	sprintf( this->sqlstr, "update %s set %s =%u where %s=%u " ,
		this->get_table_name(userid),field_type,value ,this->id_name, userid );
	return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR );	
}

int CtableRoute::set_int_value_bit(uint32_t userid ,const char * field_type ,uint32_t bitid ,uint32_t  value)
{
    if (bitid==0 ||  bitid>32 || value>1   ){
        return ENUM_OUT_OF_RANGE_ERR;
    }
    if (value==1){
        value=(1<<(bitid-1));
        sprintf( this->sqlstr, "update %s set %s =%s |%u    where %s=%u " ,
            this->get_table_name(userid),field_type,field_type ,  value , this->id_name ,userid );

    }else{
        value=0xFFFFFFFF-(1<<(bitid-1));
        sprintf( this->sqlstr, "update %s set %s =%s &%u    where %s=%u " ,
            this->get_table_name(userid),field_type,field_type ,  value , this->id_name ,userid );
    }
    return this->exec_update_sql(this->sqlstr,KEY_NOFIND_ERR );
}

int CtableRoute::change_int_value(userid_t userid ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value , uint32_t *p_cur_value, 
				int32_t *p_real_change_value, bool is_change_to_max_min)
{
	uint32_t db_value;	
	int ret; 
	ret=this->get_int_value(userid,field_type,&db_value  );
	if(ret!=SUCC) return ret;
	int real_change_value=changevalue;
	int value= (int)db_value + changevalue;
	if (value<0){
		if (!is_change_to_max_min){
			return VALUE_NOENOUGH_E;
		}else{
			value=0;
			real_change_value=value-db_value;
		}
	} else if ((uint32_t) value> max_value ){
		if (!is_change_to_max_min){
			return VALUE_MAX_E;
		}else{
			value=max_value;
			real_change_value=value-db_value;
		}
	}
	if (p_cur_value!=NULL ){
		*p_cur_value=value;
	}
	if (p_real_change_value!=NULL ){
		*p_real_change_value=real_change_value;
	};
	if (real_change_value!=0){
		return this->set_int_value(userid,field_type,value );
	}else{
		return SUCC;
	}
}

int CtableRoute::change_int_value(userid_t userid ,uint32_t key2 ,const char * field_type ,
			   	int32_t changevalue, uint32_t max_value , uint32_t *p_cur_value, 
				int32_t *p_real_change_value, bool is_change_to_max_min)
{
	uint32_t db_value;	
	int ret; 
	ret=this->get_int_value(userid,key2,field_type,&db_value  );
	if(ret!=SUCC) return ret;
	int real_change_value=changevalue;
	int value= (int)db_value + changevalue;
	DEBUG_LOG("db_value:%u, value:%d,max_value:%u",db_value,value,max_value);
	if (value<0){
		if (!is_change_to_max_min){
			return VALUE_NOENOUGH_E;
		}else{
			value=0;
			real_change_value=value-db_value;
		}
	} else if ((uint32_t) value> max_value ){
		if (!is_change_to_max_min){
			return VALUE_MAX_E;
		}else{
			value=max_value;
			real_change_value=value-db_value;
		}
	}
	if (p_cur_value!=NULL ){
		*p_cur_value=value;
	}
	
	if (p_real_change_value!=NULL ){
		*p_real_change_value=real_change_value;
	};
	if (real_change_value!=0){
		return this->set_int_value(userid,key2,field_type,value );
	}else{
		return SUCC;
	}
}

int CtableRoute::get_int_value(userid_t userid ,uint32_t key2 ,const char * field_type ,  uint32_t * p_value)
{
	sprintf( this->sqlstr, "select  %s from %s where %s=%u and %s=%u ", 
		field_type , this->get_table_name(userid),this->id_name, userid,this->key2_name,key2 );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value );
	STD_QUERY_ONE_END();
}

int CtableRoute::set_int_value(userid_t userid ,uint32_t key2 ,const char * field_type , uint32_t  value)
{
	sprintf( this->sqlstr, "update %s set %s =%u where %s=%u and %s=%u " ,
		this->get_table_name(userid),field_type,value ,this->id_name, userid,this->key2_name,key2 );
	return this->exec_update_sql(this->sqlstr,KEY_NOFIND_ERR );	
}

int CtableRoute::set_int_value_bit(uint32_t userid ,uint32_t key2 ,const char * field_type ,uint32_t bitid ,uint32_t  value)
{
    if (bitid==0 ||  bitid>32 || value>1   ){
        return ENUM_OUT_OF_RANGE_ERR;
    }
    if (value==1){
        value=(1<<(bitid-1));
        sprintf( this->sqlstr, "update %s set %s =%s |%u    where %s=%u and %s=%u  " ,
            this->get_table_name(userid),field_type,field_type ,  value , 
			this->id_name ,userid,this->key2_name,key2 );

    }else{
        value=0xFFFFFFFF-(1<<(bitid-1));
        sprintf( this->sqlstr, "update %s set %s =%s &%u    where %s=%u and %s=%u  " ,
            this->get_table_name(userid),field_type,field_type ,  value , 
			this->id_name ,userid,this->key2_name,key2 );
    }
    return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR );
}

int CtableRoute::get_insert_sql_by_userid( userid_t userid, std::string & sql_str,const char * userid_field_name, uint32_t obj_userid )
{
	char buf[100000];
	if ( obj_userid==0){
		obj_userid=userid;
	}
	sprintf( this->sqlstr, "delete from  %s  where %s =%u  ;\n" ,
            this->get_table_name(obj_userid),userid_field_name,obj_userid );
	sql_str=this->sqlstr;
    
	sprintf( this->sqlstr, "select * from  %s  where %s =%u  " ,
            this->get_table_name(userid),userid_field_name,userid );
    MYSQL_RES *res;
	int ret;
	ret=this->db->exec_query_sql(this->sqlstr,  &res);
	if(ret!=SUCC) return ret;

	MYSQL_ROW row;
	MYSQL_FIELD   *field;
	if (mysql_num_rows(res)==0){
    	mysql_free_result(res);	//free result after you get the result
		return SUCC;
	};
	//得到字段类型
	mysql_field_seek(res,0);	
	std::vector<int> is_num_field_list;
	int num_fields=mysql_num_fields(res);
	for (int i = 0; i <num_fields;  i++) {
		field= mysql_fetch_field(res);
		is_num_field_list.push_back (IS_NUM_FIELD(field)==32768);
	}


	char obj_userid_str[100];
	sprintf(obj_userid_str ,"%u",obj_userid);
	sql_str+="insert into ";
	sql_str+= this->get_table_name(obj_userid);
	sql_str+= " values ";	
    while ((row = mysql_fetch_row(res))!=NULL) {
		ulong *lengths= mysql_fetch_lengths(res);
		sql_str+="(";
		for (uint32_t i = 0; i < mysql_num_fields(res); i++) {
			if (row[i]==NULL){
				sql_str+="NULL" ;
			}else{
				if(i==0){//是userid
					sql_str+=obj_userid_str ;
				}else{
					if (is_num_field_list[i] ){
						sql_str+= row[i] ;
					}else{
						sql_str+= "'" ;
						set_mysql_string(buf,row[i],lengths[i]);
						sql_str+= buf ;
						sql_str+= "'" ;
					}
				}
			}
			sql_str+="," ;
		}
		sql_str.replace(sql_str.length()-1,1,1,')');
		sql_str+="," ;
    }
	sql_str.replace(sql_str.length()-1,1,1,';');
	sql_str+="\n" ;

    mysql_free_result(res);	//free result after you get the result
	return SUCC;
}
