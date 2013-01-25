<?php
require_once("proto_base.php");

class cdnrate_ip_isp_in {
	/* 项目编号 */
	#类型:uint32
	public $prjid;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* cdn服务器ip */
	#类型:uint32
	public $cdnip;

	/* 省编号 */
	#类型:uint32
	public $provid;

	/* 城市编号 */
	#类型:uint32
	public $cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/* cdn下载速度和 */
	#类型:uint32
	public $value;

	/* 下载次数 */
	#类型:uint32
	public $count;


	public function cdnrate_ip_isp_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->prjid)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->cdnip)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->prjid)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->cdnip)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class cdn_report_ip_in {
	/* 项目编号 */
	#类型:uint32
	public $prjid;

	/* ip地址 */
	#类型:uint32
	public $ip;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* cdn下载速度和 */
	#类型:uint32
	public $value;

	/* 下载次数 */
	#类型:uint32
	public $count;


	public function cdn_report_ip_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->prjid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->prjid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class cdn_report_prov_in {
	/* 项目编号 */
	#类型:uint32
	public $prjid;

	/* 省编号 */
	#类型:uint32
	public $provid;

	/* 城市编号 */
	#类型:uint32
	public $cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/* 下载时间 */
	#类型:uint32
	public $logtime;

	/* 下载速度和 */
	#类型:uint32
	public $value;

	/* 下载次数 */
	#类型:uint32
	public $count;


	public function cdn_report_prov_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->prjid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->prjid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_ip_report_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/* ip地址 */
	#类型:uint32
	public $ip;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* 延迟 */
	#类型:uint32
	public $delay_v;

	/* 丢包率 */
	#类型:uint32
	public $lost_v;

	/* 跳数 */
	#类型:uint32
	public $hop_v;

	/* 探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_ip_report_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->delay_v)) return false;
		if (!$ba->read_uint32($this->lost_v)) return false;
		if (!$ba->read_uint32($this->hop_v)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->delay_v)) return false;
		if (!$ba->write_uint32($this->lost_v)) return false;
		if (!$ba->write_uint32($this->hop_v)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_link_report_in {
	/* 链路编号 */
	#类型:uint32
	public $link;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* 延迟 */
	#类型:uint32
	public $delay_v;

	/* 丢包率 */
	#类型:uint32
	public $lost_v;

	/* 跳数 */
	#类型:uint32
	public $hop_v;

	/* 探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_link_report_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->link)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->delay_v)) return false;
		if (!$ba->read_uint32($this->lost_v)) return false;
		if (!$ba->read_uint32($this->hop_v)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->link)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->delay_v)) return false;
		if (!$ba->write_uint32($this->lost_v)) return false;
		if (!$ba->write_uint32($this->hop_v)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_prov_report_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/* 省编号 */
	#类型:uint32
	public $provid;

	/* 城市编号 */
	#类型:uint32
	public $cityid;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* 延迟 */
	#类型:uint32
	public $delay_v;

	/* 丢包率 */
	#类型:uint32
	public $lost_v;

	/* 跳数 */
	#类型:uint32
	public $hop_v;

	/* 探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_prov_report_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->delay_v)) return false;
		if (!$ba->read_uint32($this->lost_v)) return false;
		if (!$ba->read_uint32($this->hop_v)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->delay_v)) return false;
		if (!$ba->write_uint32($this->lost_v)) return false;
		if (!$ba->write_uint32($this->hop_v)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_all_idc_delay_in {
	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_all_idc_delay_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_all_idc_hop_in {
	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_all_idc_hop_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_all_idc_lost_in {
	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_all_idc_lost_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_idc_delay_in {
	/* idc编号 */
	#类型:uint32
	public $idc;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_idc_delay_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idc)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idc)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_idc_hop_in {
	/*  */
	#类型:uint32
	public $idc;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_idc_hop_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idc)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idc)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_idc_lost_in {
	/*  */
	#类型:uint32
	public $idc;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_idc_lost_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idc)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idc)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_ip_delay_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $ip;

	/* 探测时间 */
	#类型:uint32
	public $logtime;

	/* 时延和 */
	#类型:uint32
	public $value;

	/* 探测的次数 */
	#类型:uint32
	public $count;


	public function net_stat_report_ip_delay_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_ip_hop_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $ip;

	/*  */
	#类型:uint32
	public $logtime;

	/* 一分钟内ttl和 */
	#类型:uint32
	public $value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_report_ip_hop_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_ip_lost_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $ip;

	/*  */
	#类型:uint32
	public $logtime;

	/* 一分钟内丢包率和 */
	#类型:uint32
	public $value;

	/* 探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_report_ip_lost_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_link_delay_in {
	/* 链路编号 */
	#类型:uint32
	public $link;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_link_delay_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->link)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->link)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_link_hop_in {
	/*  */
	#类型:uint32
	public $link;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_link_hop_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->link)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->link)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_link_lost_in {
	/*  */
	#类型:uint32
	public $link;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_link_lost_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->link)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->link)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_prov_delay_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $provid;

	/*  */
	#类型:uint32
	public $cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/*  */
	#类型:uint32
	public $logtime;

	/* 一分钟内时延和 */
	#类型:uint32
	public $value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_report_prov_delay_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_prov_hop_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $provid;

	/*  */
	#类型:uint32
	public $cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/*  */
	#类型:uint32
	public $logtime;

	/*  */
	#类型:uint32
	public $value;

	/*  */
	#类型:uint32
	public $count;


	public function net_stat_report_prov_hop_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class net_stat_report_prov_lost_in {
	/* 机房id */
	#类型:uint32
	public $idcid;

	/*  */
	#类型:uint32
	public $provid;

	/*  */
	#类型:uint32
	public $cityid;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/*  */
	#类型:uint32
	public $logtime;

	/* 一分钟内丢包率和 */
	#类型:uint32
	public $value;

	/* 一分钟内探测次数 */
	#类型:uint32
	public $count;


	public function net_stat_report_prov_lost_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->idcid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->idcid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class url_rate_record_in {
	/* 项目编号 */
	#类型:uint32
	public $prjid;

	/* 用户id */
	#类型:uint32
	public $userid;

	/* 客户端ip */
	#类型:uint32
	public $clientip;

	/* 时间戳 */
	#类型:uint32
	public $time;

	/* 页面id */
	#类型:uint32
	public $pageid;

	/*起点到每个点的耗时，最多10个点*/
	#变长数组,最大长度:20, 类型:uint32_t
	public $vlist =array();


	public function url_rate_record_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->prjid)) return false;
		if (!$ba->read_uint32($this->userid)) return false;
		if (!$ba->read_uint32($this->clientip)) return false;
		if (!$ba->read_uint32($this->time)) return false;
		if (!$ba->read_uint32($this->pageid)) return false;

		$vlist_count=0 ;
		if (!$ba->read_uint32( $vlist_count )) return false;
		if ($vlist_count>20) return false;
		$this->vlist=array();
		{for($i=0; $i<$vlist_count;$i++){
			if (!$ba->read_uint32($this->vlist[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->prjid)) return false;
		if (!$ba->write_uint32($this->userid)) return false;
		if (!$ba->write_uint32($this->clientip)) return false;
		if (!$ba->write_uint32($this->time)) return false;
		if (!$ba->write_uint32($this->pageid)) return false;
		$vlist_count=count($this->vlist);
        if ($vlist_count>20 ) return false; 
        $ba->write_uint32($vlist_count);
		{for($i=0; $i<$vlist_count;$i++){
			if (!$ba->write_uint32($this->vlist[$i])) return false;
		}}
		return true;
	}

};

	
class url_stat_report_point_time_in {
	/* 项目id */
	#类型:uint32
	public $prjid;

	/* 页面id */
	#类型:uint32
	public $pageid;

	/*  */
	#类型:uint32
	public $provid;

	/*  */
	#类型:uint32
	public $cityid;

	/* ISP编号 */
	#类型:uint32
	public $ISPID;

	/* 运营商名称 */
	#定长数组,长度:64, 类型:char 
	public $comp ;

	/* 上报时间戳 */
	#类型:uint32
	public $logtime;

	/* 上报的数量 */
	#类型:uint32
	public $repot_cnt;

	/**/
	#变长数组,最大长度:20, 类型:uint32_t
	public $vlist =array();


	public function url_stat_report_point_time_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->prjid)) return false;
		if (!$ba->read_uint32($this->pageid)) return false;
		if (!$ba->read_uint32($this->provid)) return false;
		if (!$ba->read_uint32($this->cityid)) return false;
		if (!$ba->read_uint32($this->ISPID)) return false;
		if(!$ba->read_buf($this->comp,64)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->repot_cnt)) return false;

		$vlist_count=0 ;
		if (!$ba->read_uint32( $vlist_count )) return false;
		if ($vlist_count>20) return false;
		$this->vlist=array();
		{for($i=0; $i<$vlist_count;$i++){
			if (!$ba->read_uint32($this->vlist[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->prjid)) return false;
		if (!$ba->write_uint32($this->pageid)) return false;
		if (!$ba->write_uint32($this->provid)) return false;
		if (!$ba->write_uint32($this->cityid)) return false;
		if (!$ba->write_uint32($this->ISPID)) return false;
		$ba->write_buf($this->comp,64);
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->repot_cnt)) return false;
		$vlist_count=count($this->vlist);
        if ($vlist_count>20 ) return false; 
        $ba->write_uint32($vlist_count);
		{for($i=0; $i<$vlist_count;$i++){
			if (!$ba->write_uint32($this->vlist[$i])) return false;
		}}
		return true;
	}

};

	
class Ccdn_rate_proto  extends Cproto_base {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }

	/* 上报相同ip的分钟表数据 */
	/* 调用方式还可以是： 
		$in=new cdn_report_ip_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cdn_report_ip($userid,$in );
	*/

	function cdn_report_ip($userid , $prjid=null, $ip=null, $logtime=null, $value=null, $count=null){

		if ( $prjid instanceof cdn_report_ip_in ){
			$in=$prjid;
		}else{
			$in=new cdn_report_ip_in();
			$in->prjid=$prjid;
			$in->ip=$ip;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1001,$userid, $in, null, 0x55dbed83);
	}
	
	/* 按省市运营商上报cdn速度 */
	/* 调用方式还可以是： 
		$in=new cdn_report_prov_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cdn_report_prov($userid,$in );
	*/

	function cdn_report_prov($userid , $prjid=null, $provid=null, $cityid=null, $comp=null, $logtime=null, $value=null, $count=null){

		if ( $prjid instanceof cdn_report_prov_in ){
			$in=$prjid;
		}else{
			$in=new cdn_report_prov_in();
			$in->prjid=$prjid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1002,$userid, $in, null, 0x433d49cc);
	}
	
	/* 按ip上报全网测速的时延 */
	/* 调用方式还可以是： 
		$in=new net_stat_report_ip_delay_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_ip_delay($userid,$in );
	*/

	function net_stat_report_ip_delay($userid , $idcid=null, $ip=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_ip_delay_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_ip_delay_in();
			$in->idcid=$idcid;
			$in->ip=$ip;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1003,$userid, $in, null, 0x004382d1);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_prov_delay_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_prov_delay($userid,$in );
	*/

	function net_stat_report_prov_delay($userid , $idcid=null, $provid=null, $cityid=null, $comp=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_prov_delay_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_prov_delay_in();
			$in->idcid=$idcid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1004,$userid, $in, null, 0xdcf31efa);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_ip_lost_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_ip_lost($userid,$in );
	*/

	function net_stat_report_ip_lost($userid , $idcid=null, $ip=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_ip_lost_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_ip_lost_in();
			$in->idcid=$idcid;
			$in->ip=$ip;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1005,$userid, $in, null, 0x62125738);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_prov_lost_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_prov_lost($userid,$in );
	*/

	function net_stat_report_prov_lost($userid , $idcid=null, $provid=null, $cityid=null, $comp=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_prov_lost_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_prov_lost_in();
			$in->idcid=$idcid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1006,$userid, $in, null, 0x0427d85e);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_ip_hop_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_ip_hop($userid,$in );
	*/

	function net_stat_report_ip_hop($userid , $idcid=null, $ip=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_ip_hop_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_ip_hop_in();
			$in->idcid=$idcid;
			$in->ip=$ip;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1007,$userid, $in, null, 0x3e65b476);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_prov_hop_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_prov_hop($userid,$in );
	*/

	function net_stat_report_prov_hop($userid , $idcid=null, $provid=null, $cityid=null, $comp=null, $logtime=null, $value=null, $count=null){

		if ( $idcid instanceof net_stat_report_prov_hop_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_report_prov_hop_in();
			$in->idcid=$idcid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1008,$userid, $in, null, 0x88f1a8cd);
	}
	
	/* 单条链路的时延 */
	/* 调用方式还可以是： 
		$in=new net_stat_report_link_delay_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_link_delay($userid,$in );
	*/

	function net_stat_report_link_delay($userid , $link=null, $logtime=null, $value=null, $count=null){

		if ( $link instanceof net_stat_report_link_delay_in ){
			$in=$link;
		}else{
			$in=new net_stat_report_link_delay_in();
			$in->link=$link;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1009,$userid, $in, null, 0xa11782a9);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_link_lost_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_link_lost($userid,$in );
	*/

	function net_stat_report_link_lost($userid , $link=null, $logtime=null, $value=null, $count=null){

		if ( $link instanceof net_stat_report_link_lost_in ){
			$in=$link;
		}else{
			$in=new net_stat_report_link_lost_in();
			$in->link=$link;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1010,$userid, $in, null, 0xac44711e);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_link_hop_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_link_hop($userid,$in );
	*/

	function net_stat_report_link_hop($userid , $link=null, $logtime=null, $value=null, $count=null){

		if ( $link instanceof net_stat_report_link_hop_in ){
			$in=$link;
		}else{
			$in=new net_stat_report_link_hop_in();
			$in->link=$link;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1011,$userid, $in, null, 0x27390bdb);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_idc_delay_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_idc_delay($userid,$in );
	*/

	function net_stat_report_idc_delay($userid , $idc=null, $logtime=null, $value=null, $count=null){

		if ( $idc instanceof net_stat_report_idc_delay_in ){
			$in=$idc;
		}else{
			$in=new net_stat_report_idc_delay_in();
			$in->idc=$idc;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1012,$userid, $in, null, 0x4a8f5029);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_idc_lost_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_idc_lost($userid,$in );
	*/

	function net_stat_report_idc_lost($userid , $idc=null, $logtime=null, $value=null, $count=null){

		if ( $idc instanceof net_stat_report_idc_lost_in ){
			$in=$idc;
		}else{
			$in=new net_stat_report_idc_lost_in();
			$in->idc=$idc;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1013,$userid, $in, null, 0x46353d7b);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_idc_hop_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_idc_hop($userid,$in );
	*/

	function net_stat_report_idc_hop($userid , $idc=null, $logtime=null, $value=null, $count=null){

		if ( $idc instanceof net_stat_report_idc_hop_in ){
			$in=$idc;
		}else{
			$in=new net_stat_report_idc_hop_in();
			$in->idc=$idc;
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1014,$userid, $in, null, 0xe5acfd47);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_all_idc_delay_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_all_idc_delay($userid,$in );
	*/

	function net_stat_report_all_idc_delay($userid , $logtime=null, $value=null, $count=null){

		if ( $logtime instanceof net_stat_report_all_idc_delay_in ){
			$in=$logtime;
		}else{
			$in=new net_stat_report_all_idc_delay_in();
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1015,$userid, $in, null, 0xedc0dda3);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_all_idc_lost_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_all_idc_lost($userid,$in );
	*/

	function net_stat_report_all_idc_lost($userid , $logtime=null, $value=null, $count=null){

		if ( $logtime instanceof net_stat_report_all_idc_lost_in ){
			$in=$logtime;
		}else{
			$in=new net_stat_report_all_idc_lost_in();
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1016,$userid, $in, null, 0x96b2c1ba);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new net_stat_report_all_idc_hop_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_report_all_idc_hop($userid,$in );
	*/

	function net_stat_report_all_idc_hop($userid , $logtime=null, $value=null, $count=null){

		if ( $logtime instanceof net_stat_report_all_idc_hop_in ){
			$in=$logtime;
		}else{
			$in=new net_stat_report_all_idc_hop_in();
			$in->logtime=$logtime;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1017,$userid, $in, null, 0x135e812a);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new url_stat_report_point_time_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->url_stat_report_point_time($userid,$in );
	*/

	function url_stat_report_point_time($userid , $prjid=null, $pageid=null, $provid=null, $cityid=null, $ISPID=null, $comp=null, $logtime=null, $repot_cnt=null, $vlist=null){

		if ( $prjid instanceof url_stat_report_point_time_in ){
			$in=$prjid;
		}else{
			$in=new url_stat_report_point_time_in();
			$in->prjid=$prjid;
			$in->pageid=$pageid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->ISPID=$ISPID;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->repot_cnt=$repot_cnt;
			$in->vlist=$vlist;

		}
		
		return $this->send_cmd_new(0x1018,$userid, $in, null, 0x0e074eb4);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new url_rate_record_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->url_rate_record($userid,$in );
	*/

	function url_rate_record($userid , $prjid=null, $userid=null, $clientip=null, $time=null, $pageid=null, $vlist=null){

		if ( $prjid instanceof url_rate_record_in ){
			$in=$prjid;
		}else{
			$in=new url_rate_record_in();
			$in->prjid=$prjid;
			$in->userid=$userid;
			$in->clientip=$clientip;
			$in->time=$time;
			$in->pageid=$pageid;
			$in->vlist=$vlist;

		}
		
		return $this->send_cmd_new(0x1019,$userid, $in, null, 0xd0e624e9);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new cdnrate_ip_isp_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cdnrate_ip_isp($userid,$in );
	*/

	function cdnrate_ip_isp($userid , $prjid=null, $logtime=null, $cdnip=null, $provid=null, $cityid=null, $comp=null, $value=null, $count=null){

		if ( $prjid instanceof cdnrate_ip_isp_in ){
			$in=$prjid;
		}else{
			$in=new cdnrate_ip_isp_in();
			$in->prjid=$prjid;
			$in->logtime=$logtime;
			$in->cdnip=$cdnip;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->value=$value;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1201,$userid, $in, null, 0x8b99a5d5);
	}
	
	/* 该协议替换1003,1005,1007 */
	/* 调用方式还可以是： 
		$in=new net_stat_ip_report_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_ip_report($userid,$in );
	*/

	function net_stat_ip_report($userid , $idcid=null, $ip=null, $logtime=null, $delay_v=null, $lost_v=null, $hop_v=null, $count=null){

		if ( $idcid instanceof net_stat_ip_report_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_ip_report_in();
			$in->idcid=$idcid;
			$in->ip=$ip;
			$in->logtime=$logtime;
			$in->delay_v=$delay_v;
			$in->lost_v=$lost_v;
			$in->hop_v=$hop_v;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1211,$userid, $in, null, 0x20e4ac55);
	}
	
	/* 该协议替换1004,1006,1008 */
	/* 调用方式还可以是： 
		$in=new net_stat_prov_report_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_prov_report($userid,$in );
	*/

	function net_stat_prov_report($userid , $idcid=null, $provid=null, $cityid=null, $comp=null, $logtime=null, $delay_v=null, $lost_v=null, $hop_v=null, $count=null){

		if ( $idcid instanceof net_stat_prov_report_in ){
			$in=$idcid;
		}else{
			$in=new net_stat_prov_report_in();
			$in->idcid=$idcid;
			$in->provid=$provid;
			$in->cityid=$cityid;
			$in->comp=$comp;
			$in->logtime=$logtime;
			$in->delay_v=$delay_v;
			$in->lost_v=$lost_v;
			$in->hop_v=$hop_v;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1212,$userid, $in, null, 0x044b54b8);
	}
	
	/* 该协议替换1009,1010,1011 */
	/* 调用方式还可以是： 
		$in=new net_stat_link_report_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->net_stat_link_report($userid,$in );
	*/

	function net_stat_link_report($userid , $link=null, $logtime=null, $delay_v=null, $lost_v=null, $hop_v=null, $count=null){

		if ( $link instanceof net_stat_link_report_in ){
			$in=$link;
		}else{
			$in=new net_stat_link_report_in();
			$in->link=$link;
			$in->logtime=$logtime;
			$in->delay_v=$delay_v;
			$in->lost_v=$lost_v;
			$in->hop_v=$hop_v;
			$in->count=$count;

		}
		
		return $this->send_cmd_new(0x1213,$userid, $in, null, 0x28b4687a);
	}
	
};
?>
