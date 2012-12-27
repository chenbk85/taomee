<?php
require_once("proto_base.php");

class get_server_version_out {
	/*  */
	#定长数组,长度:255, 类型:char 
	public $version ;


	public function get_server_version_out(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->version,255)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->version,255);
		return true;
	}

};

	
class pop_add_game_info_in {
	/* 项目编号 */
	#类型:uint32
	public $gameid;

	/* 胜负标志0：负 1：胜 2：平 */
	#类型:uint32
	public $win_flag;


	public function pop_add_game_info_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->win_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->win_flag)) return false;
		return true;
	}

};

	
class pop_copy_user_in {
	/* 目标userid */
	#类型:uint32
	public $dsc_userid;


	public function pop_copy_user_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->dsc_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->dsc_userid)) return false;
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

	
class pop_exchange_in {
	/* 未使用 */
	#类型:uint32
	public $opt_flag;

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public $add_item_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public $del_item_list =array();


	public function pop_exchange_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->opt_flag)) return false;

		$add_item_list_count=0 ;
		if (!$ba->read_uint32( $add_item_list_count )) return false;
		$this->add_item_list=array();
		{for($i=0; $i<$add_item_list_count;$i++){
			$this->add_item_list[$i]=new item_exchange_t();
			if (!$this->add_item_list[$i]->read_from_buf($ba)) return false;
		}}

		$del_item_list_count=0 ;
		if (!$ba->read_uint32( $del_item_list_count )) return false;
		$this->del_item_list=array();
		{for($i=0; $i<$del_item_list_count;$i++){
			$this->del_item_list[$i]=new item_exchange_t();
			if (!$this->del_item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->opt_flag)) return false;
		$add_item_list_count=count($this->add_item_list);
        $ba->write_uint32($add_item_list_count);
		{for($i=0; $i<$add_item_list_count;$i++){
			if ( ! $this->add_item_list[$i] instanceof item_exchange_t ) return false; 
			if (!$this->add_item_list[$i]->write_to_buf($ba)) return false;
		}}
		$del_item_list_count=count($this->del_item_list);
        $ba->write_uint32($del_item_list_count);
		{for($i=0; $i<$del_item_list_count;$i++){
			if ( ! $this->del_item_list[$i] instanceof item_exchange_t ) return false; 
			if (!$this->del_item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class pop_find_map_add_in {
	/*  */
	#类型:uint32
	public $islandid;

	/*  */
	#类型:uint32
	public $mapid;


	public function pop_find_map_add_in(){

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

	
class user_base_info_t {
	/*  */
	#类型:uint32
	public $register_time;

	/* 0x01:用户是否注册过 */
	#类型:uint32
	public $flag;

	/*  */
	#类型:uint32
	public $xiaomee;

	/*  */
	#类型:uint32
	public $color;

	/*  */
	#类型:uint32
	public $age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/*  */
	#类型:uint32
	public $last_islandid;

	/*  */
	#类型:uint32
	public $last_mapid;

	/*  */
	#类型:uint32
	public $last_x;

	/*  */
	#类型:uint32
	public $last_y;

	/*  */
	#类型:uint32
	public $last_login;

	/*  */
	#类型:uint32
	public $online_time;


	public function user_base_info_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->register_time)) return false;
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->xiaomee)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->last_islandid)) return false;
		if (!$ba->read_uint32($this->last_mapid)) return false;
		if (!$ba->read_uint32($this->last_x)) return false;
		if (!$ba->read_uint32($this->last_y)) return false;
		if (!$ba->read_uint32($this->last_login)) return false;
		if (!$ba->read_uint32($this->online_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->register_time)) return false;
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->xiaomee)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		if (!$ba->write_uint32($this->age)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->last_islandid)) return false;
		if (!$ba->write_uint32($this->last_mapid)) return false;
		if (!$ba->write_uint32($this->last_x)) return false;
		if (!$ba->write_uint32($this->last_y)) return false;
		if (!$ba->write_uint32($this->last_login)) return false;
		if (!$ba->write_uint32($this->online_time)) return false;
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

	
class pop_get_all_info_out {
	/*  */
	#类型:user_base_info_t
	public $user_base_info;

