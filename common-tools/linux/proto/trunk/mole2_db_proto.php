<?php
require_once("proto_base.php");

class stru_cloth_info {
	/* 获取时间 */
	#类型:uint32
	public $gettime;

	/*  */
	#类型:uint32
	public $clothid;

	/* 位置标识 */
	#类型:uint32
	public $gridid;

	/* 体力最大值 */
	#类型:uint32
	public $hpmax;

	/* 魔法最大值 */
	#类型:uint32
	public $mpmax;

	/* 等级 */
	#类型:uint16
	public $level;

	/* 耐久 */
	#类型:uint16
	public $duration;

	/*  */
	#类型:uint16
	public $mduration;

	/* 攻击 */
	#类型:uint16
	public $atk;

	/* 魔法攻击 */
	#类型:uint16
	public $matk;

	/* 防御 */
	#类型:uint16
	public $def;

	/* 魔法防御 */
	#类型:uint16
	public $mdef;

	/* 速度 */
	#类型:uint16
	public $speed;

	/* 精神 */
	#类型:uint16
	public $spirit;

	/* 恢复 */
	#类型:uint16
	public $resume;

	/* 命中值 */
	#类型:uint16
	public $hit;

	/* 闪避值 */
	#类型:uint16
	public $dodge;

	/* 必杀 */
	#类型:uint16
	public $crit;

	/* 反击 */
	#类型:uint16
	public $fightback;

	/* 抗毒 */
	#类型:uint16
	public $rpoison;

	/* 抗石化 */
	#类型:uint16
	public $rlithification;

	/* 抗昏睡 */
	#类型:uint16
	public $rlethargy;

	/* 抗酒醉 */
	#类型:uint16
	public $rinebriation;

	/* 抗混乱 */
	#类型:uint16
	public $rconfusion;

	/* 抗遗忘 */
	#类型:uint16
	public $roblivion;

	/*  */
	#类型:uint32
	public $quality;

	/*  */
	#类型:uint32
	public $validday;

	/*  */
	#类型:uint32
	public $crystal_attr;

	/*  */
	#类型:uint32
	public $bless_type;


	public function stru_cloth_info(){

	}

	public function read_from_obj($obj ){
		$this->gettime=$obj->gettime;
		$this->clothid=$obj->clothid;
		$this->gridid=$obj->gridid;
		$this->hpmax=$obj->hpmax;
		$this->mpmax=$obj->mpmax;
		$this->level=$obj->level;
		$this->duration=$obj->duration;
		$this->mduration=$obj->mduration;
		$this->atk=$obj->atk;
		$this->matk=$obj->matk;
		$this->def=$obj->def;
		$this->mdef=$obj->mdef;
		$this->speed=$obj->speed;
		$this->spirit=$obj->spirit;
		$this->resume=$obj->resume;
		$this->hit=$obj->hit;
		$this->dodge=$obj->dodge;
		$this->crit=$obj->crit;
		$this->fightback=$obj->fightback;
		$this->rpoison=$obj->rpoison;
		$this->rlithification=$obj->rlithification;
		$this->rlethargy=$obj->rlethargy;
		$this->rinebriation=$obj->rinebriation;
		$this->rconfusion=$obj->rconfusion;
		$this->roblivion=$obj->roblivion;
		$this->quality=$obj->quality;
		$this->validday=$obj->validday;
		$this->crystal_attr=$obj->crystal_attr;
		$this->bless_type=$obj->bless_type;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gettime)) return false;
		if (!$ba->read_uint32($this->clothid)) return false;
		if (!$ba->read_uint32($this->gridid)) return false;
		if (!$ba->read_uint32($this->hpmax)) return false;
		if (!$ba->read_uint32($this->mpmax)) return false;
		if (!$ba->read_uint16($this->level)) return false;
		if (!$ba->read_uint16($this->duration)) return false;
		if (!$ba->read_uint16($this->mduration)) return false;
		if (!$ba->read_uint16($this->atk)) return false;
		if (!$ba->read_uint16($this->matk)) return false;
		if (!$ba->read_uint16($this->def)) return false;
		if (!$ba->read_uint16($this->mdef)) return false;
		if (!$ba->read_uint16($this->speed)) return false;
		if (!$ba->read_uint16($this->spirit)) return false;
		if (!$ba->read_uint16($this->resume)) return false;
		if (!$ba->read_uint16($this->hit)) return false;
		if (!$ba->read_uint16($this->dodge)) return false;
		if (!$ba->read_uint16($this->crit)) return false;
		if (!$ba->read_uint16($this->fightback)) return false;
		if (!$ba->read_uint16($this->rpoison)) return false;
		if (!$ba->read_uint16($this->rlithification)) return false;
		if (!$ba->read_uint16($this->rlethargy)) return false;
		if (!$ba->read_uint16($this->rinebriation)) return false;
		if (!$ba->read_uint16($this->rconfusion)) return false;
		if (!$ba->read_uint16($this->roblivion)) return false;
		if (!$ba->read_uint32($this->quality)) return false;
		if (!$ba->read_uint32($this->validday)) return false;
		if (!$ba->read_uint32($this->crystal_attr)) return false;
		if (!$ba->read_uint32($this->bless_type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gettime)) return false;
		if (!$ba->write_uint32($this->clothid)) return false;
		if (!$ba->write_uint32($this->gridid)) return false;
		if (!$ba->write_uint32($this->hpmax)) return false;
		if (!$ba->write_uint32($this->mpmax)) return false;
		if (!$ba->write_uint16($this->level)) return false;
		if (!$ba->write_uint16($this->duration)) return false;
		if (!$ba->write_uint16($this->mduration)) return false;
		if (!$ba->write_uint16($this->atk)) return false;
		if (!$ba->write_uint16($this->matk)) return false;
		if (!$ba->write_uint16($this->def)) return false;
		if (!$ba->write_uint16($this->mdef)) return false;
		if (!$ba->write_uint16($this->speed)) return false;
		if (!$ba->write_uint16($this->spirit)) return false;
		if (!$ba->write_uint16($this->resume)) return false;
		if (!$ba->write_uint16($this->hit)) return false;
		if (!$ba->write_uint16($this->dodge)) return false;
		if (!$ba->write_uint16($this->crit)) return false;
		if (!$ba->write_uint16($this->fightback)) return false;
		if (!$ba->write_uint16($this->rpoison)) return false;
		if (!$ba->write_uint16($this->rlithification)) return false;
		if (!$ba->write_uint16($this->rlethargy)) return false;
		if (!$ba->write_uint16($this->rinebriation)) return false;
		if (!$ba->write_uint16($this->rconfusion)) return false;
		if (!$ba->write_uint16($this->roblivion)) return false;
		if (!$ba->write_uint32($this->quality)) return false;
		if (!$ba->write_uint32($this->validday)) return false;
		if (!$ba->write_uint32($this->crystal_attr)) return false;
		if (!$ba->write_uint32($this->bless_type)) return false;
		return true;
	}

};

	
class buy_cloth_in_mall_in {
	/**/
	#变长数组,最大长度:32, 类型:stru_cloth_info
	public $cloths =array();


	public function buy_cloth_in_mall_in(){

	}

