<?php
require_once("proto_base.php");

class sw_get_ranged_svrlist_in {
	/*  */
	#类型:uint32
	public $start_id;

	/*  */
	#类型:uint32
	public $end_id;

	/**/
	#变长数组,最大长度:1000, 类型:uint32_t
	public $friend_list =array();


	public function sw_get_ranged_svrlist_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->start_id)) return false;
		if (!$ba->read_uint32($this->end_id)) return false;

		$friend_list_count=0 ;
		if (!$ba->read_uint32( $friend_list_count )) return false;
		$this->friend_list=array();
		{for($i=0; $i<$friend_list_count;$i++){
			if (!$ba->read_uint32($this->friend_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->start_id)) return false;
		if (!$ba->write_uint32($this->end_id)) return false;
		$friend_list_count=count($this->friend_list);
        $ba->write_uint32($friend_list_count);
		{for($i=0; $i<$friend_list_count;$i++){
			if (!$ba->write_uint32($this->friend_list[$i])) return false;
		}}
		return true;
	}

};

	
class online_item_t {
	/*  */
	#类型:uint32
	public $online_id;

	/*  */
	#类型:uint32
	public $user_num;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $online_ip ;

	/*  */
	#类型:uint16
	public $online_port;

	/*  */
	#类型:uint32
	public $friend_count;


	public function online_item_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->online_id)) return false;
		if (!$ba->read_uint32($this->user_num)) return false;
		if(!$ba->read_buf($this->online_ip,16)) return false;
		if (!$ba->read_uint16($this->online_port)) return false;
		if (!$ba->read_uint32($this->friend_count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->online_id)) return false;
		if (!$ba->write_uint32($this->user_num)) return false;
		$ba->write_buf($this->online_ip,16);
		if (!$ba->write_uint16($this->online_port)) return false;
		if (!$ba->write_uint32($this->friend_count)) return false;
		return true;
	}

};

	
class sw_get_ranged_svrlist_out {
	/*  */
	#类型:uint32
	public $max_online_id;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public $online_list =array();


	public function sw_get_ranged_svrlist_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->max_online_id)) return false;

		$online_list_count=0 ;
		if (!$ba->read_uint32( $online_list_count )) return false;
		$this->online_list=array();
		{for($i=0; $i<$online_list_count;$i++){
			$this->online_list[$i]=new online_item_t();
			if (!$this->online_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->max_online_id)) return false;
		$online_list_count=count($this->online_list);
        $ba->write_uint32($online_list_count);
		{for($i=0; $i<$online_list_count;$i++){
			if ( ! $this->online_list[$i] instanceof online_item_t ) return false; 
			if (!$this->online_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class sw_get_recommend_svr_list_out {
	/*  */
	#类型:uint32
	public $max_online_id;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public $online_list =array();


	public function sw_get_recommend_svr_list_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->max_online_id)) return false;

		$online_list_count=0 ;
		if (!$ba->read_uint32( $online_list_count )) return false;
		$this->online_list=array();
		{for($i=0; $i<$online_list_count;$i++){
			$this->online_list[$i]=new online_item_t();
			if (!$this->online_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->max_online_id)) return false;
		$online_list_count=count($this->online_list);
        $ba->write_uint32($online_list_count);
		{for($i=0; $i<$online_list_count;$i++){
			if ( ! $this->online_list[$i] instanceof online_item_t ) return false; 
			if (!$this->online_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class sw_get_user_count_out {
	/*  */
	#类型:uint32
	public $online_count;


	public function sw_get_user_count_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->online_count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->online_count)) return false;
		return true;
	}

};

	
class sw_noti_across_svr_in {
	/*  */
	#类型:uint32
	public $cmdid;

	/**/
	#变长数组,最大长度:999, 类型:char
	public $buflist ;


	public function sw_noti_across_svr_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->cmdid)) return false;

		$buflist_count=0 ;
		if (!$ba->read_uint32( $buflist_count )) return false;
		if(!$ba->read_buf($this->buflist,$buflist_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->cmdid)) return false;
		$buflist_count=strlen($this->buflist);
		$ba->write_uint32($buflist_count);
		$ba->write_buf($this->buflist,$buflist_count);
		return true;
	}

};

	
class sw_noti_across_svr_out {
	/*  */
	#类型:uint32
	public $cmdid;

	/**/
	#变长数组,最大长度:999, 类型:char
	public $buflist ;


	public function sw_noti_across_svr_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->cmdid)) return false;

		$buflist_count=0 ;
		if (!$ba->read_uint32( $buflist_count )) return false;
		if(!$ba->read_buf($this->buflist,$buflist_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->cmdid)) return false;
		$buflist_count=strlen($this->buflist);
		$ba->write_uint32($buflist_count);
		$ba->write_buf($this->buflist,$buflist_count);
		return true;
	}

};

	
class sw_report_online_info_in {
	/*  */
	#类型:uint16
	public $domain_id;