	/**/
	#变长数组,最大长度:9999, 类型:item_ex_t
	public $item_list =array();

	/*每日获取物品列表*/
	#变长数组,最大长度:9999, 类型:item_day_limit_t
	public $item_day_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public $task_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:find_map_t
	public $find_map_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public $user_log_list =array();

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public $game_info_list =array();


	public function pop_get_all_info_out(){
		$this->user_base_info=new user_base_info_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->user_base_info->read_from_buf($ba)) return false;

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new item_ex_t();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}

		$item_day_list_count=0 ;
		if (!$ba->read_uint32( $item_day_list_count )) return false;
		$this->item_day_list=array();
		{for($i=0; $i<$item_day_list_count;$i++){
			$this->item_day_list[$i]=new item_day_limit_t();
			if (!$this->item_day_list[$i]->read_from_buf($ba)) return false;
		}}

		$task_list_count=0 ;
		if (!$ba->read_uint32( $task_list_count )) return false;
		$this->task_list=array();
		{for($i=0; $i<$task_list_count;$i++){
			$this->task_list[$i]=new task_t();
			if (!$this->task_list[$i]->read_from_buf($ba)) return false;
		}}

		$find_map_list_count=0 ;
		if (!$ba->read_uint32( $find_map_list_count )) return false;
		$this->find_map_list=array();
		{for($i=0; $i<$find_map_list_count;$i++){
			$this->find_map_list[$i]=new find_map_t();
			if (!$this->find_map_list[$i]->read_from_buf($ba)) return false;
		}}

		$user_log_list_count=0 ;
		if (!$ba->read_uint32( $user_log_list_count )) return false;
		$this->user_log_list=array();
		{for($i=0; $i<$user_log_list_count;$i++){
			$this->user_log_list[$i]=new user_log_t();
			if (!$this->user_log_list[$i]->read_from_buf($ba)) return false;
		}}

		$game_info_list_count=0 ;
		if (!$ba->read_uint32( $game_info_list_count )) return false;
		$this->game_info_list=array();
		{for($i=0; $i<$game_info_list_count;$i++){
			$this->game_info_list[$i]=new game_info_t();
			if (!$this->game_info_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->user_base_info instanceof user_base_info_t ) return false; 
		if (!$this->user_base_info->write_to_buf($ba)) return false;
		$item_list_count=count($this->item_list);
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof item_ex_t ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		$item_day_list_count=count($this->item_day_list);
        $ba->write_uint32($item_day_list_count);
		{for($i=0; $i<$item_day_list_count;$i++){
			if ( ! $this->item_day_list[$i] instanceof item_day_limit_t ) return false; 
			if (!$this->item_day_list[$i]->write_to_buf($ba)) return false;
		}}
		$task_list_count=count($this->task_list);
        $ba->write_uint32($task_list_count);
		{for($i=0; $i<$task_list_count;$i++){
			if ( ! $this->task_list[$i] instanceof task_t ) return false; 
			if (!$this->task_list[$i]->write_to_buf($ba)) return false;
		}}
		$find_map_list_count=count($this->find_map_list);
        $ba->write_uint32($find_map_list_count);
		{for($i=0; $i<$find_map_list_count;$i++){
			if ( ! $this->find_map_list[$i] instanceof find_map_t ) return false; 
			if (!$this->find_map_list[$i]->write_to_buf($ba)) return false;
		}}
		$user_log_list_count=count($this->user_log_list);
        $ba->write_uint32($user_log_list_count);
		{for($i=0; $i<$user_log_list_count;$i++){
			if ( ! $this->user_log_list[$i] instanceof user_log_t ) return false; 
			if (!$this->user_log_list[$i]->write_to_buf($ba)) return false;
		}}
		$game_info_list_count=count($this->game_info_list);
        $ba->write_uint32($game_info_list_count);
		{for($i=0; $i<$game_info_list_count;$i++){
			if ( ! $this->game_info_list[$i] instanceof game_info_t ) return false; 
			if (!$this->game_info_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class pop_get_base_info_out {
	/* 最后一次登录的服务器 */
	#类型:uint32
	public $last_online_id;


	public function pop_get_base_info_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->last_online_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->last_online_id)) return false;
		return true;
	}

};

	
class pop_login_out {
	/*  */
	#类型:user_base_info_t
	public $user_base_info;

	/**/
	#变长数组,最大长度:9999, 类型:item_ex_t
	public $item_list =array();

	/*每日获取物品列表*/
	#变长数组,最大长度:9999, 类型:item_day_limit_t
	public $item_day_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public $task_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:find_map_t
	public $find_map_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public $user_log_list =array();

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public $game_info_list =array();


	public function pop_login_out(){
		$this->user_base_info=new user_base_info_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->user_base_info->read_from_buf($ba)) return false;

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new item_ex_t();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}

		$item_day_list_count=0 ;
		if (!$ba->read_uint32( $item_day_list_count )) return false;
		$this->item_day_list=array();
		{for($i=0; $i<$item_day_list_count;$i++){
			$this->item_day_list[$i]=new item_day_limit_t();
			if (!$this->item_day_list[$i]->read_from_buf($ba)) return false;
		}}

		$task_list_count=0 ;
		if (!$ba->read_uint32( $task_list_count )) return false;
		$this->task_list=array();
		{for($i=0; $i<$task_list_count;$i++){
			$this->task_list[$i]=new task_t();
			if (!$this->task_list[$i]->read_from_buf($ba)) return false;
		}}

		$find_map_list_count=0 ;
		if (!$ba->read_uint32( $find_map_list_count )) return false;
		$this->find_map_list=array();
		{for($i=0; $i<$find_map_list_count;$i++){
			$this->find_map_list[$i]=new find_map_t();
			if (!$this->find_map_list[$i]->read_from_buf($ba)) return false;
		}}

		$user_log_list_count=0 ;
		if (!$ba->read_uint32( $user_log_list_count )) return false;
		$this->user_log_list=array();
		{for($i=0; $i<$user_log_list_count;$i++){
			$this->user_log_list[$i]=new user_log_t();
			if (!$this->user_log_list[$i]->read_from_buf($ba)) return false;
		}}

		$game_info_list_count=0 ;
		if (!$ba->read_uint32( $game_info_list_count )) return false;
		$this->game_info_list=array();
		{for($i=0; $i<$game_info_list_count;$i++){
			$this->game_info_list[$i]=new game_info_t();
			if (!$this->game_info_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->user_base_info instanceof user_base_info_t ) return false; 
		if (!$this->user_base_info->write_to_buf($ba)) return false;
		$item_list_count=count($this->item_list);
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof item_ex_t ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		$item_day_list_count=count($this->item_day_list);
        $ba->write_uint32($item_day_list_count);
		{for($i=0; $i<$item_day_list_count;$i++){
			if ( ! $this->item_day_list[$i] instanceof item_day_limit_t ) return false; 
			if (!$this->item_day_list[$i]->write_to_buf($ba)) return false;
		}}
		$task_list_count=count($this->task_list);
        $ba->write_uint32($task_list_count);
		{for($i=0; $i<$task_list_count;$i++){
			if ( ! $this->task_list[$i] instanceof task_t ) return false; 
			if (!$this->task_list[$i]->write_to_buf($ba)) return false;
		}}
		$find_map_list_count=count($this->find_map_list);
        $ba->write_uint32($find_map_list_count);
		{for($i=0; $i<$find_map_list_count;$i++){
			if ( ! $this->find_map_list[$i] instanceof find_map_t ) return false; 
			if (!$this->find_map_list[$i]->write_to_buf($ba)) return false;
		}}
		$user_log_list_count=count($this->user_log_list);
        $ba->write_uint32($user_log_list_count);
		{for($i=0; $i<$user_log_list_count;$i++){
			if ( ! $this->user_log_list[$i] instanceof user_log_t ) return false; 
			if (!$this->user_log_list[$i]->write_to_buf($ba)) return false;
		}}
		$game_info_list_count=count($this->game_info_list);
        $ba->write_uint32($game_info_list_count);
		{for($i=0; $i<$game_info_list_count;$i++){
			if ( ! $this->game_info_list[$i] instanceof game_info_t ) return false; 
			if (!$this->game_info_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class pop_logout_in {
	/*  */
	#类型:uint32
	public $last_islandid;

	/*  */
	#类型:uint32
	public $last_mapid;

	/*  */
	#类型:uint32
	public $last_x;

	/*  */
	#类型:uint32
	public $last_y;

	/*  */
	#类型:uint32
	public $last_login;

	/*  */
	#类型:uint32
	public $online_time;

	/* 最后一次登录的服务器 */
	#类型:uint32
	public $last_online_id;


	public function pop_logout_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->last_islandid)) return false;
		if (!$ba->read_uint32($this->last_mapid)) return false;
		if (!$ba->read_uint32($this->last_x)) return false;
		if (!$ba->read_uint32($this->last_y)) return false;
		if (!$ba->read_uint32($this->last_login)) return false;
		if (!$ba->read_uint32($this->online_time)) return false;
		if (!$ba->read_uint32($this->last_online_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->last_islandid)) return false;
		if (!$ba->write_uint32($this->last_mapid)) return false;
		if (!$ba->write_uint32($this->last_x)) return false;
		if (!$ba->write_uint32($this->last_y)) return false;
		if (!$ba->write_uint32($this->last_login)) return false;
		if (!$ba->write_uint32($this->online_time)) return false;
		if (!$ba->write_uint32($this->last_online_id)) return false;
		return true;
	}

};

	
class pop_opt_in {
	/* 1:物品;2任务,3find_map */
	#类型:uint32
	public $opt_groupid;

	/* 1:增加/减少,2:设置 */
	#类型:uint32
	public $opt_type;

	/* 操作物的id（如ItemID） */
	#类型:uint32
	public $optid;

	/* 改变的数量 */
	#类型:int32
	public $count;

	/*  */
	#类型:uint32
	public $v1;

	/*  */
	#类型:uint32
	public $v2;


	public function pop_opt_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->opt_groupid)) return false;
		if (!$ba->read_uint32($this->opt_type)) return false;
		if (!$ba->read_uint32($this->optid)) return false;
		if (!$ba->read_int32($this->count)) return false;
		if (!$ba->read_uint32($this->v1)) return false;
		if (!$ba->read_uint32($this->v2)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->opt_groupid)) return false;
		if (!$ba->write_uint32($this->opt_type)) return false;
		if (!$ba->write_uint32($this->optid)) return false;
		if (!$ba->write_int32($this->count)) return false;
		if (!$ba->write_uint32($this->v1)) return false;
		if (!$ba->write_uint32($this->v2)) return false;
		return true;
	}

};

	
class pop_reg_in {
	/*  */
	#类型:uint32
	public $color;

