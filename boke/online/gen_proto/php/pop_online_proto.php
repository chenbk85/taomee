<?php
require_once("proto_base.php");

class cli_buy_item_in {
	/* 物品id */
	#类型:uint32
	public $itemid;


	public function cli_buy_item_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		return true;
	}

};

	
class cli_buy_item_out {
	/* 剩余多少钱 */
	#类型:uint32
	public $left_xiaomee;


	public function cli_buy_item_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->left_xiaomee)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->left_xiaomee)) return false;
		return true;
	}

};

	
class cli_buy_item_use_gamept_in {
	/* 物品ID */
	#类型:uint32
	public $itemid;


	public function cli_buy_item_use_gamept_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		return true;
	}

};

	
class cli_buy_item_use_gamept_out {
	/* 剩余可用游戏积分 */
	#类型:uint32
	public $left_gamept;


	public function cli_buy_item_use_gamept_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->left_gamept)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->left_gamept)) return false;
		return true;
	}

};

	
class cli_click_stat_in {
	/* 点击类型（1蜘蛛 2石碑） */
	#类型:uint32
	public $type;


	public function cli_click_stat_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		return true;
	}

};

	
class cli_cur_game_end_in {
	/* 自己是不是羸了 :0:不是,1:是,2:平局 */
	#类型:uint32
	public $win_flag;


	public function cli_cur_game_end_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->win_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->win_flag)) return false;
		return true;
	}

};

	
class item_t {
	/* 物品id */
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

	
class cli_del_item_in {
	/*要删除的物品列表*/
	#变长数组,最大长度:999, 类型:item_t
	public $itemlist =array();


	public function cli_del_item_in(){

	}

