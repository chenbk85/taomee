
/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_ip.cpp
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

#include    "Ccdn_rate_ip.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Ccdn_rate_ip::Ccdn_rate_ip(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_CDNIP_TB; i++) {
			for(int j = 0; j < 4 ;j++) {
				this->ipstr[k][i][j] = (char *)malloc(1024*16);
				this->ipptr[k][i][j] = this->ipstr[k][i][j];
			}

			this->ipcnt[k][i] = 0;
		}
		for(int j = 0; j < 4 ;j++) {
			this->nodestr[k][j] = (char *)malloc(1024*16);
			this->nodeptr[k][j] = this->nodestr[k][j];
		}
		this->nodecnt[k] = 0;
	}
	
}

Ccdn_rate_ip::~Ccdn_rate_ip() 
{
	DEBUG_LOG("WRITE DB---cdn_rate_ip");
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_CDNIP_TB; i++) {
			if (this->ipcnt[k][i]) {
				write_ip_tb_now(k, i);
			}
			for(int j = 0; j < 4 ;j++) {
				free(this->ipstr[k][i][j]);
			}
		}
		if (this->nodecnt[k]) {
			write_node_tb_now(k);
		}
		for(int j = 0; j < 4 ;j++) {
			free(this->nodestr[k][j]);
		}
	}
	
}

int	Ccdn_rate_ip::insert_ip_tb( cdn_report_ip_in *p_in )
{

    int min = p_in->logtime / 60;

	int db_idx = p_in->prjid;
	int ip3 = p_in->ip >> 8;
	int idx = ip3 % 10;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_CDNIP_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("prjid:%u ip:%u logtime:%u idx:%u mycnt:%u",
			p_in->prjid, p_in->ip, p_in->logtime, idx, this->ipcnt[db_idx][idx]);
	if(this->ipcnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->ipstr[db_idx][idx][i],
			"insert into cdn_rate_%02u.cdn_rate_ip%u_lv%u \
			(ip, node, seq, value, count) values ",
			p_in->prjid, idx, i);

			this->ipptr[db_idx][idx][i] = 
				this->ipstr[db_idx][idx][i] + strlen(this->ipstr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->ipptr[db_idx][idx][i], "(%u, %u, %u, %u, %u),",
				p_in->ip, ip3, seq[i], p_in->value, p_in->count);
		this->ipptr[db_idx][idx][i] += strlen(this->ipptr[db_idx][idx][i]);
	}
	this->ipcnt[db_idx][idx] ++;

	if(this->ipcnt[db_idx][idx] == SEND_COUNT) {
		write_ip_tb_now(db_idx, idx);	
	}

	return 0;
}

void Ccdn_rate_ip::write_ip_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->ipptr[di][ti][i]); // pointer go back from '\0' to ','

		sprintf(this->ipptr[di][ti][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");

		//DEBUG_LOG("CDN_RATE_IP:%s", this->ipstr[di][ti][i]);
		this->exec_insert_sql( this->ipstr[di][ti][i], USER_ID_EXISTED_ERR );
		this->ipptr[di][ti][i] = this->ipstr[di][ti][i];
	}
	this->ipcnt[di][ti] = 0;
}

int	Ccdn_rate_ip::insert_node_tb( cdn_report_ip_in *p_in )
{
	
    int min = p_in->logtime / 60;

	int db_idx = p_in->prjid;
	int ip3 = p_in->ip >> 8;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;

	if (db_idx > MAX_CDNPRJ_DB)
		ERROR_RETURN(("invalid db idx\t[%u]", db_idx), 0);

    DEBUG_LOG("insert_node_tb:prjid:%u ip:%u logtime:%u mycnt:%u",
			p_in->prjid, p_in->ip, p_in->logtime, this->nodecnt[db_idx]);
	if(this->nodecnt[db_idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->nodestr[db_idx][i],
			"insert into cdn_rate_%02u.cdn_rate_node_lv%u \
			(node, seq, value, count) values ",
			p_in->prjid, i);
			this->nodeptr[db_idx][i] = 
				this->nodestr[db_idx][i] + strlen(this->nodestr[db_idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->nodeptr[db_idx][i], "(%u, %u, %u, %u),",
				ip3, seq[i], p_in->value, p_in->count);
		this->nodeptr[db_idx][i] += strlen(this->nodeptr[db_idx][i]);
	}
	this->nodecnt[db_idx]++;

	if(this->nodecnt[db_idx] == SEND_COUNT) {
		write_node_tb_now(db_idx);	
	}

	return 0;
}

void Ccdn_rate_ip::write_node_tb_now(int di)
{
	for(int i = 0; i < 4; i++) {
		-- (this->nodeptr[di][i]); // pointer go back from '\0' to ','

		sprintf(this->nodeptr[di][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");
		//DEBUG_LOG("CDN_RATE_NODE:%s", this->nodestr[di][idx][i]);
		this->exec_insert_sql( this->nodestr[di][i], USER_ID_EXISTED_ERR );
		this->nodeptr[di][i] = this->nodestr[di][i];
	}
	this->nodecnt[di] = 0;
}