	/*  */
	#类型:uint32
	public $age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 0x01:用户是否注册过 0x02是否新米米号 */
	#类型:uint32
	public $flag;

	/**/
	#变长数组,最大长度:9999, 类型:item_exchange_t
	public $add_item_list =array();


	public function pop_reg_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->color)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->flag)) return false;

		$add_item_list_count=0 ;
		if (!$ba->read_uint32( $add_item_list_count )) return false;
		$this->add_item_list=array();
		{for($i=0; $i<$add_item_list_count;$i++){
			$this->add_item_list[$i]=new item_exchange_t();
			if (!$this->add_item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->color)) return false;
		if (!$ba->write_uint32($this->age)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->flag)) return false;
		$add_item_list_count=count($this->add_item_list);
        $ba->write_uint32($add_item_list_count);
		{for($i=0; $i<$add_item_list_count;$i++){
			if ( ! $this->add_item_list[$i] instanceof item_exchange_t ) return false; 
			if (!$this->add_item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class pop_set_color_in {
	/*  */
	#类型:uint32
	public $color;


	public function pop_set_color_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->color)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->color)) return false;
		return true;
	}

};

	
class pop_set_flag_in {
	/* 0x01:用户是否注册过 0x02是否新米米号 */
	#类型:uint32
	public $flag;