	/*  */
	#类型:uint32
	public $online_id;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $online_name ;

	/*  */
	#定长数组,长度:16, 类型:uint16_t 
	public $online_ip =array();

	/*  */
	#类型:uint16
	public $online_port;

	/*  */
	#类型:uint32
	public $user_num;

	/*  */
	#类型:uint32
	public $seqno;

	/*在线的用户列表*/
	#变长数组,最大长度:999999, 类型:uint32_t
	public $userid_list =array();


	public function sw_report_online_info_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint16($this->domain_id)) return false;
		if (!$ba->read_uint32($this->online_id)) return false;
		if(!$ba->read_buf($this->online_name,16)) return false;
		$this->online_ip=array();
		{for($i=0;$i<16;$i++){
			if (!$ba->read_uint16($this->online_ip[$i])) return false;
		}}
		if (!$ba->read_uint16($this->online_port)) return false;
		if (!$ba->read_uint32($this->user_num)) return false;
		if (!$ba->read_uint32($this->seqno)) return false;

		$userid_list_count=0 ;
		if (!$ba->read_uint32( $userid_list_count )) return false;
		$this->userid_list=array();
		{for($i=0; $i<$userid_list_count;$i++){
			if (!$ba->read_uint32($this->userid_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint16($this->domain_id)) return false;
		if (!$ba->write_uint32($this->online_id)) return false;
		$ba->write_buf($this->online_name,16);
		if(count($this->online_ip)!=16) return false; 
		{for($i=0; $i<16;$i++){
			if (!$ba->write_uint16($this->online_ip[$i])) return false;
		}}
		if (!$ba->write_uint16($this->online_port)) return false;
		if (!$ba->write_uint32($this->user_num)) return false;
		if (!$ba->write_uint32($this->seqno)) return false;
		$userid_list_count=count($this->userid_list);
        $ba->write_uint32($userid_list_count);
		{for($i=0; $i<$userid_list_count;$i++){
			if (!$ba->write_uint32($this->userid_list[$i])) return false;
		}}
		return true;
	}

};

	
class sw_report_user_onoff_in {
	/*  */
	#类型:uint32
	public $is_on_online;


	public function sw_report_user_onoff_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->is_on_online)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->is_on_online)) return false;
		return true;
	}

};

	
class find_map_t {
	/*  */
	#类型:uint32
	public $islandid;

	/*  */
	#类型:uint32
	public $mapid;


	public function find_map_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->mapid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->mapid)) return false;
		return true;
	}

};

	
class game_info_t {
	/* 项目编号 */
	#类型:uint32
	public $gameid;

	/* 胜利次数 */
	#类型:uint32
	public $win_time;

	/* 输的次数 */
	#类型:uint32
	public $lose_time;

	/* 平的次数 */
	#类型:uint32
	public $draw_time;


	public function game_info_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->win_time)) return false;
		if (!$ba->read_uint32($this->lose_time)) return false;
		if (!$ba->read_uint32($this->draw_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->win_time)) return false;
		if (!$ba->write_uint32($this->lose_time)) return false;
		if (!$ba->write_uint32($this->draw_time)) return false;
		return true;
	}

};

	
class item_day_limit_t {
	/* 物品ID（3战斗积分4在线时长） */
	#类型:uint32
	public $itemid;

	/* 总数 */
	#类型:uint32
	public $total;

	/* 当日获得次数 */
	#类型:uint32
	public $daycnt;


	public function item_day_limit_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->total)) return false;
		if (!$ba->read_uint32($this->daycnt)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->total)) return false;
		if (!$ba->write_uint32($this->daycnt)) return false;
		return true;
	}

};

	
class item_exchange_t {
	/* 1:任务，2:xiaomee */
	#类型:uint32
	public $itemid;

	/*  */
	#类型:uint32
	public $count;

	/*  */
	#类型:uint32
	public $max_count;

	/* 今日增加数量 */
	#类型:uint32
	public $day_add_cnt;


	public function item_exchange_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		if (!$ba->read_uint32($this->max_count)) return false;
		if (!$ba->read_uint32($this->day_add_cnt)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		if (!$ba->write_uint32($this->max_count)) return false;
		if (!$ba->write_uint32($this->day_add_cnt)) return false;
		return true;
	}

};

	
class item_ex_t {
	/*  */
	#类型:uint32
	public $itemid;