	public function read_from_buf($ba ){

		$itemlist_count=0 ;
		if (!$ba->read_uint32( $itemlist_count )) return false;
		$this->itemlist=array();
		{for($i=0; $i<$itemlist_count;$i++){
			$this->itemlist[$i]=new item_t();
			if (!$this->itemlist[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$itemlist_count=count($this->itemlist);
        $ba->write_uint32($itemlist_count);
		{for($i=0; $i<$itemlist_count;$i++){
			if ( ! $this->itemlist[$i] instanceof item_t ) return false; 
			if (!$this->itemlist[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_draw_lottery_out {
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public $item_get;


	public function cli_draw_lottery_out(){
		$this->item_get=new item_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->item_get->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->item_get instanceof item_t ) return false; 
		if (!$this->item_get->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_find_map_add_in {
	/*  */
	#类型:uint32
	public $islandid;

	/*  */
	#类型:uint32
	public $mapid;


	public function cli_find_map_add_in(){

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

	
class cli_game_opt_in {
	/*游戏操作信息*/
	#变长数组,最大长度:9999, 类型:char
	public $gamemsg ;


	public function cli_game_opt_in(){

	}

	public function read_from_buf($ba ){

		$gamemsg_count=0 ;
		if (!$ba->read_uint32( $gamemsg_count )) return false;
		if(!$ba->read_buf($this->gamemsg,$gamemsg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		$gamemsg_count=strlen($this->gamemsg);
		$ba->write_uint32($gamemsg_count);
		$ba->write_buf($this->gamemsg,$gamemsg_count);
		return true;
	}

};

	
class cli_game_play_with_other_in {
	/*  */
	#类型:uint32
	public $gameid;

	/* 该请求的key, 在 确认时将之一起发出 */
	#定长数组,长度:32, 类型:char 
	public $request_session ;

	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/* 是否开始,0:否(终止游戏), 1:是 */
	#类型:uint32
	public $is_start;

	/* 为什么不玩, 1:自己关，2：在换装，3：购物 */
	#类型:uint32
	public $why_not_start_flag;


	public function cli_game_play_with_other_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if(!$ba->read_buf($this->request_session,32)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;
		if (!$ba->read_uint32($this->is_start)) return false;
		if (!$ba->read_uint32($this->why_not_start_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		$ba->write_buf($this->request_session,32);
		if (!$ba->write_uint32($this->obj_userid)) return false;
		if (!$ba->write_uint32($this->is_start)) return false;
		if (!$ba->write_uint32($this->why_not_start_flag)) return false;
		return true;
	}

};

	
class cli_game_play_with_other_out {
	/* 1:不存地图，2:已经在其它游戏中 */
	#类型:uint32
	public $obj_user_stat;


	public function cli_game_play_with_other_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_user_stat)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_user_stat)) return false;
		return true;
	}

};

	
class cli_game_request_in {
	/*  */
	#类型:uint32
	public $gameid;

	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_game_request_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_game_request_out {
	/* 1:不存地图，2:已经在其它游戏中 */
	#类型:uint32
	public $obj_user_stat;


	public function cli_game_request_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_user_stat)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_user_stat)) return false;
		return true;
	}

};

	
class online_user_info_t {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/* 肤色 */
	#类型:uint32
	public $color;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 卜克豆 */
	#类型:uint32
	public $xiaomee;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验值 */
	#类型:uint32
	public $experience;

	/*  */
	#类型:int32
	public $x;

	/*  */
	#类型:int32
	public $y;

	/*勋章个数*/
	#变长数组,最大长度:100, 类型:uint32_t
	public $medal_list =array();

	/* 小游戏积分 */
	#类型:uint32
	public $game_point;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public $effect_list =array();


	public function online_user_info_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->xiaomee)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->experience)) return false;
		if (!$ba->read_int32($this->x)) return false;
		if (!$ba->read_int32($this->y)) return false;

		$medal_list_count=0 ;
		if (!$ba->read_uint32( $medal_list_count )) return false;
		$this->medal_list=array();
		{for($i=0; $i<$medal_list_count;$i++){
			if (!$ba->read_uint32($this->medal_list[$i])) return false;
		}}
		if (!$ba->read_uint32($this->game_point)) return false;

		$use_clothes_list_count=0 ;
		if (!$ba->read_uint32( $use_clothes_list_count )) return false;
		$this->use_clothes_list=array();
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->read_uint32($this->use_clothes_list[$i])) return false;
		}}

		$effect_list_count=0 ;
		if (!$ba->read_uint32( $effect_list_count )) return false;
		$this->effect_list=array();
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->read_uint32($this->effect_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->xiaomee)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->experience)) return false;
		if (!$ba->write_int32($this->x)) return false;
		if (!$ba->write_int32($this->y)) return false;
		$medal_list_count=count($this->medal_list);
        $ba->write_uint32($medal_list_count);
		{for($i=0; $i<$medal_list_count;$i++){
			if (!$ba->write_uint32($this->medal_list[$i])) return false;
		}}
		if (!$ba->write_uint32($this->game_point)) return false;
		$use_clothes_list_count=count($this->use_clothes_list);
        $ba->write_uint32($use_clothes_list_count);
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->write_uint32($this->use_clothes_list[$i])) return false;
		}}
		$effect_list_count=count($this->effect_list);
        $ba->write_uint32($effect_list_count);
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->write_uint32($this->effect_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_get_all_user_info_from_cur_map_out {
	/**/
	#变长数组,最大长度:9999, 类型:online_user_info_t
	public $online_user_info_list =array();


	public function cli_get_all_user_info_from_cur_map_out(){

	}

	public function read_from_buf($ba ){

		$online_user_info_list_count=0 ;
		if (!$ba->read_uint32( $online_user_info_list_count )) return false;
		$this->online_user_info_list=array();
		{for($i=0; $i<$online_user_info_list_count;$i++){
			$this->online_user_info_list[$i]=new online_user_info_t();
			if (!$this->online_user_info_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$online_user_info_list_count=count($this->online_user_info_list);
        $ba->write_uint32($online_user_info_list_count);
		{for($i=0; $i<$online_user_info_list_count;$i++){
			if ( ! $this->online_user_info_list[$i] instanceof online_user_info_t ) return false; 
			if (!$this->online_user_info_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_get_card_list_by_islandid_in {
	/*  */
	#类型:uint32
	public $islandid;


	public function cli_get_card_list_by_islandid_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		return true;
	}

};

	
class cli_get_card_list_by_islandid_out {
	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public $cardid_list =array();


	public function cli_get_card_list_by_islandid_out(){

	}

	public function read_from_buf($ba ){

		$cardid_list_count=0 ;
		if (!$ba->read_uint32( $cardid_list_count )) return false;
		$this->cardid_list=array();
		{for($i=0; $i<$cardid_list_count;$i++){
			$this->cardid_list[$i]=new item_t();
			if (!$this->cardid_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$cardid_list_count=count($this->cardid_list);
        $ba->write_uint32($cardid_list_count);
		{for($i=0; $i<$cardid_list_count;$i++){
			if ( ! $this->cardid_list[$i] instanceof item_t ) return false; 
			if (!$this->cardid_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class game_user_t {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/* 项目编号 */
	#类型:uint32
	public $gameid;

	/*游戏中的其他玩家列表*/
	#变长数组,最大长度:10, 类型:uint32_t
	public $userlist =array();


	public function game_user_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;
		if (!$ba->read_uint32($this->gameid)) return false;

		$userlist_count=0 ;
		if (!$ba->read_uint32( $userlist_count )) return false;
		$this->userlist=array();
		{for($i=0; $i<$userlist_count;$i++){
			if (!$ba->read_uint32($this->userlist[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		if (!$ba->write_uint32($this->gameid)) return false;
		$userlist_count=count($this->userlist);
        $ba->write_uint32($userlist_count);
		{for($i=0; $i<$userlist_count;$i++){
			if (!$ba->write_uint32($this->userlist[$i])) return false;
		}}
		return true;
	}

};

	
class cli_get_game_user_out {
	/*游戏中的用户列表*/
	#变长数组,最大长度:999, 类型:game_user_t
	public $game_user_list =array();


	public function cli_get_game_user_out(){

	}

	public function read_from_buf($ba ){

		$game_user_list_count=0 ;
		if (!$ba->read_uint32( $game_user_list_count )) return false;
		$this->game_user_list=array();
		{for($i=0; $i<$game_user_list_count;$i++){
			$this->game_user_list[$i]=new game_user_t();
			if (!$this->game_user_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$game_user_list_count=count($this->game_user_list);
        $ba->write_uint32($game_user_list_count);
		{for($i=0; $i<$game_user_list_count;$i++){
			if ( ! $this->game_user_list[$i] instanceof game_user_t ) return false; 
			if (!$this->game_user_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class island_time_t {
	/* 岛屿ID */
	#类型:uint32
	public $islandid;

	/* 当前是否开放 */
	#类型:uint32
	public $is_open;

	/* 下次状态改变时间 */
	#类型:uint32
	public $next_state_time;


	public function island_time_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->is_open)) return false;
		if (!$ba->read_uint32($this->next_state_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->is_open)) return false;
		if (!$ba->write_uint32($this->next_state_time)) return false;
		return true;
	}

};

	
class cli_get_island_time_out {
	/* 当前服务器时间 */
	#类型:uint32
	public $svr_time;

	/*岛屿开放时间列表*/
	#变长数组,最大长度:100, 类型:island_time_t
	public $islandlist =array();


	public function cli_get_island_time_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->svr_time)) return false;

		$islandlist_count=0 ;
		if (!$ba->read_uint32( $islandlist_count )) return false;
		$this->islandlist=array();
		{for($i=0; $i<$islandlist_count;$i++){
			$this->islandlist[$i]=new island_time_t();
			if (!$this->islandlist[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->svr_time)) return false;
		$islandlist_count=count($this->islandlist);
        $ba->write_uint32($islandlist_count);
		{for($i=0; $i<$islandlist_count;$i++){
			if ( ! $this->islandlist[$i] instanceof island_time_t ) return false; 
			if (!$this->islandlist[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_get_item_list_in {
	/*  */
	#类型:uint32
	public $startid;

	/*  */
	#类型:uint32
	public $endid;


	public function cli_get_item_list_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->startid)) return false;
		if (!$ba->read_uint32($this->endid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->startid)) return false;
		if (!$ba->write_uint32($this->endid)) return false;
		return true;
	}

};

	
class cli_get_item_list_out {
	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public $item_list =array();


	public function cli_get_item_list_out(){

	}

	public function read_from_buf($ba ){

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new item_t();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$item_list_count=count($this->item_list);
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof item_t ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_get_login_chest_out {
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public $item_get;


	public function cli_get_login_chest_out(){
		$this->item_get=new item_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->item_get->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->item_get instanceof item_t ) return false; 
		if (!$this->item_get->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_get_login_reward_in {
	/* 序号（0~6） */
	#类型:uint32
	public $index;


	public function cli_get_login_reward_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->index)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->index)) return false;
		return true;
	}

};

	
class cli_get_login_reward_out {
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public $item_get;


	public function cli_get_login_reward_out(){
		$this->item_get=new item_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->item_get->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->item_get instanceof item_t ) return false; 
		if (!$this->item_get->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_get_lottery_count_out {
	/*  */
	#类型:uint32
	public $count;


	public function cli_get_lottery_count_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class cli_get_spec_item_in {
	/* 物品ID（3战斗积分4在线时长5打工次数8圣诞礼券） */
	#类型:uint32
	public $itemid;


	public function cli_get_spec_item_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		return true;
	}

};

	
class cli_get_spec_item_list_in {
	/*  */
	#类型:uint32
	public $startid;

	/*  */
	#类型:uint32
	public $endid;


	public function cli_get_spec_item_list_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->startid)) return false;
		if (!$ba->read_uint32($this->endid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->startid)) return false;
		if (!$ba->write_uint32($this->endid)) return false;
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

	
class cli_get_spec_item_list_out {
	/**/
	#变长数组,最大长度:1000, 类型:item_day_limit_t
	public $spec_item_list =array();


	public function cli_get_spec_item_list_out(){

	}

	public function read_from_buf($ba ){

		$spec_item_list_count=0 ;
		if (!$ba->read_uint32( $spec_item_list_count )) return false;
		$this->spec_item_list=array();
		{for($i=0; $i<$spec_item_list_count;$i++){
			$this->spec_item_list[$i]=new item_day_limit_t();
			if (!$this->spec_item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$spec_item_list_count=count($this->spec_item_list);
        $ba->write_uint32($spec_item_list_count);
		{for($i=0; $i<$spec_item_list_count;$i++){
			if ( ! $this->spec_item_list[$i] instanceof item_day_limit_t ) return false; 
			if (!$this->spec_item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_get_spec_item_out {
	/* 物品数量 */
	#类型:item_day_limit_t
	public $item;


	public function cli_get_spec_item_out(){
		$this->item=new item_day_limit_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->item->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->item instanceof item_day_limit_t ) return false; 
		if (!$this->item->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_get_user_game_stat_in {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_get_user_game_stat_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
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

	
class cli_get_user_game_stat_out {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/*小游戏统计信息*/
	#变长数组,最大长度:9999, 类型:game_info_t
	public $game_stat_list =array();


	public function cli_get_user_game_stat_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;

		$game_stat_list_count=0 ;
		if (!$ba->read_uint32( $game_stat_list_count )) return false;
		$this->game_stat_list=array();
		{for($i=0; $i<$game_stat_list_count;$i++){
			$this->game_stat_list[$i]=new game_info_t();
			if (!$this->game_stat_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		$game_stat_list_count=count($this->game_stat_list);
        $ba->write_uint32($game_stat_list_count);
		{for($i=0; $i<$game_stat_list_count;$i++){
			if ( ! $this->game_stat_list[$i] instanceof game_info_t ) return false; 
			if (!$this->game_stat_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_get_user_info_in {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_get_user_info_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
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

	
class cli_get_user_info_out {
	/*  */
	#类型:uint32
	public $regtime;

	/* 肤色 */
	#类型:uint32
	public $color;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 剩余多少钱 */
	#类型:uint32
	public $left_xiaomee;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验值 */
	#类型:uint32
	public $experience;

	/* 小游戏积分 */
	#类型:uint32
	public $game_point;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public $effect_list =array();

	/*用户足迹列表*/
	#变长数组,最大长度:9999, 类型:user_log_t
	public $user_log_list =array();

	/*完成 岛的列表*/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $complete_islandid_list =array();


	public function cli_get_user_info_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->regtime)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->left_xiaomee)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->experience)) return false;
		if (!$ba->read_uint32($this->game_point)) return false;

		$use_clothes_list_count=0 ;
		if (!$ba->read_uint32( $use_clothes_list_count )) return false;
		$this->use_clothes_list=array();
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->read_uint32($this->use_clothes_list[$i])) return false;
		}}

		$effect_list_count=0 ;
		if (!$ba->read_uint32( $effect_list_count )) return false;
		$this->effect_list=array();
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->read_uint32($this->effect_list[$i])) return false;
		}}

		$user_log_list_count=0 ;
		if (!$ba->read_uint32( $user_log_list_count )) return false;
		$this->user_log_list=array();
		{for($i=0; $i<$user_log_list_count;$i++){
			$this->user_log_list[$i]=new user_log_t();
			if (!$this->user_log_list[$i]->read_from_buf($ba)) return false;
		}}

		$complete_islandid_list_count=0 ;
		if (!$ba->read_uint32( $complete_islandid_list_count )) return false;
		$this->complete_islandid_list=array();
		{for($i=0; $i<$complete_islandid_list_count;$i++){
			if (!$ba->read_uint32($this->complete_islandid_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->regtime)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->left_xiaomee)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->experience)) return false;
		if (!$ba->write_uint32($this->game_point)) return false;
		$use_clothes_list_count=count($this->use_clothes_list);
        $ba->write_uint32($use_clothes_list_count);
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->write_uint32($this->use_clothes_list[$i])) return false;
		}}
		$effect_list_count=count($this->effect_list);
        $ba->write_uint32($effect_list_count);
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->write_uint32($this->effect_list[$i])) return false;
		}}
		$user_log_list_count=count($this->user_log_list);
        $ba->write_uint32($user_log_list_count);
		{for($i=0; $i<$user_log_list_count;$i++){
			if ( ! $this->user_log_list[$i] instanceof user_log_t ) return false; 
			if (!$this->user_log_list[$i]->write_to_buf($ba)) return false;
		}}
		$complete_islandid_list_count=count($this->complete_islandid_list);
        $ba->write_uint32($complete_islandid_list_count);
		{for($i=0; $i<$complete_islandid_list_count;$i++){
			if (!$ba->write_uint32($this->complete_islandid_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_get_user_island_find_map_info_in {
	/*  */
	#类型:uint32
	public $islandid;


	public function cli_get_user_island_find_map_info_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		return true;
	}

};

	
class cli_get_user_island_find_map_info_out {
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $find_map_list =array();


	public function cli_get_user_island_find_map_info_out(){

	}

	public function read_from_buf($ba ){

		$find_map_list_count=0 ;
		if (!$ba->read_uint32( $find_map_list_count )) return false;
		$this->find_map_list=array();
		{for($i=0; $i<$find_map_list_count;$i++){
			if (!$ba->read_uint32($this->find_map_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$find_map_list_count=count($this->find_map_list);
        $ba->write_uint32($find_map_list_count);
		{for($i=0; $i<$find_map_list_count;$i++){
			if (!$ba->write_uint32($this->find_map_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_get_user_island_task_info_in {
	/*  */
	#类型:uint32
	public $islandid;


	public function cli_get_user_island_task_info_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		return true;
	}

};

	
class cli_get_user_island_task_info_out {
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $task_nodeid_list =array();


	public function cli_get_user_island_task_info_out(){

	}

	public function read_from_buf($ba ){

		$task_nodeid_list_count=0 ;
		if (!$ba->read_uint32( $task_nodeid_list_count )) return false;
		$this->task_nodeid_list=array();
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->read_uint32($this->task_nodeid_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$task_nodeid_list_count=count($this->task_nodeid_list);
        $ba->write_uint32($task_nodeid_list_count);
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->write_uint32($this->task_nodeid_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_get_valid_gamept_out {
	/* 剩余可用游戏积分 */
	#类型:uint32
	public $left_gamept;


	public function cli_get_valid_gamept_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->left_gamept)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->left_gamept)) return false;
		return true;
	}

};

	
class cli_hang_bell_get_item_in {
	/* 铃铛类型(0五彩1金色2银色) */
	#类型:uint32
	public $bell_type;

	/* 投中区域(0蓝1绿2红3没投中) */
	#类型:uint32
	public $zone_type;


	public function cli_hang_bell_get_item_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->bell_type)) return false;
		if (!$ba->read_uint32($this->zone_type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->bell_type)) return false;
		if (!$ba->write_uint32($this->zone_type)) return false;
		return true;
	}

};

	
class cli_hang_bell_get_item_out {
	/* 获取的物品（2卜克豆） */
	#类型:item_t
	public $item_get;


	public function cli_hang_bell_get_item_out(){
		$this->item_get=new item_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->item_get->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->item_get instanceof item_t ) return false; 
		if (!$this->item_get->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_login_in {
	/* 服务器编号 */
	#类型:uint32
	public $server_id;

	/* 米米号 */
	#类型:uint32
	public $login_userid;

	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public $session ;


	public function cli_login_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->server_id)) return false;
		if (!$ba->read_uint32($this->login_userid)) return false;
		if(!$ba->read_buf($this->session,32)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->server_id)) return false;
		if (!$ba->write_uint32($this->login_userid)) return false;
		$ba->write_buf($this->session,32);
		return true;
	}

};

	
class cli_login_out {
	/* 按位标记：0-》注册 1-》新注册米米号 2-》聊天样本 3-》点击过指引 */
	#类型:uint32
	public $regflag;

	/* 剩余多少钱 */
	#类型:uint32
	public $left_xiaomee;

	/* 肤色 */
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

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $task_nodeid_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $use_clothes_list =array();

	/*使用中的特效卡片列表*/
	#变长数组,最大长度:999, 类型:uint32_t
	public $effect_list =array();

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $find_map_list =array();


	public function cli_login_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->regflag)) return false;
		if (!$ba->read_uint32($this->left_xiaomee)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->last_islandid)) return false;
		if (!$ba->read_uint32($this->last_mapid)) return false;
		if (!$ba->read_uint32($this->last_x)) return false;
		if (!$ba->read_uint32($this->last_y)) return false;

		$task_nodeid_list_count=0 ;
		if (!$ba->read_uint32( $task_nodeid_list_count )) return false;
		$this->task_nodeid_list=array();
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->read_uint32($this->task_nodeid_list[$i])) return false;
		}}

		$use_clothes_list_count=0 ;
		if (!$ba->read_uint32( $use_clothes_list_count )) return false;
		$this->use_clothes_list=array();
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->read_uint32($this->use_clothes_list[$i])) return false;
		}}

		$effect_list_count=0 ;
		if (!$ba->read_uint32( $effect_list_count )) return false;
		$this->effect_list=array();
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->read_uint32($this->effect_list[$i])) return false;
		}}

		$find_map_list_count=0 ;
		if (!$ba->read_uint32( $find_map_list_count )) return false;
		$this->find_map_list=array();
		{for($i=0; $i<$find_map_list_count;$i++){
			if (!$ba->read_uint32($this->find_map_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->regflag)) return false;
		if (!$ba->write_uint32($this->left_xiaomee)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		if (!$ba->write_uint32($this->age)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->last_islandid)) return false;
		if (!$ba->write_uint32($this->last_mapid)) return false;
		if (!$ba->write_uint32($this->last_x)) return false;
		if (!$ba->write_uint32($this->last_y)) return false;
		$task_nodeid_list_count=count($this->task_nodeid_list);
        $ba->write_uint32($task_nodeid_list_count);
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->write_uint32($this->task_nodeid_list[$i])) return false;
		}}
		$use_clothes_list_count=count($this->use_clothes_list);
        $ba->write_uint32($use_clothes_list_count);
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->write_uint32($this->use_clothes_list[$i])) return false;
		}}
		$effect_list_count=count($this->effect_list);
        $ba->write_uint32($effect_list_count);
		{for($i=0; $i<$effect_list_count;$i++){
			if (!$ba->write_uint32($this->effect_list[$i])) return false;
		}}
		$find_map_list_count=count($this->find_map_list);
        $ba->write_uint32($find_map_list_count);
		{for($i=0; $i<$find_map_list_count;$i++){
			if (!$ba->write_uint32($this->find_map_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_noti_effect_used_out {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/* 物品ID */
	#类型:uint32
	public $itemid;

	/* 要取消的特效卡片ID（没有为0） */
	#类型:uint32
	public $unset_itemid;


	public function cli_noti_effect_used_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->unset_itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->unset_itemid)) return false;
		return true;
	}

};

	
class cli_noti_first_enter_island_out {
	/*  */
	#类型:uint32
	public $islandid;

	/* 获得卜克豆 */
	#类型:uint32
	public $xiaomee;

	/* 是否新注册的米米号 */
	#类型:uint32
	public $is_newid;


	public function cli_noti_first_enter_island_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->xiaomee)) return false;
		if (!$ba->read_uint32($this->is_newid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->xiaomee)) return false;
		if (!$ba->write_uint32($this->is_newid)) return false;
		return true;
	}

};

	
class user_game_win_t {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/* 输羸 :0:不是,1:是,2:平局 */
	#类型:uint32
	public $win_flag;

	/* 正数得到负数减去 */
	#类型:int32
	public $get_point;


	public function user_game_win_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		if (!$ba->read_uint32($this->win_flag)) return false;
		if (!$ba->read_int32($this->get_point)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		if (!$ba->write_uint32($this->win_flag)) return false;
		if (!$ba->write_int32($this->get_point)) return false;
		return true;
	}

};

	
class cli_noti_game_end_out {
	/**/
	#变长数组,最大长度:10, 类型:user_game_win_t
	public $user_win_list =array();


	public function cli_noti_game_end_out(){

	}

	public function read_from_buf($ba ){

		$user_win_list_count=0 ;
		if (!$ba->read_uint32( $user_win_list_count )) return false;
		$this->user_win_list=array();
		{for($i=0; $i<$user_win_list_count;$i++){
			$this->user_win_list[$i]=new user_game_win_t();
			if (!$this->user_win_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$user_win_list_count=count($this->user_win_list);
        $ba->write_uint32($user_win_list_count);
		{for($i=0; $i<$user_win_list_count;$i++){
			if ( ! $this->user_win_list[$i] instanceof user_game_win_t ) return false; 
			if (!$this->user_win_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_noti_game_is_start_out {
	/*  */
	#类型:uint32
	public $gameid;

	/* 是否开始,0:否(终止游戏), 1:是 */
	#类型:uint32
	public $is_start;

	/* 为什么不玩, 1:自己关，2：在换装，3：购物 */
	#类型:uint32
	public $why_not_start_flag;

	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_noti_game_is_start_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->is_start)) return false;
		if (!$ba->read_uint32($this->why_not_start_flag)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->is_start)) return false;
		if (!$ba->write_uint32($this->why_not_start_flag)) return false;
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_noti_game_opt_out {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/*游戏操作信息*/
	#变长数组,最大长度:9999, 类型:char
	public $gamemsg ;


	public function cli_noti_game_opt_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;

		$gamemsg_count=0 ;
		if (!$ba->read_uint32( $gamemsg_count )) return false;
		if(!$ba->read_buf($this->gamemsg,$gamemsg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		$gamemsg_count=strlen($this->gamemsg);
		$ba->write_uint32($gamemsg_count);
		$ba->write_buf($this->gamemsg,$gamemsg_count);
		return true;
	}

};

	
class cli_noti_game_request_out {
	/*  */
	#类型:uint32
	public $gameid;

	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/* 该请求的key, 在 确认时将之一起发出 */
	#定长数组,长度:32, 类型:char 
	public $request_session ;


	public function cli_noti_game_request_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->src_userid)) return false;
		if(!$ba->read_buf($this->request_session,32)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->src_userid)) return false;
		$ba->write_buf($this->request_session,32);
		return true;
	}

};

	
class game_seat_t {
	/* 项目编号 */
	#类型:uint32
	public $gameid;

	/* 0左1右 */
	#类型:uint32
	public $side;

	/* 在座位上的米米号 */
	#类型:uint32
	public $userid;


	public function game_seat_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->side)) return false;
		if (!$ba->read_uint32($this->userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->side)) return false;
		if (!$ba->write_uint32($this->userid)) return false;
		return true;
	}

};

	
class cli_noti_game_seat_out {
	/*小游戏列表*/
	#变长数组,最大长度:100, 类型:game_seat_t
	public $gamelist =array();


	public function cli_noti_game_seat_out(){

	}

	public function read_from_buf($ba ){

		$gamelist_count=0 ;
		if (!$ba->read_uint32( $gamelist_count )) return false;
		$this->gamelist=array();
		{for($i=0; $i<$gamelist_count;$i++){
			$this->gamelist[$i]=new game_seat_t();
			if (!$this->gamelist[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$gamelist_count=count($this->gamelist);
        $ba->write_uint32($gamelist_count);
		{for($i=0; $i<$gamelist_count;$i++){
			if ( ! $this->gamelist[$i] instanceof game_seat_t ) return false; 
			if (!$this->gamelist[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_noti_game_start_out {
	/*  */
	#类型:uint32
	public $gameid;

	/*一起玩的userid列表*/
	#变长数组,最大长度:100, 类型:uint32_t
	public $userid_list =array();

	/* 第一步的用户,如果为0，则没有限定 */
	#类型:uint32
	public $start_userid;

	/* 每一步超时时间 ,0：不限定 */
	#类型:uint32
	public $step_timeout;


	public function cli_noti_game_start_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;

		$userid_list_count=0 ;
		if (!$ba->read_uint32( $userid_list_count )) return false;
		$this->userid_list=array();
		{for($i=0; $i<$userid_list_count;$i++){
			if (!$ba->read_uint32($this->userid_list[$i])) return false;
		}}
		if (!$ba->read_uint32($this->start_userid)) return false;
		if (!$ba->read_uint32($this->step_timeout)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		$userid_list_count=count($this->userid_list);
        $ba->write_uint32($userid_list_count);
		{for($i=0; $i<$userid_list_count;$i++){
			if (!$ba->write_uint32($this->userid_list[$i])) return false;
		}}
		if (!$ba->write_uint32($this->start_userid)) return false;
		if (!$ba->write_uint32($this->step_timeout)) return false;
		return true;
	}

};

	
class cli_noti_game_user_left_game_out {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_noti_game_user_left_game_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_noti_game_user_out {
	/* 状态（1开始2结束） */
	#类型:uint32
	public $state;

	/*  */
	#类型:game_user_t
	public $game_user;


	public function cli_noti_game_user_out(){
		$this->game_user=new game_user_t(); 

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->state)) return false;
		if (!$this->game_user->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->state)) return false;
		if ( ! $this->game_user instanceof game_user_t ) return false; 
		if (!$this->game_user->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_noti_get_invitation_out {
	/* 物品邀请函ID */
	#类型:uint32
	public $itemid;


	public function cli_noti_get_invitation_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		return true;
	}

};

	
class cli_noti_get_item_list_out {
	/*得到的物品列表*/
	#变长数组,最大长度:9999, 类型:item_t
	public $item_list =array();


	public function cli_noti_get_item_list_out(){

	}

	public function read_from_buf($ba ){

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new item_t();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$item_list_count=count($this->item_list);
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof item_t ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_noti_island_complete_out {
	/*  */
	#类型:uint32
	public $userid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/*  */
	#类型:uint32
	public $islandid;


	public function cli_noti_island_complete_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->userid)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->islandid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->userid)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->islandid)) return false;
		return true;
	}

};

	
class cli_noti_leave_island_out {
	/* 当前岛ID */
	#类型:uint32
	public $cur_island;

	/* 当前地图ID */
	#类型:uint32
	public $cur_mapid;

	/* 前往的岛屿ID */
	#类型:uint32
	public $islandid;

	/* 前往的地图ID */
	#类型:uint32
	public $mapid;


	public function cli_noti_leave_island_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->cur_island)) return false;
		if (!$ba->read_uint32($this->cur_mapid)) return false;
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->mapid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->cur_island)) return false;
		if (!$ba->write_uint32($this->cur_mapid)) return false;
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->mapid)) return false;
		return true;
	}

};

	
class cli_noti_one_user_info_out {
	/*  */
	#类型:online_user_info_t
	public $user_info;


	public function cli_noti_one_user_info_out(){
		$this->user_info=new online_user_info_t(); 

	}

	public function read_from_buf($ba ){
		if (!$this->user_info->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->user_info instanceof online_user_info_t ) return false; 
		if (!$this->user_info->write_to_buf($ba)) return false;
		return true;
	}

};

	
class cli_noti_set_color_out {
	/*  */
	#类型:uint32
	public $userid;

	/* 肤色 */
	#类型:uint32
	public $color;


	public function cli_noti_set_color_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->userid)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->userid)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		return true;
	}

};

	
class cli_noti_show_out {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/* 表情ID */
	#类型:uint32
	public $expression_id;

	/* 目标userid（0：场景 1游戏中） */
	#类型:uint32
	public $obj_userid;


	public function cli_noti_show_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;
		if (!$ba->read_uint32($this->expression_id)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		if (!$ba->write_uint32($this->expression_id)) return false;
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_noti_svr_time_out {
	/*  */
	#类型:uint32
	public $timestamp;


	public function cli_noti_svr_time_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->timestamp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->timestamp)) return false;
		return true;
	}

};

	
class cli_noti_talk_out {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $src_nick ;

	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:char
	public $msg ;


	public function cli_noti_talk_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;
		if(!$ba->read_buf($this->src_nick,16)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;

		$msg_count=0 ;
		if (!$ba->read_uint32( $msg_count )) return false;
		if(!$ba->read_buf($this->msg,$msg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		$ba->write_buf($this->src_nick,16);
		if (!$ba->write_uint32($this->obj_userid)) return false;
		$msg_count=strlen($this->msg);
		$ba->write_uint32($msg_count);
		$ba->write_buf($this->msg,$msg_count);
		return true;
	}

};

	
class cli_noti_user_left_map_out {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;


	public function cli_noti_user_left_map_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_noti_user_level_up_out {
	/* 发起的userid */
	#类型:uint32
	public $src_userid;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验值 */
	#类型:uint32
	public $experience;

	/* 此次得到的经验 */
	#类型:uint32
	public $addexp;

	/* 是否升级 */
	#类型:uint32
	public $is_level_up;

	/* 升级获得的奖励物品 */
	#类型:uint32
	public $itemid;


	public function cli_noti_user_level_up_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->src_userid)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->experience)) return false;
		if (!$ba->read_uint32($this->addexp)) return false;
		if (!$ba->read_uint32($this->is_level_up)) return false;
		if (!$ba->read_uint32($this->itemid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->src_userid)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->experience)) return false;
		if (!$ba->write_uint32($this->addexp)) return false;
		if (!$ba->write_uint32($this->is_level_up)) return false;
		if (!$ba->write_uint32($this->itemid)) return false;
		return true;
	}

};

	
class cli_noti_user_move_out {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/*  */
	#类型:uint32
	public $start_x;

	/*  */
	#类型:uint32
	public $start_y;

	/*  */
	#类型:uint32
	public $mouse_x;

	/*  */
	#类型:uint32
	public $mouse_y;

	/* 移动模式 */
	#类型:uint32
	public $type;


	public function cli_noti_user_move_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;
		if (!$ba->read_uint32($this->start_x)) return false;
		if (!$ba->read_uint32($this->start_y)) return false;
		if (!$ba->read_uint32($this->mouse_x)) return false;
		if (!$ba->read_uint32($this->mouse_y)) return false;
		if (!$ba->read_uint32($this->type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		if (!$ba->write_uint32($this->start_x)) return false;
		if (!$ba->write_uint32($this->start_y)) return false;
		if (!$ba->write_uint32($this->mouse_x)) return false;
		if (!$ba->write_uint32($this->mouse_y)) return false;
		if (!$ba->write_uint32($this->type)) return false;
		return true;
	}

};

	
class cli_noti_user_online_time_out {
	/* 计时类型（0连续 1累计） */
	#类型:uint32
	public $type;

	/* 在线时间（分钟） */
	#类型:uint32
	public $time;


	public function cli_noti_user_online_time_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if (!$ba->read_uint32($this->time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		if (!$ba->write_uint32($this->time)) return false;
		return true;
	}

};

	
class cli_noti_user_use_clothes_list_out {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $use_clothes_list =array();


	public function cli_noti_user_use_clothes_list_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;

		$use_clothes_list_count=0 ;
		if (!$ba->read_uint32( $use_clothes_list_count )) return false;
		$this->use_clothes_list=array();
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->read_uint32($this->use_clothes_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		$use_clothes_list_count=count($this->use_clothes_list);
        $ba->write_uint32($use_clothes_list_count);
		{for($i=0; $i<$use_clothes_list_count;$i++){
			if (!$ba->write_uint32($this->use_clothes_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_post_msg_in {
	/* 投稿类型 */
	#类型:uint32
	public $type;

	/* 主题 */
	#定长数组,长度:60, 类型:char 
	public $title ;

	/*投稿内容*/
	#变长数组,最大长度:4096, 类型:char
	public $msg ;


	public function cli_post_msg_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if(!$ba->read_buf($this->title,60)) return false;

		$msg_count=0 ;
		if (!$ba->read_uint32( $msg_count )) return false;
		if(!$ba->read_buf($this->msg,$msg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		$ba->write_buf($this->title,60);
		$msg_count=strlen($this->msg);
		$ba->write_uint32($msg_count);
		$ba->write_buf($this->msg,$msg_count);
		return true;
	}

};

	
class cli_proto_header_in {
	/*  */
	#类型:uint32
	public $proto_length;

	/*  */
	#类型:uint16
	public $cmdid;

	/*  */
	#类型:uint32
	public $timestamp;

	/*  */
	#类型:uint32
	public $seq;

	/*  */
	#类型:uint16
	public $result;

	/*  */
	#类型:uint16
	public $useSever;


	public function cli_proto_header_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->proto_length)) return false;
		if (!$ba->read_uint16($this->cmdid)) return false;
		if (!$ba->read_uint32($this->timestamp)) return false;
		if (!$ba->read_uint32($this->seq)) return false;
		if (!$ba->read_uint16($this->result)) return false;
		if (!$ba->read_uint16($this->useSever)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->proto_length)) return false;
		if (!$ba->write_uint16($this->cmdid)) return false;
		if (!$ba->write_uint32($this->timestamp)) return false;
		if (!$ba->write_uint32($this->seq)) return false;
		if (!$ba->write_uint16($this->result)) return false;
		if (!$ba->write_uint16($this->useSever)) return false;
		return true;
	}

};

	
class cli_reg_in {
	/* 渠道id(填0) */
	#类型:uint32
	public $channelid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/*  */
	#类型:uint32
	public $age;

	/* 肤色 */
	#类型:uint32
	public $color;

	/**/
	#变长数组,最大长度:9999, 类型:item_t
	public $item_list =array();


	public function cli_reg_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->channelid)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if (!$ba->read_uint32($this->color)) return false;

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new item_t();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->channelid)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->age)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		$item_list_count=count($this->item_list);
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof item_t ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class cli_reg_out {
	/* 剩余多少钱 */
	#类型:uint32
	public $left_xiaomee;

	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;


	public function cli_reg_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->left_xiaomee)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->left_xiaomee)) return false;
		$ba->write_buf($this->nick,16);
		return true;
	}

};

	
class cli_set_busy_state_in {
	/* 忙状态 */
	#类型:uint32
	public $state;


	public function cli_set_busy_state_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->state)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->state)) return false;
		return true;
	}

};

	
class cli_set_color_in {
	/* 肤色 */
	#类型:uint32
	public $color;


	public function cli_set_color_in(){

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

	
class cli_set_effect_used_in {
	/* 物品ID */
	#类型:uint32
	public $itemid;

	/* （1穿上 0去掉） */
	#类型:uint32
	public $type;


	public function cli_set_effect_used_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->type)) return false;
		return true;
	}

};

	
class cli_set_item_used_list_in {
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $itemid_list =array();


	public function cli_set_item_used_list_in(){

	}

	public function read_from_buf($ba ){

		$itemid_list_count=0 ;
		if (!$ba->read_uint32( $itemid_list_count )) return false;
		$this->itemid_list=array();
		{for($i=0; $i<$itemid_list_count;$i++){
			if (!$ba->read_uint32($this->itemid_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$itemid_list_count=count($this->itemid_list);
        $ba->write_uint32($itemid_list_count);
		{for($i=0; $i<$itemid_list_count;$i++){
			if (!$ba->write_uint32($this->itemid_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_set_nick_in {
	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;


	public function cli_set_nick_in(){

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

	
class cli_show_in {
	/* 表情ID */
	#类型:uint32
	public $expression_id;

	/* 目标userid（0：场景 1：游戏中） */
	#类型:uint32
	public $obj_userid;


	public function cli_show_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->expression_id)) return false;
		if (!$ba->read_uint32($this->obj_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->expression_id)) return false;
		if (!$ba->write_uint32($this->obj_userid)) return false;
		return true;
	}

};

	
class cli_talk_in {
	/* 目标userid */
	#类型:uint32
	public $obj_userid;

	/**/
	#变长数组,最大长度:9999, 类型:char
	public $msg ;


	public function cli_talk_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->obj_userid)) return false;

		$msg_count=0 ;
		if (!$ba->read_uint32( $msg_count )) return false;
		if(!$ba->read_buf($this->msg,$msg_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->obj_userid)) return false;
		$msg_count=strlen($this->msg);
		$ba->write_uint32($msg_count);
		$ba->write_buf($this->msg,$msg_count);
		return true;
	}

};

	
class cli_talk_npc_in {
	/*  */
	#类型:uint32
	public $npc_id;


	public function cli_talk_npc_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->npc_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->npc_id)) return false;
		return true;
	}

};

	
class cli_task_complete_node_in {
	/*  */
	#类型:uint32
	public $islandid;

	/* 子任务id */
	#类型:uint32
	public $task_nodeid;


	public function cli_task_complete_node_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->task_nodeid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->task_nodeid)) return false;
		return true;
	}

};

	
class cli_task_complete_node_out {
	/**/
	#变长数组,最大长度:9999, 类型:uint32_t
	public $task_nodeid_list =array();


	public function cli_task_complete_node_out(){

	}

	public function read_from_buf($ba ){

		$task_nodeid_list_count=0 ;
		if (!$ba->read_uint32( $task_nodeid_list_count )) return false;
		$this->task_nodeid_list=array();
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->read_uint32($this->task_nodeid_list[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$task_nodeid_list_count=count($this->task_nodeid_list);
        $ba->write_uint32($task_nodeid_list_count);
		{for($i=0; $i<$task_nodeid_list_count;$i++){
			if (!$ba->write_uint32($this->task_nodeid_list[$i])) return false;
		}}
		return true;
	}

};

	
class cli_task_del_node_in {
	/*  */
	#类型:uint32
	public $islandid;

	/* 子任务id */
	#类型:uint32
	public $task_nodeid;


	public function cli_task_del_node_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->task_nodeid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->task_nodeid)) return false;
		return true;
	}

};

	
class cli_user_move_in {
	/*  */
	#类型:uint32
	public $start_x;

	/*  */
	#类型:uint32
	public $start_y;

	/*  */
	#类型:uint32
	public $mouse_x;

	/*  */
	#类型:uint32
	public $mouse_y;

	/* 移动模式 */
	#类型:uint32
	public $type;


	public function cli_user_move_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->start_x)) return false;
		if (!$ba->read_uint32($this->start_y)) return false;
		if (!$ba->read_uint32($this->mouse_x)) return false;
		if (!$ba->read_uint32($this->mouse_y)) return false;
		if (!$ba->read_uint32($this->type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->start_x)) return false;
		if (!$ba->write_uint32($this->start_y)) return false;
		if (!$ba->write_uint32($this->mouse_x)) return false;
		if (!$ba->write_uint32($this->mouse_y)) return false;
		if (!$ba->write_uint32($this->type)) return false;
		return true;
	}

};

	
class cli_walk_in {
	/*  */
	#类型:uint32
	public $islandid;

	/*  */
	#类型:uint32
	public $mapid;

	/*  */
	#类型:uint32
	public $x;

	/*  */
	#类型:uint32
	public $y;


	public function cli_walk_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->islandid)) return false;
		if (!$ba->read_uint32($this->mapid)) return false;
		if (!$ba->read_uint32($this->x)) return false;
		if (!$ba->read_uint32($this->y)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->islandid)) return false;
		if (!$ba->write_uint32($this->mapid)) return false;
		if (!$ba->write_uint32($this->x)) return false;
		if (!$ba->write_uint32($this->y)) return false;
		return true;
	}

};

	
class cli_work_get_lottery_out {
	/* 今天获得的次数 */
	#类型:uint32
	public $get_count;


	public function cli_work_get_lottery_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->get_count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->get_count)) return false;
		return true;
	}

};

	
class login_activate_game_in {
	/* 项目id (填12) */
	#类型:uint32
	public $which_game;

