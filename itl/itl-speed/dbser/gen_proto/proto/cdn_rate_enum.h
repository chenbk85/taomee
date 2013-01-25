#ifndef  CDN_RATE_ENUM_H
#define  CDN_RATE_ENUM_H

/*命令定义*/	
enum  enum_cdn_rate_cmd{
	 cdn_report_ip_cmd		=	0x1001, /*上报相同ip的分钟表数据*/
	 cdn_report_prov_cmd		=	0x1002, /*按省市运营商上报cdn速度*/
	 net_stat_report_ip_delay_cmd		=	0x1003, /*按ip上报全网测速的时延*/
	 net_stat_report_prov_delay_cmd		=	0x1004, /**/
	 net_stat_report_ip_lost_cmd		=	0x1005, /**/
	 net_stat_report_prov_lost_cmd		=	0x1006, /**/
	 net_stat_report_ip_hop_cmd		=	0x1007, /**/
	 net_stat_report_prov_hop_cmd		=	0x1008, /**/
	 net_stat_report_link_delay_cmd		=	0x1009, /*单条链路的时延*/
	 net_stat_report_link_lost_cmd		=	0x1010, /**/
	 net_stat_report_link_hop_cmd		=	0x1011, /**/
	 net_stat_report_idc_delay_cmd		=	0x1012, /**/
	 net_stat_report_idc_lost_cmd		=	0x1013, /**/
	 net_stat_report_idc_hop_cmd		=	0x1014, /**/
	 net_stat_report_all_idc_delay_cmd		=	0x1015, /**/
	 net_stat_report_all_idc_lost_cmd		=	0x1016, /**/
	 net_stat_report_all_idc_hop_cmd		=	0x1017, /**/
	 url_stat_report_point_time_cmd		=	0x1018, /**/
	 url_rate_record_cmd		=	0x1019, /**/
	 cdnrate_ip_isp_cmd		=	0x1201, /**/
	 net_stat_ip_report_cmd		=	0x1211, /*该协议替换1003,1005,1007*/
	 net_stat_prov_report_cmd		=	0x1212, /*该协议替换1004,1006,1008*/
	 net_stat_link_report_cmd		=	0x1213, /*该协议替换1009,1010,1011*/

};

/*错误码定义*/	
enum  enum_cdn_rate_error{
	 __SUCC__		=	0, /*成功*/

};



#endif // CDN_RATE_ENUM_H
