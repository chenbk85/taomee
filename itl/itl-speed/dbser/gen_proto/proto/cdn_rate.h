#ifndef  CDN_RATE_H
#define  CDN_RATE_H
#include "cdn.h"

class cdnrate_ip_isp_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		prjid;
		/*上报时间戳*/
		uint32_t		logtime;
		/*cdn服务器ip*/
		uint32_t		cdnip;
		/*省编号*/
		uint32_t		provid;
		/*城市编号*/
		uint32_t		cityid;
		/*运营商名称*/
		char		comp[64]; 
		/*cdn下载速度和*/
		uint32_t		value;
		/*下载次数*/
		uint32_t		count;

		cdnrate_ip_isp_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cdn_report_ip_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		prjid;
		/*ip地址*/
		uint32_t		ip;
		/*上报时间戳*/
		uint32_t		logtime;
		/*cdn下载速度和*/
		uint32_t		value;
		/*下载次数*/
		uint32_t		count;

		cdn_report_ip_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class cdn_report_prov_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		prjid;
		/*省编号*/
		uint32_t		provid;
		/*城市编号*/
		uint32_t		cityid;
		/*运营商名称*/
		char		comp[64]; 
		/*下载时间*/
		uint32_t		logtime;
		/*下载速度和*/
		uint32_t		value;
		/*下载次数*/
		uint32_t		count;

		cdn_report_prov_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_ip_report_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/*ip地址*/
		uint32_t		ip;
		/*上报时间戳*/
		uint32_t		logtime;
		/*延迟*/
		uint32_t		delay_v;
		/*丢包率*/
		uint32_t		lost_v;
		/*跳数*/
		uint32_t		hop_v;
		/*探测次数*/
		uint32_t		count;

		net_stat_ip_report_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_link_report_in: public Cmessage {
	public:
		/*链路编号*/
		uint32_t		link;
		/*上报时间戳*/
		uint32_t		logtime;
		/*延迟*/
		uint32_t		delay_v;
		/*丢包率*/
		uint32_t		lost_v;
		/*跳数*/
		uint32_t		hop_v;
		/*探测次数*/
		uint32_t		count;

		net_stat_link_report_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_prov_report_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/*省编号*/
		uint32_t		provid;
		/*城市编号*/
		uint32_t		cityid;
		/*运营商名称*/
		char		comp[64]; 
		/*上报时间戳*/
		uint32_t		logtime;
		/*延迟*/
		uint32_t		delay_v;
		/*丢包率*/
		uint32_t		lost_v;
		/*跳数*/
		uint32_t		hop_v;
		/*探测次数*/
		uint32_t		count;

		net_stat_prov_report_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_all_idc_delay_in: public Cmessage {
	public:
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_all_idc_delay_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_all_idc_hop_in: public Cmessage {
	public:
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_all_idc_hop_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_all_idc_lost_in: public Cmessage {
	public:
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_all_idc_lost_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_idc_delay_in: public Cmessage {
	public:
		/*idc编号*/
		uint32_t		idc;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_idc_delay_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_idc_hop_in: public Cmessage {
	public:
		/**/
		uint32_t		idc;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_idc_hop_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_idc_lost_in: public Cmessage {
	public:
		/**/
		uint32_t		idc;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_idc_lost_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_ip_delay_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		ip;
		/*探测时间*/
		uint32_t		logtime;
		/*时延和*/
		uint32_t		value;
		/*探测的次数*/
		uint32_t		count;

		net_stat_report_ip_delay_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_ip_hop_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		ip;
		/**/
		uint32_t		logtime;
		/*一分钟内ttl和*/
		uint32_t		value;
		/*一分钟内探测次数*/
		uint32_t		count;

		net_stat_report_ip_hop_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_ip_lost_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		ip;
		/**/
		uint32_t		logtime;
		/*一分钟内丢包率和*/
		uint32_t		value;
		/*探测次数*/
		uint32_t		count;

		net_stat_report_ip_lost_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_link_delay_in: public Cmessage {
	public:
		/*链路编号*/
		uint32_t		link;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_link_delay_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_link_hop_in: public Cmessage {
	public:
		/**/
		uint32_t		link;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_link_hop_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_link_lost_in: public Cmessage {
	public:
		/**/
		uint32_t		link;
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_link_lost_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_prov_delay_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		provid;
		/**/
		uint32_t		cityid;
		/**/
		char		comp[64]; 
		/**/
		uint32_t		logtime;
		/*一分钟内时延和*/
		uint32_t		value;
		/*一分钟内探测次数*/
		uint32_t		count;

		net_stat_report_prov_delay_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_prov_hop_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		provid;
		/**/
		uint32_t		cityid;
		/**/
		char		comp[64]; 
		/**/
		uint32_t		logtime;
		/**/
		uint32_t		value;
		/**/
		uint32_t		count;

		net_stat_report_prov_hop_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class net_stat_report_prov_lost_in: public Cmessage {
	public:
		/*机房id*/
		uint32_t		idcid;
		/**/
		uint32_t		provid;
		/**/
		uint32_t		cityid;
		/**/
		char		comp[64]; 
		/**/
		uint32_t		logtime;
		/*一分钟内丢包率和*/
		uint32_t		value;
		/*一分钟内探测次数*/
		uint32_t		count;

		net_stat_report_prov_lost_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class url_rate_record_in: public Cmessage {
	public:
		/*项目编号*/
		uint32_t		prjid;
		/*用户id*/
		uint32_t		userid;
		/*客户端ip*/
		uint32_t		clientip;
		/*时间戳*/
		uint32_t		time;
		/*页面id*/
		uint32_t		pageid;
		//起点到每个点的耗时，最多10个点
		std::vector<uint32_t>		vlist;

		url_rate_record_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class url_stat_report_point_time_in: public Cmessage {
	public:
		/*项目id*/
		uint32_t		prjid;
		/*页面id*/
		uint32_t		pageid;
		/**/
		uint32_t		provid;
		/**/
		uint32_t		cityid;
		/*ISP编号*/
		uint32_t		ISPID;
		/*运营商名称*/
		char		comp[64]; 
		/*上报时间戳*/
		uint32_t		logtime;
		/*上报的数量*/
		uint32_t		repot_cnt;
		//
		std::vector<uint32_t>		vlist;

		url_stat_report_point_time_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // CDN_RATE_H