	/* 激活码：6-8个0-9、A-Z、a-z字符，没有用到的字节填0 */
	#定长数组,长度:10, 类型:char 
	public $active_code ;

	/* 验证码ID */
	#定长数组,长度:16, 类型:char 
	public $imgid ;

	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public $verif_code ;


	public function login_activate_game_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->which_game)) return false;
		if(!$ba->read_buf($this->active_code,10)) return false;
		if(!$ba->read_buf($this->imgid,16)) return false;
		if(!$ba->read_buf($this->verif_code,6)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->which_game)) return false;
		$ba->write_buf($this->active_code,10);
		$ba->write_buf($this->imgid,16);
		$ba->write_buf($this->verif_code,6);
		return true;
	}

};

	
class login_activate_game_out {
	/* 0表示激活成功，不会发送蓝色部分的内容 1激活码错误次数过多，需要输入验证码后再激活 2验证码输入错误 */
	#类型:uint32
	public $flag;

	/* 验证图片id(空） */
	#定长数组,长度:16, 类型:char 
	public $img_id ;

	/*验证图片*/
	#变长数组,最大长度:1000, 类型:char
	public $img ;


	public function login_activate_game_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if(!$ba->read_buf($this->img_id,16)) return false;

		$img_count=0 ;
		if (!$ba->read_uint32( $img_count )) return false;
		if(!$ba->read_buf($this->img,$img_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		$ba->write_buf($this->img_id,16);
		$img_count=strlen($this->img);
		$ba->write_uint32($img_count);
		$ba->write_buf($this->img,$img_count);
		return true;
	}

};

	
class login_check_game_activate_in {
	/* 项目id (填12) */
	#类型:uint32
	public $which_game;