	/*  */
	#类型:uint32
	public $use_count;

	/*  */
	#类型:uint32
	public $count;


	public function item_ex_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->use_count)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->use_count)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class item_t {
	/*  */
	#类型:uint32
	public $itemid;

	/*  */
	#类型:uint32
	public $count;


	public function item_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class task_t {
	/*  */
	#类型:uint32
	public $taskid;

	/*  */
	#类型:uint32
	public $task_nodeid;


	public function task_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->taskid)) return false;
		if (!$ba->read_uint32($this->task_nodeid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->taskid)) return false;
		if (!$ba->write_uint32($this->task_nodeid)) return false;
		return true;
	}

};

	
class user_log_t {
	/* 记录时间 */
	#类型:uint32
	public $logtime;

	/* 1:注册 ，2：完成岛:(v2:岛id) */
	#类型:uint32
	public $v1;

	/*  */
	#类型:uint32
	public $v2;


	public function user_log_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->v1)) return false;
		if (!$ba->read_uint32($this->v2)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->v1)) return false;
		if (!$ba->write_uint32($this->v2)) return false;
		return true;
	}

};

	
class Cpop_switch_proto  extends Cproto_base {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }

	/* 得到在线人数 */

	function sw_get_user_count($userid ){

		return $this->send_cmd_new(61001,$userid, null, new sw_get_user_count_out(), 0x945b505b);
	}
	
	/* online 上报启动信息 */
	/* 调用方式还可以是： 
		$in=new sw_report_online_info_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->sw_report_online_info($userid,$in );
	*/

	function sw_report_online_info($userid , $domain_id=null, $online_id=null, $online_name=null, $online_ip=null, $online_port=null, $user_num=null, $seqno=null, $userid_list=null){

		if ( $domain_id instanceof sw_report_online_info_in ){
			$in=$domain_id;
		}else{
			$in=new sw_report_online_info_in();
			$in->domain_id=$domain_id;
			$in->online_id=$online_id;
			$in->online_name=$online_name;
			$in->online_ip=$online_ip;
			$in->online_port=$online_port;
			$in->user_num=$user_num;
			$in->seqno=$seqno;
			$in->userid_list=$userid_list;

		}
		
		return $this->send_cmd_new(62001,$userid, $in, null, 0x9d002133);
	}
	
	/* 上报用户上下线 */
	/* 调用方式还可以是： 
		$in=new sw_report_user_onoff_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->sw_report_user_onoff($userid,$in );
	*/

	function sw_report_user_onoff($userid , $is_on_online=null){

		if ( $is_on_online instanceof sw_report_user_onoff_in ){
			$in=$is_on_online;
		}else{
			$in=new sw_report_user_onoff_in();
			$in->is_on_online=$is_on_online;

		}
		
		return $this->send_cmd_new(62002,$userid, $in, null, 0xa23fe386);
	}
	
	/* 请求用户离线 */

	function sw_set_user_offline($userid ){

		return $this->send_cmd_new(62003,$userid, null, null, 0xe2948cb2);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new sw_noti_across_svr_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->sw_noti_across_svr($userid,$in );
	*/

	function sw_noti_across_svr($userid , $cmdid=null, $buflist=null){

		if ( $cmdid instanceof sw_noti_across_svr_in ){
			$in=$cmdid;
		}else{
			$in=new sw_noti_across_svr_in();
			$in->cmdid=$cmdid;
			$in->buflist=$buflist;

		}
		
		return $this->send_cmd_new(62004,$userid, $in, new sw_noti_across_svr_out(), 0xf55ca111);
	}
	
	/* 得到推荐服务器列表 */

	function sw_get_recommend_svr_list($userid ){

		return $this->send_cmd_new(62006,$userid, null, new sw_get_recommend_svr_list_out(), 0x4940b505);
	}
	
	/* 得到范围列表 */
	/* 调用方式还可以是： 
		$in=new sw_get_ranged_svrlist_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->sw_get_ranged_svrlist($userid,$in );
	*/

	function sw_get_ranged_svrlist($userid , $start_id=null, $end_id=null, $friend_list=null){

		if ( $start_id instanceof sw_get_ranged_svrlist_in ){
			$in=$start_id;
		}else{
			$in=new sw_get_ranged_svrlist_in();
			$in->start_id=$start_id;
			$in->end_id=$end_id;
			$in->friend_list=$friend_list;

		}
		
		return $this->send_cmd_new(64002,$userid, $in, new sw_get_ranged_svrlist_out(), 0xe107fe35);
	}
	
};
?>
