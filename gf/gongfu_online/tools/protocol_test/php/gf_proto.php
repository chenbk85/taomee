<?php
#changelog
# with the new head struct ,add:gf_park,gf_unpark,gf_send_cmd for purpose of remaining park,unpark,send_cmd function;

require_once("proto_base.php");

class Cgf_proto extends Cproto_base {
	function __construct( $proxyip,$proxyport){
		parent::__construct($proxyip,$proxyport) ;
	}

function gf_park($cmdid,$userid,$role_time,$private_msg){
	global $_SESSION;
	//22：报文头部长度
	$pkg_len=22+strlen($private_msg) ;
	$result=0;
	$proto_id=$this->adminid;
	if ($proto_id==0){
		$proto_id=$_SESSION["adminid" ];
	}
	return pack("L2SL3",$pkg_len,$proto_id,hexdec($cmdid),$result,$userid,$role_time)
		.$private_msg;
}

function gf_park_switch($cmdid,$userid,$recvid,$npc_type,$msg_tm,$private_msg,$msglen){
	global $_SESSION;
	//22：报文头部长度
	$pkg_len=34+$msglen ;
	$result=0;

	$fmt = "L2SL2LLLLa".$msglen;
	return pack($fmt,$pkg_len,0,$cmdid,$result,$userid,$recvid,$npc_type,$msg_tm,$pkg_len-34,$private_msg);
}


function gf_unpark($sockpkg, $private_fmt=""){
	$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid/Lrole_time",$sockpkg);
	if ($private_fmt!="" && $pkg_arr["result"]==0){//成功
		$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid/Lrole_time/".$private_fmt, 
				$sockpkg);
	}
	if ($pkg_arr){
		return $pkg_arr;
	}else{
		return array("result"=>1003 );
	}	
}

function gf_send_cmd($cmdid ,$userid, $role_time, $pri_msg , $out_msg ){
	$sendbuf=$this->gf_park( $cmdid , $userid, $role_time,  $pri_msg );
	return $this->gf_unpark($this->sock->sendmsg($sendbuf),$out_msg );
}

function gf_admin_login($userid,$md5pwd){
	$cmdid='06FF';
	$in_msg='';
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}

function gf_get_user_base_info ($userid ,$role_time){
	$cmdid='060C';
	$in_msg='';
	$out_msg='Lflag/Lregtime/Lvip/LOl_count/LOl_today/LOl_last/LOl_time/Camb_status/Lparentid/Lchild_cnt/Lachieve_cnt';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);	
}

function gf_set_user_base_info ($userid ,$flag,$regtime,$vip,$Ol_count,$Ol_today,$Ol_last,$Ol_time,$amb_status,$parentid,$child_cnt,$achieve_cnt)
{
    $cmdid='060D';
    $in_msg=pack('LLLLLLLCLLL' ,$flag,$regtime,$vip,$Ol_count,$Ol_today,$Ol_last,$Ol_time,$amb_status,$parentid,$child_cnt,$achieve_cnt );
    $out_msg='';
    return  $this->gf_send_cmd($cmdid,$userid,$regtime, $in_msg,$out_msg);
}

function gf_get_user_info ($userid ,$role_time){
		$cmdid='0601';
		$in_msg="";
		$out_msg='Lrole_regtime/Lrole_type/Lflag/Lregtime/a16nick/Lvip/Lskill_point/Lmap_id/Lxpos/Lypos/Llevel/Lexp/Lhp/Lmp/Lxiaomee/Lpvp_honour/Lpvp_fight/Lpvp_win/Lpvp_fail/Lwinning_streak/Lfumo_points/Lfumo_points_today/LOl_count/LOl_today/LOl_last/LOl_time/a40itembind/Lmsg_size/Lused_clothes_count/Lfriend_count/Lblack_count';
		return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}


function gf_get_role_list_ex($userid){
	$cmdid='06E8';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
														}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Lrole_time_$i/Lrole_type_$i/Llevel_$i/Lstatus_$i/a16nick_$i";
			///a16nick_$i/LOl_count_$i/LOl_today_$i/LOl_last_$i/LOl_time_$i";
	//	$recvarr = $this->gf_unpark($recvbuf,$fmt);
	//	$clothes_count = $recvarr['clothes_count_'.$i];
	//	for ($j=0;$j<$clothes_count;$j++)
		{
	//		$fmt=$fmt.'/'."Lattireid_$i_$j/Lgettime_$i_$j";
		}
	}
	return $this->gf_unpark( $recvbuf,$fmt);
}