	public function login_check_game_activate_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->which_game)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->which_game)) return false;
		return true;
	}

};

	
class login_check_game_activate_out {
	/* 0未激活1已激活 */
	#类型:uint32
	public $flag;


	public function login_check_game_activate_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		return true;
	}

};

	
class login_get_ranged_svr_list_in {
	/*  */
	#类型:uint32
	public $startid;

	/*  */
	#类型:uint32
	public $endid;


	public function login_get_ranged_svr_list_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->startid)) return false;
		if (!$ba->read_uint32($this->endid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->startid)) return false;
		if (!$ba->write_uint32($this->endid)) return false;
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

	
class login_get_ranged_svr_list_out {
	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public $online_list =array();


	public function login_get_ranged_svr_list_out(){

	}

	public function read_from_buf($ba ){

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
		$online_list_count=count($this->online_list);
        $ba->write_uint32($online_list_count);
		{for($i=0; $i<$online_list_count;$i++){
			if ( ! $this->online_list[$i] instanceof online_item_t ) return false; 
			if (!$this->online_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class login_get_recommeded_list_in {
	/* 用于跳转时无需密码 */
	#定长数组,长度:32, 类型:char 
	public $session ;


	public function login_get_recommeded_list_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->session,32)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->session,32);
		return true;
	}

};

	
class login_get_recommeded_list_out {
	/*  */
	#类型:uint32
	public $max_online_id;

	/*  */
	#类型:uint32
	public $vip;

	/**/
	#变长数组,最大长度:1000, 类型:online_item_t
	public $online_list =array();


	public function login_get_recommeded_list_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->max_online_id)) return false;
		if (!$ba->read_uint32($this->vip)) return false;

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
		if (!$ba->write_uint32($this->vip)) return false;
		$online_list_count=count($this->online_list);
        $ba->write_uint32($online_list_count);
		{for($i=0; $i<$online_list_count;$i++){
			if ( ! $this->online_list[$i] instanceof online_item_t ) return false; 
			if (!$this->online_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class login_login_in {
	/* 密码的两次md5值 */
	#定长数组,长度:32, 类型:char 
	public $passwd_md5_two ;

	/* 渠道id(填0) */
	#类型:uint32
	public $channelid;

	/* 项目id (填12) */
	#类型:uint32
	public $which_game;

	/* ip 地址 */
	#类型:uint32
	public $user_ip;

	/* 验证图片id(空） */
	#定长数组,长度:16, 类型:char 
	public $img_id ;

	/* 验证码 (空) */
	#定长数组,长度:6, 类型:char 
	public $verif_code ;


	public function login_login_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->passwd_md5_two,32)) return false;
		if (!$ba->read_uint32($this->channelid)) return false;
		if (!$ba->read_uint32($this->which_game)) return false;
		if (!$ba->read_uint32($this->user_ip)) return false;
		if(!$ba->read_buf($this->img_id,16)) return false;
		if(!$ba->read_buf($this->verif_code,6)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->passwd_md5_two,32);
		if (!$ba->write_uint32($this->channelid)) return false;
		if (!$ba->write_uint32($this->which_game)) return false;
		if (!$ba->write_uint32($this->user_ip)) return false;
		$ba->write_buf($this->img_id,16);
		$ba->write_buf($this->verif_code,6);
		return true;
	}

};

	
class login_login_out {
	/*  */
	#类型:uint32
	public $verif_flag;