	/* 掩码 */
	#类型:uint32
	public $mask;


	public function pop_set_flag_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->mask)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->mask)) return false;
		return true;
	}

};

	
class pop_set_item_used_list_in {
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $del_can_change_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $set_noused_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $add_can_change_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $set_used_list =array();


	public function pop_set_item_used_list_in(){

	}

	public function read_from_buf($ba ){

		$del_can_change_list_count=0 ;
		if (!$ba->read_uint32( $del_can_change_list_count )) return false;
		$this->del_can_change_list=array();
		{for($i=0; $i<$del_can_change_list_count;$i++){
			if (!$ba->read_uint32($this->del_can_change_list[$i])) return false;
		}}

		$set_noused_list_count=0 ;
		if (!$ba->read_uint32( $set_noused_list_count )) return false;
		$this->set_noused_list=array();
		{for($i=0; $i<$set_noused_list_count;$i++){
			if (!$ba->read_uint32($this->set_noused_list[$i])) return false;
		}}

		$add_can_change_list_count=0 ;
		if (!$ba->read_uint32( $add_can_change_list_count )) return false;
		$this->add_can_change_list=array();
		{for($i=0; $i<$add_can_change_list_count;$i++){
			if (!$ba->read_uint32($this->add_can_change_list[$i])) return false;
		}}

		$set_used_list_count=0 ;
		if (!$ba->read_uint32( $set_used_list_count )) return false;
		$this->set_used_list=array();
		{for($i=0; $i<$set_used_list_count;$i++){
			if (!$ba->read_uint32($this->set_used_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$del_can_change_list_count=count($this->del_can_change_list);
        $ba->write_uint32($del_can_change_list_count);
		{for($i=0; $i<$del_can_change_list_count;$i++){
			if (!$ba->write_uint32($this->del_can_change_list[$i])) return false;
		}}
		$set_noused_list_count=count($this->set_noused_list);
        $ba->write_uint32($set_noused_list_count);
		{for($i=0; $i<$set_noused_list_count;$i++){
			if (!$ba->write_uint32($this->set_noused_list[$i])) return false;
		}}
		$add_can_change_list_count=count($this->add_can_change_list);
        $ba->write_uint32($add_can_change_list_count);
		{for($i=0; $i<$add_can_change_list_count;$i++){
			if (!$ba->write_uint32($this->add_can_change_list[$i])) return false;
		}}
		$set_used_list_count=count($this->set_used_list);
        $ba->write_uint32($set_used_list_count);
		{for($i=0; $i<$set_used_list_count;$i++){
			if (!$ba->write_uint32($this->set_used_list[$i])) return false;
		}}
		return true;
	}

};

	
class pop_set_nick_in {
	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;


	public function pop_set_nick_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->nick,16)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->nick,16);
		return true;
	}

};

	
class pop_task_complete_node_in {
	/*  */
	#类型:uint32
	public $taskid;

