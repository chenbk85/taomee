/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_picture.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_PICTURE_INCL
#define    CUSER_PICTURE_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_picture:public CtableRoute100x10{
	private:
	public:
		Cuser_picture(mysql_interface * db ); 
		int add( userid_t userid, pic_add_pic_in * p_in, pic_add_pic_out * p_out);

		int  get_filelist(userid_t userid , pic_get_user_ablum_piclist_in *p_in,
			   	uint32_t *p_count,pic_get_user_ablum_piclist_out_item  ** pp_list  );

		int get_file(userid_t userid , pic_get_pic_by_photoid_userid_in *p_in,
	   	pic_get_pic_by_photoid_userid_out * p_out );
		int remove(userid_t userid, uint32_t photoid );
		int get_file_count(userid_t userid, uint32_t albumid, uint32_t *p_count);

};
#endif   /* ----- #ifndef CUSER_PICTURE_INCL  ----- */
