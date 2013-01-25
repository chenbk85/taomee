
/*
 * =========================================================================
 *
 *        Filename: Clink_stat.cpp
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

#include "Clink_stat.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Clink_stat::Clink_stat(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int j = 0; j < 4 ;j++) {
		this->mysqlstr[j] = (char *)malloc(1024*16);
		this->mysqlptr[j] = this->mysqlstr[j];
	}
	this->mycount = 0;
}

Clink_stat::~Clink_stat() 
{
	DEBUG_LOG("WRITE DB---link_stat");
	if (this->mycount) {
		write_link_tb_now();
	}
	for(int j = 0; j < 4 ;j++) {
		free(this->mysqlstr[j]);
	}
	this->mycount = 0;
}

int Clink_stat::insert (link_stat_t* als) 
{
	int min = als->logtime / 60;
	int seq[4];
	seq[0] = min;
	seq[1] = min / MIN10;
	seq[2] = min / HOUR;
	seq[3] = min / DAY;

	if(this->mycount == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->mysqlstr[i],
				"insert into link_stat.link_stat_lv%u \
				(link,seq,delay_v,delay_c,lost_v,lost_c,hop_v,hop_c) values ",
			   	i);
			//ptr point to the last character '\n'
			this->mysqlptr[i] = this->mysqlstr[i] + strlen(this->mysqlstr[i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->mysqlptr[i], "(%u, %u, %u, %u, %u, %u, %u, %u),",
			als->id, seq[i], als->d_v, als->d_c, 
			als->l_v, als->l_c, als->h_v, als->h_c);
		this->mysqlptr[i] += strlen(this->mysqlptr[i]);
	}
	this->mycount++;

	if(this->mycount == SEND_COUNT) {
		write_link_tb_now();	
	}

	return 0;
}

void Clink_stat::write_link_tb_now()
{
	for(int i = 0; i < 4; i++) {
		-- (this->mysqlptr[i]); // pointer go back from '\0' to ','
		sprintf(this->mysqlptr[i],
				" on duplicate key update \
				delay_v = delay_v + values(delay_v),\
				delay_c = delay_c + values(delay_c),\
				lost_v = lost_v + values(lost_v),\
				lost_c = lost_c + values(lost_c),\
				hop_v = hop_v + values(hop_v),\
				hop_c = hop_c + values(hop_c) ");

		this->exec_insert_sql( this->mysqlstr[i], USER_ID_EXISTED_ERR );
		this->mysqlptr[i] = this->mysqlstr[i];
	}
	this->mycount = 0;

}	

int	Clink_stat::insert( net_stat_link_report_in *p_in )
{
    DEBUG_LOG("link_stat_report:%u logtime:%u", p_in->link, p_in->logtime);
    link_stat_t lls;
	lls.id = p_in->link;
	lls.logtime = p_in->logtime;
	lls.d_v = p_in->delay_v;
	lls.d_c = p_in->count;
	lls.l_v = p_in->lost_v;
	lls.l_c = p_in->count;
	lls.h_v = p_in->hop_v;
	lls.h_c = p_in->count;
	insert(&lls);

	return 0;
}

int	Clink_stat::insert( net_stat_report_link_delay_in *p_in )
{
    DEBUG_LOG("link:%u logtime:%u", p_in->link, p_in->logtime);
	link_stat_t lls;
	memset(&lls, 0, sizeof(link_stat_t));
	lls.id = p_in->link;
	lls.logtime = p_in->logtime;
	lls.d_v = p_in->value;
	lls.d_c = p_in->count;
	insert(&lls);

	return 0;
}



int	Clink_stat::insert( net_stat_report_link_lost_in *p_in )
{
    DEBUG_LOG("link:%u logtime:%u", p_in->link, p_in->logtime);
	link_stat_t lls;
	memset(&lls, 0, sizeof(link_stat_t));
	lls.id = p_in->link;
	lls.logtime = p_in->logtime;
	lls.l_v = p_in->value;
	lls.l_c = p_in->count;
	insert(&lls);

	return 0;
}


int	Clink_stat::insert( net_stat_report_link_hop_in *p_in )
{
    DEBUG_LOG("link:%u logtime:%u", p_in->link, p_in->logtime);
	link_stat_t lls;
	memset(&lls, 0, sizeof(link_stat_t));
	lls.id = p_in->link;
	lls.logtime = p_in->logtime;
	lls.h_v = p_in->value;
	lls.h_c = p_in->count;
	insert(&lls);

	return 0;
}


int	Clink_stat::insert( net_stat_report_idc_delay_in *p_in )
{
//    DEBUG_LOG("idc:%u logtime:%u", p_in->idc, p_in->logtime);

	return 0;
}


int	Clink_stat::insert( net_stat_report_idc_lost_in *p_in )
{
//	DEBUG_LOG("idc:%u logtime:%u", p_in->idc, p_in->logtime);

	return 0;
}


int	Clink_stat::insert( net_stat_report_idc_hop_in *p_in )
{
//	DEBUG_LOG("idc:%u logtime:%u", p_in->idc, p_in->logtime);

	return 0;
}


int	Clink_stat::insert( net_stat_report_all_idc_delay_in *p_in )
{
//	DEBUG_LOG("all-idc-delay logtime:%u", p_in->logtime);
	
	return 0;
}


int	Clink_stat::insert( net_stat_report_all_idc_lost_in *p_in )
{
//	DEBUG_LOG("all-idc-delay logtime:%u", p_in->logtime);
	
	return 0;
}


int	Clink_stat::insert( net_stat_report_all_idc_hop_in *p_in )
{
//	DEBUG_LOG("all-idc-delay logtime:%u", p_in->logtime);
	
	return 0;
}


