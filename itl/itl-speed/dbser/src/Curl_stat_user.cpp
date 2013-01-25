
/*
 * =========================================================================
 *
 *        Filename: Curl_stat_user.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include "Curl_stat_user.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Curl_stat_user::Curl_stat_user(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		for(int j = 0; j < MAX_URLUSER_TB ; j++) {
			this->urlrecordstr[i][j] = (char *)malloc(1024*16);
			this->urlrecordptr[i][j] = this->urlrecordstr[i][j];
			this->urcnt[i][j] = 0;
		}
	}
}

Curl_stat_user::~Curl_stat_user() 
{
	DEBUG_LOG("WRITE DB---url_stat_user");
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		for(int j = 0; j < MAX_URLUSER_TB; j++) {
			if (this->urcnt[i][j])
				write_record_tb_now(i, j);

			free(this->urlrecordstr[i][j]);
		}
	}
}

int Curl_stat_user::insert(url_rate_record_in *p_in )
{
	url_rate_rcod_t		lurr;
	memset(&lurr, 0, sizeof(url_rate_rcod_t));
	lurr.prjid=p_in->prjid;
	lurr.userid=p_in->userid;
	lurr.time=p_in->time;
	lurr.clientip=p_in->clientip;
	lurr.pageid=p_in->pageid;
	lurr.count=p_in->vlist.size();
	if (lurr.count > MAX_POINT_NUM)
		ERROR_RETURN(("too many point\t[%u]", lurr.count), -1);
	for (unsigned int i = 0; i < lurr.count; i++) {
		lurr.v[i] = p_in->vlist[i];
	}
	
	int ret = 0;
	ret = insert_url_record(&lurr);
	
	return ret;
}


int	Curl_stat_user::insert_url_record( url_rate_rcod_t *aur )
{
	DEBUG_LOG("URL RECORD:prj:%u logtime:%u", aur->prjid, aur->time);

	int di = aur->prjid;
	int ti = aur->userid%1000;
	if (di > MAX_URLPRJ_DB) {
		ERROR_RETURN(("invalid index\t[di=%u ti=%u]", di,ti), -1);
	}
	if(this->urcnt[di][ti] == 0) {
			sprintf( this->urlrecordstr[di][ti],
			"insert into url_rate_record_%02u.t_record_user_%03u \
			(userid,time,clientip,pageid,count,\
			 v0,v1,v2,v3,v4,v5,v6,v7,v8,v9) values ",
		   	aur->prjid, ti);
			this->urlrecordptr[di][ti] = 
				this->urlrecordstr[di][ti] + strlen(this->urlrecordstr[di][ti]);
	}

	sprintf( this->urlrecordptr[di][ti], 
			"(%u,%u,%u,%u,%u, %u,%u,%u,%u,%u,%u,%u,%u,%u,%u),",
			aur->userid,aur->time,aur->clientip,aur->pageid,aur->count,
			aur->v[0],aur->v[1],aur->v[2],aur->v[3],aur->v[4],
			aur->v[5],aur->v[6],aur->v[7],aur->v[8],aur->v[9]);

	this->urlrecordptr[di][ti] += strlen(this->urlrecordptr[di][ti]);
	this->urcnt[di][ti] ++;

	if(this->urcnt[di][ti] == SEND_COUNT) {
		write_record_tb_now(di, ti);
	}

	return 0;
}

void Curl_stat_user::write_record_tb_now(int di, int ti)
{
	-- (this->urlrecordptr[di][ti]); // pointer go back from '\0' to ','

	sprintf(this->urlrecordptr[di][ti],
			"on duplicate key update v0=values(v0),v1=values(v1),\
			v2=values(v2),v3=values(v3),v4=values(v4),v5=values(v5),\
			v6=values(v6),v7=values(v7),v8=values(v8),v9=values(v9)");

	this->exec_insert_sql( this->urlrecordstr[di][ti], USER_ID_EXISTED_ERR );
	this->urlrecordptr[di][ti] = this->urlrecordstr[di][ti];
	this->urcnt[di][ti] = 0;
}
