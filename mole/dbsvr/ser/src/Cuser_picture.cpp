/*
 * =====================================================================================
 *
 *       Filename:  Cuser_picture_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_picture.h"
//user_picture
Cuser_picture::Cuser_picture(mysql_interface * db ) 
	:CtableRoute100x10( db,"PICTURE","t_user_picture","albumid")
{ 

}

int Cuser_picture::add( userid_t userid, pic_add_pic_in * p_in, pic_add_pic_out * p_out)
{
	int ret; 
	uint32_t filecount;
	ret=this->get_file_count(userid,p_in->albumid, &filecount );
	if(ret!=SUCC) return ret;
	if (filecount>=30){
		return   PIC_MAX_USER_PIC_ERR;
	}
		

	char fileid_mysql[mysql_str_len(PIC_FILE_ID_LEN)];
	set_mysql_string(fileid_mysql ,p_in->fileid,PIC_FILE_ID_LEN  );
	sprintf( this->sqlstr, "insert into %s values ( 0,%u,%u,'%s')", 
			this->get_table_name(userid), 
			userid,
			p_in->albumid,
				fileid_mysql
	   	);
		STD_INSERT_GET_ID(this->sqlstr, USER_ID_EXISTED_ERR, p_out->photoid );	
}
int  Cuser_picture ::get_filelist(userid_t userid , pic_get_user_ablum_piclist_in *p_in,
			   	uint32_t *p_count,pic_get_user_ablum_piclist_out_item  ** pp_list  )
{
	sprintf( this->sqlstr,  "select photoid,fileid  from %s \
		 	where userid=%u and albumid=%u order by photoid desc  limit %u ,%u " , 
		this->get_table_name(userid), userid,p_in->albumid, p_in->startindex,p_in->count); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->photoid); 
		BIN_CPY_NEXT_FIELD((*pp_list+i)->fileid,sizeof((*pp_list+i)->fileid));
	STD_QUERY_WHILE_END();
}
int Cuser_picture::get_file(userid_t userid , pic_get_pic_by_photoid_userid_in *p_in,
	   	pic_get_pic_by_photoid_userid_out * p_out )
{
	sprintf( this->sqlstr, "select albumid,fileid  from %s \
			where photoid=%u and   userid=%u ", 
			this->get_table_name(userid),p_in->photoid ,userid); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
		INT_CPY_NEXT_FIELD(p_out->albumid );
		BIN_CPY_NEXT_FIELD(p_out->fileid,sizeof(p_out->fileid));
	STD_QUERY_ONE_END();
}
int Cuser_picture::remove(userid_t userid, uint32_t photoid )
{
	sprintf( this->sqlstr, " delete from  %s \
		where photoid=%u and userid=%u  " ,
		this->get_table_name(userid), 
		photoid, userid );
	STD_SET_RETURN_EX(this->sqlstr,PIC_NOFIND_PIC_ERR );	
}


int Cuser_picture::get_file_count(userid_t userid, uint32_t albumid, uint32_t *p_count)
{
	sprintf( this->sqlstr,  "select count(1) from %s \
		 	where userid=%u and albumid=%u " , 
		this->get_table_name(userid), userid,albumid); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
		INT_CPY_NEXT_FIELD(*p_count );	
	STD_QUERY_ONE_END();
}

