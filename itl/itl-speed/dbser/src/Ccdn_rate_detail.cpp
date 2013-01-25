
/*
 * =========================================================================
 *
 *        Filename: Ccdn_rate_detail.cpp
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

#include "Ccdn_rate_detail.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

Ccdn_rate_detail::Ccdn_rate_detail(mysql_interface * db ) 
	:CtableRoute(db, "xxxx" , "xxx" , "xx")
{
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_DT_CDNIP_TB; i++) {
			for(int j = 0; j < 4 ;j++) {
				this->cdnipstr[k][i][j] = (char *)malloc(1024*16);
				this->cdnipptr[k][i][j] = this->cdnipstr[k][i][j];

                this->speed_dis_ipstr[k][i][j] = (char *)malloc(1024*16);
                this->speed_dis_ipptr[k][i][j] = this->speed_dis_ipstr[k][i][j];
                this->speed_dis_ipcnt[k][i][j] = 0;

			}

			this->cdnipcnt[k][i] = 0;
            //this->speed_dis_ipcnt[k][i] = 0;
		}
		for(int i = 0; i < MAX_DT_PROV_TB; i++) {
			for(int j = 0; j < 4 ;j++) {
				this->provstr[k][i][j] = (char *)malloc(1024*16);
				this->provptr[k][i][j] = this->provstr[k][i][j];

                this->speed_dis_provstr[k][i][j] = (char *)malloc(1024*16);
                this->speed_dis_provptr[k][i][j] = this->speed_dis_provstr[k][i][j];
                this->speed_dis_provcnt[k][i][j] = 0;
			}

			this->provcnt[k][i] = 0;
            //this->speed_dis_provcnt[k][i] = 0;
		}
        this->stat_str[k] = (char*)malloc(1024*16);
        this->stat_ptr[k] = this->stat_str[k];
        this->stat_cnt[k] = 0;
	}
	
}

Ccdn_rate_detail::~Ccdn_rate_detail() 
{
	DEBUG_LOG("WRITE DB---cdn_rate_detail");
	for(int k = 0; k < MAX_CDNPRJ_DB; k++) {
		for(int i = 0; i < MAX_DT_CDNIP_TB; i++) {
			if (this->cdnipcnt[k][i]) {
				write_cdnip_tb_now(k, i);
			}
            if (this->speed_dis_ipcnt[k][i][0]) {
                write_speed_dis_ip_tb_now(k, i);
            }
			for(int j = 0; j < 4 ;j++) {
				free(this->cdnipstr[k][i][j]);
                free(this->speed_dis_ipstr[k][i][j]);
			}
		}
		for(int i = 0; i < MAX_DT_PROV_TB; i++) {
			if (this->provcnt[k][i]) {
				write_area_tb_now(k, i);
			}
            if (this->speed_dis_provcnt[k][i][0]){
                write_speed_dis_area_tb_now(k, i);
            }
			for(int j = 0; j < 4 ;j++) {
				free(this->provstr[k][i][j]);
                free(this->speed_dis_provstr[k][i][j]);
			}
		}

        if (this->stat_cnt) {
            write_stat_tb_now(k);
        }
        free(this->stat_str[k]);
	}
	
}
	
int	Ccdn_rate_detail::insert_cdnip_tb( cdnrate_ip_isp_in *p_in )
{
	
    int min = p_in->logtime / 60;

	int db_idx = p_in->prjid;
	int ip3 = p_in->cdnip >> 8;
	int idx = ip3 % MAX_DT_CDNIP_TB;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;

    if (p_in->value == (uint32_t)(-1))
        return 0;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_CDNIP_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("detail_cdnip prjid:%u ip:%u logtime:%u idx:%u cdnipcnt:%u",
			p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->cdnipcnt[db_idx][idx]);
	if(this->cdnipcnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->cdnipstr[db_idx][idx][i],
			"insert into cdn_rate_detail_%02u.t_cdnrate_cdnip%02u_lv%u \
			(ip,node,seq,provid,cityid,comp,value,count) values ",
			p_in->prjid, idx, i);

			//ptr point to the last character '\n'
			this->cdnipptr[db_idx][idx][i] = 
				this->cdnipstr[db_idx][idx][i] + strlen(this->cdnipstr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->cdnipptr[db_idx][idx][i], "(%u, %u, %u, %u, %u, '%s', %u, %u),",
				p_in->cdnip, ip3,seq[i], p_in->provid, p_in->cityid, 
				p_in->comp, p_in->value, p_in->count);
		this->cdnipptr[db_idx][idx][i] += strlen(this->cdnipptr[db_idx][idx][i]);
	}
	this->cdnipcnt[db_idx][idx] ++;

	if(this->cdnipcnt[db_idx][idx] == SEND_COUNT) {
		write_cdnip_tb_now(db_idx, idx);
	}

	return 0;
}

void Ccdn_rate_detail::write_cdnip_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->cdnipptr[di][ti][i]); // pointer go back from '\0' to ','

		sprintf(this->cdnipptr[di][ti][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");

        //DEBUG_LOG("CDN_RATE_IP:%s", this->cdnipstr[di][ti][i]);
		this->exec_insert_sql( this->cdnipstr[di][ti][i], USER_ID_EXISTED_ERR );
		this->cdnipptr[di][ti][i] = this->cdnipstr[di][ti][i];
	}
	this->cdnipcnt[di][ti] = 0;
}

int	Ccdn_rate_detail::insert_area_tb( cdnrate_ip_isp_in *p_in )
{
	
    int min = p_in->logtime / 60;
	int db_idx = p_in->prjid;
	int idx = p_in->provid / 10000;
    int node = p_in->cdnip >> 8;
	int seq[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;
    
    if (p_in->value == (uint32_t)(-1))
        return 0;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_PROV_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("detail_prov prjid:%u ip:%u logtime:%u idx:%u provcnt:%u",
			p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->provcnt[db_idx][idx]);
	if(this->provcnt[db_idx][idx] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->provstr[db_idx][idx][i],
			"insert into cdn_rate_detail_%02u.t_cdnrate_prov%6u_lv%u \
			(cityid,comp,ip,node,seq,value,count) values ",
			p_in->prjid, p_in->provid, i);

			this->provptr[db_idx][idx][i] = 
				this->provstr[db_idx][idx][i] + strlen(this->provstr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
		sprintf( this->provptr[db_idx][idx][i], "(%u, '%s', %u, %u, %u, %u, %u),",
				p_in->cityid, p_in->comp, p_in->cdnip, node, seq[i], p_in->value, p_in->count);
		this->provptr[db_idx][idx][i] += strlen(this->provptr[db_idx][idx][i]);
	}
	this->provcnt[db_idx][idx] ++;

	if(this->provcnt[db_idx][idx] == SEND_COUNT) {
		write_area_tb_now(db_idx, idx);	
	}

	return 0;
}

void Ccdn_rate_detail::write_area_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
		-- (this->provptr[di][ti][i]); // pointer go back from '\0' to ','

		sprintf(this->provptr[di][ti][i],
				"on duplicate key update value = value + values(value), \
				count = count + values(count)");

        //DEBUG_LOG("CDN_RATE_IP:%s", this->provstr[di][ti][i]);
		this->exec_insert_sql( this->provstr[di][ti][i], USER_ID_EXISTED_ERR );
		this->provptr[di][ti][i] = this->provstr[di][ti][i];
	}
	this->provcnt[di][ti] = 0;

}

int	Ccdn_rate_detail::insert_speed_dis_ip_tb( cdnrate_ip_isp_in *p_in )
{
	
    int min = p_in->logtime / 60;

	int db_idx = p_in->prjid;
	int ip3 = p_in->cdnip >> 8;
	int idx = ip3 % MAX_DT_CDNIP_TB;
    int speed_tag = (p_in->value == (uint32_t)(-1)) ?-1 :p_in->value/(1024*10); //every 10kB

	int seq[4];//, mode[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;
    //mode[0] = min % 1;
    //mode[1] = min % MIN10;
    //mode[2] = min % HOUR;
    //mode[3] = min % DAY;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_CDNIP_TB)
		ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("speed_dis_cdnip prjid:%u ip:%u logtime:%u idx:%u speed_dis_ipcnt:%u",
			p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->speed_dis_ipcnt[db_idx][idx][0]);
	if(this->speed_dis_ipcnt[db_idx][idx][0] == 0) {
		for(int i = 0; i < 4; i++) {
			sprintf( this->speed_dis_ipstr[db_idx][idx][i],
			"insert into cdn_rate_speed_dis_%02u.t_cdn_speed_dis_cdnip%02u_lv%u \
			(ip,node,seq,provid,cityid,comp,speed_tag,count) values ",
			p_in->prjid, idx, i);

			//ptr point to the last character '\n'
			this->speed_dis_ipptr[db_idx][idx][i] = 
				this->speed_dis_ipstr[db_idx][idx][i] + strlen(this->speed_dis_ipstr[db_idx][idx][i]);
		}
	}

	for(int i = 0 ; i < 4; i++) {
       // if (mode[i] == 0) {
            sprintf( this->speed_dis_ipptr[db_idx][idx][i], "(%u, %u, %u, %u, %u, '%s', %d, %u),",
                    p_in->cdnip, ip3, seq[i], p_in->provid, p_in->cityid, 
                    p_in->comp, speed_tag, p_in->count);
            this->speed_dis_ipptr[db_idx][idx][i] += strlen(this->speed_dis_ipptr[db_idx][idx][i]);
            this->speed_dis_ipcnt[db_idx][idx][i] ++;
       // }
    }
	//this->speed_dis_ipcnt[db_idx][idx] ++;

	if(this->speed_dis_ipcnt[db_idx][idx][0] == SEND_COUNT) {
		write_speed_dis_ip_tb_now(db_idx, idx);
	}

	return 0;
}

void Ccdn_rate_detail::write_speed_dis_ip_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
        if (this->speed_dis_ipcnt[di][ti][i] != 0) {
            -- (this->speed_dis_ipptr[di][ti][i]); // pointer go back from '\0' to ','

            sprintf(this->speed_dis_ipptr[di][ti][i],
                    "on duplicate key update count = count + values(count)");

   //         DEBUG_LOG("CDN_RATE_IP:%s", this->cdnipstr[di][ti][i]);
            this->exec_insert_sql( this->speed_dis_ipstr[di][ti][i], USER_ID_EXISTED_ERR );
            this->speed_dis_ipptr[di][ti][i] = this->speed_dis_ipstr[di][ti][i];
            this->speed_dis_ipcnt[di][ti][i] = 0;
        }
    }
	//this->speed_dis_ipcnt[di][ti] = 0;
}

int	Ccdn_rate_detail::insert_speed_dis_area_tb( cdnrate_ip_isp_in *p_in )
{
	
    int min = p_in->logtime / 60;
	int db_idx = p_in->prjid;
	int idx = p_in->provid / 10000;
    int node = p_in->cdnip >>8;
    int speed_tag = (p_in->value == (uint32_t)(-1)) ?-1 :p_in->value/(1024*10); //every 10kB
	
    int seq[4];//,mode[4];
	seq[0] = min;
	seq[1] = min/ MIN10;
	seq[2] = min/ HOUR;
	seq[3] = min/ DAY;
    //mode[0] = min % 1;
    //mode[1] = min % MIN10;
    //mode[2] = min % HOUR;
    //mode[3] = min % DAY;

	if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_PROV_TB)
		ERROR_RETURN(("speed dis prov: invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("speed_dis_prov prjid:%u ip:%u logtime:%u idx:%u provcnt:%u",
			p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->speed_dis_provcnt[db_idx][idx][0]);
	if(this->speed_dis_provcnt[db_idx][idx][0] == 0) {
		for(int i = 0; i < 4; i++) {
            sprintf( this->speed_dis_provstr[db_idx][idx][i],
                    "insert into cdn_rate_speed_dis_%02u.t_cdn_speed_dis_prov%6u_lv%u \
                    (ip, node,cityid,seq, comp, speed_tag,count) values ",
                    p_in->prjid, p_in->provid, i);

            this->speed_dis_provptr[db_idx][idx][i] = 
                this->speed_dis_provstr[db_idx][idx][i] + strlen(this->speed_dis_provstr[db_idx][idx][i]);
	    }
    }

	for(int i = 0 ; i < 4; i++) {
        //if (mode[i] == 0) {
            sprintf( this->speed_dis_provptr[db_idx][idx][i], "(%u, %u, %u, %u, '%s', %d, %u),",
                    p_in->cdnip, node, p_in->cityid, seq[i],p_in->comp,speed_tag, p_in->count);
            this->speed_dis_provptr[db_idx][idx][i] += strlen(this->speed_dis_provptr[db_idx][idx][i]);
            this->speed_dis_provcnt[db_idx][idx][i] ++;
       // }
    }
	//this->speed_dis_provcnt[db_idx][idx] ++;

	if(this->speed_dis_provcnt[db_idx][idx][0] == SEND_COUNT) {
		write_speed_dis_area_tb_now(db_idx, idx);	
	}

	return 0;
}

void Ccdn_rate_detail::write_speed_dis_area_tb_now(int di, int ti)
{
	for(int i = 0; i < 4; i++) {
        if (this->speed_dis_provcnt[di][ti][i] != 0) {
            -- (this->speed_dis_provptr[di][ti][i]); // pointer go back from '\0' to ','

            sprintf(this->speed_dis_provptr[di][ti][i],
                    "on duplicate key update count = count + values(count)");

            //DEBUG_LOG("CDN_RATE_IP:%s", this->provstr[di][ti][i]);
            this->exec_insert_sql( this->speed_dis_provstr[di][ti][i], USER_ID_EXISTED_ERR );
            this->speed_dis_provptr[di][ti][i] = this->speed_dis_provstr[di][ti][i];
            this->speed_dis_provcnt[di][ti][i] = 0;
        }
    }
	//this->speed_dis_provcnt[di][ti] = 0;

}

int	Ccdn_rate_detail::insert_stat_tb( cdnrate_ip_isp_in *p_in )
{
	
    int seq = p_in->logtime - (p_in->logtime % 86400);//按日的起始时间
	int db_idx = p_in->prjid;
	int speed_tag = (p_in->value == (uint32_t)(-1)) ?-1 :p_in->value/(1024*10); //every 10kB


    if (db_idx > MAX_CDNPRJ_DB)
        ERROR_RETURN(("invalid db idx\t[%u]", db_idx), 0);

    if(this->stat_cnt[db_idx] == 0) {
        sprintf( this->stat_str[db_idx], "insert into cdn_rate_%02u.t_stat (seq, speed_tag, count) values ", db_idx);
        //ptr point to the last character '\n'
        this->stat_ptr[db_idx] = this->stat_str[db_idx] + strlen(this->stat_str[db_idx]);
    }

    sprintf(this->stat_ptr[db_idx], "(%u, %d, %u),", seq, speed_tag, p_in->count);
    this->stat_ptr[db_idx] += strlen(this->stat_ptr[db_idx]);
    this->stat_cnt[db_idx] ++;

    if(this->stat_cnt[db_idx] == SEND_COUNT) {
        write_stat_tb_now(db_idx);
    }

    return 0;
}

void Ccdn_rate_detail::write_stat_tb_now(int di)
{
    if (this->stat_cnt[di] != 0) {
        -- (this->stat_ptr[di]); // pointer go back from '\0' to ','

        sprintf(this->stat_ptr[di], "on duplicate key update count = count + values(count)");

        this->exec_insert_sql( this->stat_str[di], USER_ID_EXISTED_ERR );
        this->stat_ptr[di] = this->stat_str[di];
        this->stat_cnt[di] = 0;
    }
}