	/*  */
	#类型:uint32
	public $task_nodeid;


	public function pop_task_complete_node_in(){

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

	
class pop_task_del_in {
	/*  */
	#类型:uint32
	public $taskid;

	/*  */
	#类型:uint32
	public $task_nodeid;


	public function pop_task_del_in(){

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

	
class pop_user_log_add_in {
	/*  */
	#类型:user_log_t
	public $user_log;


	public function pop_user_log_add_in(){
		$this->user_log=new user_log_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->user_log->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->user_log instanceof user_log_t ) return false; 
		if (!$this->user_log->write_to_buf($ba)) return false;
		return true;
	}

};

	
class pop_user_set_field_value_in {
	/* 字段名 */
	#定长数组,长度:64, 类型:char 
	public $field_name ;

	/* 字段值 */
	#定长数组,长度:255, 类型:char 
	public $field_value ;


	public function pop_user_set_field_value_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->field_name,64)) return false;
		if(!$ba->read_buf($this->field_value,255)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->field_name,64);
		$ba->write_buf($this->field_value,255);
		return true;
	}

};

	
class udp_post_msg_in {
	/* 项目编号 */
	#类型:uint32
	public $gameid;

	/* 无效填0 */
	#类型:uint32
	public $nouse;

	/* 投稿类型 */
	#类型:uint32
	public $type;

	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 主题 */
	#定长数组,长度:60, 类型:char 
	public $title ;

	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public $msg ;


