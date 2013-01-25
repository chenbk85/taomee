#include "cdn_rate.h"

cdnrate_ip_isp_in::cdnrate_ip_isp_in(){
	this->init();
}
void cdnrate_ip_isp_in::init(){
	this->prjid=0;
	this->logtime=0;
	this->cdnip=0;
	this->provid=0;
	this->cityid=0;
	this->value=0;
	this->count=0;

}
bool  cdnrate_ip_isp_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prjid)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->cdnip)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool cdnrate_ip_isp_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prjid)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->cdnip)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
cdn_report_ip_in::cdn_report_ip_in(){
	this->init();
}
void cdn_report_ip_in::init(){
	this->prjid=0;
	this->ip=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  cdn_report_ip_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prjid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool cdn_report_ip_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prjid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
cdn_report_prov_in::cdn_report_prov_in(){
	this->init();
}
void cdn_report_prov_in::init(){
	this->prjid=0;
	this->provid=0;
	this->cityid=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  cdn_report_prov_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prjid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool cdn_report_prov_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prjid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_ip_report_in::net_stat_ip_report_in(){
	this->init();
}
void net_stat_ip_report_in::init(){
	this->idcid=0;
	this->ip=0;
	this->logtime=0;
	this->delay_v=0;
	this->lost_v=0;
	this->hop_v=0;
	this->count=0;

}
bool  net_stat_ip_report_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->delay_v)) return false;
	if (!ba.read_uint32(this->lost_v)) return false;
	if (!ba.read_uint32(this->hop_v)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_ip_report_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->delay_v)) return false;
	if (!ba.write_uint32(this->lost_v)) return false;
	if (!ba.write_uint32(this->hop_v)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_link_report_in::net_stat_link_report_in(){
	this->init();
}
void net_stat_link_report_in::init(){
	this->link=0;
	this->logtime=0;
	this->delay_v=0;
	this->lost_v=0;
	this->hop_v=0;
	this->count=0;

}
bool  net_stat_link_report_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->link)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->delay_v)) return false;
	if (!ba.read_uint32(this->lost_v)) return false;
	if (!ba.read_uint32(this->hop_v)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_link_report_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->link)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->delay_v)) return false;
	if (!ba.write_uint32(this->lost_v)) return false;
	if (!ba.write_uint32(this->hop_v)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_prov_report_in::net_stat_prov_report_in(){
	this->init();
}
void net_stat_prov_report_in::init(){
	this->idcid=0;
	this->provid=0;
	this->cityid=0;
	this->logtime=0;
	this->delay_v=0;
	this->lost_v=0;
	this->hop_v=0;
	this->count=0;

}
bool  net_stat_prov_report_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->delay_v)) return false;
	if (!ba.read_uint32(this->lost_v)) return false;
	if (!ba.read_uint32(this->hop_v)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_prov_report_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->delay_v)) return false;
	if (!ba.write_uint32(this->lost_v)) return false;
	if (!ba.write_uint32(this->hop_v)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_all_idc_delay_in::net_stat_report_all_idc_delay_in(){
	this->init();
}
void net_stat_report_all_idc_delay_in::init(){
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_all_idc_delay_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_all_idc_delay_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_all_idc_hop_in::net_stat_report_all_idc_hop_in(){
	this->init();
}
void net_stat_report_all_idc_hop_in::init(){
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_all_idc_hop_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_all_idc_hop_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_all_idc_lost_in::net_stat_report_all_idc_lost_in(){
	this->init();
}
void net_stat_report_all_idc_lost_in::init(){
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_all_idc_lost_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_all_idc_lost_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_idc_delay_in::net_stat_report_idc_delay_in(){
	this->init();
}
void net_stat_report_idc_delay_in::init(){
	this->idc=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_idc_delay_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idc)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_idc_delay_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idc)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_idc_hop_in::net_stat_report_idc_hop_in(){
	this->init();
}
void net_stat_report_idc_hop_in::init(){
	this->idc=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_idc_hop_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idc)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_idc_hop_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idc)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_idc_lost_in::net_stat_report_idc_lost_in(){
	this->init();
}
void net_stat_report_idc_lost_in::init(){
	this->idc=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_idc_lost_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idc)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_idc_lost_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idc)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_ip_delay_in::net_stat_report_ip_delay_in(){
	this->init();
}
void net_stat_report_ip_delay_in::init(){
	this->idcid=0;
	this->ip=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_ip_delay_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_ip_delay_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_ip_hop_in::net_stat_report_ip_hop_in(){
	this->init();
}
void net_stat_report_ip_hop_in::init(){
	this->idcid=0;
	this->ip=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_ip_hop_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_ip_hop_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_ip_lost_in::net_stat_report_ip_lost_in(){
	this->init();
}
void net_stat_report_ip_lost_in::init(){
	this->idcid=0;
	this->ip=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_ip_lost_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->ip)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_ip_lost_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->ip)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_link_delay_in::net_stat_report_link_delay_in(){
	this->init();
}
void net_stat_report_link_delay_in::init(){
	this->link=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_link_delay_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->link)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_link_delay_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->link)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_link_hop_in::net_stat_report_link_hop_in(){
	this->init();
}
void net_stat_report_link_hop_in::init(){
	this->link=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_link_hop_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->link)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_link_hop_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->link)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_link_lost_in::net_stat_report_link_lost_in(){
	this->init();
}
void net_stat_report_link_lost_in::init(){
	this->link=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_link_lost_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->link)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_link_lost_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->link)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_prov_delay_in::net_stat_report_prov_delay_in(){
	this->init();
}
void net_stat_report_prov_delay_in::init(){
	this->idcid=0;
	this->provid=0;
	this->cityid=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_prov_delay_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_prov_delay_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_prov_hop_in::net_stat_report_prov_hop_in(){
	this->init();
}
void net_stat_report_prov_hop_in::init(){
	this->idcid=0;
	this->provid=0;
	this->cityid=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_prov_hop_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_prov_hop_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
net_stat_report_prov_lost_in::net_stat_report_prov_lost_in(){
	this->init();
}
void net_stat_report_prov_lost_in::init(){
	this->idcid=0;
	this->provid=0;
	this->cityid=0;
	this->logtime=0;
	this->value=0;
	this->count=0;

}
bool  net_stat_report_prov_lost_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->idcid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->value)) return false;
	if (!ba.read_uint32(this->count)) return false;
	return true;
}

