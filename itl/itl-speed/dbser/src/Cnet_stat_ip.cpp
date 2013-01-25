
/*
 * =========================================================================
 *
 *        Filename: Cnet_stat_ip.cpp
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

#include "Cnet_stat_ip.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Cnet_stat_ip::Cnet_stat_ip(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int k = 0; k < MAX_IDC_DB; k++) {
		for(int i = 0; i < MAX_IP_TB; i++) {
			for(int j = 0; j < 4 ;j++) {
				this->nistr[k][i][j] = (char *)malloc(1024*16);
				this->niptr[k][i][j] = this->nistr[k][i][j];
			}
			this->nicnt[k][i] = 0;
		}
	}
}

Cnet_stat_ip::~Cnet_stat_ip() 
{
	DEBUG_LOG("WRITE DB---net_stat_ip");
	for(int k = 0; k < MAX_IDC_DB; k++) {
		for(int i = 0; i < MAX_IP_TB; i++) {
			if (this->nicnt[k][i]) {
				write_ip_tb_now(k, i);
			}
			for(int j = 0; j < 4 ;j++) {
				free(this->nistr[k][i][j]);
			}
		}
	}
}

int Cnet_stat_ip::insert (int db_idx, int idx, netstat_ip_t* ani) 
{
	int min = ani->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;

	if (db_idx > MAX_IDC_DB || idx > MAX_IP_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

	if(this->nicnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->nistr[db_idx][idx][i],
				"insert into net_stat_%02u.net_stat_ip%u_lv%u \
				(ip,seq,delay_v,delay_c,lost_v,lost_c,hop_v,hop_c) values ",
			   	db_idx, idx, i);
			this->niptr[db_idx][idx][i] = 
				this->nistr[db_idx][idx][i] + strlen(this->nistr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->niptr[db_idx][idx][i], "(%u, %u, %u, %u, %u, %u, %u, %u),",
			ani->id, seq[i], ani->d_v, ani->d_c,
			ani->l_v, ani->l_c, ani->h_v, ani->h_c);
		this->niptr[db_idx][idx][i] += strlen(this->niptr[db_idx][idx][i]);
	}
	this->nicnt[db_idx][idx] ++;

	if(this->nicnt[db_idx][idx] == SEND_COUNT) {
		write_ip_tb_now(db_idx, idx);	
	}

	return 0;
}

void Cnet_stat_ip::write_ip_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->niptr[di][ti][i]); // pointer go back from '\0' to ','

		sprintf(this->niptr[di][ti][i],
				" on duplicate key update \
				delay_v = delay_v + values(delay_v),\
				delay_c = delay_c + values(delay_c),\
				lost_v = lost_v + values(lost_v),\
				lost_c = lost_c + values(lost_c),\
				hop_v = hop_v + values(hop_v),\
				hop_c = hop_c + values(hop_c) ");

		this->exec_insert_sql( this->nistr[di][ti][i], USER_ID_EXISTED_ERR );
		this->niptr[di][ti][i] = this->nistr[di][ti][i];
	}
	this->nicnt[di][ti] = 0;

}

int	Cnet_stat_ip::insert( net_stat_ip_report_in *p_in )
{
    DEBUG_LOG("ip:%u logtime:%u all", p_in->ip, p_in->logtime);
	netstat_ip_t lni;
	lni.id = p_in->ip;
	lni.logtime = p_in->logtime;
	lni.d_v = p_in->delay_v;
	lni.d_c = p_in->count;
	lni.l_v = p_in->lost_v;
	lni.l_c = p_in->count;
	lni.h_v = p_in->hop_v;
	lni.h_c = p_in->count;
	insert(p_in->idcid, lni.id % MAX_IP_TB, &lni);
	return 0;
}

int	Cnet_stat_ip::insert( net_stat_report_ip_delay_in *p_in )
{
	DEBUG_LOG("ip:%u logtime:%u delay", p_in->ip, p_in->logtime);
	netstat_ip_t lni;
	memset(&lni, 0, sizeof(netstat_ip_t));
	lni.id = p_in->ip;
	lni.logtime = p_in->logtime;
	lni.d_v = p_in->value;
	lni.d_c = p_in->count;
	insert(p_in->idcid, lni.id % MAX_IP_TB, &lni);
	return 0;
}

int	Cnet_stat_ip::insert( net_stat_report_ip_lost_in *p_in )
{
	DEBUG_LOG("ip:%u logtime:%u lost", p_in->ip, p_in->logtime);
	netstat_ip_t lni;
	memset(&lni, 0, sizeof(netstat_ip_t));
	lni.id = p_in->ip;
	lni.logtime = p_in->logtime;
	lni.l_v = p_in->value;
	lni.l_c = p_in->count;
	insert(p_in->idcid, lni.id % MAX_IP_TB, &lni);
	return 0;
}

int	Cnet_stat_ip::insert( net_stat_report_ip_hop_in *p_in )
{
	DEBUG_LOG("ip:%u logtime:%u hop", p_in->ip, p_in->logtime);
	netstat_ip_t lni;
	memset(&lni, 0, sizeof(netstat_ip_t));
	lni.id = p_in->ip;
	lni.logtime = p_in->logtime;
	lni.h_v = p_in->value;
	lni.h_c = p_in->count;
	insert(p_in->idcid, lni.id % MAX_IP_TB, &lni);
	return 0;

}


