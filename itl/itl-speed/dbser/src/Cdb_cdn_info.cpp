/** 
 * ========================================================================
 * @file Cdb_cdn_info.cpp
 * @brief 
 * @author elvacheng
 * @version 1.0.0
 * @date 2012-09-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include  "Cdb_cdn_info.h"
#define MAX_STR_BUF_LEN    1024*20
#define MAX_SPEED_TYPE     32

Cdb_cdn_info::Cdb_cdn_info(mysql_interface * db)
   :CtableRoute(db, "xxxx" , "xxx" , "xx")
{
    int i,j,k;
    for( k = 0; k < MAX_CDNPRJ_DB; k++ )
    {
        for( i = 0; i < 4; i++ )
        {
            this->str_node_list[k][i] = (char *)malloc(MAX_STR_BUF_LEN);
            this->ptr_node_list[k][i] = str_node_list[k][i];
            this->cnt_node_list[k] = 0;
        }

        for( j = 0; j < MAX_DT_NOTE_TB; j++ )
        {
            for( i = 0; i < 4; i++ )
            {
                this->str_node_avg[k][j][i] = (char *)malloc(MAX_STR_BUF_LEN);
                this->ptr_node_avg[k][j][i] = this->str_node_avg[k][j][i];
                this->cnt_node_avg[k][j][i] = 0;

                this->str_node_visit[k][j][i] = (char *)malloc(MAX_STR_BUF_LEN);
                this->ptr_node_visit[k][j][i] = this->str_node_visit[k][j][i];
                this->cnt_node_visit[k][j][i] = 0;

                this->str_ip_detail[k][j][i] = (char *)malloc(MAX_STR_BUF_LEN);
                this->ptr_ip_detail[k][j][i] = this->str_ip_detail[k][j][i];
                this->cnt_ip_detail[k][j] = 0;
            }
        }

        for( j = 0; j < MAX_DT_PROV_TB; j++ )
       {
           for( i = 0; i<4; i++ )
           { 
               this->str_prov[k][j][i] = (char *)malloc(MAX_STR_BUF_LEN);
               this->ptr_prov[k][j][i] = this->str_prov[k][j][i];
               this->cnt_prov[k][j][i] = 0;
           }
       }

        this->str_stat[k] = (char *)malloc(MAX_STR_BUF_LEN);
        this->ptr_stat[k] = this->str_stat[k];
        this->cnt_stat[k] = 0;

    }
}

Cdb_cdn_info::~Cdb_cdn_info()
{
    int i, j, k;
    
    for( k = 0; k < MAX_CDNPRJ_DB; k++ )
    {
        if(this->cnt_node_list[k] > 0)
        {
            write_node_list_tb_now(k);
        }
        free(this->str_node_list);

        for( j = 0; j < MAX_DT_NOTE_TB; j++ )
        {
           // 缓存中的语句先执行
           if( this->cnt_node_avg[k][j][0] > 0 ){
               write_node_avg_tb_now(k, j);
           }

           if( this->cnt_node_visit[k][j][0] > 0 ){
               write_node_visit_tb_now(k, j);
           }

           if( this->cnt_ip_detail[k][j]> 0 ){
               write_ip_detail_tb_now(k, j);
           }

           for( i = 0; i < 4; i++ )
           {
               free(this->str_node_avg[k][j][i]); 
               free(this->str_node_visit[k][j][i]);
               free(this->str_ip_detail[k][j][i]);
           }
       }
        
       for( j = 0; j < MAX_DT_PROV_TB; j++ )
       {
           if( this->cnt_prov[k][j][0] > 0 ){
               write_prov_tb_now(k, j);
           }

           for( i = 0; i<4; i++ )
           { 
             free(this->str_prov[k][j][i]);
           }
       } 

       if(this->cnt_stat[k] > 0){
           write_stat_tb_now(k);
       }
       free(this->str_stat[k]);
    }
    
}

int Cdb_cdn_info::insert_tables(cdnrate_ip_isp_in *p_in)
{
    int ret = 0; 

    ret = insert_node_list_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_node_list_tb return %d", ret);
        return ret;
    }

    ret = insert_node_avg_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_node_avg_tb return %d", ret);
        return ret;
    }

    ret = insert_node_visit_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_node_visit_tb return %d", ret);
        return ret;
    }

    ret = insert_ip_detail_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_ip_detail_tb return %d", ret);
        return ret;
    }
    ret = insert_prov_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_prov_tb return %d", ret);
    }

    ret = insert_stat_tb(p_in);
    if(ret !=0 ) {
        ERROR_LOG("insert_stat_tb return %d", ret);
    }
    return ret;
}

int Cdb_cdn_info::insert_node_list_tb( cdnrate_ip_isp_in *p_in )
{
    int min = p_in->logtime / 60;
    int db_idx = p_in->prjid;
    int node = p_in->cdnip >> 8;

    int seq[4];
    seq[0] = min;
    seq[1] = min/ MIN10;
    seq[2] = min/ HOUR;
    seq[3] = min/ DAY;
    
    if(db_idx > MAX_CDNPRJ_DB){
        ERROR_RETURN(("invalid db idx[%u]", db_idx), 0);
    }

    DEBUG_LOG("ELVA:insert_node_list_tb prjid:%u ip:%u node:%u logtime:%u  cnt_node_list:%u",
            p_in->prjid, p_in->cdnip, node, p_in->logtime, cnt_node_list[db_idx]);

    if(this->cnt_node_list[db_idx]== 0) {

        for(int i = 0; i < 4; i++) {
            sprintf( this->str_node_list[db_idx][i],
                    "insert into db_cdn_info_%02u.t_node_list_lv%u \
                    (ip,node,seq) values ",
                    p_in->prjid, i);

            //ptr point to the last character '\n'
            this->ptr_node_list[db_idx][i] = 
                this->str_node_list[db_idx][i] + strlen(this->str_node_list[db_idx][i]);
        }
    }

    char s_item[128];
    sprintf( s_item, "(%u, %u, %u)", p_in->cdnip, node, seq[0] );
    if( strstr(this->ptr_node_list[db_idx][0],s_item) == NULL){// 去除重复的项
        for(int i = 0 ; i < 4; i++) {
            sprintf( this->ptr_node_list[db_idx][i], "(%u, %u, %u),",
                    p_in->cdnip, node,seq[i]);
            this->ptr_node_list[db_idx][i] += strlen(this->ptr_node_list[db_idx][i]);
        }

        this->cnt_node_list[db_idx] ++;
        if(this->cnt_node_list[db_idx] == CDN_SEND_COUNT)  {
            write_node_list_tb_now(db_idx);
        }
    }
    
    return 0;	
}

int Cdb_cdn_info::write_node_list_tb_now( int di )
{
    for(int i = 0; i < 4; i++)
    {
        -- (this->ptr_node_list[di][i]); // pointer go back from '\0' to ','
        sprintf(this->ptr_node_list[di][i],
                "on duplicate key update node=node");

        // DEBUG_LOG("ELVA write_node_list_tb_now:%s", this->str_node_list[di][i]);
        this->exec_insert_sql( this->str_node_list[di][i], USER_ID_EXISTED_ERR );
        this->ptr_node_list[di][i] = this->str_node_list[di][i];
    }

    this->cnt_node_list[di] = 0;
    return 0;
}


int Cdb_cdn_info::insert_node_avg_tb( cdnrate_ip_isp_in *p_in )
{
    int min = p_in->logtime / 60;

    int db_idx = p_in->prjid;
    int node = p_in->cdnip >> 8;
    int idx = node % MAX_DT_NOTE_TB;
    int seq[4];
    seq[0] = min;
    seq[1] = min/ MIN10;
    seq[2] = min/ HOUR;
    seq[3] = min/ DAY;

    uint32_t  speed_cnt[MAX_SPEED_TYPE] = { 0 };
    int index = 0;

    if(p_in->value == (uint32_t)(-1)) {// 0KB
        //2012-11-15 modify by tonyliu
        return 0;//不用入库
        //p_in->value = 0;
        //index = 0;
    }
    else if( p_in->value > 30*(10*1024)){ // 300 KB 
        index = 31;
    }
    else {
        index = p_in->value / (1024*10);
    }

    speed_cnt[index] = 1;

    if (db_idx > MAX_CDNPRJ_DB)
        ERROR_RETURN(("invalid db_idx[%u]", db_idx), 0);

    DEBUG_LOG("ELVA:insert_node_avg_tb prjid:%u ip:%u logtime:%u idx:%u cnt:%u",
            p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->cnt_node_avg[db_idx][idx][0]); 

    if( cnt_node_avg[db_idx][idx][0] == 0 ){
        for( int i = 0; i < 4; i++ ){
            sprintf( this->str_node_avg[db_idx][idx][i],
                    "insert into db_cdn_info_%02u.t_node%02u_avg_lv%u (node, seq, value_sum, count_sum, speed0_count,speed1_count,speed2_count, speed3_count, speed4_count, speed5_count, speed6_count,speed7_count,speed8_count, speed9_count, speed10_count,speed11_count,speed12_count,speed13_count,speed14_count,speed15_count,speed16_count,speed17_count,speed18_count,speed19_count,speed20_count,speed21_count,speed22_count,speed23_count,speed24_count,speed25_count,speed26_count,speed27_count,speed28_count,speed29_count,speed30_count, speed_over30_count) values ",
                    p_in->prjid, idx, i);

            //ptr point to the last character '\n'
            this->ptr_node_avg[db_idx][idx][i] = 
                this->str_node_avg[db_idx][idx][i] + strlen(this->str_node_avg[db_idx][idx][i]);

        }
    }

    // sprintf 参数最多30个 
    for(int i = 0; i < 4; i++)
    {
        sprintf( this->ptr_node_avg[db_idx][idx][i], "(%u, %u, %u, %u",
                node, seq[i], p_in->value, p_in->count );

        this->ptr_node_avg[db_idx][idx][i] += strlen(this->ptr_node_avg[db_idx][idx][i]);


        for(int k = 0; k < 4; k++ ) {
            int index = k * 8;
            sprintf( this->ptr_node_avg[db_idx][idx][i], 
                    ", %u, %u, %u, %u, %u, %u, %u, %u",
                    speed_cnt[index], speed_cnt[index + 1], speed_cnt[index + 2], speed_cnt[index + 3],
                    speed_cnt[index + 4],speed_cnt[index + 5], speed_cnt[index + 6],speed_cnt[index + 7] );
            this->ptr_node_avg[db_idx][idx][i] += strlen(this->ptr_node_avg[db_idx][idx][i]);
        }

        sprintf( this->ptr_node_avg[db_idx][idx][i], "%s", "),");
        this->ptr_node_avg[db_idx][idx][i] += strlen(this->ptr_node_avg[db_idx][idx][i]);
        this->cnt_node_avg[db_idx][idx][i] ++;
    }

    if( cnt_node_avg[db_idx][idx][0] == CDN_SEND_COUNT ){

        write_node_avg_tb_now(db_idx, idx);
    }

    return 0;
}

int Cdb_cdn_info::write_node_avg_tb_now( int di, int ti )
{
    for(int i = 0; i < 4; i++)
    {
        --(this->ptr_node_avg[di][ti][i]); // pointer go back from '\0' to ','
        sprintf(this->ptr_node_avg[di][ti][i],
                "on duplicate key update value_sum =  value_sum + values(value_sum), \
                count_sum = count_sum + values(count_sum), ");
        this->ptr_node_avg[di][ti][i] += strlen(this->ptr_node_avg[di][ti][i]);
        for(int k = 0; k < MAX_SPEED_TYPE - 1; k++ )
        {
            sprintf(this->ptr_node_avg[di][ti][i],"speed%d_count = speed%d_count + values(speed%d_count),",
                    k, k , k);
            this->ptr_node_avg[di][ti][i] += strlen(this->ptr_node_avg[di][ti][i]);
        }

        sprintf(this->ptr_node_avg[di][ti][i],"%s","speed_over30_count = speed_over30_count + values(speed_over30_count)");

        DEBUG_LOG("ELVA: write_node_avg_tb_now:%ld", strlen(this->str_node_avg[di][ti][i]));
        //DEBUG_LOG("ELVA: write_node_avg_tb_now:%s", this->str_node_avg[di][ti][i]);
        this->exec_insert_sql( this->str_node_avg[di][ti][i], USER_ID_EXISTED_ERR );

        this->ptr_node_avg[di][ti][i] = this->str_node_avg[di][ti][i];
        this->cnt_node_avg[di][ti][i] = 0;
    }

    return 0;
}


int Cdb_cdn_info::insert_node_visit_tb( cdnrate_ip_isp_in *p_in )
{
    int min = p_in->logtime / 60;

    int db_idx = p_in->prjid;
    int node = p_in->cdnip >> 8;
    int idx = node % MAX_DT_NOTE_TB;
    int seq[4];
    seq[0] = min;
    seq[1] = min/ MIN10;
    seq[2] = min/ HOUR;
    seq[3] = min/ DAY;

    int  speed_cnt[MAX_SPEED_TYPE] = {0 };
    int index = 0;

    if(p_in->value == (uint32_t)(-1)) {// 0KB
        //2012-11-15 modify by tonyliu
        return 0;//不用入库
        //index = 0;
        //p_in->value = 0;
    }
    else if( p_in->value > 30*(10*1024)){ // 300 KB 
        index = 31;
    }
    else
    {
        index = p_in->value / (1024*10);
    }

    speed_cnt[index] = 1;

    if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_NOTE_TB)
        ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("ELVA insert_node_visit_tb  prjid:%u ip:%u logtime:%u idx:%u cnt:%u",
            p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->cnt_node_avg[db_idx][idx][0]); 

    if( cnt_node_visit[db_idx][idx][0] == 0 ){
        for( int i = 0; i < 4; i++ ){

            sprintf( this->str_node_visit[db_idx][idx][i],
                    "insert into db_cdn_info_%02u.t_node%02u_visit_lv%u (node, prov_id, seq,comp,  value_sum, count_sum, speed0_count,speed1_count,speed2_count, speed3_count, speed4_count, speed5_count,speed6_count,speed7_count,speed8_count, speed9_count, speed10_count,speed11_count,speed12_count,speed13_count,speed14_count,speed15_count,speed16_count,speed17_count,speed18_count,speed19_count,speed20_count,speed21_count,speed22_count,speed23_count,speed24_count,speed25_count,speed26_count,speed27_count,speed28_count,speed29_count,speed30_count, speed_over30_count) values ",
                    p_in->prjid, idx, i);

            //ptr point to the last character '\n'
            this->ptr_node_visit[db_idx][idx][i] = 
                this->str_node_visit[db_idx][idx][i] + strlen(this->str_node_visit[db_idx][idx][i]);

        }
    }


    // srintf 参数最多30个 
    for(int i = 0; i < 4; i++) {

        sprintf( this->ptr_node_visit[db_idx][idx][i], "(%u, %u, %u, '%s',%u, %u",
                node, p_in->provid, seq[i], p_in->comp, p_in->value, p_in->count);
        this->ptr_node_visit[db_idx][idx][i] += strlen(this->ptr_node_visit[db_idx][idx][i]);

        for(int k = 0; k < 4; k++ ) {
            int index = k * 8;
            sprintf( this->ptr_node_visit[db_idx][idx][i], 
                    ", %u, %u, %u, %u, %u, %u, %u, %u",
                    speed_cnt[index], speed_cnt[index+1], speed_cnt[index + 2], speed_cnt[index + 3],
                    speed_cnt[index + 4],speed_cnt[index + 5], speed_cnt[index + 6],speed_cnt[index + 7] );
            this->ptr_node_visit[db_idx][idx][i] += strlen(this->ptr_node_visit[db_idx][idx][i]);
        }

        sprintf( this->ptr_node_visit[db_idx][idx][i], "%s", "),");
        this->ptr_node_visit[db_idx][idx][i] += strlen(this->ptr_node_visit[db_idx][idx][i]);
        this->cnt_node_visit[db_idx][idx][i] ++;
    }

    if( cnt_node_visit[db_idx][idx][0] == CDN_SEND_COUNT ){

        write_node_visit_tb_now(db_idx, idx);
    }

    return 0;
}


int Cdb_cdn_info::write_node_visit_tb_now( int di, int ti )
{ 
    for(int i = 0; i < 4; i++)
    {
        --(this->ptr_node_visit[di][ti][i]); // pointer go back from '\0' to ','
        sprintf(this->ptr_node_visit[di][ti][i],
                "on duplicate key update value_sum =  value_sum + values(value_sum), \
                count_sum = count_sum + values(count_sum), ");
        this->ptr_node_visit[di][ti][i] += strlen(this->ptr_node_visit[di][ti][i]);
        for(int k = 0; k < MAX_SPEED_TYPE - 1; k++ )
        {
            sprintf(this->ptr_node_visit[di][ti][i],"speed%d_count = speed%d_count + values(speed%d_count),",
                    k, k , k);
            this->ptr_node_visit[di][ti][i] += strlen(this->ptr_node_visit[di][ti][i]);
        }

        sprintf(this->ptr_node_visit[di][ti][i],"%s",
                "speed_over30_count = speed_over30_count + values(speed_over30_count)");

        DEBUG_LOG("ELVA write_node_visit_tb_now:%ld", strlen(this->str_node_visit[di][ti][i]));
        this->exec_insert_sql( this->str_node_visit[di][ti][i], USER_ID_EXISTED_ERR );

        this->ptr_node_visit[di][ti][i] = this->str_node_visit[di][ti][i];
        this->cnt_node_visit[di][ti][i] = 0;
    }

    return 0;
}


int Cdb_cdn_info::insert_ip_detail_tb( cdnrate_ip_isp_in *p_in )
{
    int min = p_in->logtime / 60;

    int db_idx = p_in->prjid;
    int node = p_in->cdnip >> 8;
    int idx = node % MAX_DT_NOTE_TB;
    int seq[4];
    seq[0] = min;
    seq[1] = min/ MIN10;
    seq[2] = min/ HOUR;
    seq[3] = min/ DAY;

    int  speed_cnt[MAX_SPEED_TYPE] = { 0 };
    int index = 0;

    if(p_in->value == (uint32_t)(-1)) {// 0KB
        //2012-11-15 modify by tonyliu
        return 0;//不用入库
        //p_in->value = 0;
        //index = 0;
    }
    else if( p_in->value > 30*(10*1024)){ // 300 KB 
        index = 31;
    }
    else
    {
        index = p_in->value / (1024*10);
    }

    speed_cnt[index] = 1;

    if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_NOTE_TB)
        ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("ELVA insert_ip_detail_tb prjid:%u ip:%u logtime:%u idx:%u cnt:%u",
            p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->cnt_ip_detail[db_idx][idx]); 

    if( cnt_ip_detail[db_idx][idx] == 0 ){
        for(int i = 0; i < 4; i++ ){
            sprintf( this->str_ip_detail[db_idx][idx][i],
                    "insert into db_cdn_info_%02u.t_ip%02u_detail_lv%u (ip, seq, value_sum, count_sum, speed0_count,speed1_count,speed2_count, speed3_count, speed4_count, speed5_count, speed6_count,speed7_count,speed8_count, speed9_count, speed10_count,speed11_count,speed12_count, speed13_count,speed14_count,speed15_count,speed16_count,speed17_count,speed18_count,speed19_count,speed20_count,speed21_count,speed22_count,speed23_count,speed24_count,speed25_count,speed26_count, speed27_count,speed28_count,speed29_count,speed30_count, speed_over30_count) values ",
                    p_in->prjid, idx, i);

            //DEBUG_LOG("t_ip_detail_lv%d:%s",i, this->str_ip_detail[db_idx][idx][i]);
            //ptr point to the last character '\n'
            this->ptr_ip_detail[db_idx][idx][i] = 
                this->str_ip_detail[db_idx][idx][i] + strlen(this->str_ip_detail[db_idx][idx][i]);

        }
    }

    // srintf 参数最多30个 
    for(int i = 0; i < 4; i++)
    {
        sprintf( this->ptr_ip_detail[db_idx][idx][i], "(%u, %u, %u, %u",
                p_in->cdnip, seq[i], p_in->value, p_in->count);
        this->ptr_ip_detail[db_idx][idx][i] += strlen(this->ptr_ip_detail[db_idx][idx][i]);

        for(int k = 0; k < 4; k++ ) {
            int index = k * 8;
            sprintf( this->ptr_ip_detail[db_idx][idx][i], 
                    ", %u, %u, %u, %u, %u, %u, %u, %u",
                    speed_cnt[index], speed_cnt[index + 1], speed_cnt[index + 2], speed_cnt[index + 3],
                    speed_cnt[index + 4],speed_cnt[index + 5], speed_cnt[index + 6],speed_cnt[index + 7] );
            this->ptr_ip_detail[db_idx][idx][i] += strlen(this->ptr_ip_detail[db_idx][idx][i]);
        }

        sprintf( this->ptr_ip_detail[db_idx][idx][i], "%s", "),");
        this->ptr_ip_detail[db_idx][idx][i] += strlen(this->ptr_ip_detail[db_idx][idx][i]);

    }

    this->cnt_ip_detail[db_idx][idx] ++;
    if( cnt_ip_detail[db_idx][idx] == CDN_SEND_COUNT){

        write_ip_detail_tb_now(db_idx, idx);
    }


    return 0;
}


int Cdb_cdn_info::write_ip_detail_tb_now( int di, int ti )
{
    for(int i = 0; i < 4; i++)
    {
        --(this->ptr_ip_detail[di][ti][i]); // pointer go back from '\0' to ','
        sprintf(this->ptr_ip_detail[di][ti][i],
                "on duplicate key update value_sum =  value_sum + values(value_sum), \
                count_sum = count_sum + values(count_sum), ");
        this->ptr_ip_detail[di][ti][i] += strlen(this->ptr_ip_detail[di][ti][i]);
        for(int k = 0; k < MAX_SPEED_TYPE - 1; k++ )
        {
            sprintf(this->ptr_ip_detail[di][ti][i],"speed%d_count = speed%d_count + values(speed%d_count),",
                    k, k , k);
            this->ptr_ip_detail[di][ti][i] += strlen(this->ptr_ip_detail[di][ti][i]);
        }

        sprintf(this->ptr_ip_detail[di][ti][i],"%s",
                "speed_over30_count = speed_over30_count + values(speed_over30_count)");

        DEBUG_LOG("ELVA write_ip_detail_tb_now :%ld", strlen(this->str_ip_detail[di][ti][i]));
        //DEBUG_LOG("ELVA write_ip_detail_tb_now :%s", this->str_ip_detail[di][ti][i]);
        this->exec_insert_sql( this->str_ip_detail[di][ti][i], USER_ID_EXISTED_ERR );

        this->ptr_ip_detail[di][ti][i] = this->str_ip_detail[di][ti][i];
        this->cnt_ip_detail[di][ti] = 0;
    }
    return 0;
}


int Cdb_cdn_info::insert_prov_tb( cdnrate_ip_isp_in *p_in )
{   
    int min = p_in->logtime / 60;
    int db_idx = p_in->prjid;
    int idx = p_in->provid / 10000;
    int seq[4];
    seq[0] = min;
    seq[1] = min/ MIN10;
    seq[2] = min/ HOUR;
    seq[3] = min/ DAY;

    int  speed_cnt[MAX_SPEED_TYPE] = {0 };
    int index = 0;

    if(p_in->value == (uint32_t)(-1)) {// 0KB
        //2012-11-15 modify by tonyliu
        return 0;//不用入库
        //index = 0;
        //p_in->value = 0;
    }
    else if( p_in->value > 30*(10*1024)){ // 300 KB 
        index = 31;
    }
    else
    {
        index = p_in->value / (1024*10);
    }

    speed_cnt[index] = 1;

    if (db_idx > MAX_CDNPRJ_DB || idx > MAX_DT_NOTE_TB)
        ERROR_RETURN(("invalid db idx or tb idx\t[%u %u]", db_idx, idx), 0);

    DEBUG_LOG("ELVA insert_prov_tb  prjid:%u ip:%u logtime:%u idx:%u cnt:%u",
            p_in->prjid, p_in->cdnip, p_in->logtime, idx, this->cnt_prov[db_idx][idx][0]); 

    if( cnt_prov[db_idx][idx][0] == 0 ){
        for( int i = 0; i < 4; i++ ){
            sprintf( this->str_prov[db_idx][idx][i],
                    "insert into db_cdn_info_%02u.t_prov%06u_lv%u (seq, comp, value_sum, count_sum, speed0_count,speed1_count,speed2_count, speed3_count, speed4_count, speed5_count, speed6_count,speed7_count,speed8_count, speed9_count, speed10_count,speed11_count,speed12_count, speed13_count,speed14_count,speed15_count,speed16_count,speed17_count,speed18_count,speed19_count,speed20_count,speed21_count,speed22_count, speed23_count,speed24_count,speed25_count,speed26_count,speed27_count,speed28_count,speed29_count,speed30_count, speed_over30_count) values ",
                    p_in->prjid, p_in->provid, i);

            //ptr point to the last character '\n'
            this->ptr_prov[db_idx][idx][i] = 
                this->str_prov[db_idx][idx][i] + strlen(this->str_prov[db_idx][idx][i]);

        }
    }

    // srintf 参数最多30个 
    for(int i = 0; i < 4; i++)
    {
        sprintf( this->ptr_prov[db_idx][idx][i], "(%u, '%s',%u, %u",
                seq[i], p_in->comp, p_in->value, p_in->count);
        
        this->ptr_prov[db_idx][idx][i] += strlen(this->ptr_prov[db_idx][idx][i]);
        for(int k = 0; k < 4; k++ ) {
            int index = k * 8;
            sprintf( this->ptr_prov[db_idx][idx][i], 
                    ", %u, %u, %u, %u, %u, %u, %u, %u",
                    speed_cnt[index], speed_cnt[index + 1], speed_cnt[index + 2], speed_cnt[index + 3],
                    speed_cnt[index + 4],speed_cnt[index + 5], speed_cnt[index + 6],speed_cnt[index + 7] );

            this->ptr_prov[db_idx][idx][i] += strlen(this->ptr_prov[db_idx][idx][i]);
        }

        sprintf( this->ptr_prov[db_idx][idx][i], "%s", "),");
        this->ptr_prov[db_idx][idx][i] += strlen(this->ptr_prov[db_idx][idx][i]);
        this->cnt_prov[db_idx][idx][i] ++;
    }



    if( cnt_prov[db_idx][idx][0] == CDN_SEND_COUNT ){

        write_prov_tb_now(db_idx, idx);
    }



    return 0;
}


int Cdb_cdn_info::write_prov_tb_now( int di, int ti )
{
    for(int i = 0; i < 4; i++)
    {
        if(i==0)
            DEBUG_LOG("ELVA write_prov_tb_now:len[%ld], %s",strlen(this->str_prov[di][ti][i]), this->str_prov[di][ti][i]);
       
        -- (this->ptr_prov[di][ti][i]); // pointer go back from '\0' to ','
        sprintf(this->ptr_prov[di][ti][i], "on duplicate key update value_sum =  value_sum + values(value_sum), \
                count_sum = count_sum + values(count_sum), ");
        this->ptr_prov[di][ti][i] += strlen(this->ptr_prov[di][ti][i]);

        for(int k = 0; k < MAX_SPEED_TYPE - 1; k++ )
        {
            sprintf(this->ptr_prov[di][ti][i],"speed%d_count = speed%d_count + values(speed%d_count),",
                    k, k , k);
            this->ptr_prov[di][ti][i] += strlen(this->ptr_prov[di][ti][i]);
        }

        sprintf(this->ptr_prov[di][ti][i],"%s",
                "speed_over30_count = speed_over30_count + values(speed_over30_count)");

        DEBUG_LOG("ELVA write_prov_tb_now:len[%ld]",strlen(this->str_prov[di][ti][i]));
        this->exec_insert_sql( this->str_prov[di][ti][i], USER_ID_EXISTED_ERR );

        this->ptr_prov[di][ti][i] = this->str_prov[di][ti][i];
        this->cnt_prov[di][ti][i] = 0;
    }

    return 0;
}

int Cdb_cdn_info::insert_stat_tb(cdnrate_ip_isp_in *p_in)
{
    int seq = p_in->logtime - (p_in->logtime % 86400);//按日的起始时间
    int db_idx = p_in->prjid;
    int speed_tag = (p_in->value == (uint32_t)(-1)) ?-1 :p_in->value/(1024*10); //every 10kB


    if (db_idx > MAX_CDNPRJ_DB)
        ERROR_RETURN(("invalid db idx\t[%u]", db_idx), 0);

    DEBUG_LOG("ELVA:insert_stat_tb-- seq[%d] db_index[%d] speed_tag[%d]", seq, db_idx, speed_tag);
    if(this->cnt_stat[db_idx] == 0) {
        sprintf( this->str_stat[db_idx], "insert into db_cdn_info_%02u.t_stat (seq, speed_tag, count) values ", db_idx);
        //ptr point to the last character '\n'
        this->ptr_stat[db_idx] = this->str_stat[db_idx] + strlen(this->str_stat[db_idx]);
    }

    sprintf(this->ptr_stat[db_idx], "(%u, %d, %u),", seq, speed_tag, p_in->count);
    this->ptr_stat[db_idx] += strlen(this->ptr_stat[db_idx]);
    this->cnt_stat[db_idx] ++;

    if(this->cnt_stat[db_idx] == CDN_SEND_COUNT) {
        write_stat_tb_now(db_idx);
    }

    return 0;
}

int Cdb_cdn_info::write_stat_tb_now(int di)
{
    if (this->cnt_stat[di] != 0) {
        -- (this->ptr_stat[di]); // pointer go back from '\0' to ','
        sprintf(this->ptr_stat[di], "on duplicate key update count = count + values(count)");

        this->exec_insert_sql( this->str_stat[di], USER_ID_EXISTED_ERR );
        this->ptr_stat[di] = this->str_stat[di];
        this->cnt_stat[di] = 0;
    }

    return 0;  
}