bool net_stat_report_prov_lost_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->idcid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->value)) return false;
	if (!ba.write_uint32(this->count)) return false;
	return true;
}
	
url_rate_record_in::url_rate_record_in(){
	this->init();
}
void url_rate_record_in::init(){
	this->prjid=0;
	this->userid=0;
	this->clientip=0;
	this->time=0;
	this->pageid=0;
	this->vlist.clear();

}
bool  url_rate_record_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prjid)) return false;
	if (!ba.read_uint32(this->userid)) return false;
	if (!ba.read_uint32(this->clientip)) return false;
	if (!ba.read_uint32(this->time)) return false;
	if (!ba.read_uint32(this->pageid)) return false;

	uint32_t vlist_count ;
	if (!ba.read_uint32( vlist_count )) return false;
	if (vlist_count>20) return false;
	uint32_t  vlist_item;
	this->vlist.clear();
	{for(uint32_t i=0; i<vlist_count;i++){
		if (!ba.read_uint32(vlist_item)) return false;
		this->vlist.push_back(vlist_item);
	}}
	return true;
}

bool url_rate_record_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prjid)) return false;
	if (!ba.write_uint32(this->userid)) return false;
	if (!ba.write_uint32(this->clientip)) return false;
	if (!ba.write_uint32(this->time)) return false;
	if (!ba.write_uint32(this->pageid)) return false;
	if (this->vlist.size()>20 ) return false;
	if (!ba.write_uint32(this->vlist.size())) return false;
	{for(uint32_t i=0; i<this->vlist.size() ;i++){
		if (!ba.write_uint32(this->vlist[i])) return false;
	}}
	return true;
}
	
url_stat_report_point_time_in::url_stat_report_point_time_in(){
	this->init();
}
void url_stat_report_point_time_in::init(){
	this->prjid=0;
	this->pageid=0;
	this->provid=0;
	this->cityid=0;
	this->ISPID=0;
	this->logtime=0;
	this->repot_cnt=0;
	this->vlist.clear();

}
bool  url_stat_report_point_time_in::read_from_buf(byte_array_t & ba ){
	if (!ba.read_uint32(this->prjid)) return false;
	if (!ba.read_uint32(this->pageid)) return false;
	if (!ba.read_uint32(this->provid)) return false;
	if (!ba.read_uint32(this->cityid)) return false;
	if (!ba.read_uint32(this->ISPID)) return false;
	if(!ba.read_buf(this->comp,64)) return false;
	if (!ba.read_uint32(this->logtime)) return false;
	if (!ba.read_uint32(this->repot_cnt)) return false;

	uint32_t vlist_count ;
	if (!ba.read_uint32( vlist_count )) return false;
	if (vlist_count>20) return false;
	uint32_t  vlist_item;
	this->vlist.clear();
	{for(uint32_t i=0; i<vlist_count;i++){
		if (!ba.read_uint32(vlist_item)) return false;
		this->vlist.push_back(vlist_item);
	}}
	return true;
}

bool url_stat_report_point_time_in::write_to_buf(byte_array_t & ba ){
	if (!ba.write_uint32(this->prjid)) return false;
	if (!ba.write_uint32(this->pageid)) return false;
	if (!ba.write_uint32(this->provid)) return false;
	if (!ba.write_uint32(this->cityid)) return false;
	if (!ba.write_uint32(this->ISPID)) return false;
	if(!ba.write_buf(this->comp,64)) return false;
	if (!ba.write_uint32(this->logtime)) return false;
	if (!ba.write_uint32(this->repot_cnt)) return false;
	if (this->vlist.size()>20 ) return false;
	if (!ba.write_uint32(this->vlist.size())) return false;
	{for(uint32_t i=0; i<this->vlist.size() ;i++){
		if (!ba.write_uint32(this->vlist[i])) return false;
	}}
	return true;
}
	