function gf_get_clothes_list_ex ($userid,$role_time ){
	$cmdid='06E0';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Lattireid_$i/Lid_$i/Lusedflag_$i/Lduration_$i";
	}
	return $this->gf_unpark( $recvbuf,$fmt);	
}

function gf_get_killed_boss ($userid,$role_time ){
	$cmdid='06D8';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Lstageid_$i";
		$fmt=$fmt .'/'. "Cgrade1_$i";
		$fmt=$fmt .'/'. "Cgrade2_$i";
		$fmt=$fmt .'/'. "Cgrade3_$i";
		$fmt=$fmt .'/'. "Cgrade4_$i";
	}
	return $this->gf_unpark( $recvbuf,$fmt);	
}


function gf_del_attire ($userid ,$role_time,$attireid,$gettime){
	$cmdid='070E';
	$in_msg=pack('LL' ,$attireid,$gettime );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}
function gf_attire_set_int_value ($userid ,$role_time,$indexid,$columnname,$value){
	$cmdid='0712';
	$in_msg=pack('La32L' ,$indexid,$columnname,$value );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}
function gf_add_attire ($userid ,$role_time,$attireid,$gettime,$usedflag,$duration){
	$cmdid='070F';
	$in_msg=pack('LLLL' ,$attireid,$gettime,$usedflag,$duration );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}
function gf_add_role($userid,$role_time,$role_type,$nick,$level,$xiaomee)
{
	$cmdid='07E1';
	$in_msg=pack('LLLLa16',$role_type,$level,10,10,$nick );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);

}

function gf_del_role ($userid ,$role_time){
	$cmdid='07F1';
	$in_msg="";//pack('L' ,$role_time );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}


function gf_add_item($userid,$role_time,$itemid,$count)
{
	$cmdid='0756';
	$in_msg=pack('LL',$itemid,$count );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);

}





function gf_set_role_delflg($userid ,$role_time){
	$cmdid='07F0';
	$in_msg="";//pack('L' ,$role_time );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}


function gf_get_friend_list_type($userid,$friend_type)
{
	$cmdid='0635';
	$in_msg=pack('L',$friend_type);
	$sendbuf=$this->gf_park($cmdid,$userid,0,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);

	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
														
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Luserid_$i/Lfriend_type_$i";
											
	}
	return $this->gf_unpark( $recvbuf,$fmt);
}
function gf_add_friend ($userid,$role_time,$friend_id,$friend_type){
	if ($friend_type==1)
		$cmdid='0730';
	else if ($friend_type==2)
		$cmdid='0732';
	else
		return 0;
	$in_msg=pack('L' ,$friend_id );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
								
}

function gf_del_friend ($userid,$role_time,$friend_id,$friend_type){
	if ($friend_type==1)
	{
		$cmdid = '0731';
	}
	else if ($friend_type==2) 
	{
		$cmdid = '0733';
	}
	else
		return 0;
	$in_msg=pack('L' ,$friend_id );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);				
}
function gf_set_nick ($userid,$role_time ,$nick){
	$cmdid='0702';
	$in_msg=pack('a16' ,$nick );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
											
}
function gf_role_set_int_value($userid,$role_time,$column_name,$value ){
	$cmdid='07E5';
	$in_msg=pack('a32L' ,$column_name,$value );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}


function gf_item_set_int_value($userid,$role_time,$itemid,$column_name,$value ){
	$cmdid='0755';
	$in_msg=pack('a32LL' ,$column_name,$itemid,$value );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}



function gf_get_user_item_list ($userid,$role_time ){
	$cmdid='0642';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;

	}
//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Litemid_$i/Lcount_$i";
																
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																					
}

function gf_del_item ($userid, $role_time ,$itemid){

	$cmdid='0757';
	$in_msg=pack('LL' ,$itemid,0xFFFFFFFF );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}

function gf_del_item_by_num ($userid, $role_time ,$itemid, $num){
    $cmdid='0757';
    $in_msg=pack('LL' ,$itemid,$num );
    $out_msg='';
    return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}

