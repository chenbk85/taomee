
/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_prov.cpp
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

#include "Ccdn_rate_prov.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Ccdn_rate_prov::Ccdn_rate_prov(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_CDNPROV_TB; i++) {
			for(int j = 0; j < 4 ;j++) {
				this->citystr[k][i][j] = (char *)malloc(1024*16);
				this->cityptr[k][i][j] = this->citystr[k][i][j];
			}
			this->citycnt[k][i] = 0;
		}
		for(int j = 0; j < 4 ;j++) {
			this->provstr[k][j] = (char *)malloc(1024*16);
			this->provptr[k][j] = this->provstr[k][j];
		}
		this->provcnt[k] = 0;
	}
}

Ccdn_rate_prov::~Ccdn_rate_prov() 
{
	DEBUG_LOG("WRITE DB---cdn_rate_prov");
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_CDNPROV_TB; i++) {
			if (this->citycnt[k][i]) {
				write_city_tb_now(k, i);
			}
			for(int j = 0; j < 4 ;j++) {
				free(this->citystr[k][i][j]);
			}
		}
		if (this->provcnt[k]) {
			write_prov_tb_now(k);
		}
		for(int j = 0; j < 4 ;j++) {
			free(this->provstr[k][j]);
		}
	}
}
	
int	Ccdn_rate_prov::insert_city_tb( cdn_report_prov_in *p_in )
{
	
	//lv1-lv4
	int min = p_in->logtime / 60;
	int db_idx = p_in->prjid;
	int idx = p_in->provid/10000;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_CDNPROV_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

	DEBUG_LOG("prjid:%u prov:%u logtime:%u idx:%u mycnt:%u", 
			p_in->prjid, p_in->provid, p_in->logtime, 
			idx, this->citycnt[db_idx][idx]);
	if(this->citycnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->citystr[db_idx][idx][i],
			"insert into cdn_rate_%02u.cdn_rate_prov_%06u_lv%u \
			(cityid, comp, seq, value, count) values ",
			db_idx, p_in->provid, i);

			this->cityptr[db_idx][idx][i] = 
				this->citystr[db_idx][idx][i] + strlen(this->citystr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->cityptr[db_idx][idx][i], "(%u, '%s', %u, %u, %u),",
				p_in->cityid, p_in->comp, seq[i], p_in->value, p_in->count);
		this->cityptr[db_idx][idx][i] += strlen(this->cityptr[db_idx][idx][i]);
	}
	this->citycnt[db_idx][idx] ++;

	if(this->citycnt[db_idx][idx] == SEND_COUNT) {
		write_city_tb_now(db_idx, idx);
	}
	
	return 0;
}

void Ccdn_rate_prov::write_city_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->cityptr[di][ti][i]); // pointer go back from '\0' to ','
		sprintf(this->cityptr[di][ti][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");

		this->exec_insert_sql( this->citystr[di][ti][i], USER_ID_EXISTED_ERR );
		this->cityptr[di][ti][i] = this->citystr[di][ti][i];
	}
	this->citycnt[di][ti] = 0;
}

int	Ccdn_rate_prov::insert_prov_tb( cdn_report_prov_in *p_in )
{
	
	int min = p_in->logtime / 60;
	int db_idx = p_in->prjid;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;

	if (db_idx > MAX_CDNPRJ_DB)
		ERROR_RETURN(("invalid db idx\t[%u]", db_idx), 0);

	DEBUG_LOG("prjid:%u prov:%u logtime:%u provcnt:%u", 
			p_in->prjid, p_in->provid, p_in->logtime, this->provcnt[db_idx]);
	if(this->provcnt[db_idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->provstr[db_idx][i],
			"insert into cdn_rate_%02u.cdn_rate_prov_lv%u \
			(provid, seq, value, count) values ",
			db_idx, i);
			this->provptr[db_idx][i] = 
				this->provstr[db_idx][i] + strlen(this->provstr[db_idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->provptr[db_idx][i], "(%u, %u, %u, %u),",
				p_in->provid, seq[i], p_in->value, p_in->count);
		this->provptr[db_idx][i] += strlen(this->provptr[db_idx][i]);
	}
	this->provcnt[db_idx] ++;

	if(this->provcnt[db_idx] == SEND_COUNT) {
		write_prov_tb_now(db_idx);	
	}
	
	return 0;
}

void Ccdn_rate_prov::write_prov_tb_now(int di)
{
	for(int i = 0; i < 4; i++) {
		-- (this->provptr[di][i]); // pointer go back from '\0' to ','
		sprintf(this->provptr[di][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");

		this->exec_insert_sql( this->provstr[di][i], USER_ID_EXISTED_ERR );
		this->provptr[di][i] = this->provstr[di][i];
	}
	this->provcnt[di] = 0;
}