	/* 用于跳转时无需密码 */
	#定长数组,长度:16, 类型:char 
	public $session ;

	/*  */
	#类型:uint32
	public $create_role_flag;


	public function login_login_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->verif_flag)) return false;
		if(!$ba->read_buf($this->session,16)) return false;
		if (!$ba->read_uint32($this->create_role_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->verif_flag)) return false;
		$ba->write_buf($this->session,16);
		if (!$ba->write_uint32($this->create_role_flag)) return false;
		return true;
	}

};

	
class login_refresh_img_in {
	/* 0正常登录； 1使用激活码时（可以不发送，兼容旧版本即正常登录） */
	#类型:uint32
	public $flag;


	public function login_refresh_img_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		return true;
	}

};

	
class login_refresh_img_out {
	/* 0不需要输入验证码，1需要输入验证（为1时才有验证码)） */
	#类型:uint32
	public $flag;

	/* 图片ID。发送验证码时，需要把这个ID带上 */
	#定长数组,长度:16, 类型:char 
	public $imgid ;

	/*验证图片*/
	#变长数组,最大长度:1000, 类型:char
	public $img ;


	public function login_refresh_img_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if(!$ba->read_buf($this->imgid,16)) return false;

		$img_count=0 ;
		if (!$ba->read_uint32( $img_count )) return false;
		if(!$ba->read_buf($this->img,$img_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		$ba->write_buf($this->imgid,16);
		$img_count=strlen($this->img);
		$ba->write_uint32($img_count);
		$ba->write_buf($this->img,$img_count);
		return true;
	}

};

	
class noti_cli_leave_out {
	/* 离线原因 （1午夜休息） */
	#类型:uint32
	public $reason;


	public function noti_cli_leave_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->reason)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->reason)) return false;
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
	/* 物品id */
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

	
class task_t {
	/* 主任务id */
	#类型:uint32
	public $taskid;

	/* 子任务id */
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

	
class Cpop_online_proto  extends Cproto_base {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }

	/* 报文头部<font color=red >小端字节序</font> */
	/* 调用方式还可以是： 
		$in=new cli_proto_header_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_proto_header($userid,$in );
	*/

	function cli_proto_header($userid , $proto_length=null, $cmdid=null, $timestamp=null, $seq=null, $result=null, $useSever=null){

		if ( $proto_length instanceof cli_proto_header_in ){
			$in=$proto_length;
		}else{
			$in=new cli_proto_header_in();
			$in->proto_length=$proto_length;
			$in->cmdid=$cmdid;
			$in->timestamp=$timestamp;
			$in->seq=$seq;
			$in->result=$result;
			$in->useSever=$useSever;

		}
		
		return $this->send_cmd_new(1,$userid, $in, null, 0x2e14bf71);
	}
	
	/* 重新获取验证码 */
	/* 调用方式还可以是： 
		$in=new login_refresh_img_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_refresh_img($userid,$in );
	*/

	function login_refresh_img($userid , $flag=null){

		if ( $flag instanceof login_refresh_img_in ){
			$in=$flag;
		}else{
			$in=new login_refresh_img_in();
			$in->flag=$flag;

		}
		
		return $this->send_cmd_new(101,$userid, $in, new login_refresh_img_out(), 0xed66049a);
	}
	
	/* 检查游戏是否激活 */
	/* 调用方式还可以是： 
		$in=new login_check_game_activate_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_check_game_activate($userid,$in );
	*/

	function login_check_game_activate($userid , $which_game=null){

		if ( $which_game instanceof login_check_game_activate_in ){
			$in=$which_game;
		}else{
			$in=new login_check_game_activate_in();
			$in->which_game=$which_game;

		}
		
		return $this->send_cmd_new(102,$userid, $in, new login_check_game_activate_out(), 0x68dbed7e);
	}
	
	/* 登录 */
	/* 调用方式还可以是： 
		$in=new login_login_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_login($userid,$in );
	*/

	function login_login($userid , $passwd_md5_two=null, $channelid=null, $which_game=null, $user_ip=null, $img_id=null, $verif_code=null){

		if ( $passwd_md5_two instanceof login_login_in ){
			$in=$passwd_md5_two;
		}else{
			$in=new login_login_in();
			$in->passwd_md5_two=$passwd_md5_two;
			$in->channelid=$channelid;
			$in->which_game=$which_game;
			$in->user_ip=$user_ip;
			$in->img_id=$img_id;
			$in->verif_code=$verif_code;

		}
		
		return $this->send_cmd_new(103,$userid, $in, new login_login_out(), 0xc0774b87);
	}
	
	/* 激活游戏 */
	/* 调用方式还可以是： 
		$in=new login_activate_game_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_activate_game($userid,$in );
	*/

	function login_activate_game($userid , $which_game=null, $active_code=null, $imgid=null, $verif_code=null){

		if ( $which_game instanceof login_activate_game_in ){
			$in=$which_game;
		}else{
			$in=new login_activate_game_in();
			$in->which_game=$which_game;
			$in->active_code=$active_code;
			$in->imgid=$imgid;
			$in->verif_code=$verif_code;

		}
		
		return $this->send_cmd_new(104,$userid, $in, new login_activate_game_out(), 0xec256cd8);
	}
	
	/* 得到推荐服务器列表 */
	/* 调用方式还可以是： 
		$in=new login_get_recommeded_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_get_recommeded_list($userid,$in );
	*/

	function login_get_recommeded_list($userid , $session=null){

		if ( $session instanceof login_get_recommeded_list_in ){
			$in=$session;
		}else{
			$in=new login_get_recommeded_list_in();
			$in->session=$session;

		}
		
		return $this->send_cmd_new(105,$userid, $in, new login_get_recommeded_list_out(), 0x842984ca);
	}
	
	/* 得到服务器范围列表 */
	/* 调用方式还可以是： 
		$in=new login_get_ranged_svr_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->login_get_ranged_svr_list($userid,$in );
	*/

	function login_get_ranged_svr_list($userid , $startid=null, $endid=null){

		if ( $startid instanceof login_get_ranged_svr_list_in ){
			$in=$startid;
		}else{
			$in=new login_get_ranged_svr_list_in();
			$in->startid=$startid;
			$in->endid=$endid;

		}
		
		return $this->send_cmd_new(106,$userid, $in, new login_get_ranged_svr_list_out(), 0xb38d89ee);
	}
	
	/* 登入 */
	/* 调用方式还可以是： 
		$in=new cli_login_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_login($userid,$in );
	*/

	function cli_login($userid , $server_id=null, $login_userid=null, $session=null){

		if ( $server_id instanceof cli_login_in ){
			$in=$server_id;
		}else{
			$in=new cli_login_in();
			$in->server_id=$server_id;
			$in->login_userid=$login_userid;
			$in->session=$session;

		}
		
		return $this->send_cmd_new(1001,$userid, $in, new cli_login_out(), 0x0a410403);
	}
	
	/* 用户注册 */
	/* 调用方式还可以是： 
		$in=new cli_reg_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_reg($userid,$in );
	*/

	function cli_reg($userid , $channelid=null, $nick=null, $age=null, $color=null, $item_list=null){

		if ( $channelid instanceof cli_reg_in ){
			$in=$channelid;
		}else{
			$in=new cli_reg_in();
			$in->channelid=$channelid;
			$in->nick=$nick;
			$in->age=$age;
			$in->color=$color;
			$in->item_list=$item_list;

		}
		
		return $this->send_cmd_new(1002,$userid, $in, new cli_reg_out(), 0xc558bcb8);
	}
	
	/* 通知用户离线 */

	function noti_cli_leave($userid ){

		return $this->send_cmd_new(1003,$userid, null, new noti_cli_leave_out(), 0x8154f01d);
	}
	
	/* 通知用户某人岛屿完成 */

	function cli_noti_island_complete($userid ){

		return $this->send_cmd_new(1004,$userid, null, new cli_noti_island_complete_out(), 0x5117b356);
	}
	
	/* 完成任务节点 */
	/* 调用方式还可以是： 
		$in=new cli_task_complete_node_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_task_complete_node($userid,$in );
	*/

	function cli_task_complete_node($userid , $islandid=null, $task_nodeid=null){

		if ( $islandid instanceof cli_task_complete_node_in ){
			$in=$islandid;
		}else{
			$in=new cli_task_complete_node_in();
			$in->islandid=$islandid;
			$in->task_nodeid=$task_nodeid;

		}
		
		return $this->send_cmd_new(1011,$userid, $in, new cli_task_complete_node_out(), 0xb9a905ba);
	}
	
	/* 得到物品列表 */
	/* 调用方式还可以是： 
		$in=new cli_get_item_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_item_list($userid,$in );
	*/

	function cli_get_item_list($userid , $startid=null, $endid=null){

		if ( $startid instanceof cli_get_item_list_in ){
			$in=$startid;
		}else{
			$in=new cli_get_item_list_in();
			$in->startid=$startid;
			$in->endid=$endid;

		}
		
		return $this->send_cmd_new(1012,$userid, $in, new cli_get_item_list_out(), 0xedc9abd8);
	}
	
	/* 获取特殊物品数量 */
	/* 调用方式还可以是： 
		$in=new cli_get_spec_item_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_spec_item($userid,$in );
	*/

	function cli_get_spec_item($userid , $itemid=null){

		if ( $itemid instanceof cli_get_spec_item_in ){
			$in=$itemid;
		}else{
			$in=new cli_get_spec_item_in();
			$in->itemid=$itemid;

		}
		
		return $this->send_cmd_new(1013,$userid, $in, new cli_get_spec_item_out(), 0xd8099850);
	}
	
	/* 进入场景 */
	/* 调用方式还可以是： 
		$in=new cli_walk_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_walk($userid,$in );
	*/

	function cli_walk($userid , $islandid=null, $mapid=null, $x=null, $y=null){

		if ( $islandid instanceof cli_walk_in ){
			$in=$islandid;
		}else{
			$in=new cli_walk_in();
			$in->islandid=$islandid;
			$in->mapid=$mapid;
			$in->x=$x;
			$in->y=$y;

		}
		
		return $this->send_cmd_new(1014,$userid, $in, null, 0x23916811);
	}
	
	/* 得到用户在该岛上的任务信息 */
	/* 调用方式还可以是： 
		$in=new cli_get_user_island_task_info_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_user_island_task_info($userid,$in );
	*/

	function cli_get_user_island_task_info($userid , $islandid=null){

		if ( $islandid instanceof cli_get_user_island_task_info_in ){
			$in=$islandid;
		}else{
			$in=new cli_get_user_island_task_info_in();
			$in->islandid=$islandid;

		}
		
		return $this->send_cmd_new(1015,$userid, $in, new cli_get_user_island_task_info_out(), 0xffd0451d);
	}
	
	/* 增加找到的地图 */
	/* 调用方式还可以是： 
		$in=new cli_find_map_add_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_find_map_add($userid,$in );
	*/

	function cli_find_map_add($userid , $islandid=null, $mapid=null){

		if ( $islandid instanceof cli_find_map_add_in ){
			$in=$islandid;
		}else{
			$in=new cli_find_map_add_in();
			$in->islandid=$islandid;
			$in->mapid=$mapid;

		}
		
		return $this->send_cmd_new(1016,$userid, $in, null, 0x200d7500);
	}
	
	/* 设置昵称 */
	/* 调用方式还可以是： 
		$in=new cli_set_nick_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_set_nick($userid,$in );
	*/

	function cli_set_nick($userid , $nick=null){

		if ( $nick instanceof cli_set_nick_in ){
			$in=$nick;
		}else{
			$in=new cli_set_nick_in();
			$in->nick=$nick;

		}
		
		return $this->send_cmd_new(1017,$userid, $in, null, 0x52ac0fda);
	}
	
	/* 设置使用物品列表 */
	/* 调用方式还可以是： 
		$in=new cli_set_item_used_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_set_item_used_list($userid,$in );
	*/

	function cli_set_item_used_list($userid , $itemid_list=null){

		if ( $itemid_list instanceof cli_set_item_used_list_in ){
			$in=$itemid_list;
		}else{
			$in=new cli_set_item_used_list_in();
			$in->itemid_list=$itemid_list;

		}
		
		return $this->send_cmd_new(1018,$userid, $in, null, 0xccc87448);
	}
	
	/* 删除任务 */
	/* 调用方式还可以是： 
		$in=new cli_task_del_node_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_task_del_node($userid,$in );
	*/