function gf_get_task_finished ($userid,$role_time ){
	$cmdid='06D1';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
																		
	}
//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Ltaskid_$i";
																					
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																						
}

function gf_get_task_in_hand ($userid,$role_time ){
	$cmdid='06D2';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
										
	}
		//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Ltaskid_$i"."/a20ser_value_$i/a40as_value_$i";
																								
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																							
}
function gf_get_all_task ($userid,$role_time ){
	$cmdid='06D6';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
										
	}
		//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Ltaskid_$i/Ltask_flg_$i"."/a20ser_value_$i/a40as_value_$i";
																								
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																							
}


function gf_add_killed_boss ($userid ,$role_time,$bossid){
	$cmdid='07D7';
	$in_msg=pack('L' ,$bossid );

	$sendbuf = $this->gf_park($cmdid, $userid, $role_time, $in_msg);
    return $recvbuf=$this->sock->sendmsg_without_returnmsg($sendbuf);
}

function gf_add_task ($userid ,$role_time,$taskid,$flag){
	$cmdid='07D3';
	$in_msg=pack('LL' ,$taskid,$flag );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
											
}
function gf_set_task_flg ($userid,$role_time ,$taskid,$flag){
	$cmdid='07D3';
	$in_msg=pack('LL' ,$taskid,$flag );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
											
}



function gf_get_skill_list ($userid,$role_time ){
	$cmdid='06C3';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
																				
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Lskill_id_$i/Lskill_lv_$i/Lskill_points_$i";
																							
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																							
}


function gf_add_skill ($userid,$role_time ,$skill_id,$skill_lv,$book_id){
	$cmdid='07C0';
	$in_msg=pack('LLL' ,$skill_id,$skill_lv,$book_id );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
											
}

function gf_add_skill_with_no_book ($userid,$role_time ,$skill_id,$skill_lv){
	$cmdid='07C6';
	$in_msg=pack('LL' ,$skill_id,$skill_lv);
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
											
}

function gf_del_skill ($userid ,$role_time,$skill_id){
	$cmdid='07C1';
	$in_msg=pack('L' ,$skill_id );
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}
 

function gf_upgrade_skill ($userid,$role_time ,$skill_id,$skill_lv){
	$cmdid='07C2';
	$in_msg=pack('LLLL' ,$skill_id,$skill_lv,0,0);
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}


function gf_get_skill_bind_key ($userid,$role_time ){
	$cmdid='06C5';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
																					
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
	$fmt=$fmt .'/'. "Lskill_id_$i/Lskill_lv_$i/Lbind_key_$i";
															
	}
	return $this->gf_unpark( $recvbuf,$fmt);
																						
}

function gf_set_skill_bind_key ($userid,$role_time,$skill_id,$bind_key ){

	$cmdid='07C4';
	$in_msg=pack('LLL',1,$skill_id,$bind_key);
	$out_msg='';
	return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
										
}

function gf_get_itembind ($userid,$role_time)
{
	$cmdid="0749";
	$out_msg='';
    $this->gf_send_cmd($cmdid,$userid,$role_time,"",$out_msg);
	$fmt='Lcount';
	$recvarr=$this->gf_unpark($recvbuf,$fmt );
	if ($recvarr && $recvarr['result']!=SUCC){
		return $recvarr;
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt .'/'. "Lfunid_$i/Litemid_$i";
	}
	return $this->gf_unpark( $recvbuf,$fmt);	

}
function gf_get_summon_list ($userid,$role_time)
{
    $cmdid='0620';
    $in_msg='';

    $sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
    $recvbuf=$this->sock->sendmsg($sendbuf);
    $fmt='Lcount';
    $recvarr=$this->gf_unpark($recvbuf,$fmt );
    if ($recvarr && $recvarr['result']!=SUCC){
        return $recvarr;
    }

    //SUCC
    $recv_count=$recvarr['count'];
    for ($i=0;$i<$recv_count;$i++){
        $fmt=$fmt .'/'. "Lmon_tm_$i/Lmon_type_$i/a16mon_nick_$i/Lexp_$i/Llv_$i/Lfight_value_$i/Lstatus_$i";
    }
    return $this->gf_unpark( $recvbuf,$fmt);
}
function gf_set_summon_property ($userid,$role_time,$mon_tm,$lv,$exp,$fight_val)
{
    $cmdid='0725';
    $in_msg=pack('LLLL' ,$mon_tm,$lv,$exp,$fight_val );
    $out_msg='';
    return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}