	public function udp_post_msg_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->nouse)) return false;
		if (!$ba->read_uint32($this->type)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if(!$ba->read_buf($this->title,60)) return false;

		$msg_count=0 ;
		if (!$ba->read_uint32( $msg_count )) return false;
		if(!$ba->read_buf($this->msg,$msg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->nouse)) return false;
		if (!$ba->write_uint32($this->type)) return false;
		$ba->write_buf($this->nick,16);
		$ba->write_buf($this->title,60);
		$msg_count=strlen($this->msg);
		$ba->write_uint32($msg_count);
		$ba->write_buf($this->msg,$msg_count);
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

	
class Cpop_db_proto  extends Cproto_base {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }

	/* 得到版本信息 */

	function get_server_version($userid ){

		return $this->send_cmd_new(0x1000,$userid, null, new get_server_version_out(), 0xe3716819);
	}
	
	/* 登入 */

	function pop_login($userid ){

		return $this->send_cmd_new(0x7401,$userid, null, new pop_login_out(), 0x02822bfc);
	}
	
	/* 注册 */
	/* 调用方式还可以是： 
		$in=new pop_reg_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_reg($userid,$in );
	*/

	function pop_reg($userid , $color=null, $age=null, $nick=null, $flag=null, $add_item_list=null){

		if ( $color instanceof pop_reg_in ){
			$in=$color;
		}else{
			$in=new pop_reg_in();
			$in->color=$color;
			$in->age=$age;
			$in->nick=$nick;
			$in->flag=$flag;
			$in->add_item_list=$add_item_list;

		}
		
		return $this->send_cmd_new(0x7402,$userid, $in, null, 0xb792ae08);
	}
	
	/* 设置昵称 */
	/* 调用方式还可以是： 
		$in=new pop_set_nick_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_set_nick($userid,$in );
	*/

	function pop_set_nick($userid , $nick=null){

		if ( $nick instanceof pop_set_nick_in ){
			$in=$nick;
		}else{
			$in=new pop_set_nick_in();
			$in->nick=$nick;

		}
		
		return $this->send_cmd_new(0x7403,$userid, $in, null, 0x874eb534);
	}
	
	/* 复制用户 */
	/* 调用方式还可以是： 
		$in=new pop_copy_user_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_copy_user($userid,$in );
	*/

	function pop_copy_user($userid , $dsc_userid=null){

		if ( $dsc_userid instanceof pop_copy_user_in ){
			$in=$dsc_userid;
		}else{
			$in=new pop_copy_user_in();
			$in->dsc_userid=$dsc_userid;

		}
		
		return $this->send_cmd_new(0x7404,$userid, $in, null, 0x5394bd57);
	}
	
	/*  */

	function pop_get_base_info($userid ){

		return $this->send_cmd_new(0x7405,$userid, null, new pop_get_base_info_out(), 0xd9a8fc35);
	}
	
	/* 设置用户标识 */
	/* 调用方式还可以是： 
		$in=new pop_set_flag_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_set_flag($userid,$in );
	*/

	function pop_set_flag($userid , $flag=null, $mask=null){

		if ( $flag instanceof pop_set_flag_in ){
			$in=$flag;
		}else{
			$in=new pop_set_flag_in();
			$in->flag=$flag;
			$in->mask=$mask;

		}
		
		return $this->send_cmd_new(0x7406,$userid, $in, null, 0x13813167);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new pop_set_color_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_set_color($userid,$in );
	*/

	function pop_set_color($userid , $color=null){

		if ( $color instanceof pop_set_color_in ){
			$in=$color;
		}else{
			$in=new pop_set_color_in();
			$in->color=$color;

		}
		
		return $this->send_cmd_new(0x7407,$userid, $in, null, 0x941960fe);
	}
	
	/* 交换协议，物品，任务，钱 */
	/* 调用方式还可以是： 
		$in=new pop_exchange_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_exchange($userid,$in );
	*/

	function pop_exchange($userid , $opt_flag=null, $add_item_list=null, $del_item_list=null){

		if ( $opt_flag instanceof pop_exchange_in ){
			$in=$opt_flag;
		}else{
			$in=new pop_exchange_in();
			$in->opt_flag=$opt_flag;
			$in->add_item_list=$add_item_list;
			$in->del_item_list=$del_item_list;

		}
		
		return $this->send_cmd_new(0x740A,$userid, $in, null, 0x79a82f26);
	}
	
	/* 设置使用列表，会将原有使用的设置为未使用 */
	/* 调用方式还可以是： 
		$in=new pop_set_item_used_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_set_item_used_list($userid,$in );
	*/

	function pop_set_item_used_list($userid , $del_can_change_list=null, $set_noused_list=null, $add_can_change_list=null, $set_used_list=null){

		if ( $del_can_change_list instanceof pop_set_item_used_list_in ){
			$in=$del_can_change_list;
		}else{
			$in=new pop_set_item_used_list_in();
			$in->del_can_change_list=$del_can_change_list;
			$in->set_noused_list=$set_noused_list;
			$in->add_can_change_list=$add_can_change_list;
			$in->set_used_list=$set_used_list;

		}
		
		return $this->send_cmd_new(0x740C,$userid, $in, null, 0xc71f127c);
	}
	
	/* 完成子节点任务 */
	/* 调用方式还可以是： 
		$in=new pop_task_complete_node_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_task_complete_node($userid,$in );
	*/

	function pop_task_complete_node($userid , $taskid=null, $task_nodeid=null){

		if ( $taskid instanceof pop_task_complete_node_in ){
			$in=$taskid;
		}else{
			$in=new pop_task_complete_node_in();
			$in->taskid=$taskid;
			$in->task_nodeid=$task_nodeid;

		}
		
		return $this->send_cmd_new(0x7410,$userid, $in, null, 0x18cc6534);
	}
	
	/* 删除任务 */
	/* 调用方式还可以是： 
		$in=new pop_task_del_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_task_del($userid,$in );
	*/

	function pop_task_del($userid , $taskid=null, $task_nodeid=null){

		if ( $taskid instanceof pop_task_del_in ){
			$in=$taskid;
		}else{
			$in=new pop_task_del_in();
			$in->taskid=$taskid;
			$in->task_nodeid=$task_nodeid;

		}
		
		return $this->send_cmd_new(0x7411,$userid, $in, null, 0x4f39999d);
	}
	
	/* 找到哪个地图 */
	/* 调用方式还可以是： 
		$in=new pop_find_map_add_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_find_map_add($userid,$in );
	*/

	function pop_find_map_add($userid , $islandid=null, $mapid=null){

		if ( $islandid instanceof pop_find_map_add_in ){
			$in=$islandid;
		}else{
			$in=new pop_find_map_add_in();
			$in->islandid=$islandid;
			$in->mapid=$mapid;

		}
		
		return $this->send_cmd_new(0x741A,$userid, $in, null, 0x4d028768);
	}
	
	/* 退出 */
	/* 调用方式还可以是： 
		$in=new pop_logout_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_logout($userid,$in );
	*/

	function pop_logout($userid , $last_islandid=null, $last_mapid=null, $last_x=null, $last_y=null, $last_login=null, $online_time=null, $last_online_id=null){

		if ( $last_islandid instanceof pop_logout_in ){
			$in=$last_islandid;
		}else{
			$in=new pop_logout_in();
			$in->last_islandid=$last_islandid;
			$in->last_mapid=$last_mapid;
			$in->last_x=$last_x;
			$in->last_y=$last_y;
			$in->last_login=$last_login;
			$in->online_time=$online_time;
			$in->last_online_id=$last_online_id;

		}
		
		return $this->send_cmd_new(0x741B,$userid, $in, null, 0x4d68721b);
	}
	
	/* 用户足迹 */
	/* 调用方式还可以是： 
		$in=new pop_user_log_add_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_user_log_add($userid,$in );
	*/

	function pop_user_log_add($userid , $user_log=null){

		if ( $user_log instanceof pop_user_log_add_in ){
			$in=$user_log;
		}else{
			$in=new pop_user_log_add_in();
			$in->user_log=$user_log;

		}
		
		return $this->send_cmd_new(0x741C,$userid, $in, null, 0xb2be2b7b);
	}
	
	/* 得到用户的所有信息 */

	function pop_get_all_info($userid ){

		return $this->send_cmd_new(0x74A0,$userid, null, new pop_get_all_info_out(), 0x151b1b12);
	}
	
	/* 设置user表某一字段的值 */
	/* 调用方式还可以是： 
		$in=new pop_user_set_field_value_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_user_set_field_value($userid,$in );
	*/

	function pop_user_set_field_value($userid , $field_name=null, $field_value=null){

		if ( $field_name instanceof pop_user_set_field_value_in ){
			$in=$field_name;
		}else{
			$in=new pop_user_set_field_value_in();
			$in->field_name=$field_name;
			$in->field_value=$field_value;

		}
		
		return $this->send_cmd_new(0x74A1,$userid, $in, null, 0x50ea630f);
	}
	
	/* 设置相关的值 */
	/* 调用方式还可以是： 
		$in=new pop_opt_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_opt($userid,$in );
	*/

	function pop_opt($userid , $opt_groupid=null, $opt_type=null, $optid=null, $count=null, $v1=null, $v2=null){

		if ( $opt_groupid instanceof pop_opt_in ){
			$in=$opt_groupid;
		}else{
			$in=new pop_opt_in();
			$in->opt_groupid=$opt_groupid;
			$in->opt_type=$opt_type;
			$in->optid=$optid;
			$in->count=$count;
			$in->v1=$v1;
			$in->v2=$v2;

		}
		
		return $this->send_cmd_new(0x74A2,$userid, $in, null, 0x392d057e);
	}
	
	/* 增加小游戏统计 */
	/* 调用方式还可以是： 
		$in=new pop_add_game_info_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pop_add_game_info($userid,$in );
	*/

	function pop_add_game_info($userid , $gameid=null, $win_flag=null){

		if ( $gameid instanceof pop_add_game_info_in ){
			$in=$gameid;
		}else{
			$in=new pop_add_game_info_in();
			$in->gameid=$gameid;
			$in->win_flag=$win_flag;

		}
		
		return $this->send_cmd_new(0x74A4,$userid, $in, null, 0x95f2cb99);
	}
	
	/* udp 投稿 */
	/* 调用方式还可以是： 
		$in=new udp_post_msg_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->udp_post_msg($userid,$in );
	*/

	function udp_post_msg($userid , $gameid=null, $nouse=null, $type=null, $nick=null, $title=null, $msg=null){

		if ( $gameid instanceof udp_post_msg_in ){
			$in=$gameid;
		}else{
			$in=new udp_post_msg_in();
			$in->gameid=$gameid;
			$in->nouse=$nouse;
			$in->type=$type;
			$in->nick=$nick;
			$in->title=$title;
			$in->msg=$msg;

		}
		
		return $this->send_cmd_new(0xF130,$userid, $in, null, 0xbe74ec43);
	}
	
};
?>