	public function read_from_obj($obj ){
		$this->cloths=array();
		{for($i=0;$i<count($obj->cloths);$i++){
			$this->cloths[$i]=new stru_cloth_info();
			if (!$this->cloths[$i]->read_from_obj($obj->cloths )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$cloths_count=0 ;
		if (!$ba->read_uint32( $cloths_count )) return false;
		if ($cloths_count>32) return false;
		$this->cloths=array();
		{for($i=0; $i<$cloths_count;$i++){
			$this->cloths[$i]=new stru_cloth_info();
			if (!$this->cloths[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$cloths_count=count($this->cloths);
        if ($cloths_count>32 ) return false; 
        $ba->write_uint32($cloths_count);
		{for($i=0; $i<$cloths_count;$i++){
			if ( ! $this->cloths[$i] instanceof stru_cloth_info ) return false; 
			if (!$this->cloths[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class stru_item_count {
	/*  */
	#类型:uint32
	public $itemid;

	/* 个数 */
	#类型:uint32
	public $count;


	public function stru_item_count(){

	}

	public function read_from_obj($obj ){
		$this->itemid=$obj->itemid;
		$this->count=$obj->count;
		return true;
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

	
class buy_item_in_mall_in {
	/**/
	#变长数组,最大长度:256, 类型:stru_item_count
	public $items =array();


	public function buy_item_in_mall_in(){

	}

	public function read_from_obj($obj ){
		$this->items=array();
		{for($i=0;$i<count($obj->items);$i++){
			$this->items[$i]=new stru_item_count();
			if (!$this->items[$i]->read_from_obj($obj->items )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$items_count=0 ;
		if (!$ba->read_uint32( $items_count )) return false;
		if ($items_count>256) return false;
		$this->items=array();
		{for($i=0; $i<$items_count;$i++){
			$this->items[$i]=new stru_item_count();
			if (!$this->items[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$items_count=count($this->items);
        if ($items_count>256 ) return false; 
        $ba->write_uint32($items_count);
		{for($i=0; $i<$items_count;$i++){
			if ( ! $this->items[$i] instanceof stru_item_count ) return false; 
			if (!$this->items[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class comm_get_dbser_version_out {
	/* 消息 */
	#定长数组,长度:255, 类型:char 
	public $msg ;


	public function comm_get_dbser_version_out(){

	}

	public function read_from_obj($obj ){
		$this->msg=$obj->msg;
		return true;
	}


	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->msg,255)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->msg,255);
		return true;
	}

};

	
class get_all_pet_type_out {
	/**/
	#变长数组,最大长度:3000, 类型:uint32_t
	public $petlist =array();


	public function get_all_pet_type_out(){

	}

	public function read_from_obj($obj ){
		$this->petlist=array();
		{for($i=0;$i<count($obj->petlist);$i++){
			 $this->petlist=$obj->petlist;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$petlist_count=0 ;
		if (!$ba->read_uint32( $petlist_count )) return false;
		if ($petlist_count>3000) return false;
		$this->petlist=array();
		{for($i=0; $i<$petlist_count;$i++){
			if (!$ba->read_uint32($this->petlist[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$petlist_count=count($this->petlist);
        if ($petlist_count>3000 ) return false; 
        $ba->write_uint32($petlist_count);
		{for($i=0; $i<$petlist_count;$i++){
			if (!$ba->write_uint32($this->petlist[$i])) return false;
		}}
		return true;
	}

};

	
class get_rand_info_range_in {
	/*  */
	#类型:uint32
	public $minid;

	/*  */
	#类型:uint32
	public $maxid;


	public function get_rand_info_range_in(){

	}

	public function read_from_obj($obj ){
		$this->minid=$obj->minid;
		$this->maxid=$obj->maxid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->minid)) return false;
		if (!$ba->read_uint32($this->maxid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->minid)) return false;
		if (!$ba->write_uint32($this->maxid)) return false;
		return true;
	}

};

	
class stru_rand_info {
	/* 时间 */
	#类型:uint32
	public $time;

	/*  */
	#类型:uint32
	public $randid;

	/* 个数 */
	#类型:uint32
	public $count;


	public function stru_rand_info(){

	}

	public function read_from_obj($obj ){
		$this->time=$obj->time;
		$this->randid=$obj->randid;
		$this->count=$obj->count;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->time)) return false;
		if (!$ba->read_uint32($this->randid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->time)) return false;
		if (!$ba->write_uint32($this->randid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class get_rand_info_range_out {
	/**/
	#变长数组,最大长度:2500, 类型:stru_rand_info
	public $item_list =array();


	public function get_rand_info_range_out(){

	}

	public function read_from_obj($obj ){
		$this->item_list=array();
		{for($i=0;$i<count($obj->item_list);$i++){
			$this->item_list[$i]=new stru_rand_info();
			if (!$this->item_list[$i]->read_from_obj($obj->item_list )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		if ($item_list_count>2500) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new stru_rand_info();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$item_list_count=count($this->item_list);
        if ($item_list_count>2500 ) return false; 
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof stru_rand_info ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class log_vip_item_in {
	/*  */
	#类型:uint32
	public $logtype;

	/*  */
	#类型:uint32
	public $itemid;

	/* 个数 */
	#类型:int32
	public $count;


	public function log_vip_item_in(){

	}

	public function read_from_obj($obj ){
		$this->logtype=$obj->logtype;
		$this->itemid=$obj->itemid;
		$this->count=$obj->count;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->logtype)) return false;
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_int32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->logtype)) return false;
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_int32($this->count)) return false;
		return true;
	}

};

	
class stru_cloth_limit {
	/*  */
	#类型:uint32
	public $clothid;

	/* 个数 */
	#类型:uint32
	public $count;

	/*  */
	#类型:uint32
	public $limit;


	public function stru_cloth_limit(){

	}

	public function read_from_obj($obj ){
		$this->clothid=$obj->clothid;
		$this->count=$obj->count;
		$this->limit=$obj->limit;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->clothid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		if (!$ba->read_uint32($this->limit)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->clothid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		if (!$ba->write_uint32($this->limit)) return false;
		return true;
	}

};

	
class mall_chk_cloths_limits_in {
	/**/
	#变长数组,最大长度:32, 类型:stru_cloth_limit
	public $cloths =array();


	public function mall_chk_cloths_limits_in(){

	}

	public function read_from_obj($obj ){
		$this->cloths=array();
		{for($i=0;$i<count($obj->cloths);$i++){
			$this->cloths[$i]=new stru_cloth_limit();
			if (!$this->cloths[$i]->read_from_obj($obj->cloths )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$cloths_count=0 ;
		if (!$ba->read_uint32( $cloths_count )) return false;
		if ($cloths_count>32) return false;
		$this->cloths=array();
		{for($i=0; $i<$cloths_count;$i++){
			$this->cloths[$i]=new stru_cloth_limit();
			if (!$this->cloths[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$cloths_count=count($this->cloths);
        if ($cloths_count>32 ) return false; 
        $ba->write_uint32($cloths_count);
		{for($i=0; $i<$cloths_count;$i++){
			if ( ! $this->cloths[$i] instanceof stru_cloth_limit ) return false; 
			if (!$this->cloths[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_add_day_count_in {
	/*  */
	#类型:uint32
	public $itemid;

	/* 个数 */
	#类型:uint32
	public $count;


	public function mole2_add_day_count_in(){

	}

	public function read_from_obj($obj ){
		$this->itemid=$obj->itemid;
		$this->count=$obj->count;
		return true;
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

	
class mole2_add_graduation_uid_in {
	/*  */
	#类型:uint32
	public $uid;


	public function mole2_add_graduation_uid_in(){

	}

	public function read_from_obj($obj ){
		$this->uid=$obj->uid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->uid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->uid)) return false;
		return true;
	}

};

	
class mole2_add_medals_in {
	/*  */
	#类型:uint32
	public $medals;


	public function mole2_add_medals_in(){

	}

	public function read_from_obj($obj ){
		$this->medals=$obj->medals;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->medals)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->medals)) return false;
		return true;
	}

};

	
class mole2_add_medals_out {
	/*  */
	#类型:uint32
	public $medals;


	public function mole2_add_medals_out(){

	}

	public function read_from_obj($obj ){
		$this->medals=$obj->medals;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->medals)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->medals)) return false;
		return true;
	}

};

	
class mole2_add_relation_exp_in {
	/* 经验 */
	#类型:int32
	public $exp;


	public function mole2_add_relation_exp_in(){

	}

	public function read_from_obj($obj ){
		$this->exp=$obj->exp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_int32($this->exp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_int32($this->exp)) return false;
		return true;
	}

};

	
class mole2_add_relation_uid_in {
	/* flag =1 加师傅,自己是徒弟,2,加徒弟,自己是师傅 */
	#类型:uint32
	public $flag;

	/*  */
	#类型:uint32
	public $uid;


	public function mole2_add_relation_uid_in(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->uid=$obj->uid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->uid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->uid)) return false;
		return true;
	}

};

	
class mole2_add_relation_uid_out {
	/* 标志 */
	#类型:uint32
	public $flag;

	/*  */
	#类型:uint32
	public $uid;


	public function mole2_add_relation_uid_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->uid=$obj->uid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->uid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->uid)) return false;
		return true;
	}

};

	
class mole2_add_relation_val_in {
	/*  */
	#类型:uint32
	public $uid;

	/*  */
	#类型:int32
	public $addval;


	public function mole2_add_relation_val_in(){

	}

	public function read_from_obj($obj ){
		$this->uid=$obj->uid;
		$this->addval=$obj->addval;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->uid)) return false;
		if (!$ba->read_int32($this->addval)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->uid)) return false;
		if (!$ba->write_int32($this->addval)) return false;
		return true;
	}

};

	
class mole2_add_relation_val_out {
	/*  */
	#类型:uint32
	public $uid;

	/* 数值 */
	#类型:uint32
	public $value;


	public function mole2_add_relation_val_out(){

	}

	public function read_from_obj($obj ){
		$this->uid=$obj->uid;
		$this->value=$obj->value;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->uid)) return false;
		if (!$ba->read_uint32($this->value)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->uid)) return false;
		if (!$ba->write_uint32($this->value)) return false;
		return true;
	}

};

	
class mole2_add_skill_exp_in {
	/*  */
	#类型:int32
	public $add_exp;


	public function mole2_add_skill_exp_in(){

	}

	public function read_from_obj($obj ){
		$this->add_exp=$obj->add_exp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_int32($this->add_exp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_int32($this->add_exp)) return false;
		return true;
	}

};

	
class mole2_add_skill_exp_out {
	/*  */
	#类型:uint32
	public $cur_exp;


	public function mole2_add_skill_exp_out(){

	}

	public function read_from_obj($obj ){
		$this->cur_exp=$obj->cur_exp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->cur_exp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->cur_exp)) return false;
		return true;
	}

};

	
class mole2_del_relation_uid_in {
	/* 标志 */
	#类型:uint32
	public $flag;

	/*  */
	#类型:uint32
	public $uid;

	/*  */
	#类型:uint32
	public $op_uid;


	public function mole2_del_relation_uid_in(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->uid=$obj->uid;
		$this->op_uid=$obj->op_uid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->uid)) return false;
		if (!$ba->read_uint32($this->op_uid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->uid)) return false;
		if (!$ba->write_uint32($this->op_uid)) return false;
		return true;
	}

};

	
class stru_relation_item {
	/*  */
	#类型:uint32
	public $uid;

	/*  */
	#类型:uint32
	public $day;

	/*  */
	#类型:uint32
	public $val;


	public function stru_relation_item(){

	}

	public function read_from_obj($obj ){
		$this->uid=$obj->uid;
		$this->day=$obj->day;
		$this->val=$obj->val;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->uid)) return false;
		if (!$ba->read_uint32($this->day)) return false;
		if (!$ba->read_uint32($this->val)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->uid)) return false;
		if (!$ba->write_uint32($this->day)) return false;
		if (!$ba->write_uint32($this->val)) return false;
		return true;
	}

};

	
class mole2_del_relation_uid_out {
	/* 标志 */
	#类型:uint32
	public $flag;

	/* 经验 */
	#类型:uint32
	public $exp;

	/*  */
	#类型:uint32
	public $graduation;

	/*  */
	#类型:uint32
	public $total_val;

	/**/
	#变长数组,最大长度:5, 类型:stru_relation_item
	public $relations =array();


	public function mole2_del_relation_uid_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->exp=$obj->exp;
		$this->graduation=$obj->graduation;
		$this->total_val=$obj->total_val;
		$this->relations=array();
		{for($i=0;$i<count($obj->relations);$i++){
			$this->relations[$i]=new stru_relation_item();
			if (!$this->relations[$i]->read_from_obj($obj->relations )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		if (!$ba->read_uint32($this->graduation)) return false;
		if (!$ba->read_uint32($this->total_val)) return false;

		$relations_count=0 ;
		if (!$ba->read_uint32( $relations_count )) return false;
		if ($relations_count>5) return false;
		$this->relations=array();
		{for($i=0; $i<$relations_count;$i++){
			$this->relations[$i]=new stru_relation_item();
			if (!$this->relations[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		if (!$ba->write_uint32($this->graduation)) return false;
		if (!$ba->write_uint32($this->total_val)) return false;
		$relations_count=count($this->relations);
        if ($relations_count>5 ) return false; 
        $ba->write_uint32($relations_count);
		{for($i=0; $i<$relations_count;$i++){
			if ( ! $this->relations[$i] instanceof stru_relation_item ) return false; 
			if (!$this->relations[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_del_vip_buff_in {
	/*宠物ID*/
	#变长数组,最大长度:100, 类型:uint32_t
	public $petid =array();


	public function mole2_del_vip_buff_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=array();
		{for($i=0;$i<count($obj->petid);$i++){
			 $this->petid=$obj->petid;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$petid_count=0 ;
		if (!$ba->read_uint32( $petid_count )) return false;
		if ($petid_count>100) return false;
		$this->petid=array();
		{for($i=0; $i<$petid_count;$i++){
			if (!$ba->read_uint32($this->petid[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$petid_count=count($this->petid);
        if ($petid_count>100 ) return false; 
        $ba->write_uint32($petid_count);
		{for($i=0; $i<$petid_count;$i++){
			if (!$ba->write_uint32($this->petid[$i])) return false;
		}}
		return true;
	}

};

	
class mole2_get_day_count_in {
	/*  */
	#类型:uint32
	public $itemid;


	public function mole2_get_day_count_in(){

	}

	public function read_from_obj($obj ){
		$this->itemid=$obj->itemid;
		return true;
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

	
class mole2_get_day_count_out {
	/*  */
	#类型:uint32
	public $total;

	/* 个数 */
	#类型:uint32
	public $count;


	public function mole2_get_day_count_out(){

	}

	public function read_from_obj($obj ){
		$this->total=$obj->total;
		$this->count=$obj->count;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->total)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->total)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class mole2_get_hero_team_out {
	/*  */
	#类型:uint32
	public $teamid;

	/*  */
	#类型:uint32
	public $medals;


	public function mole2_get_hero_team_out(){

	}

	public function read_from_obj($obj ){
		$this->teamid=$obj->teamid;
		$this->medals=$obj->medals;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->teamid)) return false;
		if (!$ba->read_uint32($this->medals)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->teamid)) return false;
		if (!$ba->write_uint32($this->medals)) return false;
		return true;
	}

};

	
class mole2_get_relation_out {
	/* 标志 */
	#类型:uint32
	public $flag;

	/* 经验 */
	#类型:uint32
	public $exp;

	/*  */
	#类型:uint32
	public $graduation;

	/*  */
	#类型:uint32
	public $total_val;

	/**/
	#变长数组,最大长度:5, 类型:stru_relation_item
	public $relations =array();


	public function mole2_get_relation_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->exp=$obj->exp;
		$this->graduation=$obj->graduation;
		$this->total_val=$obj->total_val;
		$this->relations=array();
		{for($i=0;$i<count($obj->relations);$i++){
			$this->relations[$i]=new stru_relation_item();
			if (!$this->relations[$i]->read_from_obj($obj->relations )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		if (!$ba->read_uint32($this->graduation)) return false;
		if (!$ba->read_uint32($this->total_val)) return false;

		$relations_count=0 ;
		if (!$ba->read_uint32( $relations_count )) return false;
		if ($relations_count>5) return false;
		$this->relations=array();
		{for($i=0; $i<$relations_count;$i++){
			$this->relations[$i]=new stru_relation_item();
			if (!$this->relations[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		if (!$ba->write_uint32($this->graduation)) return false;
		if (!$ba->write_uint32($this->total_val)) return false;
		$relations_count=count($this->relations);
        if ($relations_count>5 ) return false; 
        $ba->write_uint32($relations_count);
		{for($i=0; $i<$relations_count;$i++){
			if ( ! $this->relations[$i] instanceof stru_relation_item ) return false; 
			if (!$this->relations[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class stru_team_rank_t {
	/*  */
	#类型:uint32
	public $teamid;

	/* 个数 */
	#类型:uint32
	public $count;

	/*  */
	#类型:uint32
	public $medals;


	public function stru_team_rank_t(){

	}

	public function read_from_obj($obj ){
		$this->teamid=$obj->teamid;
		$this->count=$obj->count;
		$this->medals=$obj->medals;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->teamid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		if (!$ba->read_uint32($this->medals)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->teamid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		if (!$ba->write_uint32($this->medals)) return false;
		return true;
	}

};

	
class mole2_get_team_rank_out {
	/**/
	#变长数组,最大长度:4, 类型:stru_team_rank_t
	public $ranks =array();


	public function mole2_get_team_rank_out(){

	}

	public function read_from_obj($obj ){
		$this->ranks=array();
		{for($i=0;$i<count($obj->ranks);$i++){
			$this->ranks[$i]=new stru_team_rank_t();
			if (!$this->ranks[$i]->read_from_obj($obj->ranks )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$ranks_count=0 ;
		if (!$ba->read_uint32( $ranks_count )) return false;
		if ($ranks_count>4) return false;
		$this->ranks=array();
		{for($i=0; $i<$ranks_count;$i++){
			$this->ranks[$i]=new stru_team_rank_t();
			if (!$this->ranks[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$ranks_count=count($this->ranks);
        if ($ranks_count>4 ) return false; 
        $ba->write_uint32($ranks_count);
		{for($i=0; $i<$ranks_count;$i++){
			if ( ! $this->ranks[$i] instanceof stru_team_rank_t ) return false; 
			if (!$this->ranks[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_get_type_pets_in {
	/* 宠物类型ID */
	#类型:uint32
	public $pettype;

	/*  */
	#类型:uint32
	public $start;

	/*  */
	#类型:uint32
	public $limit;


	public function mole2_get_type_pets_in(){

	}

	public function read_from_obj($obj ){
		$this->pettype=$obj->pettype;
		$this->start=$obj->start;
		$this->limit=$obj->limit;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->pettype)) return false;
		if (!$ba->read_uint32($this->start)) return false;
		if (!$ba->read_uint32($this->limit)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->pettype)) return false;
		if (!$ba->write_uint32($this->start)) return false;
		if (!$ba->write_uint32($this->limit)) return false;
		return true;
	}

};

	
class stru_skill_info {
	/* 技能ID */
	#类型:uint32
	public $skillid;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $exp;


	public function stru_skill_info(){

	}

	public function read_from_obj($obj ){
		$this->skillid=$obj->skillid;
		$this->level=$obj->level;
		$this->exp=$obj->exp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->skillid)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->skillid)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		return true;
	}

};

	
class stru_pet_info {
	/* 获取时间 */
	#类型:uint32
	public $gettime;

	/* 宠物类型ID */
	#类型:uint32
	public $pettype;

	/* 种族 */
	#类型:uint32
	public $race;

	/* 标志 */
	#类型:uint32
	public $flag;

	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $exp;

	/* 体力初值 */
	#类型:uint16
	public $physiqueinit;

	/* 力量初值 */
	#类型:uint16
	public $strengthinit;

	/* 耐力初值 */
	#类型:uint16
	public $enduranceinit;

	/* 速度初值 */
	#类型:uint16
	public $quickinit;

	/* 智慧初值 */
	#类型:uint16
	public $iqinit;

	/* 体力成长值 */
	#类型:uint16
	public $physiqueparam;

	/* 力量成长值 */
	#类型:uint16
	public $strengthparam;

	/* 耐力成长值 */
	#类型:uint16
	public $enduranceparam;

	/* 速度成长值 */
	#类型:uint16
	public $quickparam;

	/* 智慧成长值 */
	#类型:uint16
	public $iqparam;

	/* 体力配点值 */
	#类型:uint16
	public $physique_add;

	/* 力量配点值 */
	#类型:uint16
	public $strength_add;

	/* 耐力配点值 */
	#类型:uint16
	public $endurance_add;

	/* 速度配点值 */
	#类型:uint16
	public $quick_add;

	/* 智慧配点值 */
	#类型:uint16
	public $iq_add;

	/* 剩余属性点 */
	#类型:uint16
	public $attr_addition;

	/* 体力 */
	#类型:uint32
	public $hp;

	/* 魔法值 */
	#类型:uint32
	public $mp;

	/* 地 */
	#类型:uint8
	public $earth;

	/* 水 */
	#类型:uint8
	public $water;

	/* 火 */
	#类型:uint8
	public $fire;

	/* 风 */
	#类型:uint8
	public $wind;

	/* 受伤等级 0:未受伤, 1:白 2:黄 3:红 */
	#类型:uint32
	public $injury_state;

	/* 位置 ,不能为0 */
	#类型:uint32
	public $location;

	/**/
	#变长数组,最大长度:10, 类型:stru_skill_info
	public $skills =array();


	public function stru_pet_info(){

	}

	public function read_from_obj($obj ){
		$this->gettime=$obj->gettime;
		$this->pettype=$obj->pettype;
		$this->race=$obj->race;
		$this->flag=$obj->flag;
		$this->nick=$obj->nick;
		$this->level=$obj->level;
		$this->exp=$obj->exp;
		$this->physiqueinit=$obj->physiqueinit;
		$this->strengthinit=$obj->strengthinit;
		$this->enduranceinit=$obj->enduranceinit;
		$this->quickinit=$obj->quickinit;
		$this->iqinit=$obj->iqinit;
		$this->physiqueparam=$obj->physiqueparam;
		$this->strengthparam=$obj->strengthparam;
		$this->enduranceparam=$obj->enduranceparam;
		$this->quickparam=$obj->quickparam;
		$this->iqparam=$obj->iqparam;
		$this->physique_add=$obj->physique_add;
		$this->strength_add=$obj->strength_add;
		$this->endurance_add=$obj->endurance_add;
		$this->quick_add=$obj->quick_add;
		$this->iq_add=$obj->iq_add;
		$this->attr_addition=$obj->attr_addition;
		$this->hp=$obj->hp;
		$this->mp=$obj->mp;
		$this->earth=$obj->earth;
		$this->water=$obj->water;
		$this->fire=$obj->fire;
		$this->wind=$obj->wind;
		$this->injury_state=$obj->injury_state;
		$this->location=$obj->location;
		$this->skills=array();
		{for($i=0;$i<count($obj->skills);$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_obj($obj->skills )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gettime)) return false;
		if (!$ba->read_uint32($this->pettype)) return false;
		if (!$ba->read_uint32($this->race)) return false;
		if (!$ba->read_uint32($this->flag)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		if (!$ba->read_uint16($this->physiqueinit)) return false;
		if (!$ba->read_uint16($this->strengthinit)) return false;
		if (!$ba->read_uint16($this->enduranceinit)) return false;
		if (!$ba->read_uint16($this->quickinit)) return false;
		if (!$ba->read_uint16($this->iqinit)) return false;
		if (!$ba->read_uint16($this->physiqueparam)) return false;
		if (!$ba->read_uint16($this->strengthparam)) return false;
		if (!$ba->read_uint16($this->enduranceparam)) return false;
		if (!$ba->read_uint16($this->quickparam)) return false;
		if (!$ba->read_uint16($this->iqparam)) return false;
		if (!$ba->read_uint16($this->physique_add)) return false;
		if (!$ba->read_uint16($this->strength_add)) return false;
		if (!$ba->read_uint16($this->endurance_add)) return false;
		if (!$ba->read_uint16($this->quick_add)) return false;
		if (!$ba->read_uint16($this->iq_add)) return false;
		if (!$ba->read_uint16($this->attr_addition)) return false;
		if (!$ba->read_uint32($this->hp)) return false;
		if (!$ba->read_uint32($this->mp)) return false;
		if (!$ba->read_uint8($this->earth)) return false;
		if (!$ba->read_uint8($this->water)) return false;
		if (!$ba->read_uint8($this->fire)) return false;
		if (!$ba->read_uint8($this->wind)) return false;
		if (!$ba->read_uint32($this->injury_state)) return false;
		if (!$ba->read_uint32($this->location)) return false;

		$skills_count=0 ;
		if (!$ba->read_uint32( $skills_count )) return false;
		if ($skills_count>10) return false;
		$this->skills=array();
		{for($i=0; $i<$skills_count;$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gettime)) return false;
		if (!$ba->write_uint32($this->pettype)) return false;
		if (!$ba->write_uint32($this->race)) return false;
		if (!$ba->write_uint32($this->flag)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		if (!$ba->write_uint16($this->physiqueinit)) return false;
		if (!$ba->write_uint16($this->strengthinit)) return false;
		if (!$ba->write_uint16($this->enduranceinit)) return false;
		if (!$ba->write_uint16($this->quickinit)) return false;
		if (!$ba->write_uint16($this->iqinit)) return false;
		if (!$ba->write_uint16($this->physiqueparam)) return false;
		if (!$ba->write_uint16($this->strengthparam)) return false;
		if (!$ba->write_uint16($this->enduranceparam)) return false;
		if (!$ba->write_uint16($this->quickparam)) return false;
		if (!$ba->write_uint16($this->iqparam)) return false;
		if (!$ba->write_uint16($this->physique_add)) return false;
		if (!$ba->write_uint16($this->strength_add)) return false;
		if (!$ba->write_uint16($this->endurance_add)) return false;
		if (!$ba->write_uint16($this->quick_add)) return false;
		if (!$ba->write_uint16($this->iq_add)) return false;
		if (!$ba->write_uint16($this->attr_addition)) return false;
		if (!$ba->write_uint32($this->hp)) return false;
		if (!$ba->write_uint32($this->mp)) return false;
		if (!$ba->write_uint8($this->earth)) return false;
		if (!$ba->write_uint8($this->water)) return false;
		if (!$ba->write_uint8($this->fire)) return false;
		if (!$ba->write_uint8($this->wind)) return false;
		if (!$ba->write_uint32($this->injury_state)) return false;
		if (!$ba->write_uint32($this->location)) return false;
		$skills_count=count($this->skills);
        if ($skills_count>10 ) return false; 
        $ba->write_uint32($skills_count);
		{for($i=0; $i<$skills_count;$i++){
			if ( ! $this->skills[$i] instanceof stru_skill_info ) return false; 
			if (!$this->skills[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_get_type_pets_out {
	/*  */
	#类型:uint32
	public $total;

	/*  */
	#类型:uint32
	public $start;

	/**/
	#变长数组,最大长度:1000, 类型:stru_pet_info
	public $pets =array();


	public function mole2_get_type_pets_out(){

	}

	public function read_from_obj($obj ){
		$this->total=$obj->total;
		$this->start=$obj->start;
		$this->pets=array();
		{for($i=0;$i<count($obj->pets);$i++){
			$this->pets[$i]=new stru_pet_info();
			if (!$this->pets[$i]->read_from_obj($obj->pets )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->total)) return false;
		if (!$ba->read_uint32($this->start)) return false;

		$pets_count=0 ;
		if (!$ba->read_uint32( $pets_count )) return false;
		if ($pets_count>1000) return false;
		$this->pets=array();
		{for($i=0; $i<$pets_count;$i++){
			$this->pets[$i]=new stru_pet_info();
			if (!$this->pets[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->total)) return false;
		if (!$ba->write_uint32($this->start)) return false;
		$pets_count=count($this->pets);
        if ($pets_count>1000 ) return false; 
        $ba->write_uint32($pets_count);
		{for($i=0; $i<$pets_count;$i++){
			if ( ! $this->pets[$i] instanceof stru_pet_info ) return false; 
			if (!$this->pets[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_get_user_cli_buff_in {
	/*  */
	#类型:uint32
	public $min_id;

	/*  */
	#类型:uint32
	public $max_id;


	public function mole2_get_user_cli_buff_in(){

	}

	public function read_from_obj($obj ){
		$this->min_id=$obj->min_id;
		$this->max_id=$obj->max_id;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->min_id)) return false;
		if (!$ba->read_uint32($this->max_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->min_id)) return false;
		if (!$ba->write_uint32($this->max_id)) return false;
		return true;
	}

};

	
class stru_cli_buff {
	/* 类型 */
	#类型:uint32
	public $type;

	/*  */
	#定长数组,长度:512, 类型:char 
	public $buff ;


	public function stru_cli_buff(){

	}

	public function read_from_obj($obj ){
		$this->type=$obj->type;
		$this->buff=$obj->buff;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if(!$ba->read_buf($this->buff,512)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		$ba->write_buf($this->buff,512);
		return true;
	}

};

	
class mole2_get_user_cli_buff_out {
	/**/
	#变长数组,最大长度:9999, 类型:stru_cli_buff
	public $item_list =array();


	public function mole2_get_user_cli_buff_out(){

	}

	public function read_from_obj($obj ){
		$this->item_list=array();
		{for($i=0;$i<count($obj->item_list);$i++){
			$this->item_list[$i]=new stru_cli_buff();
			if (!$this->item_list[$i]->read_from_obj($obj->item_list )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$item_list_count=0 ;
		if (!$ba->read_uint32( $item_list_count )) return false;
		if ($item_list_count>9999) return false;
		$this->item_list=array();
		{for($i=0; $i<$item_list_count;$i++){
			$this->item_list[$i]=new stru_cli_buff();
			if (!$this->item_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$item_list_count=count($this->item_list);
        if ($item_list_count>9999 ) return false; 
        $ba->write_uint32($item_list_count);
		{for($i=0; $i<$item_list_count;$i++){
			if ( ! $this->item_list[$i] instanceof stru_cli_buff ) return false; 
			if (!$this->item_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class stru_user_rank_t {
	/* 米米号 */
	#类型:uint32
	public $userid;

	/*  */
	#类型:uint32
	public $teamid;

	/*  */
	#类型:uint32
	public $medals;


	public function stru_user_rank_t(){

	}

	public function read_from_obj($obj ){
		$this->userid=$obj->userid;
		$this->teamid=$obj->teamid;
		$this->medals=$obj->medals;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->userid)) return false;
		if (!$ba->read_uint32($this->teamid)) return false;
		if (!$ba->read_uint32($this->medals)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->userid)) return false;
		if (!$ba->write_uint32($this->teamid)) return false;
		if (!$ba->write_uint32($this->medals)) return false;
		return true;
	}

};

	
class mole2_get_user_rank_out {
	/**/
	#变长数组,最大长度:100, 类型:stru_user_rank_t
	public $ranks =array();


	public function mole2_get_user_rank_out(){

	}

	public function read_from_obj($obj ){
		$this->ranks=array();
		{for($i=0;$i<count($obj->ranks);$i++){
			$this->ranks[$i]=new stru_user_rank_t();
			if (!$this->ranks[$i]->read_from_obj($obj->ranks )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$ranks_count=0 ;
		if (!$ba->read_uint32( $ranks_count )) return false;
		if ($ranks_count>100) return false;
		$this->ranks=array();
		{for($i=0; $i<$ranks_count;$i++){
			$this->ranks[$i]=new stru_user_rank_t();
			if (!$this->ranks[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$ranks_count=count($this->ranks);
        if ($ranks_count>100 ) return false; 
        $ba->write_uint32($ranks_count);
		{for($i=0; $i<$ranks_count;$i++){
			if ( ! $this->ranks[$i] instanceof stru_user_rank_t ) return false; 
			if (!$this->ranks[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_get_user_sql_out {
	/**/
	#变长数组,最大长度:8000000, 类型:char
	public $sql_str ;


	public function mole2_get_user_sql_out(){

	}

	public function read_from_obj($obj ){
		$this->sql_str=$obj->sql_str;
		return true;
	}


	public function read_from_buf($ba ){

		$sql_str_count=0 ;
		if (!$ba->read_uint32( $sql_str_count )) return false;
		if ($sql_str_count>8000000) return false;
		if(!$ba->read_buf($this->sql_str,$sql_str_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		$sql_str_count=strlen($this->sql_str);
		if ($sql_str_count>8000000 ) return false; 
		$ba->write_uint32($sql_str_count);
		$ba->write_buf($this->sql_str,$sql_str_count);
		return true;
	}

};

	
class mole2_get_vip_buff_in {
	/*宠物ID*/
	#变长数组,最大长度:100, 类型:uint32_t
	public $petid =array();


	public function mole2_get_vip_buff_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=array();
		{for($i=0;$i<count($obj->petid);$i++){
			 $this->petid=$obj->petid;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$petid_count=0 ;
		if (!$ba->read_uint32( $petid_count )) return false;
		if ($petid_count>100) return false;
		$this->petid=array();
		{for($i=0; $i<$petid_count;$i++){
			if (!$ba->read_uint32($this->petid[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$petid_count=count($this->petid);
        if ($petid_count>100 ) return false; 
        $ba->write_uint32($petid_count);
		{for($i=0; $i<$petid_count;$i++){
			if (!$ba->write_uint32($this->petid[$i])) return false;
		}}
		return true;
	}

};

	
class stru_vip_buff {
	/* 宠物ID */
	#类型:uint32
	public $petid;

	/*  */
	#定长数组,长度:128, 类型:char 
	public $buff ;


	public function stru_vip_buff(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		$this->buff=$obj->buff;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		if(!$ba->read_buf($this->buff,128)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		$ba->write_buf($this->buff,128);
		return true;
	}

};

	
class mole2_get_vip_buff_out {
	/**/
	#变长数组,最大长度:100, 类型:stru_vip_buff
	public $petlist =array();


	public function mole2_get_vip_buff_out(){

	}

	public function read_from_obj($obj ){
		$this->petlist=array();
		{for($i=0;$i<count($obj->petlist);$i++){
			$this->petlist[$i]=new stru_vip_buff();
			if (!$this->petlist[$i]->read_from_obj($obj->petlist )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$petlist_count=0 ;
		if (!$ba->read_uint32( $petlist_count )) return false;
		if ($petlist_count>100) return false;
		$this->petlist=array();
		{for($i=0; $i<$petlist_count;$i++){
			$this->petlist[$i]=new stru_vip_buff();
			if (!$this->petlist[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$petlist_count=count($this->petlist);
        if ($petlist_count>100 ) return false; 
        $ba->write_uint32($petlist_count);
		{for($i=0; $i<$petlist_count;$i++){
			if ( ! $this->petlist[$i] instanceof stru_vip_buff ) return false; 
			if (!$this->petlist[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_get_vip_flag_out {
	/* 0x01:vip,0x100,曾经是vip */
	#类型:uint32
	public $vip_flag;


	public function mole2_get_vip_flag_out(){

	}

	public function read_from_obj($obj ){
		$this->vip_flag=$obj->vip_flag;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->vip_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->vip_flag)) return false;
		return true;
	}

};

	
class stru_vip_args {
	/* 类型 */
	#类型:uint32
	public $type;

	/*  */
	#类型:uint32
	public $ex_val;

	/*  */
	#类型:uint32
	public $base_val;

	/*  */
	#类型:uint32
	public $end_time;

	/*  */
	#类型:uint32
	public $begin_time;


	public function stru_vip_args(){

	}

	public function read_from_obj($obj ){
		$this->type=$obj->type;
		$this->ex_val=$obj->ex_val;
		$this->base_val=$obj->base_val;
		$this->end_time=$obj->end_time;
		$this->begin_time=$obj->begin_time;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if (!$ba->read_uint32($this->ex_val)) return false;
		if (!$ba->read_uint32($this->base_val)) return false;
		if (!$ba->read_uint32($this->end_time)) return false;
		if (!$ba->read_uint32($this->begin_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		if (!$ba->write_uint32($this->ex_val)) return false;
		if (!$ba->write_uint32($this->base_val)) return false;
		if (!$ba->write_uint32($this->end_time)) return false;
		if (!$ba->write_uint32($this->begin_time)) return false;
		return true;
	}

};

	
class stru_battle_info {
	/*  */
	#类型:uint32
	public $all_score;

	/*  */
	#类型:uint32
	public $day_score;

	/*  */
	#类型:uint32
	public $update_time;


	public function stru_battle_info(){

	}

	public function read_from_obj($obj ){
		$this->all_score=$obj->all_score;
		$this->day_score=$obj->day_score;
		$this->update_time=$obj->update_time;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->all_score)) return false;
		if (!$ba->read_uint32($this->day_score)) return false;
		if (!$ba->read_uint32($this->update_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->all_score)) return false;
		if (!$ba->write_uint32($this->day_score)) return false;
		if (!$ba->write_uint32($this->update_time)) return false;
		return true;
	}

};

	
class stru_item_info {
	/*  */
	#类型:uint32
	public $itemid;

	/* 个数 */
	#类型:uint32
	public $count;

	/*  */
	#类型:uint32
	public $storage_cnt;


	public function stru_item_info(){

	}

	public function read_from_obj($obj ){
		$this->itemid=$obj->itemid;
		$this->count=$obj->count;
		$this->storage_cnt=$obj->storage_cnt;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		if (!$ba->read_uint32($this->storage_cnt)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		if (!$ba->write_uint32($this->storage_cnt)) return false;
		return true;
	}

};

	
class stru_task_done {
	/* 任务ID */
	#类型:uint32
	public $taskid;

	/*  */
	#类型:uint32
	public $node;

	/* 状态。0：未接任务，1：任务进行，2：任务完成 */
	#类型:uint32
	public $state;

	/* 上次任务完成时间 */
	#类型:uint32
	public $optdate;

	/*  */
	#类型:uint32
	public $fin_time;

	/*  */
	#类型:uint32
	public $fin_num;


	public function stru_task_done(){

	}

	public function read_from_obj($obj ){
		$this->taskid=$obj->taskid;
		$this->node=$obj->node;
		$this->state=$obj->state;
		$this->optdate=$obj->optdate;
		$this->fin_time=$obj->fin_time;
		$this->fin_num=$obj->fin_num;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->taskid)) return false;
		if (!$ba->read_uint32($this->node)) return false;
		if (!$ba->read_uint32($this->state)) return false;
		if (!$ba->read_uint32($this->optdate)) return false;
		if (!$ba->read_uint32($this->fin_time)) return false;
		if (!$ba->read_uint32($this->fin_num)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->taskid)) return false;
		if (!$ba->write_uint32($this->node)) return false;
		if (!$ba->write_uint32($this->state)) return false;
		if (!$ba->write_uint32($this->optdate)) return false;
		if (!$ba->write_uint32($this->fin_time)) return false;
		if (!$ba->write_uint32($this->fin_num)) return false;
		return true;
	}

};

	
class stru_task_doing {
	/* 任务ID */
	#类型:uint32
	public $taskid;

	/*  */
	#类型:uint32
	public $node;

	/* 状态。0：未接任务，1：任务进行，2：任务完成 */
	#类型:uint32
	public $state;

	/* 上次任务完成时间 */
	#类型:uint32
	public $optdate;

	/*  */
	#类型:uint32
	public $fin_time;

	/*  */
	#类型:uint32
	public $fin_num;

	/*  */
	#定长数组,长度:128, 类型:char 
	public $cli_buf ;

	/*  */
	#定长数组,长度:20, 类型:char 
	public $ser_buf ;


	public function stru_task_doing(){

	}

	public function read_from_obj($obj ){
		$this->taskid=$obj->taskid;
		$this->node=$obj->node;
		$this->state=$obj->state;
		$this->optdate=$obj->optdate;
		$this->fin_time=$obj->fin_time;
		$this->fin_num=$obj->fin_num;
		$this->cli_buf=$obj->cli_buf;
		$this->ser_buf=$obj->ser_buf;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->taskid)) return false;
		if (!$ba->read_uint32($this->node)) return false;
		if (!$ba->read_uint32($this->state)) return false;
		if (!$ba->read_uint32($this->optdate)) return false;
		if (!$ba->read_uint32($this->fin_time)) return false;
		if (!$ba->read_uint32($this->fin_num)) return false;
		if(!$ba->read_buf($this->cli_buf,128)) return false;
		if(!$ba->read_buf($this->ser_buf,20)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->taskid)) return false;
		if (!$ba->write_uint32($this->node)) return false;
		if (!$ba->write_uint32($this->state)) return false;
		if (!$ba->write_uint32($this->optdate)) return false;
		if (!$ba->write_uint32($this->fin_time)) return false;
		if (!$ba->write_uint32($this->fin_num)) return false;
		$ba->write_buf($this->cli_buf,128);
		$ba->write_buf($this->ser_buf,20);
		return true;
	}

};

	
class stru_map_state {
	/* 地图编号 */
	#类型:uint32
	public $mapid;

	/* 状态。0：未接任务，1：任务进行，2：任务完成 */
	#类型:uint32
	public $state;


	public function stru_map_state(){

	}

	public function read_from_obj($obj ){
		$this->mapid=$obj->mapid;
		$this->state=$obj->state;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->mapid)) return false;
		if (!$ba->read_uint32($this->state)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->mapid)) return false;
		if (!$ba->write_uint32($this->state)) return false;
		return true;
	}

};

	
class stru_beast_book {
	/*  */
	#类型:uint32
	public $beastid;

	/* 状态。0：未接任务，1：任务进行，2：任务完成 */
	#类型:uint32
	public $state;

	/* 个数 */
	#类型:uint32
	public $count;


	public function stru_beast_book(){

	}

	public function read_from_obj($obj ){
		$this->beastid=$obj->beastid;
		$this->state=$obj->state;
		$this->count=$obj->count;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->beastid)) return false;
		if (!$ba->read_uint32($this->state)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->beastid)) return false;
		if (!$ba->write_uint32($this->state)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class stru_day_limit {
	/*  */
	#类型:uint32
	public $ssid;

	/*  */
	#类型:uint32
	public $total;

	/* 个数 */
	#类型:uint32
	public $count;


	public function stru_day_limit(){

	}

	public function read_from_obj($obj ){
		$this->ssid=$obj->ssid;
		$this->total=$obj->total;
		$this->count=$obj->count;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->ssid)) return false;
		if (!$ba->read_uint32($this->total)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->ssid)) return false;
		if (!$ba->write_uint32($this->total)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		return true;
	}

};

	
class mole2_online_login_out {
	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 标志 */
	#类型:uint32
	public $flag;

	/* 颜色 */
	#类型:uint32
	public $color;

	/* 注册时间 */
	#类型:uint32
	public $register_time;

	/* 种族 */
	#类型:uint32
	public $race;

	/* 职业 0：无 1:剑士 2:弓箭手 3:魔法师 4:传教士 */
	#类型:uint32
	public $professtion;

	/*  */
	#类型:uint32
	public $joblevel;

	/* 荣誉值 */
	#类型:uint32
	public $honor;

	/* 小米 */
	#类型:uint32
	public $xiaomee;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $experience;

	/* 体力 */
	#类型:uint16
	public $physique;

	/* 力量 */
	#类型:uint16
	public $strength;

	/* 耐力 */
	#类型:uint16
	public $endurance;

	/* 速度 */
	#类型:uint16
	public $quick;

	/* 智力 */
	#类型:uint16
	public $intelligence;

	/* 剩余属性点 */
	#类型:uint16
	public $attr_addition;

	/* 体力 */
	#类型:uint32
	public $hp;

	/* 魔法值 */
	#类型:uint32
	public $mp;

	/* 受伤等级 0:未受伤, 1:白 2:黄 3:红 */
	#类型:uint32
	public $injury_state;

	/* 人物战斗默认站位 1:前排 0:后排 */
	#类型:uint32
	public $in_front;

	/* 人物携带最大装备数 */
	#类型:uint32
	public $max_attire;

	/* 人物携带最大药剂数 */
	#类型:uint32
	public $max_medicine;

	/* 人物携带最大材料数 */
	#类型:uint32
	public $max_stuff;

	/* 时间 */
	#类型:uint32
	public $time;

	/* 坐标x轴 */
	#类型:uint32
	public $axis_x;

	/* 坐标y轴 */
	#类型:uint32
	public $axis_y;

	/* 地图编号 */
	#类型:uint32
	public $mapid;

	/*  */
	#类型:uint32
	public $daytime;

	/*  */
	#类型:uint32
	public $fly_mapid;

	/*  */
	#类型:uint32
	public $expbox;

	/* 电量值 */
	#类型:uint32
	public $energy;

	/*  */
	#类型:uint32
	public $skill_expbox;

	/*  */
	#类型:uint32
	public $flag_ex;

	/*  */
	#类型:uint32
	public $winbossid;

	/*  */
	#类型:uint32
	public $parent;

	/*  */
	#类型:stru_vip_args
	public $vipargs;

	/*  */
	#类型:stru_battle_info
	public $battle;

	/*  */
	#类型:uint32
	public $all_pet_cnt;

	/**/
	#变长数组,最大长度:20, 类型:stru_skill_info
	public $skills =array();

	/**/
	#变长数组,最大长度:6, 类型:stru_pet_info
	public $pets =array();

	/**/
	#变长数组,最大长度:8, 类型:stru_cloth_info
	public $cloths =array();

	/**/
	#变长数组,最大长度:2000, 类型:stru_cloth_info
	public $cloths_in_bag =array();

	/**/
	#变长数组,最大长度:5000, 类型:stru_item_info
	public $items =array();

	/**/
	#变长数组,最大长度:1000, 类型:uint32_t
	public $titles =array();

	/**/
	#变长数组,最大长度:1000, 类型:stru_task_done
	public $task_done =array();

	/**/
	#变长数组,最大长度:1000, 类型:stru_task_doing
	public $task_doing =array();

	/**/
	#变长数组,最大长度:10000, 类型:stru_map_state
	public $mapstates =array();

	/**/
	#变长数组,最大长度:10000, 类型:stru_beast_book
	public $beastbook =array();

	/**/
	#变长数组,最大长度:10000, 类型:stru_day_limit
	public $daylimits =array();

	/**/
	#变长数组,最大长度:1000, 类型:stru_vip_buff
	public $vipbuffs =array();

	/*消息列表*/
	#变长数组,最大长度:2000, 类型:char
	public $msglist ;


	public function mole2_online_login_out(){
		$this->vipargs=new stru_vip_args(); 
		$this->battle=new stru_battle_info(); 

	}

	public function read_from_obj($obj ){
		$this->nick=$obj->nick;
		$this->flag=$obj->flag;
		$this->color=$obj->color;
		$this->register_time=$obj->register_time;
		$this->race=$obj->race;
		$this->professtion=$obj->professtion;
		$this->joblevel=$obj->joblevel;
		$this->honor=$obj->honor;
		$this->xiaomee=$obj->xiaomee;
		$this->level=$obj->level;
		$this->experience=$obj->experience;
		$this->physique=$obj->physique;
		$this->strength=$obj->strength;
		$this->endurance=$obj->endurance;
		$this->quick=$obj->quick;
		$this->intelligence=$obj->intelligence;
		$this->attr_addition=$obj->attr_addition;
		$this->hp=$obj->hp;
		$this->mp=$obj->mp;
		$this->injury_state=$obj->injury_state;
		$this->in_front=$obj->in_front;
		$this->max_attire=$obj->max_attire;
		$this->max_medicine=$obj->max_medicine;
		$this->max_stuff=$obj->max_stuff;
		$this->time=$obj->time;
		$this->axis_x=$obj->axis_x;
		$this->axis_y=$obj->axis_y;
		$this->mapid=$obj->mapid;
		$this->daytime=$obj->daytime;
		$this->fly_mapid=$obj->fly_mapid;
		$this->expbox=$obj->expbox;
		$this->energy=$obj->energy;
		$this->skill_expbox=$obj->skill_expbox;
		$this->flag_ex=$obj->flag_ex;
		$this->winbossid=$obj->winbossid;
		$this->parent=$obj->parent;
		if (!$this->vipargs->read_from_obj($obj->vipargs)) return false;
		if (!$this->battle->read_from_obj($obj->battle)) return false;
		$this->all_pet_cnt=$obj->all_pet_cnt;
		$this->skills=array();
		{for($i=0;$i<count($obj->skills);$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_obj($obj->skills )) return false;
		}}
		$this->pets=array();
		{for($i=0;$i<count($obj->pets);$i++){
			$this->pets[$i]=new stru_pet_info();
			if (!$this->pets[$i]->read_from_obj($obj->pets )) return false;
		}}
		$this->cloths=array();
		{for($i=0;$i<count($obj->cloths);$i++){
			$this->cloths[$i]=new stru_cloth_info();
			if (!$this->cloths[$i]->read_from_obj($obj->cloths )) return false;
		}}
		$this->cloths_in_bag=array();
		{for($i=0;$i<count($obj->cloths_in_bag);$i++){
			$this->cloths_in_bag[$i]=new stru_cloth_info();
			if (!$this->cloths_in_bag[$i]->read_from_obj($obj->cloths_in_bag )) return false;
		}}
		$this->items=array();
		{for($i=0;$i<count($obj->items);$i++){
			$this->items[$i]=new stru_item_info();
			if (!$this->items[$i]->read_from_obj($obj->items )) return false;
		}}
		$this->titles=array();
		{for($i=0;$i<count($obj->titles);$i++){
			 $this->titles=$obj->titles;
		}}
		$this->task_done=array();
		{for($i=0;$i<count($obj->task_done);$i++){
			$this->task_done[$i]=new stru_task_done();
			if (!$this->task_done[$i]->read_from_obj($obj->task_done )) return false;
		}}
		$this->task_doing=array();
		{for($i=0;$i<count($obj->task_doing);$i++){
			$this->task_doing[$i]=new stru_task_doing();
			if (!$this->task_doing[$i]->read_from_obj($obj->task_doing )) return false;
		}}
		$this->mapstates=array();
		{for($i=0;$i<count($obj->mapstates);$i++){
			$this->mapstates[$i]=new stru_map_state();
			if (!$this->mapstates[$i]->read_from_obj($obj->mapstates )) return false;
		}}
		$this->beastbook=array();
		{for($i=0;$i<count($obj->beastbook);$i++){
			$this->beastbook[$i]=new stru_beast_book();
			if (!$this->beastbook[$i]->read_from_obj($obj->beastbook )) return false;
		}}
		$this->daylimits=array();
		{for($i=0;$i<count($obj->daylimits);$i++){
			$this->daylimits[$i]=new stru_day_limit();
			if (!$this->daylimits[$i]->read_from_obj($obj->daylimits )) return false;
		}}
		$this->vipbuffs=array();
		{for($i=0;$i<count($obj->vipbuffs);$i++){
			$this->vipbuffs[$i]=new stru_vip_buff();
			if (!$this->vipbuffs[$i]->read_from_obj($obj->vipbuffs )) return false;
		}}
		$this->msglist=$obj->msglist;
		return true;
	}


	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if (!$ba->read_uint32($this->register_time)) return false;
		if (!$ba->read_uint32($this->race)) return false;
		if (!$ba->read_uint32($this->professtion)) return false;
		if (!$ba->read_uint32($this->joblevel)) return false;
		if (!$ba->read_uint32($this->honor)) return false;
		if (!$ba->read_uint32($this->xiaomee)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->experience)) return false;
		if (!$ba->read_uint16($this->physique)) return false;
		if (!$ba->read_uint16($this->strength)) return false;
		if (!$ba->read_uint16($this->endurance)) return false;
		if (!$ba->read_uint16($this->quick)) return false;
		if (!$ba->read_uint16($this->intelligence)) return false;
		if (!$ba->read_uint16($this->attr_addition)) return false;
		if (!$ba->read_uint32($this->hp)) return false;
		if (!$ba->read_uint32($this->mp)) return false;
		if (!$ba->read_uint32($this->injury_state)) return false;
		if (!$ba->read_uint32($this->in_front)) return false;
		if (!$ba->read_uint32($this->max_attire)) return false;
		if (!$ba->read_uint32($this->max_medicine)) return false;
		if (!$ba->read_uint32($this->max_stuff)) return false;
		if (!$ba->read_uint32($this->time)) return false;
		if (!$ba->read_uint32($this->axis_x)) return false;
		if (!$ba->read_uint32($this->axis_y)) return false;
		if (!$ba->read_uint32($this->mapid)) return false;
		if (!$ba->read_uint32($this->daytime)) return false;
		if (!$ba->read_uint32($this->fly_mapid)) return false;
		if (!$ba->read_uint32($this->expbox)) return false;
		if (!$ba->read_uint32($this->energy)) return false;
		if (!$ba->read_uint32($this->skill_expbox)) return false;
		if (!$ba->read_uint32($this->flag_ex)) return false;
		if (!$ba->read_uint32($this->winbossid)) return false;
		if (!$ba->read_uint32($this->parent)) return false;
		if (!$this->vipargs->read_from_buf($ba)) return false;
		if (!$this->battle->read_from_buf($ba)) return false;
		if (!$ba->read_uint32($this->all_pet_cnt)) return false;

		$skills_count=0 ;
		if (!$ba->read_uint32( $skills_count )) return false;
		if ($skills_count>20) return false;
		$this->skills=array();
		{for($i=0; $i<$skills_count;$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_buf($ba)) return false;
		}}

		$pets_count=0 ;
		if (!$ba->read_uint32( $pets_count )) return false;
		if ($pets_count>6) return false;
		$this->pets=array();
		{for($i=0; $i<$pets_count;$i++){
			$this->pets[$i]=new stru_pet_info();
			if (!$this->pets[$i]->read_from_buf($ba)) return false;
		}}

		$cloths_count=0 ;
		if (!$ba->read_uint32( $cloths_count )) return false;
		if ($cloths_count>8) return false;
		$this->cloths=array();
		{for($i=0; $i<$cloths_count;$i++){
			$this->cloths[$i]=new stru_cloth_info();
			if (!$this->cloths[$i]->read_from_buf($ba)) return false;
		}}

		$cloths_in_bag_count=0 ;
		if (!$ba->read_uint32( $cloths_in_bag_count )) return false;
		if ($cloths_in_bag_count>2000) return false;
		$this->cloths_in_bag=array();
		{for($i=0; $i<$cloths_in_bag_count;$i++){
			$this->cloths_in_bag[$i]=new stru_cloth_info();
			if (!$this->cloths_in_bag[$i]->read_from_buf($ba)) return false;
		}}

		$items_count=0 ;
		if (!$ba->read_uint32( $items_count )) return false;
		if ($items_count>5000) return false;
		$this->items=array();
		{for($i=0; $i<$items_count;$i++){
			$this->items[$i]=new stru_item_info();
			if (!$this->items[$i]->read_from_buf($ba)) return false;
		}}

		$titles_count=0 ;
		if (!$ba->read_uint32( $titles_count )) return false;
		if ($titles_count>1000) return false;
		$this->titles=array();
		{for($i=0; $i<$titles_count;$i++){
			if (!$ba->read_uint32($this->titles[$i])) return false;
		}}

		$task_done_count=0 ;
		if (!$ba->read_uint32( $task_done_count )) return false;
		if ($task_done_count>1000) return false;
		$this->task_done=array();
		{for($i=0; $i<$task_done_count;$i++){
			$this->task_done[$i]=new stru_task_done();
			if (!$this->task_done[$i]->read_from_buf($ba)) return false;
		}}

		$task_doing_count=0 ;
		if (!$ba->read_uint32( $task_doing_count )) return false;
		if ($task_doing_count>1000) return false;
		$this->task_doing=array();
		{for($i=0; $i<$task_doing_count;$i++){
			$this->task_doing[$i]=new stru_task_doing();
			if (!$this->task_doing[$i]->read_from_buf($ba)) return false;
		}}

		$mapstates_count=0 ;
		if (!$ba->read_uint32( $mapstates_count )) return false;
		if ($mapstates_count>10000) return false;
		$this->mapstates=array();
		{for($i=0; $i<$mapstates_count;$i++){
			$this->mapstates[$i]=new stru_map_state();
			if (!$this->mapstates[$i]->read_from_buf($ba)) return false;
		}}

		$beastbook_count=0 ;
		if (!$ba->read_uint32( $beastbook_count )) return false;
		if ($beastbook_count>10000) return false;
		$this->beastbook=array();
		{for($i=0; $i<$beastbook_count;$i++){
			$this->beastbook[$i]=new stru_beast_book();
			if (!$this->beastbook[$i]->read_from_buf($ba)) return false;
		}}

		$daylimits_count=0 ;
		if (!$ba->read_uint32( $daylimits_count )) return false;
		if ($daylimits_count>10000) return false;
		$this->daylimits=array();
		{for($i=0; $i<$daylimits_count;$i++){
			$this->daylimits[$i]=new stru_day_limit();
			if (!$this->daylimits[$i]->read_from_buf($ba)) return false;
		}}

		$vipbuffs_count=0 ;
		if (!$ba->read_uint32( $vipbuffs_count )) return false;
		if ($vipbuffs_count>1000) return false;
		$this->vipbuffs=array();
		{for($i=0; $i<$vipbuffs_count;$i++){
			$this->vipbuffs[$i]=new stru_vip_buff();
			if (!$this->vipbuffs[$i]->read_from_buf($ba)) return false;
		}}

		$msglist_count=0 ;
		if (!$ba->read_uint32( $msglist_count )) return false;
		if ($msglist_count>2000) return false;
		if(!$ba->read_buf($this->msglist,$msglist_count))return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		if (!$ba->write_uint32($this->register_time)) return false;
		if (!$ba->write_uint32($this->race)) return false;
		if (!$ba->write_uint32($this->professtion)) return false;
		if (!$ba->write_uint32($this->joblevel)) return false;
		if (!$ba->write_uint32($this->honor)) return false;
		if (!$ba->write_uint32($this->xiaomee)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->experience)) return false;
		if (!$ba->write_uint16($this->physique)) return false;
		if (!$ba->write_uint16($this->strength)) return false;
		if (!$ba->write_uint16($this->endurance)) return false;
		if (!$ba->write_uint16($this->quick)) return false;
		if (!$ba->write_uint16($this->intelligence)) return false;
		if (!$ba->write_uint16($this->attr_addition)) return false;
		if (!$ba->write_uint32($this->hp)) return false;
		if (!$ba->write_uint32($this->mp)) return false;
		if (!$ba->write_uint32($this->injury_state)) return false;
		if (!$ba->write_uint32($this->in_front)) return false;
		if (!$ba->write_uint32($this->max_attire)) return false;
		if (!$ba->write_uint32($this->max_medicine)) return false;
		if (!$ba->write_uint32($this->max_stuff)) return false;
		if (!$ba->write_uint32($this->time)) return false;
		if (!$ba->write_uint32($this->axis_x)) return false;
		if (!$ba->write_uint32($this->axis_y)) return false;
		if (!$ba->write_uint32($this->mapid)) return false;
		if (!$ba->write_uint32($this->daytime)) return false;
		if (!$ba->write_uint32($this->fly_mapid)) return false;
		if (!$ba->write_uint32($this->expbox)) return false;
		if (!$ba->write_uint32($this->energy)) return false;
		if (!$ba->write_uint32($this->skill_expbox)) return false;
		if (!$ba->write_uint32($this->flag_ex)) return false;
		if (!$ba->write_uint32($this->winbossid)) return false;
		if (!$ba->write_uint32($this->parent)) return false;
		if ( ! $this->vipargs instanceof stru_vip_args ) return false; 
		if (!$this->vipargs->write_to_buf($ba)) return false;
		if ( ! $this->battle instanceof stru_battle_info ) return false; 
		if (!$this->battle->write_to_buf($ba)) return false;
		if (!$ba->write_uint32($this->all_pet_cnt)) return false;
		$skills_count=count($this->skills);
        if ($skills_count>20 ) return false; 
        $ba->write_uint32($skills_count);
		{for($i=0; $i<$skills_count;$i++){
			if ( ! $this->skills[$i] instanceof stru_skill_info ) return false; 
			if (!$this->skills[$i]->write_to_buf($ba)) return false;
		}}
		$pets_count=count($this->pets);
        if ($pets_count>6 ) return false; 
        $ba->write_uint32($pets_count);
		{for($i=0; $i<$pets_count;$i++){
			if ( ! $this->pets[$i] instanceof stru_pet_info ) return false; 
			if (!$this->pets[$i]->write_to_buf($ba)) return false;
		}}
		$cloths_count=count($this->cloths);
        if ($cloths_count>8 ) return false; 
        $ba->write_uint32($cloths_count);
		{for($i=0; $i<$cloths_count;$i++){
			if ( ! $this->cloths[$i] instanceof stru_cloth_info ) return false; 
			if (!$this->cloths[$i]->write_to_buf($ba)) return false;
		}}
		$cloths_in_bag_count=count($this->cloths_in_bag);
        if ($cloths_in_bag_count>2000 ) return false; 
        $ba->write_uint32($cloths_in_bag_count);
		{for($i=0; $i<$cloths_in_bag_count;$i++){
			if ( ! $this->cloths_in_bag[$i] instanceof stru_cloth_info ) return false; 
			if (!$this->cloths_in_bag[$i]->write_to_buf($ba)) return false;
		}}
		$items_count=count($this->items);
        if ($items_count>5000 ) return false; 
        $ba->write_uint32($items_count);
		{for($i=0; $i<$items_count;$i++){
			if ( ! $this->items[$i] instanceof stru_item_info ) return false; 
			if (!$this->items[$i]->write_to_buf($ba)) return false;
		}}
		$titles_count=count($this->titles);
        if ($titles_count>1000 ) return false; 
        $ba->write_uint32($titles_count);
		{for($i=0; $i<$titles_count;$i++){
			if (!$ba->write_uint32($this->titles[$i])) return false;
		}}
		$task_done_count=count($this->task_done);
        if ($task_done_count>1000 ) return false; 
        $ba->write_uint32($task_done_count);
		{for($i=0; $i<$task_done_count;$i++){
			if ( ! $this->task_done[$i] instanceof stru_task_done ) return false; 
			if (!$this->task_done[$i]->write_to_buf($ba)) return false;
		}}
		$task_doing_count=count($this->task_doing);
        if ($task_doing_count>1000 ) return false; 
        $ba->write_uint32($task_doing_count);
		{for($i=0; $i<$task_doing_count;$i++){
			if ( ! $this->task_doing[$i] instanceof stru_task_doing ) return false; 
			if (!$this->task_doing[$i]->write_to_buf($ba)) return false;
		}}
		$mapstates_count=count($this->mapstates);
        if ($mapstates_count>10000 ) return false; 
        $ba->write_uint32($mapstates_count);
		{for($i=0; $i<$mapstates_count;$i++){
			if ( ! $this->mapstates[$i] instanceof stru_map_state ) return false; 
			if (!$this->mapstates[$i]->write_to_buf($ba)) return false;
		}}
		$beastbook_count=count($this->beastbook);
        if ($beastbook_count>10000 ) return false; 
        $ba->write_uint32($beastbook_count);
		{for($i=0; $i<$beastbook_count;$i++){
			if ( ! $this->beastbook[$i] instanceof stru_beast_book ) return false; 
			if (!$this->beastbook[$i]->write_to_buf($ba)) return false;
		}}
		$daylimits_count=count($this->daylimits);
        if ($daylimits_count>10000 ) return false; 
        $ba->write_uint32($daylimits_count);
		{for($i=0; $i<$daylimits_count;$i++){
			if ( ! $this->daylimits[$i] instanceof stru_day_limit ) return false; 
			if (!$this->daylimits[$i]->write_to_buf($ba)) return false;
		}}
		$vipbuffs_count=count($this->vipbuffs);
        if ($vipbuffs_count>1000 ) return false; 
        $ba->write_uint32($vipbuffs_count);
		{for($i=0; $i<$vipbuffs_count;$i++){
			if ( ! $this->vipbuffs[$i] instanceof stru_vip_buff ) return false; 
			if (!$this->vipbuffs[$i]->write_to_buf($ba)) return false;
		}}
		$msglist_count=strlen($this->msglist);
		if ($msglist_count>2000 ) return false; 
		$ba->write_uint32($msglist_count);
		$ba->write_buf($this->msglist,$msglist_count);
		return true;
	}

};

	
class stru_survey_reply {
	/*  */
	#类型:uint32
	public $option_id;

	/*  */
	#类型:uint32
	public $value_id;


	public function stru_survey_reply(){

	}

	public function read_from_obj($obj ){
		$this->option_id=$obj->option_id;
		$this->value_id=$obj->value_id;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->option_id)) return false;
		if (!$ba->read_uint32($this->value_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->option_id)) return false;
		if (!$ba->write_uint32($this->value_id)) return false;
		return true;
	}

};

	
class mole2_reply_question_in {
	/*  */
	#类型:uint32
	public $surveyid;

	/**/
	#变长数组,最大长度:100, 类型:stru_survey_reply
	public $replys =array();


	public function mole2_reply_question_in(){

	}

	public function read_from_obj($obj ){
		$this->surveyid=$obj->surveyid;
		$this->replys=array();
		{for($i=0;$i<count($obj->replys);$i++){
			$this->replys[$i]=new stru_survey_reply();
			if (!$this->replys[$i]->read_from_obj($obj->replys )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->surveyid)) return false;

		$replys_count=0 ;
		if (!$ba->read_uint32( $replys_count )) return false;
		if ($replys_count>100) return false;
		$this->replys=array();
		{for($i=0; $i<$replys_count;$i++){
			$this->replys[$i]=new stru_survey_reply();
			if (!$this->replys[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->surveyid)) return false;
		$replys_count=count($this->replys);
        if ($replys_count>100 ) return false; 
        $ba->write_uint32($replys_count);
		{for($i=0; $i<$replys_count;$i++){
			if ( ! $this->replys[$i] instanceof stru_survey_reply ) return false; 
			if (!$this->replys[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_set_flag_bits_in {
	/*  */
	#类型:uint32
	public $mask;

	/*  */
	#类型:uint32
	public $bits;


	public function mole2_set_flag_bits_in(){

	}

	public function read_from_obj($obj ){
		$this->mask=$obj->mask;
		$this->bits=$obj->bits;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->mask)) return false;
		if (!$ba->read_uint32($this->bits)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->mask)) return false;
		if (!$ba->write_uint32($this->bits)) return false;
		return true;
	}

};

	
class mole2_set_flag_bits_out {
	/* 标志 */
	#类型:uint32
	public $flag;


	public function mole2_set_flag_bits_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		return true;
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

	
class mole2_set_hero_team_in {
	/*  */
	#类型:uint32
	public $teamid;


	public function mole2_set_hero_team_in(){

	}

	public function read_from_obj($obj ){
		$this->teamid=$obj->teamid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->teamid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->teamid)) return false;
		return true;
	}

};

	
class mole2_set_levelup_time_in {
	/* 时间 */
	#类型:uint32
	public $time;


	public function mole2_set_levelup_time_in(){

	}

	public function read_from_obj($obj ){
		$this->time=$obj->time;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->time)) return false;
		return true;
	}

};

	
class mole2_set_pet_attributes_in {
	/* 宠物ID */
	#类型:uint32
	public $petid;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $exp;

	/* 体力初值 */
	#类型:uint16
	public $physiqueinit;

	/* 力量初值 */
	#类型:uint16
	public $strengthinit;

	/* 耐力初值 */
	#类型:uint16
	public $enduranceinit;

	/* 速度初值 */
	#类型:uint16
	public $quickinit;

	/* 智慧初值 */
	#类型:uint16
	public $iqinit;

	/* 体力成长值 */
	#类型:uint16
	public $physiqueparam;

	/* 力量成长值 */
	#类型:uint16
	public $strengthparam;

	/* 耐力成长值 */
	#类型:uint16
	public $enduranceparam;

	/* 速度成长值 */
	#类型:uint16
	public $quickparam;

	/* 智慧成长值 */
	#类型:uint16
	public $iqparam;

	/* 体力配点值 */
	#类型:uint16
	public $physiqueadd;

	/* 力量配点值 */
	#类型:uint16
	public $strengthadd;

	/* 耐力配点值 */
	#类型:uint16
	public $enduranceadd;

	/* 速度配点值 */
	#类型:uint16
	public $quickadd;

	/* 智慧配点值 */
	#类型:uint16
	public $iqadd;

	/*  */
	#类型:uint16
	public $attradd;

	/* 体力 */
	#类型:uint32
	public $hp;

	/* 魔法值 */
	#类型:uint32
	public $mp;


	public function mole2_set_pet_attributes_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		$this->level=$obj->level;
		$this->exp=$obj->exp;
		$this->physiqueinit=$obj->physiqueinit;
		$this->strengthinit=$obj->strengthinit;
		$this->enduranceinit=$obj->enduranceinit;
		$this->quickinit=$obj->quickinit;
		$this->iqinit=$obj->iqinit;
		$this->physiqueparam=$obj->physiqueparam;
		$this->strengthparam=$obj->strengthparam;
		$this->enduranceparam=$obj->enduranceparam;
		$this->quickparam=$obj->quickparam;
		$this->iqparam=$obj->iqparam;
		$this->physiqueadd=$obj->physiqueadd;
		$this->strengthadd=$obj->strengthadd;
		$this->enduranceadd=$obj->enduranceadd;
		$this->quickadd=$obj->quickadd;
		$this->iqadd=$obj->iqadd;
		$this->attradd=$obj->attradd;
		$this->hp=$obj->hp;
		$this->mp=$obj->mp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		if (!$ba->read_uint16($this->physiqueinit)) return false;
		if (!$ba->read_uint16($this->strengthinit)) return false;
		if (!$ba->read_uint16($this->enduranceinit)) return false;
		if (!$ba->read_uint16($this->quickinit)) return false;
		if (!$ba->read_uint16($this->iqinit)) return false;
		if (!$ba->read_uint16($this->physiqueparam)) return false;
		if (!$ba->read_uint16($this->strengthparam)) return false;
		if (!$ba->read_uint16($this->enduranceparam)) return false;
		if (!$ba->read_uint16($this->quickparam)) return false;
		if (!$ba->read_uint16($this->iqparam)) return false;
		if (!$ba->read_uint16($this->physiqueadd)) return false;
		if (!$ba->read_uint16($this->strengthadd)) return false;
		if (!$ba->read_uint16($this->enduranceadd)) return false;
		if (!$ba->read_uint16($this->quickadd)) return false;
		if (!$ba->read_uint16($this->iqadd)) return false;
		if (!$ba->read_uint16($this->attradd)) return false;
		if (!$ba->read_uint32($this->hp)) return false;
		if (!$ba->read_uint32($this->mp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		if (!$ba->write_uint16($this->physiqueinit)) return false;
		if (!$ba->write_uint16($this->strengthinit)) return false;
		if (!$ba->write_uint16($this->enduranceinit)) return false;
		if (!$ba->write_uint16($this->quickinit)) return false;
		if (!$ba->write_uint16($this->iqinit)) return false;
		if (!$ba->write_uint16($this->physiqueparam)) return false;
		if (!$ba->write_uint16($this->strengthparam)) return false;
		if (!$ba->write_uint16($this->enduranceparam)) return false;
		if (!$ba->write_uint16($this->quickparam)) return false;
		if (!$ba->write_uint16($this->iqparam)) return false;
		if (!$ba->write_uint16($this->physiqueadd)) return false;
		if (!$ba->write_uint16($this->strengthadd)) return false;
		if (!$ba->write_uint16($this->enduranceadd)) return false;
		if (!$ba->write_uint16($this->quickadd)) return false;
		if (!$ba->write_uint16($this->iqadd)) return false;
		if (!$ba->write_uint16($this->attradd)) return false;
		if (!$ba->write_uint32($this->hp)) return false;
		if (!$ba->write_uint32($this->mp)) return false;
		return true;
	}

};

	
class mole2_set_pet_attributes_out {
	/* 宠物ID */
	#类型:uint32
	public $petid;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $exp;

	/* 体力初值 */
	#类型:uint16
	public $physiqueinit;

	/* 力量初值 */
	#类型:uint16
	public $strengthinit;

	/* 耐力初值 */
	#类型:uint16
	public $enduranceinit;

	/* 速度初值 */
	#类型:uint16
	public $quickinit;

	/* 智慧初值 */
	#类型:uint16
	public $iqinit;

	/* 体力成长值 */
	#类型:uint16
	public $physiqueparam;

	/* 力量成长值 */
	#类型:uint16
	public $strengthparam;

	/* 耐力成长值 */
	#类型:uint16
	public $enduranceparam;

	/* 速度成长值 */
	#类型:uint16
	public $quickparam;

	/* 智慧成长值 */
	#类型:uint16
	public $iqparam;

	/* 体力配点值 */
	#类型:uint16
	public $physiqueadd;

	/* 力量配点值 */
	#类型:uint16
	public $strengthadd;

	/* 耐力配点值 */
	#类型:uint16
	public $enduranceadd;

	/* 速度配点值 */
	#类型:uint16
	public $quickadd;

	/* 智慧配点值 */
	#类型:uint16
	public $iqadd;

	/*  */
	#类型:uint16
	public $attradd;

	/* 体力 */
	#类型:uint32
	public $hp;

	/* 魔法值 */
	#类型:uint32
	public $mp;


	public function mole2_set_pet_attributes_out(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		$this->level=$obj->level;
		$this->exp=$obj->exp;
		$this->physiqueinit=$obj->physiqueinit;
		$this->strengthinit=$obj->strengthinit;
		$this->enduranceinit=$obj->enduranceinit;
		$this->quickinit=$obj->quickinit;
		$this->iqinit=$obj->iqinit;
		$this->physiqueparam=$obj->physiqueparam;
		$this->strengthparam=$obj->strengthparam;
		$this->enduranceparam=$obj->enduranceparam;
		$this->quickparam=$obj->quickparam;
		$this->iqparam=$obj->iqparam;
		$this->physiqueadd=$obj->physiqueadd;
		$this->strengthadd=$obj->strengthadd;
		$this->enduranceadd=$obj->enduranceadd;
		$this->quickadd=$obj->quickadd;
		$this->iqadd=$obj->iqadd;
		$this->attradd=$obj->attradd;
		$this->hp=$obj->hp;
		$this->mp=$obj->mp;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->exp)) return false;
		if (!$ba->read_uint16($this->physiqueinit)) return false;
		if (!$ba->read_uint16($this->strengthinit)) return false;
		if (!$ba->read_uint16($this->enduranceinit)) return false;
		if (!$ba->read_uint16($this->quickinit)) return false;
		if (!$ba->read_uint16($this->iqinit)) return false;
		if (!$ba->read_uint16($this->physiqueparam)) return false;
		if (!$ba->read_uint16($this->strengthparam)) return false;
		if (!$ba->read_uint16($this->enduranceparam)) return false;
		if (!$ba->read_uint16($this->quickparam)) return false;
		if (!$ba->read_uint16($this->iqparam)) return false;
		if (!$ba->read_uint16($this->physiqueadd)) return false;
		if (!$ba->read_uint16($this->strengthadd)) return false;
		if (!$ba->read_uint16($this->enduranceadd)) return false;
		if (!$ba->read_uint16($this->quickadd)) return false;
		if (!$ba->read_uint16($this->iqadd)) return false;
		if (!$ba->read_uint16($this->attradd)) return false;
		if (!$ba->read_uint32($this->hp)) return false;
		if (!$ba->read_uint32($this->mp)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->exp)) return false;
		if (!$ba->write_uint16($this->physiqueinit)) return false;
		if (!$ba->write_uint16($this->strengthinit)) return false;
		if (!$ba->write_uint16($this->enduranceinit)) return false;
		if (!$ba->write_uint16($this->quickinit)) return false;
		if (!$ba->write_uint16($this->iqinit)) return false;
		if (!$ba->write_uint16($this->physiqueparam)) return false;
		if (!$ba->write_uint16($this->strengthparam)) return false;
		if (!$ba->write_uint16($this->enduranceparam)) return false;
		if (!$ba->write_uint16($this->quickparam)) return false;
		if (!$ba->write_uint16($this->iqparam)) return false;
		if (!$ba->write_uint16($this->physiqueadd)) return false;
		if (!$ba->write_uint16($this->strengthadd)) return false;
		if (!$ba->write_uint16($this->enduranceadd)) return false;
		if (!$ba->write_uint16($this->quickadd)) return false;
		if (!$ba->write_uint16($this->iqadd)) return false;
		if (!$ba->write_uint16($this->attradd)) return false;
		if (!$ba->write_uint32($this->hp)) return false;
		if (!$ba->write_uint32($this->mp)) return false;
		return true;
	}

};

	
class mole2_set_skill_def_level_in {
	/* 设置人物时petid=0 */
	#类型:uint32
	public $petid;

	/* 技能ID */
	#类型:uint32
	public $skillid;

	/*  */
	#类型:int32
	public $def_level;


	public function mole2_set_skill_def_level_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		$this->skillid=$obj->skillid;
		$this->def_level=$obj->def_level;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		if (!$ba->read_uint32($this->skillid)) return false;
		if (!$ba->read_int32($this->def_level)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		if (!$ba->write_uint32($this->skillid)) return false;
		if (!$ba->write_int32($this->def_level)) return false;
		return true;
	}

};

	
class mole2_set_system_time_in {
	/*  */
	#定长数组,长度:20, 类型:char 
	public $datetime ;


	public function mole2_set_system_time_in(){

	}

	public function read_from_obj($obj ){
		$this->datetime=$obj->datetime;
		return true;
	}


	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->datetime,20)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->datetime,20);
		return true;
	}

};

	
class mole2_set_user_cli_buff_in {
	/* 类型 */
	#类型:uint32
	public $type;

	/*  */
	#定长数组,长度:512, 类型:char 
	public $buff ;


	public function mole2_set_user_cli_buff_in(){

	}

	public function read_from_obj($obj ){
		$this->type=$obj->type;
		$this->buff=$obj->buff;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if(!$ba->read_buf($this->buff,512)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		$ba->write_buf($this->buff,512);
		return true;
	}

};

	
class mole2_set_vip_buff_in {
	/**/
	#变长数组,最大长度:100, 类型:stru_vip_buff
	public $petlist =array();


	public function mole2_set_vip_buff_in(){

	}

	public function read_from_obj($obj ){
		$this->petlist=array();
		{for($i=0;$i<count($obj->petlist);$i++){
			$this->petlist[$i]=new stru_vip_buff();
			if (!$this->petlist[$i]->read_from_obj($obj->petlist )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$petlist_count=0 ;
		if (!$ba->read_uint32( $petlist_count )) return false;
		if ($petlist_count>100) return false;
		$this->petlist=array();
		{for($i=0; $i<$petlist_count;$i++){
			$this->petlist[$i]=new stru_vip_buff();
			if (!$this->petlist[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$petlist_count=count($this->petlist);
        if ($petlist_count>100 ) return false; 
        $ba->write_uint32($petlist_count);
		{for($i=0; $i<$petlist_count;$i++){
			if ( ! $this->petlist[$i] instanceof stru_vip_buff ) return false; 
			if (!$this->petlist[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class mole2_user_add_vip_ex_val_in {
	/*  */
	#类型:uint32
	public $vip_ex_val;


	public function mole2_user_add_vip_ex_val_in(){

	}

	public function read_from_obj($obj ){
		$this->vip_ex_val=$obj->vip_ex_val;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->vip_ex_val)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->vip_ex_val)) return false;
		return true;
	}

};

	
class mole2_user_set_vip_base_val_in {
	/*  */
	#类型:uint32
	public $growth_base;

	/*  */
	#类型:uint16
	public $chnl_id;


	public function mole2_user_set_vip_base_val_in(){

	}

	public function read_from_obj($obj ){
		$this->growth_base=$obj->growth_base;
		$this->chnl_id=$obj->chnl_id;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->growth_base)) return false;
		if (!$ba->read_uint16($this->chnl_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->growth_base)) return false;
		if (!$ba->write_uint16($this->chnl_id)) return false;
		return true;
	}

};

	
class mole2_user_set_vip_info_in {
	/* 标志 */
	#类型:uint32
	public $flag;

	/*  */
	#类型:uint32
	public $months;

	/* vip过期时间 */
	#类型:uint32
	public $vip_end_time;

	/*  */
	#类型:uint32
	public $vip_auto;

	/*  */
	#类型:uint32
	public $vip_begin_time;

	/*  */
	#类型:uint32
	public $last_charge_chnl_id;

	/*  */
	#类型:uint8
	public $vip_type;


	public function mole2_user_set_vip_info_in(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		$this->months=$obj->months;
		$this->vip_end_time=$obj->vip_end_time;
		$this->vip_auto=$obj->vip_auto;
		$this->vip_begin_time=$obj->vip_begin_time;
		$this->last_charge_chnl_id=$obj->last_charge_chnl_id;
		$this->vip_type=$obj->vip_type;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->months)) return false;
		if (!$ba->read_uint32($this->vip_end_time)) return false;
		if (!$ba->read_uint32($this->vip_auto)) return false;
		if (!$ba->read_uint32($this->vip_begin_time)) return false;
		if (!$ba->read_uint32($this->last_charge_chnl_id)) return false;
		if (!$ba->read_uint8($this->vip_type)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->months)) return false;
		if (!$ba->write_uint32($this->vip_end_time)) return false;
		if (!$ba->write_uint32($this->vip_auto)) return false;
		if (!$ba->write_uint32($this->vip_begin_time)) return false;
		if (!$ba->write_uint32($this->last_charge_chnl_id)) return false;
		if (!$ba->write_uint8($this->vip_type)) return false;
		return true;
	}

};

	
class mole2_user_set_vip_level_in {
	/*  */
	#类型:uint32
	public $vip_level;


	public function mole2_user_set_vip_level_in(){

	}

	public function read_from_obj($obj ){
		$this->vip_level=$obj->vip_level;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->vip_level)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->vip_level)) return false;
		return true;
	}

};

	
class struct_test_info {
	/*  */
	#类型:uint32
	public $n1;

	/*  */
	#类型:uint16
	public $n2;

	/**/
	#变长数组,最大长度:10, 类型:char
	public $n3 ;

	/*  */
	#定长数组,长度:9, 类型:char 
	public $n4 ;


	public function struct_test_info(){

	}

	public function read_from_obj($obj ){
		$this->n1=$obj->n1;
		$this->n2=$obj->n2;
		$this->n3=$obj->n3;
		$this->n4=$obj->n4;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->n1)) return false;
		if (!$ba->read_uint16($this->n2)) return false;

		$n3_count=0 ;
		if (!$ba->read_uint32( $n3_count )) return false;
		if ($n3_count>10) return false;
		if(!$ba->read_buf($this->n3,$n3_count))return false;
		if(!$ba->read_buf($this->n4,9)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->n1)) return false;
		if (!$ba->write_uint16($this->n2)) return false;
		$n3_count=strlen($this->n3);
		if ($n3_count>10 ) return false; 
		$ba->write_uint32($n3_count);
		$ba->write_buf($this->n3,$n3_count);
		$ba->write_buf($this->n4,9);
		return true;
	}

};

	
class su_mole2_add_black_in {
	/*  */
	#类型:uint32
	public $black_userid;


	public function su_mole2_add_black_in(){

	}

	public function read_from_obj($obj ){
		$this->black_userid=$obj->black_userid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->black_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->black_userid)) return false;
		return true;
	}

};

	
class su_mole2_add_friend_in {
	/*  */
	#类型:uint32
	public $friendid;


	public function su_mole2_add_friend_in(){

	}

	public function read_from_obj($obj ){
		$this->friendid=$obj->friendid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->friendid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->friendid)) return false;
		return true;
	}

};

	
class su_mole2_add_item_in {
	/*  */
	#类型:uint32
	public $itemid;

	/*  */
	#类型:int32
	public $add_count;

	/*  */
	#类型:int32
	public $add_storage;


	public function su_mole2_add_item_in(){

	}

	public function read_from_obj($obj ){
		$this->itemid=$obj->itemid;
		$this->add_count=$obj->add_count;
		$this->add_storage=$obj->add_storage;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_int32($this->add_count)) return false;
		if (!$ba->read_int32($this->add_storage)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_int32($this->add_count)) return false;
		if (!$ba->write_int32($this->add_storage)) return false;
		return true;
	}

};

	
class su_mole2_add_mail_in {
	/* 类型 */
	#类型:uint32
	public $type;

	/*  */
	#类型:uint32
	public $themeid;

	/*  */
	#类型:uint32
	public $sender_id;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $sender_nick ;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $title ;

	/*  */
	#定长数组,长度:256, 类型:char 
	public $message ;

	/*  */
	#定长数组,长度:64, 类型:char 
	public $items ;


	public function su_mole2_add_mail_in(){

	}

	public function read_from_obj($obj ){
		$this->type=$obj->type;
		$this->themeid=$obj->themeid;
		$this->sender_id=$obj->sender_id;
		$this->sender_nick=$obj->sender_nick;
		$this->title=$obj->title;
		$this->message=$obj->message;
		$this->items=$obj->items;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->type)) return false;
		if (!$ba->read_uint32($this->themeid)) return false;
		if (!$ba->read_uint32($this->sender_id)) return false;
		if(!$ba->read_buf($this->sender_nick,64)) return false;
		if(!$ba->read_buf($this->title,64)) return false;
		if(!$ba->read_buf($this->message,256)) return false;
		if(!$ba->read_buf($this->items,64)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->type)) return false;
		if (!$ba->write_uint32($this->themeid)) return false;
		if (!$ba->write_uint32($this->sender_id)) return false;
		$ba->write_buf($this->sender_nick,64);
		$ba->write_buf($this->title,64);
		$ba->write_buf($this->message,256);
		$ba->write_buf($this->items,64);
		return true;
	}

};

	
class su_mole2_add_pet_in {
	/*  */
	#类型:stru_pet_info
	public $pet;


	public function su_mole2_add_pet_in(){
		$this->pet=new stru_pet_info(); 

	}

	public function read_from_obj($obj ){
		if (!$this->pet->read_from_obj($obj->pet)) return false;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$this->pet->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->pet instanceof stru_pet_info ) return false; 
		if (!$this->pet->write_to_buf($ba)) return false;
		return true;
	}

};

	
class su_mole2_add_skill_in {
	/* 技巧 */
	#类型:stru_skill_info
	public $skill;


	public function su_mole2_add_skill_in(){
		$this->skill=new stru_skill_info(); 

	}

	public function read_from_obj($obj ){
		if (!$this->skill->read_from_obj($obj->skill)) return false;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$this->skill->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->skill instanceof stru_skill_info ) return false; 
		if (!$this->skill->write_to_buf($ba)) return false;
		return true;
	}

};

	
class su_mole2_add_xiaomee_in {
	/* 小米 */
	#类型:int32
	public $xiaomee;


	public function su_mole2_add_xiaomee_in(){

	}

	public function read_from_obj($obj ){
		$this->xiaomee=$obj->xiaomee;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_int32($this->xiaomee)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_int32($this->xiaomee)) return false;
		return true;
	}

};

	
class su_mole2_del_black_in {
	/*  */
	#类型:uint32
	public $black_userid;


	public function su_mole2_del_black_in(){

	}

	public function read_from_obj($obj ){
		$this->black_userid=$obj->black_userid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->black_userid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->black_userid)) return false;
		return true;
	}

};

	
class su_mole2_del_friend_in {
	/*  */
	#类型:uint32
	public $friendid;


	public function su_mole2_del_friend_in(){

	}

	public function read_from_obj($obj ){
		$this->friendid=$obj->friendid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->friendid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->friendid)) return false;
		return true;
	}

};

	
class su_mole2_del_pet_in {
	/* 宠物ID */
	#类型:uint32
	public $petid;


	public function su_mole2_del_pet_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		return true;
	}

};

	
class su_mole2_del_skill_in {
	/* 技能ID */
	#类型:uint32
	public $skillid;


	public function su_mole2_del_skill_in(){

	}

	public function read_from_obj($obj ){
		$this->skillid=$obj->skillid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->skillid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->skillid)) return false;
		return true;
	}

};

	
class su_mole2_del_task_in {
	/* 任务ID */
	#类型:uint32
	public $taskid;


	public function su_mole2_del_task_in(){

	}

	public function read_from_obj($obj ){
		$this->taskid=$obj->taskid;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->taskid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->taskid)) return false;
		return true;
	}

};

	
class su_mole2_get_ban_flag_out {
	/* 标志 */
	#类型:uint32
	public $flag;


	public function su_mole2_get_ban_flag_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		return true;
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

	
class su_mole2_get_blacks_out {
	/**/
	#变长数组,最大长度:200, 类型:uint32_t
	public $blacks =array();


	public function su_mole2_get_blacks_out(){

	}

	public function read_from_obj($obj ){
		$this->blacks=array();
		{for($i=0;$i<count($obj->blacks);$i++){
			 $this->blacks=$obj->blacks;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$blacks_count=0 ;
		if (!$ba->read_uint32( $blacks_count )) return false;
		if ($blacks_count>200) return false;
		$this->blacks=array();
		{for($i=0; $i<$blacks_count;$i++){
			if (!$ba->read_uint32($this->blacks[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$blacks_count=count($this->blacks);
        if ($blacks_count>200 ) return false; 
        $ba->write_uint32($blacks_count);
		{for($i=0; $i<$blacks_count;$i++){
			if (!$ba->write_uint32($this->blacks[$i])) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_expbox_out {
	/*  */
	#类型:uint32
	public $expbox;


	public function su_mole2_get_expbox_out(){

	}

	public function read_from_obj($obj ){
		$this->expbox=$obj->expbox;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->expbox)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->expbox)) return false;
		return true;
	}

};

	
class su_mole2_get_friends_out {
	/**/
	#变长数组,最大长度:200, 类型:uint32_t
	public $friends =array();


	public function su_mole2_get_friends_out(){

	}

	public function read_from_obj($obj ){
		$this->friends=array();
		{for($i=0;$i<count($obj->friends);$i++){
			 $this->friends=$obj->friends;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$friends_count=0 ;
		if (!$ba->read_uint32( $friends_count )) return false;
		if ($friends_count>200) return false;
		$this->friends=array();
		{for($i=0; $i<$friends_count;$i++){
			if (!$ba->read_uint32($this->friends[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$friends_count=count($this->friends);
        if ($friends_count>200 ) return false; 
        $ba->write_uint32($friends_count);
		{for($i=0; $i<$friends_count;$i++){
			if (!$ba->write_uint32($this->friends[$i])) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_items_out {
	/**/
	#变长数组,最大长度:5000, 类型:stru_item_info
	public $items =array();


	public function su_mole2_get_items_out(){

	}

	public function read_from_obj($obj ){
		$this->items=array();
		{for($i=0;$i<count($obj->items);$i++){
			$this->items[$i]=new stru_item_info();
			if (!$this->items[$i]->read_from_obj($obj->items )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$items_count=0 ;
		if (!$ba->read_uint32( $items_count )) return false;
		if ($items_count>5000) return false;
		$this->items=array();
		{for($i=0; $i<$items_count;$i++){
			$this->items[$i]=new stru_item_info();
			if (!$this->items[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$items_count=count($this->items);
        if ($items_count>5000 ) return false; 
        $ba->write_uint32($items_count);
		{for($i=0; $i<$items_count;$i++){
			if ( ! $this->items[$i] instanceof stru_item_info ) return false; 
			if (!$this->items[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_pets_out {
	/**/
	#变长数组,最大长度:6, 类型:stru_pet_info
	public $pets_in_bag =array();

	/**/
	#变长数组,最大长度:2500, 类型:stru_pet_info
	public $pets_in_home =array();


	public function su_mole2_get_pets_out(){

	}

	public function read_from_obj($obj ){
		$this->pets_in_bag=array();
		{for($i=0;$i<count($obj->pets_in_bag);$i++){
			$this->pets_in_bag[$i]=new stru_pet_info();
			if (!$this->pets_in_bag[$i]->read_from_obj($obj->pets_in_bag )) return false;
		}}
		$this->pets_in_home=array();
		{for($i=0;$i<count($obj->pets_in_home);$i++){
			$this->pets_in_home[$i]=new stru_pet_info();
			if (!$this->pets_in_home[$i]->read_from_obj($obj->pets_in_home )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$pets_in_bag_count=0 ;
		if (!$ba->read_uint32( $pets_in_bag_count )) return false;
		if ($pets_in_bag_count>6) return false;
		$this->pets_in_bag=array();
		{for($i=0; $i<$pets_in_bag_count;$i++){
			$this->pets_in_bag[$i]=new stru_pet_info();
			if (!$this->pets_in_bag[$i]->read_from_buf($ba)) return false;
		}}

		$pets_in_home_count=0 ;
		if (!$ba->read_uint32( $pets_in_home_count )) return false;
		if ($pets_in_home_count>2500) return false;
		$this->pets_in_home=array();
		{for($i=0; $i<$pets_in_home_count;$i++){
			$this->pets_in_home[$i]=new stru_pet_info();
			if (!$this->pets_in_home[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$pets_in_bag_count=count($this->pets_in_bag);
        if ($pets_in_bag_count>6 ) return false; 
        $ba->write_uint32($pets_in_bag_count);
		{for($i=0; $i<$pets_in_bag_count;$i++){
			if ( ! $this->pets_in_bag[$i] instanceof stru_pet_info ) return false; 
			if (!$this->pets_in_bag[$i]->write_to_buf($ba)) return false;
		}}
		$pets_in_home_count=count($this->pets_in_home);
        if ($pets_in_home_count>2500 ) return false; 
        $ba->write_uint32($pets_in_home_count);
		{for($i=0; $i<$pets_in_home_count;$i++){
			if ( ! $this->pets_in_home[$i] instanceof stru_pet_info ) return false; 
			if (!$this->pets_in_home[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_skills_out {
	/**/
	#变长数组,最大长度:20, 类型:stru_skill_info
	public $skills =array();


	public function su_mole2_get_skills_out(){

	}

	public function read_from_obj($obj ){
		$this->skills=array();
		{for($i=0;$i<count($obj->skills);$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_obj($obj->skills )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$skills_count=0 ;
		if (!$ba->read_uint32( $skills_count )) return false;
		if ($skills_count>20) return false;
		$this->skills=array();
		{for($i=0; $i<$skills_count;$i++){
			$this->skills[$i]=new stru_skill_info();
			if (!$this->skills[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$skills_count=count($this->skills);
        if ($skills_count>20 ) return false; 
        $ba->write_uint32($skills_count);
		{for($i=0; $i<$skills_count;$i++){
			if ( ! $this->skills[$i] instanceof stru_skill_info ) return false; 
			if (!$this->skills[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_tasks_out {
	/**/
	#变长数组,最大长度:100, 类型:stru_task_doing
	public $tasks_doing =array();

	/**/
	#变长数组,最大长度:100, 类型:stru_task_done
	public $tasks_done =array();


	public function su_mole2_get_tasks_out(){

	}

	public function read_from_obj($obj ){
		$this->tasks_doing=array();
		{for($i=0;$i<count($obj->tasks_doing);$i++){
			$this->tasks_doing[$i]=new stru_task_doing();
			if (!$this->tasks_doing[$i]->read_from_obj($obj->tasks_doing )) return false;
		}}
		$this->tasks_done=array();
		{for($i=0;$i<count($obj->tasks_done);$i++){
			$this->tasks_done[$i]=new stru_task_done();
			if (!$this->tasks_done[$i]->read_from_obj($obj->tasks_done )) return false;
		}}
		return true;
	}


	public function read_from_buf($ba ){

		$tasks_doing_count=0 ;
		if (!$ba->read_uint32( $tasks_doing_count )) return false;
		if ($tasks_doing_count>100) return false;
		$this->tasks_doing=array();
		{for($i=0; $i<$tasks_doing_count;$i++){
			$this->tasks_doing[$i]=new stru_task_doing();
			if (!$this->tasks_doing[$i]->read_from_buf($ba)) return false;
		}}

		$tasks_done_count=0 ;
		if (!$ba->read_uint32( $tasks_done_count )) return false;
		if ($tasks_done_count>100) return false;
		$this->tasks_done=array();
		{for($i=0; $i<$tasks_done_count;$i++){
			$this->tasks_done[$i]=new stru_task_done();
			if (!$this->tasks_done[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$tasks_doing_count=count($this->tasks_doing);
        if ($tasks_doing_count>100 ) return false; 
        $ba->write_uint32($tasks_doing_count);
		{for($i=0; $i<$tasks_doing_count;$i++){
			if ( ! $this->tasks_doing[$i] instanceof stru_task_doing ) return false; 
			if (!$this->tasks_doing[$i]->write_to_buf($ba)) return false;
		}}
		$tasks_done_count=count($this->tasks_done);
        if ($tasks_done_count>100 ) return false; 
        $ba->write_uint32($tasks_done_count);
		{for($i=0; $i<$tasks_done_count;$i++){
			if ( ! $this->tasks_done[$i] instanceof stru_task_done ) return false; 
			if (!$this->tasks_done[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class su_mole2_get_user_out {
	/* 用户昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/* 标志 */
	#类型:uint32
	public $flag;

	/* 颜色 */
	#类型:uint32
	public $color;

	/*  */
	#类型:uint32
	public $regist_time;

	/* 种族 */
	#类型:uint32
	public $race;

	/*  */
	#类型:uint32
	public $profession;

	/*  */
	#类型:uint32
	public $joblevel;

	/* 荣誉值 */
	#类型:uint32
	public $honor;

	/* 小米 */
	#类型:uint32
	public $xiaomee;

	/* 等级 */
	#类型:uint32
	public $level;

	/* 经验 */
	#类型:uint32
	public $experience;

	/* 体力 */
	#类型:uint32
	public $physique;

	/* 力量 */
	#类型:uint32
	public $strength;

	/* 耐力 */
	#类型:uint32
	public $endurance;

	/* 速度 */
	#类型:uint32
	public $quick;

	/* 智力 */
	#类型:uint32
	public $intelligence;

	/* 剩余属性点 */
	#类型:uint32
	public $attr_addition;

	/* 体力 */
	#类型:uint32
	public $hp;

	/* 魔法值 */
	#类型:uint32
	public $mp;

	/* 受伤等级 0:未受伤, 1:白 2:黄 3:红 */
	#类型:uint32
	public $injury_state;

	/* 人物战斗默认站位 1:前排 0:后排 */
	#类型:uint32
	public $in_front;

	/* 人物携带最大装备数 */
	#类型:uint32
	public $max_attire;

	/* 人物携带最大药剂数 */
	#类型:uint32
	public $max_medicine;

	/* 人物携带最大材料数 */
	#类型:uint32
	public $max_stuff;

	/* 时间 */
	#类型:uint32
	public $time;

	/* 坐标x轴 */
	#类型:uint32
	public $axis_x;

	/* 坐标y轴 */
	#类型:uint32
	public $axis_y;

	/* 地图编号 */
	#类型:uint32
	public $mapid;

	/*  */
	#类型:uint32
	public $daytime;

	/*  */
	#类型:uint32
	public $fly_mapid;

	/*  */
	#类型:uint32
	public $expbox;

	/* 电量值 */
	#类型:uint32
	public $energy;

	/*  */
	#类型:uint32
	public $skill_expbox;

	/*  */
	#类型:uint32
	public $flag_ex;

	/*  */
	#类型:uint32
	public $winbossid;

	/*  */
	#类型:uint32
	public $parent;

	/*  */
	#类型:stru_battle_info
	public $battle;


	public function su_mole2_get_user_out(){
		$this->battle=new stru_battle_info(); 

	}

	public function read_from_obj($obj ){
		$this->nick=$obj->nick;
		$this->flag=$obj->flag;
		$this->color=$obj->color;
		$this->regist_time=$obj->regist_time;
		$this->race=$obj->race;
		$this->profession=$obj->profession;
		$this->joblevel=$obj->joblevel;
		$this->honor=$obj->honor;
		$this->xiaomee=$obj->xiaomee;
		$this->level=$obj->level;
		$this->experience=$obj->experience;
		$this->physique=$obj->physique;
		$this->strength=$obj->strength;
		$this->endurance=$obj->endurance;
		$this->quick=$obj->quick;
		$this->intelligence=$obj->intelligence;
		$this->attr_addition=$obj->attr_addition;
		$this->hp=$obj->hp;
		$this->mp=$obj->mp;
		$this->injury_state=$obj->injury_state;
		$this->in_front=$obj->in_front;
		$this->max_attire=$obj->max_attire;
		$this->max_medicine=$obj->max_medicine;
		$this->max_stuff=$obj->max_stuff;
		$this->time=$obj->time;
		$this->axis_x=$obj->axis_x;
		$this->axis_y=$obj->axis_y;
		$this->mapid=$obj->mapid;
		$this->daytime=$obj->daytime;
		$this->fly_mapid=$obj->fly_mapid;
		$this->expbox=$obj->expbox;
		$this->energy=$obj->energy;
		$this->skill_expbox=$obj->skill_expbox;
		$this->flag_ex=$obj->flag_ex;
		$this->winbossid=$obj->winbossid;
		$this->parent=$obj->parent;
		if (!$this->battle->read_from_obj($obj->battle)) return false;
		return true;
	}


	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->flag)) return false;
		if (!$ba->read_uint32($this->color)) return false;
		if (!$ba->read_uint32($this->regist_time)) return false;
		if (!$ba->read_uint32($this->race)) return false;
		if (!$ba->read_uint32($this->profession)) return false;
		if (!$ba->read_uint32($this->joblevel)) return false;
		if (!$ba->read_uint32($this->honor)) return false;
		if (!$ba->read_uint32($this->xiaomee)) return false;
		if (!$ba->read_uint32($this->level)) return false;
		if (!$ba->read_uint32($this->experience)) return false;
		if (!$ba->read_uint32($this->physique)) return false;
		if (!$ba->read_uint32($this->strength)) return false;
		if (!$ba->read_uint32($this->endurance)) return false;
		if (!$ba->read_uint32($this->quick)) return false;
		if (!$ba->read_uint32($this->intelligence)) return false;
		if (!$ba->read_uint32($this->attr_addition)) return false;
		if (!$ba->read_uint32($this->hp)) return false;
		if (!$ba->read_uint32($this->mp)) return false;
		if (!$ba->read_uint32($this->injury_state)) return false;
		if (!$ba->read_uint32($this->in_front)) return false;
		if (!$ba->read_uint32($this->max_attire)) return false;
		if (!$ba->read_uint32($this->max_medicine)) return false;
		if (!$ba->read_uint32($this->max_stuff)) return false;
		if (!$ba->read_uint32($this->time)) return false;
		if (!$ba->read_uint32($this->axis_x)) return false;
		if (!$ba->read_uint32($this->axis_y)) return false;
		if (!$ba->read_uint32($this->mapid)) return false;
		if (!$ba->read_uint32($this->daytime)) return false;
		if (!$ba->read_uint32($this->fly_mapid)) return false;
		if (!$ba->read_uint32($this->expbox)) return false;
		if (!$ba->read_uint32($this->energy)) return false;
		if (!$ba->read_uint32($this->skill_expbox)) return false;
		if (!$ba->read_uint32($this->flag_ex)) return false;
		if (!$ba->read_uint32($this->winbossid)) return false;
		if (!$ba->read_uint32($this->parent)) return false;
		if (!$this->battle->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->flag)) return false;
		if (!$ba->write_uint32($this->color)) return false;
		if (!$ba->write_uint32($this->regist_time)) return false;
		if (!$ba->write_uint32($this->race)) return false;
		if (!$ba->write_uint32($this->profession)) return false;
		if (!$ba->write_uint32($this->joblevel)) return false;
		if (!$ba->write_uint32($this->honor)) return false;
		if (!$ba->write_uint32($this->xiaomee)) return false;
		if (!$ba->write_uint32($this->level)) return false;
		if (!$ba->write_uint32($this->experience)) return false;
		if (!$ba->write_uint32($this->physique)) return false;
		if (!$ba->write_uint32($this->strength)) return false;
		if (!$ba->write_uint32($this->endurance)) return false;
		if (!$ba->write_uint32($this->quick)) return false;
		if (!$ba->write_uint32($this->intelligence)) return false;
		if (!$ba->write_uint32($this->attr_addition)) return false;
		if (!$ba->write_uint32($this->hp)) return false;
		if (!$ba->write_uint32($this->mp)) return false;
		if (!$ba->write_uint32($this->injury_state)) return false;
		if (!$ba->write_uint32($this->in_front)) return false;
		if (!$ba->write_uint32($this->max_attire)) return false;
		if (!$ba->write_uint32($this->max_medicine)) return false;
		if (!$ba->write_uint32($this->max_stuff)) return false;
		if (!$ba->write_uint32($this->time)) return false;
		if (!$ba->write_uint32($this->axis_x)) return false;
		if (!$ba->write_uint32($this->axis_y)) return false;
		if (!$ba->write_uint32($this->mapid)) return false;
		if (!$ba->write_uint32($this->daytime)) return false;
		if (!$ba->write_uint32($this->fly_mapid)) return false;
		if (!$ba->write_uint32($this->expbox)) return false;
		if (!$ba->write_uint32($this->energy)) return false;
		if (!$ba->write_uint32($this->skill_expbox)) return false;
		if (!$ba->write_uint32($this->flag_ex)) return false;
		if (!$ba->write_uint32($this->winbossid)) return false;
		if (!$ba->write_uint32($this->parent)) return false;
		if ( ! $this->battle instanceof stru_battle_info ) return false; 
		if (!$this->battle->write_to_buf($ba)) return false;
		return true;
	}

};

	
class su_mole2_get_vip_flag_out {
	/* 标志 */
	#类型:uint32
	public $flag;


	public function su_mole2_get_vip_flag_out(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		return true;
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

	
class su_mole2_get_vip_info_out {
	/*  */
	#类型:uint32
	public $vip_auto_charge;

	/*  */
	#类型:uint32
	public $vip_level;

	/*  */
	#类型:uint32
	public $vip_value;

	/*  */
	#类型:uint32
	public $vip_base_value;

	/*  */
	#类型:uint32
	public $vip_end_time;

	/*  */
	#类型:uint32
	public $vip_begin_time;


	public function su_mole2_get_vip_info_out(){

	}

	public function read_from_obj($obj ){
		$this->vip_auto_charge=$obj->vip_auto_charge;
		$this->vip_level=$obj->vip_level;
		$this->vip_value=$obj->vip_value;
		$this->vip_base_value=$obj->vip_base_value;
		$this->vip_end_time=$obj->vip_end_time;
		$this->vip_begin_time=$obj->vip_begin_time;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->vip_auto_charge)) return false;
		if (!$ba->read_uint32($this->vip_level)) return false;
		if (!$ba->read_uint32($this->vip_value)) return false;
		if (!$ba->read_uint32($this->vip_base_value)) return false;
		if (!$ba->read_uint32($this->vip_end_time)) return false;
		if (!$ba->read_uint32($this->vip_begin_time)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->vip_auto_charge)) return false;
		if (!$ba->write_uint32($this->vip_level)) return false;
		if (!$ba->write_uint32($this->vip_value)) return false;
		if (!$ba->write_uint32($this->vip_base_value)) return false;
		if (!$ba->write_uint32($this->vip_end_time)) return false;
		if (!$ba->write_uint32($this->vip_begin_time)) return false;
		return true;
	}

};

	
class su_mole2_set_ban_flag_in {
	/* 标志 */
	#类型:uint32
	public $flag;


	public function su_mole2_set_ban_flag_in(){

	}

	public function read_from_obj($obj ){
		$this->flag=$obj->flag;
		return true;
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

	
class su_mole2_set_base_attr_in {
	/* 体力 */
	#类型:int32
	public $physique;

	/* 力量 */
	#类型:int32
	public $strength;

	/* 耐力 */
	#类型:int32
	public $endurance;

	/* 速度 */
	#类型:int32
	public $quick;

	/* 智力 */
	#类型:int32
	public $intelligence;

	/*  */
	#类型:uint32
	public $addition;


	public function su_mole2_set_base_attr_in(){

	}

	public function read_from_obj($obj ){
		$this->physique=$obj->physique;
		$this->strength=$obj->strength;
		$this->endurance=$obj->endurance;
		$this->quick=$obj->quick;
		$this->intelligence=$obj->intelligence;
		$this->addition=$obj->addition;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_int32($this->physique)) return false;
		if (!$ba->read_int32($this->strength)) return false;
		if (!$ba->read_int32($this->endurance)) return false;
		if (!$ba->read_int32($this->quick)) return false;
		if (!$ba->read_int32($this->intelligence)) return false;
		if (!$ba->read_uint32($this->addition)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_int32($this->physique)) return false;
		if (!$ba->write_int32($this->strength)) return false;
		if (!$ba->write_int32($this->endurance)) return false;
		if (!$ba->write_int32($this->quick)) return false;
		if (!$ba->write_int32($this->intelligence)) return false;
		if (!$ba->write_uint32($this->addition)) return false;
		return true;
	}

};

	
class su_mole2_set_expbox_in {
	/*  */
	#类型:uint32
	public $expbox;


	public function su_mole2_set_expbox_in(){

	}

	public function read_from_obj($obj ){
		$this->expbox=$obj->expbox;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->expbox)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->expbox)) return false;
		return true;
	}

};

	
class su_mole2_set_field_value_in {
	/* 宠物ID */
	#类型:uint32
	public $petid;

	/* <font color="red">用户数据:nick 昵称 flag 标志位  color 颜色  regist_time 注册时间 race 种族  profession 职业 joblevel 职业等级 honor 荣誉值 xiaomee 星豆  level 等级 experience 经验 physique 体力 strength 力量 endurance 耐力 quick 速度 intelligence 智力 attr_addition 剩余属性点 hp 体力  mp 魔法值 injury_state 受伤等级 0:未受伤 in_front 人物战斗默认站位 1:前排 0:后排 max_attire 人物携带最大装备数 max_medicine 人物携带最大药剂数 max_stuff 人物携带最大材料数 time	时间 axis_x 坐标x轴 axis_y 坐标y轴 mapid 地图编号  expbox 经验树 energy 电量值 skill_expbox 技能经验树</font><br><font color="blue">宠物数据： gettime 获取时间 pettype 宠物类型ID race 种族 flag 标志  nick 昵称 level 等级 exp 经验 physiqueinit 体力初值 strengthinit 力量初值 enduranceinit 耐力初值 quickinit 速度初值 iqinit 智慧初值 physiqueparam 体力成长值 strengthparam 力量成长值 enduranceparam 耐力成长值 quickparam 速度成长值 iqparam 智慧成长值 physique_add 体力配点值 strength_add 力量配点值 endurance_add 耐力配点值 quick_add 速度配点值 iq_add 智慧配点值 attr_addition 剩余属性点 hp 体力 mp 魔法值 earth 地 water 水 fire 火 wind 风 injury_state 受伤等级 0:未受伤, 1:白 2:黄 3:红 location 位置. 0：未定义，1：休息， 2：待命， 3：出战，[10000,20000):仓库</font> */
	#定长数组,长度:32, 类型:char 
	public $field ;

	/* 数值 */
	#定长数组,长度:255, 类型:char 
	public $value ;


	public function su_mole2_set_field_value_in(){

	}

	public function read_from_obj($obj ){
		$this->petid=$obj->petid;
		$this->field=$obj->field;
		$this->value=$obj->value;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->petid)) return false;
		if(!$ba->read_buf($this->field,32)) return false;
		if(!$ba->read_buf($this->value,255)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->petid)) return false;
		$ba->write_buf($this->field,32);
		$ba->write_buf($this->value,255);
		return true;
	}

};

	
class su_mole2_set_nick_in {
	/* 昵称 */
	#定长数组,长度:16, 类型:char 
	public $nick ;


	public function su_mole2_set_nick_in(){

	}

	public function read_from_obj($obj ){
		$this->nick=$obj->nick;
		return true;
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

	
class su_mole2_set_pet_in {
	/*  */
	#类型:stru_pet_info
	public $pet;


	public function su_mole2_set_pet_in(){
		$this->pet=new stru_pet_info(); 

	}

	public function read_from_obj($obj ){
		if (!$this->pet->read_from_obj($obj->pet)) return false;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$this->pet->read_from_buf($ba)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if ( ! $this->pet instanceof stru_pet_info ) return false; 
		if (!$this->pet->write_to_buf($ba)) return false;
		return true;
	}

};

	
class su_mole2_set_position_in {
	/*  */
	#类型:uint32
	public $position;


	public function su_mole2_set_position_in(){

	}

	public function read_from_obj($obj ){
		$this->position=$obj->position;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->position)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->position)) return false;
		return true;
	}

};

	
class su_mole2_set_profession_in {
	/*  */
	#类型:uint32
	public $profession;


	public function su_mole2_set_profession_in(){

	}

	public function read_from_obj($obj ){
		$this->profession=$obj->profession;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->profession)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->profession)) return false;
		return true;
	}

};

	
class su_mole2_set_task_in {
	/* 任务ID */
	#类型:uint32
	public $taskid;

	/* 1:未完成,2:已完成 */
	#类型:uint32
	public $state;


	public function su_mole2_set_task_in(){

	}

	public function read_from_obj($obj ){
		$this->taskid=$obj->taskid;
		$this->state=$obj->state;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->taskid)) return false;
		if (!$ba->read_uint32($this->state)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->taskid)) return false;
		if (!$ba->write_uint32($this->state)) return false;
		return true;
	}

};

	
class su_switch_send_msg_in {
	/* 消息 */
	#定长数组,长度:1024, 类型:char 
	public $msg ;


	public function su_switch_send_msg_in(){

	}

	public function read_from_obj($obj ){
		$this->msg=$obj->msg;
		return true;
	}


	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->msg,1024)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->msg,1024);
		return true;
	}

};

	
class stru_game_change_add {
	/* 1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 21:帕拉巫 */
	#类型:uint32
	public $gameid;

	/* 米米号 */
	#类型:uint32
	public $userid;

	/* 记录时间 */
	#类型:uint32
	public $logtime;

	/* 操作类型：0-4: 对v1-v5中的值加1 */
	#类型:uint32
	public $opt_type;

	/*  */
	#类型:int32
	public $ex_v1;

	/*  */
	#类型:int32
	public $ex_v2;

	/*  */
	#类型:int32
	public $ex_v3;

	/*  */
	#类型:int32
	public $ex_v4;


	public function stru_game_change_add(){

	}

	public function read_from_obj($obj ){
		$this->gameid=$obj->gameid;
		$this->userid=$obj->userid;
		$this->logtime=$obj->logtime;
		$this->opt_type=$obj->opt_type;
		$this->ex_v1=$obj->ex_v1;
		$this->ex_v2=$obj->ex_v2;
		$this->ex_v3=$obj->ex_v3;
		$this->ex_v4=$obj->ex_v4;
		return true;
	}


	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->userid)) return false;
		if (!$ba->read_uint32($this->logtime)) return false;
		if (!$ba->read_uint32($this->opt_type)) return false;
		if (!$ba->read_int32($this->ex_v1)) return false;
		if (!$ba->read_int32($this->ex_v2)) return false;
		if (!$ba->read_int32($this->ex_v3)) return false;
		if (!$ba->read_int32($this->ex_v4)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->userid)) return false;
		if (!$ba->write_uint32($this->logtime)) return false;
		if (!$ba->write_uint32($this->opt_type)) return false;
		if (!$ba->write_int32($this->ex_v1)) return false;
		if (!$ba->write_int32($this->ex_v2)) return false;
		if (!$ba->write_int32($this->ex_v3)) return false;
		if (!$ba->write_int32($this->ex_v4)) return false;
		return true;
	}

};

	
class Cmole2_db_proto  extends Cproto_base {
	var $cmd_id_name_map=array();
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
		$this->cmd_id_name_map[0x3E00]=array('su_mole2_get_user','','su_mole2_get_user_out');
		$this->cmd_id_name_map[0x3E01]=array('su_mole2_get_tasks','','su_mole2_get_tasks_out');
		$this->cmd_id_name_map[0x3E02]=array('su_mole2_del_task','su_mole2_del_task_in','');
		$this->cmd_id_name_map[0x3E03]=array('su_mole2_get_items','','su_mole2_get_items_out');
		$this->cmd_id_name_map[0x3E04]=array('su_mole2_add_item','su_mole2_add_item_in','');
		$this->cmd_id_name_map[0x3E05]=array('su_mole2_get_skills','','su_mole2_get_skills_out');
		$this->cmd_id_name_map[0x3E06]=array('su_mole2_add_skill','su_mole2_add_skill_in','');
		$this->cmd_id_name_map[0x3E07]=array('su_mole2_del_skill','su_mole2_del_skill_in','');
		$this->cmd_id_name_map[0x3E08]=array('su_mole2_get_pets','','su_mole2_get_pets_out');
		$this->cmd_id_name_map[0x3E09]=array('su_mole2_add_pet','su_mole2_add_pet_in','');
		$this->cmd_id_name_map[0x3E0A]=array('su_mole2_set_pet','su_mole2_set_pet_in','');
		$this->cmd_id_name_map[0x3E0B]=array('su_mole2_del_pet','su_mole2_del_pet_in','');
		$this->cmd_id_name_map[0x3E0C]=array('su_mole2_get_friends','','su_mole2_get_friends_out');
		$this->cmd_id_name_map[0x3E0D]=array('su_mole2_add_friend','su_mole2_add_friend_in','');
		$this->cmd_id_name_map[0x3E0E]=array('su_mole2_del_friend','su_mole2_del_friend_in','');
		$this->cmd_id_name_map[0x3E0F]=array('su_mole2_get_blacks','','su_mole2_get_blacks_out');
		$this->cmd_id_name_map[0x3E10]=array('su_mole2_add_black','su_mole2_add_black_in','');
		$this->cmd_id_name_map[0x3E11]=array('su_mole2_del_black','su_mole2_del_black_in','');
		$this->cmd_id_name_map[0x3E12]=array('su_mole2_add_mail','su_mole2_add_mail_in','');
		$this->cmd_id_name_map[0x3E13]=array('su_mole2_get_vip_flag','','su_mole2_get_vip_flag_out');
		$this->cmd_id_name_map[0x3E14]=array('su_mole2_get_expbox','','su_mole2_get_expbox_out');
		$this->cmd_id_name_map[0x3E15]=array('su_mole2_get_ban_flag','','su_mole2_get_ban_flag_out');
		$this->cmd_id_name_map[0x3E16]=array('su_mole2_set_ban_flag','su_mole2_set_ban_flag_in','');
		$this->cmd_id_name_map[0x3E17]=array('su_mole2_set_expbox','su_mole2_set_expbox_in','');
		$this->cmd_id_name_map[0x3E18]=array('su_mole2_set_nick','su_mole2_set_nick_in','');
		$this->cmd_id_name_map[0x3E19]=array('su_mole2_set_profession','su_mole2_set_profession_in','');
		$this->cmd_id_name_map[0x3E1A]=array('su_mole2_add_xiaomee','su_mole2_add_xiaomee_in','');
		$this->cmd_id_name_map[0x3E1B]=array('su_mole2_set_base_attr','su_mole2_set_base_attr_in','');
		$this->cmd_id_name_map[0x3E1C]=array('su_mole2_set_position','su_mole2_set_position_in','');
		$this->cmd_id_name_map[0x3E1D]=array('su_mole2_set_field_value','su_mole2_set_field_value_in','');
		$this->cmd_id_name_map[0x3E1E]=array('su_mole2_get_vip_info','','su_mole2_get_vip_info_out');
		$this->cmd_id_name_map[0xF234]=array('su_switch_send_msg','su_switch_send_msg_in','');
		$this->cmd_id_name_map[0xF23B]=array('su_switch_mail_notify','','');
		$this->cmd_id_name_map[0xF618]=array('su_switch_kick_player','','');

    }
	
	function send_obj_cmd($obj){
		$cmdid=$obj["cmdid"];
		$userid=$obj["userid"];

		$cmd_item=$this->cmd_id_name_map[$cmdid];

		if (!$cmd_item) {
			return array("result"=>1104);
		}

		$cmd_name=$cmd_item[0];
		$cmd_in=$cmd_item[1];
		$cmd_out=$cmd_item[2];
		$in=null;
		if ($cmd_in){
			eval("\$in=new {$cmd_in}();");
			$in->read_from_obj($obj["pri_in"]);
			eval("\$arr=\$this->$cmd_name(\$userid,\$in);" );
			return $arr;
		}else{
			eval("\$arr=\$this->$cmd_name(\$userid);" );
			return $arr;
		}
	}




	/* 拉取用户信息 */

	function su_mole2_get_user($userid ){

		return $this->send_cmd_new(0x3E00,$userid, null, new su_mole2_get_user_out(), 0xd11add8e);
	}
	
	/* 拉取任务列表 */

	function su_mole2_get_tasks($userid ){

		return $this->send_cmd_new(0x3E01,$userid, null, new su_mole2_get_tasks_out(), 0xd5451405);
	}
	
	/* 删除任务 */
	/* 调用方式还可以是： 
		$in=new su_mole2_del_task_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_del_task($userid,$in );
	*/

	function su_mole2_del_task($userid , $taskid=null){

		if ( $taskid instanceof su_mole2_del_task_in ){
			$in=$taskid;
		}else{
			$in=new su_mole2_del_task_in();
			$in->taskid=$taskid;

		}
		
		return $this->send_cmd_new(0x3E02,$userid, $in, null, 0xecec20cf);
	}
	
	/* 拉取用户物品列表 */

	function su_mole2_get_items($userid ){

		return $this->send_cmd_new(0x3E03,$userid, null, new su_mole2_get_items_out(), 0x65db39e9);
	}
	
	/* 添加物品 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_item_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_item($userid,$in );
	*/

	function su_mole2_add_item($userid , $itemid=null, $add_count=null, $add_storage=null){

		if ( $itemid instanceof su_mole2_add_item_in ){
			$in=$itemid;
		}else{
			$in=new su_mole2_add_item_in();
			$in->itemid=$itemid;
			$in->add_count=$add_count;
			$in->add_storage=$add_storage;

		}
		
		return $this->send_cmd_new(0x3E04,$userid, $in, null, 0xc07395a5);
	}
	
	/* 拉取人物技能列表 */

	function su_mole2_get_skills($userid ){

		return $this->send_cmd_new(0x3E05,$userid, null, new su_mole2_get_skills_out(), 0xc81a4682);
	}
	
	/* 添加人物技能 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_skill_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_skill($userid,$in );
	*/

	function su_mole2_add_skill($userid , $skill=null){

		if ( $skill instanceof su_mole2_add_skill_in ){
			$in=$skill;
		}else{
			$in=new su_mole2_add_skill_in();
			$in->skill=$skill;

		}
		
		return $this->send_cmd_new(0x3E06,$userid, $in, null, 0x989ccde5);
	}
	
	/* 删除任务技能 */
	/* 调用方式还可以是： 
		$in=new su_mole2_del_skill_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_del_skill($userid,$in );
	*/

	function su_mole2_del_skill($userid , $skillid=null){

		if ( $skillid instanceof su_mole2_del_skill_in ){
			$in=$skillid;
		}else{
			$in=new su_mole2_del_skill_in();
			$in->skillid=$skillid;

		}
		
		return $this->send_cmd_new(0x3E07,$userid, $in, null, 0x19004f71);
	}
	
	/* 拉取宠物列表 */

	function su_mole2_get_pets($userid ){

		return $this->send_cmd_new(0x3E08,$userid, null, new su_mole2_get_pets_out(), 0xbd4a58de);
	}
	
	/* 添加宠物 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_pet_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_pet($userid,$in );
	*/

	function su_mole2_add_pet($userid , $pet=null){

		if ( $pet instanceof su_mole2_add_pet_in ){
			$in=$pet;
		}else{
			$in=new su_mole2_add_pet_in();
			$in->pet=$pet;

		}
		
		return $this->send_cmd_new(0x3E09,$userid, $in, null, 0x62850a7c);
	}
	
	/* 设置宠物 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_pet_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_pet($userid,$in );
	*/

	function su_mole2_set_pet($userid , $pet=null){

		if ( $pet instanceof su_mole2_set_pet_in ){
			$in=$pet;
		}else{
			$in=new su_mole2_set_pet_in();
			$in->pet=$pet;

		}
		
		return $this->send_cmd_new(0x3E0A,$userid, $in, null, 0x7032a8c6);
	}
	
	/* 删除宠物 */
	/* 调用方式还可以是： 
		$in=new su_mole2_del_pet_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_del_pet($userid,$in );
	*/

	function su_mole2_del_pet($userid , $petid=null){

		if ( $petid instanceof su_mole2_del_pet_in ){
			$in=$petid;
		}else{
			$in=new su_mole2_del_pet_in();
			$in->petid=$petid;

		}
		
		return $this->send_cmd_new(0x3E0B,$userid, $in, null, 0x1240f316);
	}
	
	/* 拉取好友列表 */

	function su_mole2_get_friends($userid ){

		return $this->send_cmd_new(0x3E0C,$userid, null, new su_mole2_get_friends_out(), 0x8e319ce1);
	}
	
	/* 添加好友 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_friend_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_friend($userid,$in );
	*/

	function su_mole2_add_friend($userid , $friendid=null){

		if ( $friendid instanceof su_mole2_add_friend_in ){
			$in=$friendid;
		}else{
			$in=new su_mole2_add_friend_in();
			$in->friendid=$friendid;

		}
		
		return $this->send_cmd_new(0x3E0D,$userid, $in, null, 0xadeff3cc);
	}
	
	/* 删除好友 */
	/* 调用方式还可以是： 
		$in=new su_mole2_del_friend_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_del_friend($userid,$in );
	*/

	function su_mole2_del_friend($userid , $friendid=null){

		if ( $friendid instanceof su_mole2_del_friend_in ){
			$in=$friendid;
		}else{
			$in=new su_mole2_del_friend_in();
			$in->friendid=$friendid;

		}
		
		return $this->send_cmd_new(0x3E0E,$userid, $in, null, 0x10945751);
	}
	
	/* 拉取黑名单列表 */

	function su_mole2_get_blacks($userid ){

		return $this->send_cmd_new(0x3E0F,$userid, null, new su_mole2_get_blacks_out(), 0xa2b8382b);
	}
	
	/* 添加米米号到黑名单 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_black_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_black($userid,$in );
	*/

	function su_mole2_add_black($userid , $black_userid=null){

		if ( $black_userid instanceof su_mole2_add_black_in ){
			$in=$black_userid;
		}else{
			$in=new su_mole2_add_black_in();
			$in->black_userid=$black_userid;

		}
		
		return $this->send_cmd_new(0x3E10,$userid, $in, null, 0xe8780477);
	}
	
	/* 删除黑名单中的某个米米号 */
	/* 调用方式还可以是： 
		$in=new su_mole2_del_black_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_del_black($userid,$in );
	*/

	function su_mole2_del_black($userid , $black_userid=null){

		if ( $black_userid instanceof su_mole2_del_black_in ){
			$in=$black_userid;
		}else{
			$in=new su_mole2_del_black_in();
			$in->black_userid=$black_userid;

		}
		
		return $this->send_cmd_new(0x3E11,$userid, $in, null, 0xd8d8ab30);
	}
	
	/* 添加邮件 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_mail_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_mail($userid,$in );
	*/

	function su_mole2_add_mail($userid , $type=null, $themeid=null, $sender_id=null, $sender_nick=null, $title=null, $message=null, $items=null){

		if ( $type instanceof su_mole2_add_mail_in ){
			$in=$type;
		}else{
			$in=new su_mole2_add_mail_in();
			$in->type=$type;
			$in->themeid=$themeid;
			$in->sender_id=$sender_id;
			$in->sender_nick=$sender_nick;
			$in->title=$title;
			$in->message=$message;
			$in->items=$items;

		}
		
		return $this->send_cmd_new(0x3E12,$userid, $in, null, 0x5cb70797);
	}
	
	/* 拉取vip标志 */

	function su_mole2_get_vip_flag($userid ){

		return $this->send_cmd_new(0x3E13,$userid, null, new su_mole2_get_vip_flag_out(), 0x09ade143);
	}
	
	/* 拉取经验树经验值 */

	function su_mole2_get_expbox($userid ){

		return $this->send_cmd_new(0x3E14,$userid, null, new su_mole2_get_expbox_out(), 0xdea67f96);
	}
	
	/* 拉取封号标志 */

	function su_mole2_get_ban_flag($userid ){

		return $this->send_cmd_new(0x3E15,$userid, null, new su_mole2_get_ban_flag_out(), 0x056a7580);
	}
	
	/* 设置封号标志 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_ban_flag_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_ban_flag($userid,$in );
	*/

	function su_mole2_set_ban_flag($userid , $flag=null){

		if ( $flag instanceof su_mole2_set_ban_flag_in ){
			$in=$flag;
		}else{
			$in=new su_mole2_set_ban_flag_in();
			$in->flag=$flag;

		}
		
		return $this->send_cmd_new(0x3E16,$userid, $in, null, 0x6fc78827);
	}
	
	/* 设置经验树经验值 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_expbox_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_expbox($userid,$in );
	*/

	function su_mole2_set_expbox($userid , $expbox=null){

		if ( $expbox instanceof su_mole2_set_expbox_in ){
			$in=$expbox;
		}else{
			$in=new su_mole2_set_expbox_in();
			$in->expbox=$expbox;

		}
		
		return $this->send_cmd_new(0x3E17,$userid, $in, null, 0x42561668);
	}
	
	/* 设置昵称 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_nick_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_nick($userid,$in );
	*/

	function su_mole2_set_nick($userid , $nick=null){

		if ( $nick instanceof su_mole2_set_nick_in ){
			$in=$nick;
		}else{
			$in=new su_mole2_set_nick_in();
			$in->nick=$nick;

		}
		
		return $this->send_cmd_new(0x3E18,$userid, $in, null, 0x73dbc887);
	}
	
	/* 设置职业 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_profession_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_profession($userid,$in );
	*/

	function su_mole2_set_profession($userid , $profession=null){

		if ( $profession instanceof su_mole2_set_profession_in ){
			$in=$profession;
		}else{
			$in=new su_mole2_set_profession_in();
			$in->profession=$profession;

		}
		
		return $this->send_cmd_new(0x3E19,$userid, $in, null, 0xf4403d01);
	}
	
	/* 更改星豆数量 */
	/* 调用方式还可以是： 
		$in=new su_mole2_add_xiaomee_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_add_xiaomee($userid,$in );
	*/

	function su_mole2_add_xiaomee($userid , $xiaomee=null){

		if ( $xiaomee instanceof su_mole2_add_xiaomee_in ){
			$in=$xiaomee;
		}else{
			$in=new su_mole2_add_xiaomee_in();
			$in->xiaomee=$xiaomee;

		}
		
		return $this->send_cmd_new(0x3E1A,$userid, $in, null, 0xd7846d09);
	}
	
	/* 设置几项基本属性 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_base_attr_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_base_attr($userid,$in );
	*/

	function su_mole2_set_base_attr($userid , $physique=null, $strength=null, $endurance=null, $quick=null, $intelligence=null, $addition=null){

		if ( $physique instanceof su_mole2_set_base_attr_in ){
			$in=$physique;
		}else{
			$in=new su_mole2_set_base_attr_in();
			$in->physique=$physique;
			$in->strength=$strength;
			$in->endurance=$endurance;
			$in->quick=$quick;
			$in->intelligence=$intelligence;
			$in->addition=$addition;

		}
		
		return $this->send_cmd_new(0x3E1B,$userid, $in, null, 0x8bd35b89);
	}
	
	/* 设置对战前后排 */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_position_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_position($userid,$in );
	*/

	function su_mole2_set_position($userid , $position=null){

		if ( $position instanceof su_mole2_set_position_in ){
			$in=$position;
		}else{
			$in=new su_mole2_set_position_in();
			$in->position=$position;

		}
		
		return $this->send_cmd_new(0x3E1C,$userid, $in, null, 0x4ba9364f);
	}
	
	/* <font color=red>修改人物/宠物属性</font> */
	/* 调用方式还可以是： 
		$in=new su_mole2_set_field_value_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_mole2_set_field_value($userid,$in );
	*/

	function su_mole2_set_field_value($userid , $petid=null, $field=null, $value=null){

		if ( $petid instanceof su_mole2_set_field_value_in ){
			$in=$petid;
		}else{
			$in=new su_mole2_set_field_value_in();
			$in->petid=$petid;
			$in->field=$field;
			$in->value=$value;

		}
		
		return $this->send_cmd_new(0x3E1D,$userid, $in, null, 0xe480f2c5);
	}
	
	/*  */

	function su_mole2_get_vip_info($userid ){

		return $this->send_cmd_new(0x3E1E,$userid, null, new su_mole2_get_vip_info_out(), 0x0e9a0797);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new su_switch_send_msg_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->su_switch_send_msg($userid,$in );
	*/

	function su_switch_send_msg($userid , $msg=null){

		if ( $msg instanceof su_switch_send_msg_in ){
			$in=$msg;
		}else{
			$in=new su_switch_send_msg_in();
			$in->msg=$msg;

		}
		
		return $this->send_cmd_new(0xF234,$userid, $in, null, 0x1fb4369a);
	}
	
	/*  */

	function su_switch_mail_notify($userid ){

		return $this->send_cmd_new(0xF23B,$userid, null, null, 0x676b5912);
	}
	
	/*  */

	function su_switch_kick_player($userid ){

		return $this->send_cmd_new(0xF618,$userid, null, null, 0x0779131b);
	}
	
};
?>