function gf_get_max_times_chapter ($userid, $role_time)
{
    $cmdid='06DB';
    $out_msg='Lmax_times_chapter';
    return $this->gf_send_cmd($cmdid,$userid,$role_time,"",$out_msg);
}

function gf_set_max_times_chapter ($userid, $role_time, $now_chapter)
{
    $cmdid='07DB';
    $in_msg=pack('L' ,$now_chapter );
    $out_msg='';
    return  $this->gf_send_cmd($cmdid,$userid,$role_time,$in_msg,$out_msg);
}

function gf_get_user_vip ($userid){
    $cmdid='0660';
    $in_msg='';
    $out_msg='Lvip_is/Lvip_month_cnt/Lstart_tm/Lend_tm/Lauto_incr/Lmethod';
    return  $this->gf_send_cmd($cmdid,$userid, 0, $in_msg,$out_msg);
}

function gf_get_invi_code($sendid,$role_time,$num)
{
    $cmdid="0E03";
	$in_msg = pack("L", $num);
    $out_msg='';
	
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
    $recvbuf=$this->sock->sendmsg($sendbuf);
	
    $fmt='Lcount';
    $recvarr=$this->gf_unpark($recvbuf,$fmt );
    if ($recvarr && $recvarr['result']!=SUCC){
       return $recvarr;
								    
	}
   //SUCC
   $recv_count=$recvarr['count'];
   for ($i=0;$i<$recv_count;$i++){
       $fmt=$fmt .'/'. "a16code_$i";
	}
    return $this->gf_unpark( $recvbuf,$fmt);
}

function get_warehouse_item_list($userid ,$role_time)
{
	$cmdid='0625';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);	
	
  $fmt='Lcount';
  $recvarr=$this->gf_unpark($recvbuf,$fmt );
  if ($recvarr && $recvarr['result']!=SUCC){
     return $recvarr;						    
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
	   $fmt=$fmt .'/'. "Litemid_$i/Lcount_$i";
	}
  return $this->gf_unpark( $recvbuf,$fmt);
}

function get_warehouse_clothes_item_list($userid ,$role_time)
{
	$cmdid='0628';
	$in_msg='';
	$sendbuf=$this->gf_park($cmdid,$userid,$role_time,$in_msg);
	$recvbuf=$this->sock->sendmsg($sendbuf);	
	
  $fmt='Lcount';
  $recvarr=$this->gf_unpark($recvbuf,$fmt );
  if ($recvarr && $recvarr['result']!=SUCC){
     return $recvarr;						    
	}
	//SUCC
	$recv_count=$recvarr['count'];
	for ($i=0;$i<$recv_count;$i++){
	   $fmt=$fmt .'/'. "Lid_$i/Lattireid_$i/Lget_time_$i/Lattire_rank_$i/Lduration_$i";
	}
  return $this->gf_unpark( $recvbuf,$fmt);
}

function gf_set_vip_info($uid,$rtm,$vip,$month,$endtm,$auto,$starttm,$method)
{
    $cmdid='0761'
    $in_msg=pack('LLLLLL' ,$vip,$month,$endtm,$auto,$starttm,$method );
    return $this->gf_send_cmd($cmdid,$uid,$rtm,$in_msg,$out_msg);
    //$sendbuf = $this->gf_pack(0761,$uid,$rtm,$vip,$month,$endtm,$auto,$starttm,$method);
}

function gf_post_vipsender_msg($sendid,$recvid,$npc_type,$msg_tm, $msg, $msglen)
{
	$sendbuf = $this->gf_park(0763, $sendid,$recvid,$npc_type,$msg_tm, $msg, $msglen);
	return $recvbuf=$this->sock->sendmsg_without_returnmsg($sendbuf);
}


// for switch server
function gf_post_msg($sendid,$recvid,$npc_type,$msg_tm, $msg, $msglen)
{
	$sendbuf = $this->gf_park_switch(60001, $sendid,$recvid,$npc_type,$msg_tm, $msg, $msglen);
	return $recvbuf=$this->sock->sendmsg_without_returnmsg($sendbuf);
}

}


?>