	function cli_task_del_node($userid , $islandid=null, $task_nodeid=null){

		if ( $islandid instanceof cli_task_del_node_in ){
			$in=$islandid;
		}else{
			$in=new cli_task_del_node_in();
			$in->islandid=$islandid;
			$in->task_nodeid=$task_nodeid;

		}
		
		return $this->send_cmd_new(1019,$userid, $in, null, 0xa80dbeed);
	}
	
	/* 得到岛上的找到地图的信息 */
	/* 调用方式还可以是： 
		$in=new cli_get_user_island_find_map_info_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_user_island_find_map_info($userid,$in );
	*/

	function cli_get_user_island_find_map_info($userid , $islandid=null){

		if ( $islandid instanceof cli_get_user_island_find_map_info_in ){
			$in=$islandid;
		}else{
			$in=new cli_get_user_island_find_map_info_in();
			$in->islandid=$islandid;

		}
		
		return $this->send_cmd_new(1020,$userid, $in, new cli_get_user_island_find_map_info_out(), 0xa91abb1c);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new cli_get_card_list_by_islandid_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_card_list_by_islandid($userid,$in );
	*/

	function cli_get_card_list_by_islandid($userid , $islandid=null){

		if ( $islandid instanceof cli_get_card_list_by_islandid_in ){
			$in=$islandid;
		}else{
			$in=new cli_get_card_list_by_islandid_in();
			$in->islandid=$islandid;

		}
		
		return $this->send_cmd_new(1021,$userid, $in, new cli_get_card_list_by_islandid_out(), 0x3d8e05fa);
	}
	
	/* 和npc聊天 */
	/* 调用方式还可以是： 
		$in=new cli_talk_npc_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_talk_npc($userid,$in );
	*/

	function cli_talk_npc($userid , $npc_id=null){

		if ( $npc_id instanceof cli_talk_npc_in ){
			$in=$npc_id;
		}else{
			$in=new cli_talk_npc_in();
			$in->npc_id=$npc_id;

		}
		
		return $this->send_cmd_new(1022,$userid, $in, null, 0x3c2927f4);
	}
	
	/* 得到用户信息 */
	/* 调用方式还可以是： 
		$in=new cli_get_user_info_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_user_info($userid,$in );
	*/

	function cli_get_user_info($userid , $obj_userid=null){

		if ( $obj_userid instanceof cli_get_user_info_in ){
			$in=$obj_userid;
		}else{
			$in=new cli_get_user_info_in();
			$in->obj_userid=$obj_userid;

		}
		
		return $this->send_cmd_new(1023,$userid, $in, new cli_get_user_info_out(), 0x7e293ceb);
	}
	
	/* 获取用户的小游戏统计 */
	/* 调用方式还可以是： 
		$in=new cli_get_user_game_stat_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_user_game_stat($userid,$in );
	*/

	function cli_get_user_game_stat($userid , $obj_userid=null){

		if ( $obj_userid instanceof cli_get_user_game_stat_in ){
			$in=$obj_userid;
		}else{
			$in=new cli_get_user_game_stat_in();
			$in->obj_userid=$obj_userid;

		}
		
		return $this->send_cmd_new(1024,$userid, $in, new cli_get_user_game_stat_out(), 0xc11a3f99);
	}
	
	/* 通知客户端用户在线时间（分钟） */

	function cli_noti_user_online_time($userid ){

		return $this->send_cmd_new(1025,$userid, null, new cli_noti_user_online_time_out(), 0x94941e35);
	}
	
	/* 通知用户第一次进某岛 */

	function cli_noti_first_enter_island($userid ){

		return $this->send_cmd_new(1026,$userid, null, new cli_noti_first_enter_island_out(), 0xc775a40e);
	}
	
	/* 使用特效卡片 */
	/* 调用方式还可以是： 
		$in=new cli_set_effect_used_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_set_effect_used($userid,$in );
	*/

	function cli_set_effect_used($userid , $itemid=null, $type=null){

		if ( $itemid instanceof cli_set_effect_used_in ){
			$in=$itemid;
		}else{
			$in=new cli_set_effect_used_in();
			$in->itemid=$itemid;
			$in->type=$type;

		}
		
		return $this->send_cmd_new(1028,$userid, $in, null, 0xf86b3c0a);
	}
	
	/* 通知有人使用特效卡片 */

	function cli_noti_effect_used($userid ){

		return $this->send_cmd_new(1029,$userid, null, new cli_noti_effect_used_out(), 0xffd34345);
	}
	
	/* 通知客户端得到一些物品 */

	function cli_noti_get_item_list($userid ){

		return $this->send_cmd_new(1030,$userid, null, new cli_noti_get_item_list_out(), 0xc0680138);
	}
	
	/* 删除一些物品 */
	/* 调用方式还可以是： 
		$in=new cli_del_item_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_del_item($userid,$in );
	*/

	function cli_del_item($userid , $itemlist=null){

		if ( $itemlist instanceof cli_del_item_in ){
			$in=$itemlist;
		}else{
			$in=new cli_del_item_in();
			$in->itemlist=$itemlist;

		}
		
		return $this->send_cmd_new(1031,$userid, $in, null, 0x53797358);
	}
	
	/* 获取限时岛屿的开放时间 */

	function cli_get_island_time($userid ){

		return $this->send_cmd_new(1032,$userid, null, new cli_get_island_time_out(), 0xca1743b9);
	}
	
	/* 通知用户离开限时岛屿 */

	function cli_noti_leave_island($userid ){

		return $this->send_cmd_new(1033,$userid, null, new cli_noti_leave_island_out(), 0x9b5762ff);
	}
	
	/* 拉取范围的特殊物品 */
	/* 调用方式还可以是： 
		$in=new cli_get_spec_item_list_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_spec_item_list($userid,$in );
	*/

	function cli_get_spec_item_list($userid , $startid=null, $endid=null){

		if ( $startid instanceof cli_get_spec_item_list_in ){
			$in=$startid;
		}else{
			$in=new cli_get_spec_item_list_in();
			$in->startid=$startid;
			$in->endid=$endid;

		}
		
		return $this->send_cmd_new(1034,$userid, $in, new cli_get_spec_item_list_out(), 0xfdf51e9a);
	}
	
	/* 换肤 */
	/* 调用方式还可以是： 
		$in=new cli_set_color_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_set_color($userid,$in );
	*/

	function cli_set_color($userid , $color=null){

		if ( $color instanceof cli_set_color_in ){
			$in=$color;
		}else{
			$in=new cli_set_color_in();
			$in->color=$color;

		}
		
		return $this->send_cmd_new(1035,$userid, $in, null, 0x2dfde07c);
	}
	
	/*  */

	function cli_noti_set_color($userid ){

		return $this->send_cmd_new(1036,$userid, null, new cli_noti_set_color_out(), 0xfcdf2bf3);
	}
	
	/* 通知单个用户在线信息(别人进来了) */

	function cli_noti_one_user_info($userid ){

		return $this->send_cmd_new(1100,$userid, null, new cli_noti_one_user_info_out(), 0x4854c772);
	}
	
	/* 得到当前地图的所有人信息 */

	function cli_get_all_user_info_from_cur_map($userid ){

		return $this->send_cmd_new(1101,$userid, null, new cli_get_all_user_info_from_cur_map_out(), 0x5b4b845a);
	}
	
	/* 用户移动 多人模式下 */
	/* 调用方式还可以是： 
		$in=new cli_user_move_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_user_move($userid,$in );
	*/

	function cli_user_move($userid , $start_x=null, $start_y=null, $mouse_x=null, $mouse_y=null, $type=null){

		if ( $start_x instanceof cli_user_move_in ){
			$in=$start_x;
		}else{
			$in=new cli_user_move_in();
			$in->start_x=$start_x;
			$in->start_y=$start_y;
			$in->mouse_x=$mouse_x;
			$in->mouse_y=$mouse_y;
			$in->type=$type;

		}
		
		return $this->send_cmd_new(1102,$userid, $in, null, 0x20304753);
	}
	
	/* 通知用户移动 */

	function cli_noti_user_move($userid ){

		return $this->send_cmd_new(1103,$userid, null, new cli_noti_user_move_out(), 0x55ccb12f);
	}
	
	/* 通知用户离开地图 */

	function cli_noti_user_left_map($userid ){

		return $this->send_cmd_new(1104,$userid, null, new cli_noti_user_left_map_out(), 0x9e97ab66);
	}
	
	/* 聊天 */
	/* 调用方式还可以是： 
		$in=new cli_talk_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_talk($userid,$in );
	*/

	function cli_talk($userid , $obj_userid=null, $msg=null){

		if ( $obj_userid instanceof cli_talk_in ){
			$in=$obj_userid;
		}else{
			$in=new cli_talk_in();
			$in->obj_userid=$obj_userid;
			$in->msg=$msg;

		}
		
		return $this->send_cmd_new(1105,$userid, $in, null, 0x0e444aef);
	}
	
	/* 通知用户更换装扮 */

	function cli_noti_user_use_clothes_list($userid ){

		return $this->send_cmd_new(1106,$userid, null, new cli_noti_user_use_clothes_list_out(), 0xaf464111);
	}
	
	/* 通知聊天 */

	function cli_noti_talk($userid ){

		return $this->send_cmd_new(1107,$userid, null, new cli_noti_talk_out(), 0x52b6ada9);
	}
	
	/* 发表情 */
	/* 调用方式还可以是： 
		$in=new cli_show_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_show($userid,$in );
	*/

	function cli_show($userid , $expression_id=null, $obj_userid=null){

		if ( $expression_id instanceof cli_show_in ){
			$in=$expression_id;
		}else{
			$in=new cli_show_in();
			$in->expression_id=$expression_id;
			$in->obj_userid=$obj_userid;

		}
		
		return $this->send_cmd_new(1108,$userid, $in, null, 0xe8810497);
	}
	
	/* 通知某用户做表情 */

	function cli_noti_show($userid ){

		return $this->send_cmd_new(1109,$userid, null, new cli_noti_show_out(), 0x33e74a5e);
	}
	
	/* 设置忙状态 */
	/* 调用方式还可以是： 
		$in=new cli_set_busy_state_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_set_busy_state($userid,$in );
	*/

	function cli_set_busy_state($userid , $state=null){

		if ( $state instanceof cli_set_busy_state_in ){
			$in=$state;
		}else{
			$in=new cli_set_busy_state_in();
			$in->state=$state;

		}
		
		return $this->send_cmd_new(1110,$userid, $in, null, 0x43f7232d);
	}
	
	/* 通知有人升级 */

	function cli_noti_user_level_up($userid ){

		return $this->send_cmd_new(1111,$userid, null, new cli_noti_user_level_up_out(), 0x01d4492a);
	}
	
	/* 请求和某人玩游戏 */
	/* 调用方式还可以是： 
		$in=new cli_game_request_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_game_request($userid,$in );
	*/

	function cli_game_request($userid , $gameid=null, $obj_userid=null){

		if ( $gameid instanceof cli_game_request_in ){
			$in=$gameid;
		}else{
			$in=new cli_game_request_in();
			$in->gameid=$gameid;
			$in->obj_userid=$obj_userid;

		}
		
		return $this->send_cmd_new(1201,$userid, $in, new cli_game_request_out(), 0xba487b08);
	}
	
	/* 通知游戏请求 */

	function cli_noti_game_request($userid ){

		return $this->send_cmd_new(1202,$userid, null, new cli_noti_game_request_out(), 0xddb60a52);
	}
	
	/* 确认请求 */
	/* 调用方式还可以是： 
		$in=new cli_game_play_with_other_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_game_play_with_other($userid,$in );
	*/

	function cli_game_play_with_other($userid , $gameid=null, $request_session=null, $obj_userid=null, $is_start=null, $why_not_start_flag=null){

		if ( $gameid instanceof cli_game_play_with_other_in ){
			$in=$gameid;
		}else{
			$in=new cli_game_play_with_other_in();
			$in->gameid=$gameid;
			$in->request_session=$request_session;
			$in->obj_userid=$obj_userid;
			$in->is_start=$is_start;
			$in->why_not_start_flag=$why_not_start_flag;

		}
		
		return $this->send_cmd_new(1203,$userid, $in, new cli_game_play_with_other_out(), 0x5fb58c09);
	}
	
	/* 通知请求游戏是否开始,如果is_start=1,（发给双方)，is_start=0(发给请求方) */

	function cli_noti_game_is_start($userid ){

		return $this->send_cmd_new(1204,$userid, null, new cli_noti_game_is_start_out(), 0xaf7927e5);
	}
	
	/* 客户端确认游戏可以开始了( 1204 返回is_start=1 时，在加载游戏完成时发) */

	function cli_game_can_start($userid ){

		return $this->send_cmd_new(1205,$userid, null, null, 0x511673c0);
	}
	
	/* 通知游戏开始 */

	function cli_noti_game_start($userid ){

		return $this->send_cmd_new(1206,$userid, null, new cli_noti_game_start_out(), 0x81759933);
	}
	
	/* 用户游戏操作 */
	/* 调用方式还可以是： 
		$in=new cli_game_opt_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_game_opt($userid,$in );
	*/

	function cli_game_opt($userid , $gamemsg=null){

		if ( $gamemsg instanceof cli_game_opt_in ){
			$in=$gamemsg;
		}else{
			$in=new cli_game_opt_in();
			$in->gamemsg=$gamemsg;

		}
		
		return $this->send_cmd_new(1207,$userid, $in, null, 0xe2b1aad9);
	}
	
	/* 通知用户游戏操作 */

	function cli_noti_game_opt($userid ){

		return $this->send_cmd_new(1208,$userid, null, new cli_noti_game_opt_out(), 0x0d347d17);
	}
	
	/* 当前一局游戏结束 */
	/* 调用方式还可以是： 
		$in=new cli_cur_game_end_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_cur_game_end($userid,$in );
	*/

	function cli_cur_game_end($userid , $win_flag=null){

		if ( $win_flag instanceof cli_cur_game_end_in ){
			$in=$win_flag;
		}else{
			$in=new cli_cur_game_end_in();
			$in->win_flag=$win_flag;

		}
		
		return $this->send_cmd_new(1209,$userid, $in, null, 0xf944db64);
	}
	
	/* 通知游戏结束 */

	function cli_noti_game_end($userid ){

		return $this->send_cmd_new(1210,$userid, null, new cli_noti_game_end_out(), 0xf86ab508);
	}
	
	/* 离开游戏 */

	function cli_game_user_left_game($userid ){

		return $this->send_cmd_new(1211,$userid, null, null, 0x5b5a3ffd);
	}
	
	/* 用户离开游戏通知 */

	function cli_noti_game_user_left_game($userid ){

		return $this->send_cmd_new(1212,$userid, null, new cli_noti_game_user_left_game_out(), 0xbb260459);
	}
	
	/* 购买物品 */
	/* 调用方式还可以是： 
		$in=new cli_buy_item_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_buy_item($userid,$in );
	*/

	function cli_buy_item($userid , $itemid=null){

		if ( $itemid instanceof cli_buy_item_in ){
			$in=$itemid;
		}else{
			$in=new cli_buy_item_in();
			$in->itemid=$itemid;

		}
		
		return $this->send_cmd_new(1213,$userid, $in, new cli_buy_item_out(), 0x98174eba);
	}
	
	/* 通知场景中的所有玩家有人进入游戏 */

	function cli_noti_game_user($userid ){

		return $this->send_cmd_new(1214,$userid, null, new cli_noti_game_user_out(), 0x0ba31f2c);
	}
	
	/* 获取当前场景正在游戏的玩家 */

	function cli_get_game_user($userid ){

		return $this->send_cmd_new(1215,$userid, null, new cli_get_game_user_out(), 0xcdb47db1);
	}
	
	/* 使用游戏积分购买物品 */
	/* 调用方式还可以是： 
		$in=new cli_buy_item_use_gamept_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_buy_item_use_gamept($userid,$in );
	*/

	function cli_buy_item_use_gamept($userid , $itemid=null){

		if ( $itemid instanceof cli_buy_item_use_gamept_in ){
			$in=$itemid;
		}else{
			$in=new cli_buy_item_use_gamept_in();
			$in->itemid=$itemid;

		}
		
		return $this->send_cmd_new(1216,$userid, $in, new cli_buy_item_use_gamept_out(), 0x937ad5bd);
	}
	
	/* 获取游戏座位信息 */

	function cli_noti_game_seat($userid ){

		return $this->send_cmd_new(1217,$userid, null, new cli_noti_game_seat_out(), 0xbac528d8);
	}
	
	/* 拉取有效的小游戏积分 */

	function cli_get_valid_gamept($userid ){

		return $this->send_cmd_new(1218,$userid, null, new cli_get_valid_gamept_out(), 0x459682e3);
	}
	
	/* 投稿投诉 */
	/* 调用方式还可以是： 
		$in=new cli_post_msg_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_post_msg($userid,$in );
	*/

	function cli_post_msg($userid , $type=null, $title=null, $msg=null){

		if ( $type instanceof cli_post_msg_in ){
			$in=$type;
		}else{
			$in=new cli_post_msg_in();
			$in->type=$type;
			$in->title=$title;
			$in->msg=$msg;

		}
		
		return $this->send_cmd_new(1301,$userid, $in, null, 0xedc17e31);
	}
	
	/* 打工获得抽奖机会 */

	function cli_work_get_lottery($userid ){

		return $this->send_cmd_new(1401,$userid, null, new cli_work_get_lottery_out(), 0x491f80bf);
	}
	
	/* 抽奖 */

	function cli_draw_lottery($userid ){

		return $this->send_cmd_new(1402,$userid, null, new cli_draw_lottery_out(), 0x4983d8c5);
	}
	
	/* 获取剩余抽奖机会 */

	function cli_get_lottery_count($userid ){

		return $this->send_cmd_new(1403,$userid, null, new cli_get_lottery_count_out(), 0x51b8de2e);
	}
	
	/* 挂铃铛赢奖励 */
	/* 调用方式还可以是： 
		$in=new cli_hang_bell_get_item_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_hang_bell_get_item($userid,$in );
	*/

	function cli_hang_bell_get_item($userid , $bell_type=null, $zone_type=null){

		if ( $bell_type instanceof cli_hang_bell_get_item_in ){
			$in=$bell_type;
		}else{
			$in=new cli_hang_bell_get_item_in();
			$in->bell_type=$bell_type;
			$in->zone_type=$zone_type;

		}
		
		return $this->send_cmd_new(1404,$userid, $in, new cli_hang_bell_get_item_out(), 0xd39e6602);
	}
	
	/* 点击圣诞树统计 */

	function cli_click_chris_tree($userid ){

		return $this->send_cmd_new(1405,$userid, null, null, 0xcf1c12cc);
	}
	
	/* 点击新手指引 */

	function cli_click_guide($userid ){

		return $this->send_cmd_new(1406,$userid, null, null, 0x9f0e3ad1);
	}
	
	/* 点击统计 */
	/* 调用方式还可以是： 
		$in=new cli_click_stat_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_click_stat($userid,$in );
	*/

	function cli_click_stat($userid , $type=null){

		if ( $type instanceof cli_click_stat_in ){
			$in=$type;
		}else{
			$in=new cli_click_stat_in();
			$in->type=$type;

		}
		
		return $this->send_cmd_new(1407,$userid, $in, null, 0x6997c174);
	}
	
	/* 点击游戏王公告 */

	function cli_click_game_notice($userid ){

		return $this->send_cmd_new(1408,$userid, null, null, 0x73830881);
	}
	
	/* 通知用户得到邀请函 */

	function cli_noti_get_invitation($userid ){

		return $this->send_cmd_new(1409,$userid, null, new cli_noti_get_invitation_out(), 0xb33f420b);
	}
	
	/* 通知客户端系统时间 */

	function cli_noti_svr_time($userid ){

		return $this->send_cmd_new(1410,$userid, null, new cli_noti_svr_time_out(), 0xd317b23a);
	}
	
	/* 点击宠物蛋 */

	function cli_click_pet_egg($userid ){

		return $this->send_cmd_new(1411,$userid, null, null, 0x3de04bd1);
	}
	
	/* 获取登录宝箱 */

	function cli_get_login_chest($userid ){

		return $this->send_cmd_new(1412,$userid, null, new cli_get_login_chest_out(), 0x50943a28);
	}
	
	/* 获取登录奖励 */
	/* 调用方式还可以是： 
		$in=new cli_get_login_reward_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->cli_get_login_reward($userid,$in );
	*/

	function cli_get_login_reward($userid , $index=null){

		if ( $index instanceof cli_get_login_reward_in ){
			$in=$index;
		}else{
			$in=new cli_get_login_reward_in();
			$in->index=$index;

		}
		
		return $this->send_cmd_new(1413,$userid, $in, new cli_get_login_reward_out(), 0x10d92a72);
	}
	
};
?>
