/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:  
 *
 *		Version:  1.0
 *		Created:  2009年3月02日 16时39分06秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Croute_func.h" 

extern  char * version_str;

#include "./proto/cdn_rate.h"
stru_cmd_item_t g_cmd_map[]={
#include "./proto/cdn_rate_bind.h"
};
#define DEALFUN_COUNT  (int(sizeof(g_cmd_map )/sizeof(g_cmd_map[0])))

stru_cmd_db_name g_cmd_db_name_list[]={
    { 0x1000,{ "" } } ,
    { 0x1200,{ "" } } ,
};


/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Croute_func::Croute_func (mysql_interface * db):Cfunc_route_db_v2(db),
	cdn_rate_ip(db),cdn_rate_prov(db),cdn_rate_detail(db),
	net_stat_ip(db),net_stat_prov(db),link_stat(db),
	url_stat_prov(db),url_stat_user(db), db_cdn_info(db)
{
	DEBUG_LOG("cmdid count:%u",DEALFUN_COUNT );
	this->set_cmd_list(g_cmd_map,DEALFUN_COUNT );

    this->init_cmd_db_map(g_cmd_db_name_list,
            sizeof(g_cmd_db_name_list )/sizeof(g_cmd_db_name_list[0]) );

}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */

/**/
int Croute_func::cdn_report_ip(DEAL_FUNC_ARG)
{
	cdn_report_ip_in* p_in=P_IN;
    return 0;
    // elva 注释及屏蔽 
    // 2012-11-01
    //ret = this->cdn_rate_ip.insert_ip_tb(p_in);
    //if (ret) return ret;

    //ret = this->cdn_rate_ip.insert_node_tb(p_in);

	//return ret;
}

/* ----------------CODE FOR cdn_report_prov  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::cdn_report_prov(DEAL_FUNC_ARG)
{
    cdn_report_prov_in* p_in=P_IN;
    return 0;
    // elva 注释及屏蔽
    // 2012-11-01
    //ret = this->cdn_rate_prov.insert_city_tb(p_in);
    //if (ret) return ret;

    //ret = this->cdn_rate_prov.insert_prov_tb(p_in);

    //return ret;
}


int Croute_func::net_stat_report_ip_delay(DEAL_FUNC_ARG)
{
    net_stat_report_ip_delay_in* p_in=P_IN;

    ret=this->net_stat_ip.insert(p_in);
    return ret;

}

int Croute_func::net_stat_report_prov_delay(DEAL_FUNC_ARG)
{
    net_stat_report_prov_delay_in* p_in=P_IN;


    ret=this->net_stat_prov.insert(p_in);
    return ret;

}


int Croute_func::net_stat_report_ip_lost(DEAL_FUNC_ARG)
{

    net_stat_report_ip_lost_in* p_in=P_IN;

    ret=this->net_stat_ip.insert(p_in);
    return ret;

}

int Croute_func::net_stat_report_prov_lost(DEAL_FUNC_ARG)
{

    net_stat_report_prov_lost_in* p_in=P_IN;

    ret=this->net_stat_prov.insert(p_in);
    return ret;
}

int Croute_func::net_stat_report_ip_hop(DEAL_FUNC_ARG)
{

    net_stat_report_ip_hop_in* p_in=P_IN;


    ret=this->net_stat_ip.insert(p_in);
    return ret;

}

int Croute_func::net_stat_report_prov_hop(DEAL_FUNC_ARG)
{

    net_stat_report_prov_hop_in* p_in=P_IN;


    ret=this->net_stat_prov.insert(p_in);

    return ret;

}


int Croute_func::net_stat_report_link_delay(DEAL_FUNC_ARG){
    net_stat_report_link_delay_in* p_in=P_IN;


    ret=this->link_stat.insert(p_in);

    return ret;
}

int Croute_func::net_stat_report_link_lost(DEAL_FUNC_ARG){
    net_stat_report_link_lost_in* p_in=P_IN;


    ret=this->link_stat.insert(p_in);

    return ret;
}

int Croute_func::net_stat_report_link_hop(DEAL_FUNC_ARG){
    net_stat_report_link_hop_in* p_in=P_IN;


    ret=this->link_stat.insert(p_in);

    return ret;
}


/* ----------------CODE FOR net_stat_report_idc_delay  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_idc_delay(DEAL_FUNC_ARG){
    net_stat_report_idc_delay_in* p_in=P_IN;


    ret=this->link_stat.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_report_idc_lost  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_idc_lost(DEAL_FUNC_ARG){
    net_stat_report_idc_lost_in* p_in=P_IN;


    //	ret=this->link_stat.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_report_idc_hop  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_idc_hop(DEAL_FUNC_ARG){
    net_stat_report_idc_hop_in* p_in=P_IN;


    //	ret=this->link_stat.insert(p_in);

    return ret;
}


/* ----------------CODE FOR net_stat_report_all_idc_delay  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_all_idc_delay(DEAL_FUNC_ARG){
    net_stat_report_all_idc_delay_in* p_in=P_IN;


    //	ret=this->link_stat.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_report_all_idc_lost  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_all_idc_lost(DEAL_FUNC_ARG){
    net_stat_report_all_idc_lost_in* p_in=P_IN;


    //	ret=this->link_stat.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_report_all_idc_hop  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::net_stat_report_all_idc_hop(DEAL_FUNC_ARG){
    net_stat_report_all_idc_hop_in* p_in=P_IN;


    //	ret=this->link_stat.insert(p_in);

    return ret;
}

/* ----------------CODE FOR url_stat_report_point_time  ---------*/
// ./Croute_func.cpp

