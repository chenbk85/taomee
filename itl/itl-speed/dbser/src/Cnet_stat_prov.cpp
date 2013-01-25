
/*
 * =========================================================================
 *
 *        Filename: Cnet_stat_prov.cpp
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

#include "Cnet_stat_prov.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Cnet_stat_prov::Cnet_stat_prov(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int i = 0; i < MAX_IDC_DB; i++) {
		for(int j = 0; j < MAX_PROV_TB; j++) {
			for(int k = 0; k < 4 ;k++) {
				this->citystr[i][j][k] = (char *)malloc(1024*16);
				this->cityptr[i][j][k] = this->citystr[i][j][k];
			}
			this->citycnt[i][j] = 0;

		}
	}
}

Cnet_stat_prov::~Cnet_stat_prov() 
{
	DEBUG_LOG("WRITE DB---net_stat_prov");
	for(int i = 0; i < MAX_IDC_DB; i++) {
		for(int j = 0; j < MAX_PROV_TB; j++) {
			if (this->citycnt[i][j]) {
				write_city_tb_now(i, j);
			}
			for(int k = 0; k < 4 ;k++) {
				free(this->citystr[i][j][k]);
			}
		}
	}
}

int Cnet_stat_prov::insert_city_tb(netstat_prov_t* anp)
{
	int min = anp->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;

	uint32_t db_idx = anp->idcid;
	int idx = anp->provid/10000;

	if (db_idx > MAX_IDC_DB || idx > MAX_PROV_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

	if(this->citycnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->citystr[db_idx][idx][i],
				"insert into net_stat_%02u.%s%06u_lv%u \
				(cityid, comp, seq, delay_v, delay_c, \
				 lost_v, lost_c, hop_v, hop_c) values ",
			   	db_idx, "net_stat_prov_", anp->provid, i);
			//ptr point to the last character '\n'
			this->cityptr[db_idx][idx][i] = 
				this->citystr[db_idx][idx][i] + strlen(this->citystr[db_idx][idx][i]);
		}
	}

	for(int i = 0; i < 4; i++) {
		sprintf(this->cityptr[db_idx][idx][i],
				"(%u, '%s', %u, %u, %u, %u, %u, %u, %u),",
				anp->cityid, anp->comp, seq[i], anp->d_v, 
				anp->d_c, anp->l_v, anp->l_c, anp->h_v, anp->h_c);
		this->cityptr[db_idx][idx][i] += strlen(this->cityptr[db_idx][idx][i]);
	}
	this->citycnt[db_idx][idx] ++;

	if(this->citycnt[db_idx][idx] == SEND_COUNT) {
		write_city_tb_now(db_idx, idx);	
	}

	return 0;
}

void Cnet_stat_prov::write_city_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->cityptr[di][ti][i]); // pointer go back from '\0' to ','

		sprintf(this->cityptr[di][ti][i],
				" on duplicate key update \
				delay_v = delay_v + values(delay_v), delay_c = delay_c + values(delay_c),\
				lost_v = lost_v + values(lost_v), lost_c = lost_c + values(lost_c),\
				hop_v = hop_v+ values(hop_v), hop_c = hop_c + values(hop_c)");

		this->exec_insert_sql( this->citystr[di][ti][i], USER_ID_EXISTED_ERR );
		this->cityptr[di][ti][i] = this->citystr[di][ti][i];
	}
	this->citycnt[di][ti] = 0;

}

int	Cnet_stat_prov::insert( net_stat_prov_report_in *p_in )
{
	DEBUG_LOG("net_stat_prov_report:%u logtime:%u", p_in->provid, p_in->logtime);
	netstat_prov_t lnp;
	memset(&lnp, 0, sizeof(netstat_prov_t));
	lnp.idcid = p_in->idcid;
	lnp.provid = p_in->provid;
	lnp.cityid = p_in->cityid;
	memcpy(lnp.comp, p_in->comp, 64);
	lnp.logtime = p_in->logtime;
	lnp.d_v = p_in->delay_v;
	lnp.d_c = p_in->count;
	lnp.l_v = p_in->lost_v;
	lnp.l_c = p_in->count;
	lnp.h_v = p_in->hop_v;
	lnp.h_c = p_in->count;
	insert_city_tb(&lnp);
	return 0;
}

int	Cnet_stat_prov::insert( net_stat_report_prov_delay_in *p_in )
{
	netstat_prov_t lnp;
	memset(&lnp, 0, sizeof(netstat_prov_t));
	lnp.idcid = p_in->idcid;
	lnp.provid = p_in->provid;
	lnp.cityid = p_in->cityid;
	memcpy(lnp.comp, p_in->comp, 64);
	lnp.logtime = p_in->logtime;

	DEBUG_LOG("lost prov:%u logtime:%u idcid:%u id:%u", 
		p_in->provid, p_in->logtime, p_in->idcid, lnp.idcid);
	lnp.d_v = p_in->value;
	lnp.d_c = p_in->count;
	insert_city_tb(&lnp);

	return 0;
}


int	Cnet_stat_prov::insert( net_stat_report_prov_lost_in *p_in )
{
	netstat_prov_t lnp;
	memset(&lnp, 0, sizeof(netstat_prov_t));
	lnp.idcid = p_in->idcid;
	lnp.provid = p_in->provid;
	lnp.cityid = p_in->cityid;
	memcpy(lnp.comp, p_in->comp, 64);
	lnp.logtime = p_in->logtime;

	DEBUG_LOG("lost prov:%u logtime:%u idcid:%u id:%u", 
		p_in->provid, p_in->logtime, p_in->idcid, lnp.idcid);
	lnp.l_v = p_in->value;
	lnp.l_c = p_in->count;
	insert_city_tb(&lnp);

	return 0;
}


int	Cnet_stat_prov::insert( net_stat_report_prov_hop_in *p_in )
{
	DEBUG_LOG("hop prov:%u logtime:%u", p_in->provid, p_in->logtime);
	netstat_prov_t lnp;
	memset(&lnp, 0, sizeof(netstat_prov_t));
	lnp.idcid = p_in->idcid;
	lnp.provid = p_in->provid;
	lnp.cityid = p_in->cityid;
	memcpy(lnp.comp, p_in->comp, 64);
	lnp.logtime = p_in->logtime;
	
	lnp.h_v = p_in->value;
	lnp.h_c = p_in->count;
	insert_city_tb(&lnp);

	return 0;
}

