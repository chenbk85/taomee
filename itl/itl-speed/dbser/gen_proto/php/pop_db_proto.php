<?php
require_once("proto_base.php");

class item_exchange_t {
	/*  */
	#类型:uint32
	public $itemid;

	/*  */
	#类型:uint32
	public $count;

	/*  */
	#类型:uint32
	public $max_count;


	public function item_exchange_t(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->itemid)) return false;
		if (!$ba->read_uint32($this->count)) return false;
		if (!$ba->read_uint32($this->max_count)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->itemid)) return false;
		if (!$ba->write_uint32($this->count)) return false;
		if (!$ba->write_uint32($this->max_count)) return false;
		return true;
	}

};

	
class pop_exchange_in {
	/*  */
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
		if ($add_item_list_count>9999) return false;
		$this->add_item_list=array();
		{for($i=0; $i<$add_item_list_count;$i++){
			$this->add_item_list[$i]=new item_exchange_t();
			if (!$this->add_item_list[$i]->read_from_buf($ba)) return false;
		}}

		$del_item_list_count=0 ;
		if (!$ba->read_uint32( $del_item_list_count )) return false;
		if ($del_item_list_count>9999) return false;
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
        if ($add_item_list_count>9999 ) return false; 
        $ba->write_uint32($add_item_list_count);
		{for($i=0; $i<$add_item_list_count;$i++){
			if ( ! $this->add_item_list[$i] instanceof item_exchange_t ) return false; 
			if (!$this->add_item_list[$i]->write_to_buf($ba)) return false;
		}}
		$del_item_list_count=count($this->del_item_list);
        if ($del_item_list_count>9999 ) return false; 
        $ba->write_uint32($del_item_list_count);
		{for($i=0; $i<$del_item_list_count;$i++){
			if ( ! $this->del_item_list[$i] instanceof item_exchange_t ) return false; 
			if (!$this->del_item_list[$i]->write_to_buf($ba)) return false;
		}}
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

	
class pop_login_out {
	/*  */
	#类型:uint32
	public $register_time;

	/*  */
	#类型:uint32
	public $sex;

	/*  */
	#类型:uint32
	public $age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public $task_list =array();


	public function pop_login_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->register_time)) return false;
		if (!$ba->read_uint32($this->sex)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;

		$task_list_count=0 ;
		if (!$ba->read_uint32( $task_list_count )) return false;
		if ($task_list_count>9999) return false;
		$this->task_list=array();
		{for($i=0; $i<$task_list_count;$i++){
			$this->task_list[$i]=new task_t();
			if (!$this->task_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->register_time)) return false;
		if (!$ba->write_uint32($this->sex)) return false;
		if (!$ba->write_uint32($this->age)) return false;
		$ba->write_buf($this->nick,16);
		$task_list_count=count($this->task_list);
        if ($task_list_count>9999 ) return false; 
        $ba->write_uint32($task_list_count);
		{for($i=0; $i<$task_list_count;$i++){
			if ( ! $this->task_list[$i] instanceof task_t ) return false; 
			if (!$this->task_list[$i]->write_to_buf($ba)) return false;
		}}
		return true;
	}

};

	
class pop_reg_in {
	/*  */
	#类型:uint32
	public $sex;

	/*  */
	#类型:uint32
	public $age;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $nick ;

	/*  */
	#类型:uint32
	public $item_configid;


	public function pop_reg_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->sex)) return false;
		if (!$ba->read_uint32($this->age)) return false;
		if(!$ba->read_buf($this->nick,16)) return false;
		if (!$ba->read_uint32($this->item_configid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->sex)) return false;
		if (!$ba->write_uint32($this->age)) return false;
		$ba->write_buf($this->nick,16);
		if (!$ba->write_uint32($this->item_configid)) return false;
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

	
class pop_task_get_list_out {
	/**/
	#变长数组,最大长度:9999, 类型:task_t
	public $task_list =array();


	public function pop_task_get_list_out(){

	}

	public function read_from_buf($ba ){

		$task_list_count=0 ;
		if (!$ba->read_uint32( $task_list_count )) return false;
		if ($task_list_count>9999) return false;
		$this->task_list=array();
		{for($i=0; $i<$task_list_count;$i++){
			$this->task_list[$i]=new task_t();
			if (!$this->task_list[$i]->read_from_buf($ba)) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		$task_list_count=count($this->task_list);
        if ($task_list_count>9999 ) return false; 
        $ba->write_uint32($task_list_count);
		{for($i=0; $i<$task_list_count;$i++){
			if ( ! $this->task_list[$i] instanceof task_t ) return false; 
			if (!$this->task_list[$i]->write_to_buf($ba)) return false;
		}}
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

	/* 登入 */

	function pop_login($userid ){

		return $this->send_cmd_new(0x7401,$userid, null, new pop_login_out(), 0x45b4c3d2);
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

	function pop_reg($userid , $sex=null, $age=null, $nick=null, $item_configid=null){

		if ( $sex instanceof pop_reg_in ){
			$in=$sex;
		}else{
			$in=new pop_reg_in();
			$in->sex=$sex;
			$in->age=$age;
			$in->nick=$nick;
			$in->item_configid=$item_configid;

		}
		
		return $this->send_cmd_new(0x7402,$userid, $in, null, 0xe0fcffe6);
	}
	
	/* 交换物品 */
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
		
		return $this->send_cmd_new(0x740A,$userid, $in, null, 0x97036323);
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
	
	/* 得到完成任务的进度列表 */

	function pop_task_get_list($userid ){

		return $this->send_cmd_new(0x7411,$userid, null, new pop_task_get_list_out(), 0x8029753a);
	}
	
};
?>