/**/
int Croute_func::url_stat_report_point_time(DEAL_FUNC_ARG){

    url_stat_report_point_time_in* p_in=P_IN;
    //	url_stat_report_point_time_out* p_out=P_OUT;

    ret=this->url_stat_prov.insert(p_in);

    //	p_out->prjid = p_in->prjid;
    //	p_out->pageid = p_in->pageid;
    //	p_out->offst = p_in->offst;
    return ret;

}

int Croute_func::url_rate_record(DEAL_FUNC_ARG){

    url_rate_record_in* p_in=P_IN;
    ret=this->url_stat_user.insert(p_in);


    return ret;

}

/* ----------------CODE FOR cdnrate_ip_isp  ---------*/
// ./Croute_func.cpp
int Croute_func::cdnrate_ip_isp(DEAL_FUNC_ARG){

    cdnrate_ip_isp_in* p_in=P_IN;
    ret=this->cdn_rate_detail.insert_cdnip_tb(p_in);
    if (ret)
        return ret;

    ret=this->cdn_rate_detail.insert_area_tb(p_in);
    if (ret)
        return ret;

    ret=this->cdn_rate_detail.insert_speed_dis_ip_tb(p_in);

    if (ret)
        return ret;

    ret=this->cdn_rate_detail.insert_speed_dis_area_tb(p_in);

    if (ret)
        return ret;

    //2012-11-01
    //ret = this->cdn_rate_detail.insert_stat_tb(p_in); 
    //if (ret) {
    //    return ret;
    //}
    // elva
    ret = this->db_cdn_info.insert_tables(p_in);

    return ret;

}

/* ----------------code for net_stat_ip_report  ---------*/
// ./croute_func.cpp

/*该协议替换1003,1005,1007*/
int Croute_func::net_stat_ip_report(DEAL_FUNC_ARG){
    net_stat_ip_report_in* p_in=P_IN;


    ret=this->net_stat_ip.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_prov_report  ---------*/
// ./Croute_func.cpp

/*该协议替换1004,1006,1008*/
int Croute_func::net_stat_prov_report(DEAL_FUNC_ARG){
    net_stat_prov_report_in* p_in=P_IN;


    ret=this->net_stat_prov.insert(p_in);

    return ret;
}

/* ----------------CODE FOR net_stat_link_report  ---------*/
// ./Croute_func.cpp

/*该协议替换1009,1010,1011*/
int Croute_func::net_stat_link_report(DEAL_FUNC_ARG){
    net_stat_link_report_in* p_in=P_IN;

    ret=this->link_stat.insert(p_in);

    return ret;
}

