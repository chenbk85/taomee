
/*
 * =========================================================================
 *
 *        Filename: Curl_stat_prov.cpp
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

#include "Curl_stat_prov.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Curl_stat_prov::Curl_stat_prov(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		for(int j = 0; j < 4 ; j++) {
			this->urlispstr[i][j] = (char *)malloc(1024*16);
			this->urlispptr[i][j] = this->urlispstr[i][j];
		}
		this->uicnt[i] = 0;
	}
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		for(int j = 0; j < 4 ; j++) {
			this->urlprovstr[i][j] = (char *)malloc(1024*16);
			this->urlprovptr[i][j] = this->urlprovstr[i][j];
		}
		this->upcnt[i] = 0;
	}
	for(int k = 0; k < MAX_URLPRJ_DB ; k++) {
		for(int i = 0; i < MAX_URLPROV_TB ; i++) {
			for(int j = 0; j < 4 ; j++) {
				this->urldetailstr[k][i][j] = (char *)malloc(1024*16);
				this->urldetailptr[k][i][j] = this->urldetailstr[k][i][j];
			}
			this->udcnt[k][i] = 0;
		}
	}
}

Curl_stat_prov::~Curl_stat_prov() 
{
	DEBUG_LOG("WRITE DB---url_stat_prov");
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		if (this->uicnt[i])
			write_isp_tb_now(i);
		for(int j = 0; j < 4 ; j++) {
			free(this->urlispstr[i][j]);
		}
	}
	for(int i = 0; i < MAX_URLPRJ_DB ; i++) {
		if (this->upcnt[i])
			write_prov_tb_now(i);
		for(int j = 0; j < 4 ; j++) {
			free(this->urlprovstr[i][j]);
		}
	}
	for(int k = 0; k < MAX_URLPRJ_DB ; k++) {
		for(int i = 0; i < MAX_URLPROV_TB ; i++) {
			if (this->udcnt[k][i])
				write_detail_tb_now(k, i);
			for(int j = 0; j < 4 ; j++) {
				free(this->urldetailstr[k][i][j]);
			}
		}
	}
}

int Curl_stat_prov::insert(url_stat_report_point_time_in *p_in )
{
	url_rate_info_t		luri;
	memset(&luri, 0, sizeof(url_rate_info_t));
	luri.prjid=p_in->prjid;
	luri.pageid=p_in->pageid;
	luri.provid=p_in->provid;
	luri.cityid=p_in->cityid;
	luri.ISPID=p_in->ISPID;
	memcpy(&luri.comp, p_in->comp, sizeof(luri.comp));
	luri.logtime=p_in->logtime;
	luri.repot_cnt=p_in->repot_cnt;
	luri.count=p_in->vlist.size();
	if (luri.count > MAX_POINT_NUM)
		ERROR_RETURN(("too many point\t[%u]", luri.count), -1);
	for (unsigned int i = 0; i < luri.count; i++) {
		luri.v[i] = p_in->vlist[i];
	}
	
	int ret = 0;
	ret = insert_url_isp(&luri);
	if (ret) return ret;

	ret = insert_url_prov(&luri);
	if (ret) return ret;

	ret = insert_url_detail(&luri);
	
	return ret;
}

int	Curl_stat_prov::insert_url_isp( url_rate_info_t *aur )
{
	DEBUG_LOG("URL ISPID:%u logtime:%u", aur->ISPID, aur->logtime);

	int min = aur->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;
	
	int di = aur->prjid;
	if (di > MAX_URLPRJ_DB) {
		ERROR_RETURN(("too big project id\t[%u]", di), -1);
	}
	if(this->uicnt[di] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->urlispstr[di][i],
			"insert into url_stat_%02u.t_url_isp_lv%u \
			(ISPID,seq,pageid,comp,repot_cnt,count,v0,\
			 v1,v2,v3,v4,v5,v6,v7,v8,v9) values ",
		   	aur->prjid, i);

			//ptr point to the last character '\n'
			this->urlispptr[di][i] = 
				this->urlispstr[di][i] + strlen(this->urlispstr[di][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->urlispptr[di][i], 
				"(%u,%u,%u,'%s',%u,%u, %u,%u,%u,%u,%u,%u,%u,%u,%u,%u),",
				aur->ISPID,seq[i],aur->pageid,aur->comp,aur->repot_cnt,aur->count,
				aur->v[0],aur->v[1],aur->v[2],aur->v[3],aur->v[4],
				aur->v[5],aur->v[6],aur->v[7],aur->v[8],aur->v[9]);

		this->urlispptr[di][i] += strlen(this->urlispptr[di][i]);
	}
	this->uicnt[di] ++;

	if(this->uicnt[di] == SEND_COUNT) {
		write_isp_tb_now(di);
	}
	return 0;
}

void Curl_stat_prov::write_isp_tb_now(int di) 
{
	for(int i = 0; i < 4; i++) {
		-- (this->urlispptr[di][i]); // pointer go back from '\0' to ','

		sprintf(this->urlispptr[di][i],
				"on duplicate key update repot_cnt=repot_cnt+values(repot_cnt),\
				v0=v0+values(v0),v1=v1+values(v1),v2=v2+values(v2),v3=v3+values(v3),\
				v4=v4+values(v4),v5=v5+values(v5),v6=v6+values(v6),v7=v7+values(v7),\
				v8=v8+values(v8),v9=v9+values(v9)");
		this->exec_insert_sql( this->urlispstr[di][i], USER_ID_EXISTED_ERR );
		this->urlispptr[di][i] = this->urlispstr[di][i];
	}
	this->uicnt[di] = 0;

}

int	Curl_stat_prov::insert_url_prov( url_rate_info_t *aur )
{
	DEBUG_LOG("URL PROVID:%u logtime:%u", aur->provid, aur->logtime);

	int min = aur->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;
	
	int di = aur->prjid;
	if (di > MAX_URLPRJ_DB) {
		ERROR_RETURN(("too big project id\t[%u]", di), -1);
	}
	if(this->upcnt[di] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->urlprovstr[di][i],
			"insert into url_stat_%02u.t_url_prov_lv%u \
			(provid,seq,pageid,repot_cnt,count,v0,v1,\
			 v2,v3,v4,v5,v6,v7,v8,v9) values ",
		   	aur->prjid, i);

			//ptr point to the last character '\n'
			this->urlprovptr[di][i] = 
				this->urlprovstr[di][i] + strlen(this->urlprovstr[di][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->urlprovptr[di][i], 
				"(%u,%u,%u,%u,%u, %u,%u,%u,%u,%u,%u,%u,%u,%u,%u),",
				aur->provid,seq[i],aur->pageid,aur->repot_cnt,aur->count,
				aur->v[0],aur->v[1],aur->v[2],aur->v[3],aur->v[4],
				aur->v[5],aur->v[6],aur->v[7],aur->v[8],aur->v[9]);

		this->urlprovptr[di][i] += strlen(this->urlprovptr[di][i]);
	}
	this->upcnt[di] ++;

	if(this->upcnt[di] == SEND_COUNT) {
		write_prov_tb_now(di);
	}
	return 0;
}

void Curl_stat_prov::write_prov_tb_now(int di) 
{
	for(int i = 0; i < 4; i++) {
		-- (this->urlprovptr[di][i]); // pointer go back from '\0' to ','

		sprintf(this->urlprovptr[di][i],
				"on duplicate key update repot_cnt=repot_cnt+values(repot_cnt),\
				v0=v0+values(v0),v1=v1+values(v1),v2=v2+values(v2),v3=v3+values(v3),\
				v4=v4+values(v4),v5=v5+values(v5),v6=v6+values(v6),v7=v7+values(v7),\
				v8=v8+values(v8),v9=v9+values(v9)");
		this->exec_insert_sql( this->urlprovstr[di][i], USER_ID_EXISTED_ERR );
		this->urlprovptr[di][i] = this->urlprovstr[di][i];
	}
	this->upcnt[di] = 0;

}

int	Curl_stat_prov::insert_url_detail( url_rate_info_t *aur )
{
	DEBUG_LOG("URL DETAIL:prj:%u logtime:%u", aur->prjid, aur->logtime);

	int min = aur->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;
	
	int di = aur->prjid;
	int ti = aur->provid/10000;
	if (di > MAX_URLPRJ_DB || ti > MAX_URLPROV_TB) {
		ERROR_RETURN(("invalid index\t[di=%u ti=%u]", di,ti), -1);
	}
	if(this->udcnt[di][ti] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->urldetailstr[di][ti][i],
			"insert into url_stat_%02u.t_url_prov_%06u_lv%u \
			(seq,pageid,ISPID,cityid,comp,repot_cnt,count,\
			 v0,v1,v2,v3,v4,v5,v6,v7,v8,v9) values ",
		   	aur->prjid, aur->provid, i);

			this->urldetailptr[di][ti][i] = 
				this->urldetailstr[di][ti][i] + strlen(this->urldetailstr[di][ti][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->urldetailptr[di][ti][i], 
				"(%u,%u,%u,%u,'%s',%u,%u, %u,%u,%u,%u,%u,%u,%u,%u,%u,%u),",
				seq[i],aur->pageid,aur->ISPID,aur->cityid,
				aur->comp,aur->repot_cnt,aur->count,
				aur->v[0],aur->v[1],aur->v[2],aur->v[3],aur->v[4],
				aur->v[5],aur->v[6],aur->v[7],aur->v[8],aur->v[9]);

		this->urldetailptr[di][ti][i] += strlen(this->urldetailptr[di][ti][i]);
	}
	this->udcnt[di][ti] ++;

	if(this->udcnt[di][ti] == SEND_COUNT) {
		write_detail_tb_now(di, ti);		
	}

	return 0;
}

void Curl_stat_prov::write_detail_tb_now(int di, int ti) 
{
	for(int i = 0; i < 4; i++) {
		-- (this->urldetailptr[di][ti][i]); // pointer go back from '\0' to ','
		sprintf(this->urldetailptr[di][ti][i],
				"on duplicate key update repot_cnt=repot_cnt+values(repot_cnt),\
				v0=v0+values(v0),v1=v1+values(v1),v2=v2+values(v2),v3=v3+values(v3),\
				v4=v4+values(v4),v5=v5+values(v5),v6=v6+values(v6),v7=v7+values(v7),\
				v8=v8+values(v8),v9=v9+values(v9)");

		this->exec_insert_sql( this->urldetailstr[di][ti][i], USER_ID_EXISTED_ERR );
		this->urldetailptr[di][ti][i] = this->urldetailstr[di][ti][i];
	}
	this->udcnt[di][ti] = 0;
}
