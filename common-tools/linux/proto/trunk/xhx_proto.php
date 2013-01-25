<?php
require_once("proto_base.php");

class Cxhx_proto extends Cproto_base {
	function __construct( $proxyip,$proxyport){
		parent::__construct($proxyip,$proxyport) ;
	}

	function xhx_reg($userid,$birthday,$nationality,$nick,$skin,
			$face,$eye,$eyebrow,$lip,$count,$n_1){
		$pri_msg=pack("LLa16LLLLLLL",$birthday,$nationality,$nick,$skin,
				$face,$eye,$eyebrow,$lip,$count,$n_1);
		$sendbuf=$this->park("0300",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get($userid){
		$sendbuf=$this->park("02F8",$userid,$pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get_all($userid)
	{
		$sendbuf=$this->park("0205", $userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt_msg = "Lcount_msg";
		$fmt_sys = "Lcount_sys";

		$all = "Lregtime/Lbirthday/Lnationality/Llove/Lbeautiful/Lwisdom/Lplant_grade/Lspirit_grade/Lol_count/Lol_time/Lol_last/Lol_today".
			"/a16nick/Lskin/Lface/Leye/Leyebrow/Llip/Lvip_flag/Lvip_month/Lvip_begin_time/Lvip_end_time/Lvip_auto_charge/".
			"Llast_charge_chnl_id/Lwonderful_val/Lfriend_add/Llike_medal/Lnew_task/Lcard_exp/Lmoney/Lwonderful_val_extra/Lpower/Lparty_grade/Lcount_msg/Lcount_sys";
		return $this->unpark($recvbuf,$all);
	}

	function xhx_get_basic_info($userid)
	{
		$sendbuf=$this->park("020D", $userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$all = "Lregtime/Lbirthday/Lnationality/Llove/Lbeautiful/Lwisdom/Lplant_grade/Lspirit_grade/Lol_count/Lol_time/Lol_last/Lol_today".
			"/a16nick/Lskin/Lface/Leye/Leyebrow/Llip/Lvip_flag/Lvip_month/Lvip_begin_time/Lvip_end_time/Lvip_auto_charge/".
			"Llast_charge_chnl_id/Lwonderful_val/Lfriend_add/Llike_medal/Lnew_task/Lcard_exp/Lmoney/Lwonderful_val_extra/Lpower/Lparty_grade/Lcount_msg/Lcount_sys";
		return $this->unpark($recvbuf,$all);
	}

	function xhx_get_bus_info($userid)
	{
		$sendbuf=$this->park("022D",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lgrade/a16nick";
		return $this->unpark($recvbuf,$fmt);
	}
	
	function xhx_get_nationality($userid)
	{
		$sendbuf=$this->park("022E",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lnationality";
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_get_vip_flag($userid)
	{

		$sendbuf=$this->park("02F5",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lvip_flag";
		return $this->unpark($recvbuf,$fmt);

	}

	function xhx_home_get_all ($userid ){
		$sendbuf=$this->park("02F7", $userid, $pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);

		$fmt = "a16nick/Lnationality/Lgarden_glamour/Lplant_grade/Lvisit_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}

		$visit_count = $recvarr["visit_count"];
		for ($i=0;$i<$visit_count;$i++){
			$fmt = $fmt ."/Lvisit_id_$i/Lvisit_time_$i/Lvisit_opt_$i/a16visit_nick_$i";
		}

		$fmt =$fmt ."/Ltotal_present_flower/Lpresent_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		$present_count=$recvarr["present_count"];
		for ($i=0;$i<$present_count;$i++){
			$fmt = $fmt ."/Lpresent_id_$i/Lpresent_time_$i";
		}
/*
		$fmt =$fmt ."/Lsend_cnt/Ltotal_recv_cnt/Lrecv_cnt";
		$recvarr=$this->unpark($recvbuf,$fmt);
		$recv_count=$recvarr["recv_cnt"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt = $fmt ."/Lsend_id_$i/Lsend_time_$i/a16magic_nick_$i";
		}
*/
		$fmt =$fmt ."/Lplant_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		$plant_count=$recvarr["plant_count"];
		for ($i=0;$i<$plant_count;$i++){
			$fmt=$fmt . "/Lget_time_$i/Lplantpos_$i/Lgardenerid_$i/Lplantid_$i/Lplanttime_$i/Lplantstate_$i/Lplantval_$i/Lmoisture_$i/Lnutrition_$i/Lseedoutput_$i/Lfloweroutput_$i/Lpollenoutput_$i/Lplantquality_$i/Ltoolstate_$i/Ltool_left_time_$i/Lpotid_$i/Lpoll_type_$i/Lmoisture_time_$i/Lpicker_count_$i/a40pick_id_$i";
		}
		$fmt = $fmt ."/Ltool_house_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		$tool_house_count=$recvarr["tool_house_count"];
		for ($i=0;$i<$tool_house_count;$i++){
			$fmt=$fmt . "/Lhouse_id_$i/Lpos_$i/Lopen_$i";
		}
		$fmt = $fmt ."/Lgar_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		$gar_count=$recvarr["gar_count"];
		for ($i=0;$i<$gar_count;$i++){
			$fmt=$fmt . "/Lgarden_type_$i/Lgarden_flag_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_update_online_info($userid,$time)
	{
		$pri_msg=pack("L",$time);
		$sendbuf=$this->park("0306", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_info($userid,$love,$beautiful,$wisdom,$plant_grade)
	{
		$pri_msg=pack("LLLL", $love,$beautiful,$wisdom,$plant_grade);
		$sendbuf=$this->park("0301", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_new_task($userid,$new_task)
	{
		$pri_msg=pack("L",$new_task);
		$sendbuf=$this->park("0319",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_card_exp($userid,$card_exp)
	{
		$pri_msg=pack("l",$card_exp);
		$sendbuf=$this->park("0314",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_seal_account($userid,$seal_flag,$seal_reason)
	{
		$pri_msg=pack("LL",$seal_flag,$seal_reason);
		$sendbuf=$this->park("0321",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_body($userid,$skin,$face,$eye,$eyebrow,$lip)
	{
		$pri_msg=pack("LLLLL",$skin,$face,$eye,$eyebrow,$lip);
		$sendbuf=$this->park("0302",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get_body($userid)
	{
		$sendbuf=$this->park("0204",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "a16nick/Lskin/Lface/Leye/Leyebrow/Llip/Lregtime/Lbirthday/Llove/Lbeautiful/Lwisdom/Lplant_grade/Lspirit_grade/Lmoney/Lnationality/Lvip_flag/Lvip_month/Lvip_begin_time/Lvip_end_time/Lvip_auto_charge/Llast_charge_chnl_id/Lwonderful_val/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){

			$fmt=$fmt .'/'. "Lattireid_$i";

		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_update_nick($userid,$nick)
	{
		$pri_msg=pack("a16",$nick);
		$sendbuf=$this->park("0303",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function xhx_update_money($userid,$money)
	{

		$pri_msg=pack("L",$money);
		$sendbuf=$this->park("0324",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_update_old_last($userid,$old_last)
	{
		$pri_msg=pack("L",$old_last);
		$sendbuf=$this->park("030C",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_ol_today($userid,$ol_today)
	{
		$pri_msg=pack("L",$ol_today);
		$sendbuf=$this->park("030E",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_nationality($userid,$nationality)
	{
		$pri_msg=pack("L",$nationality);
		$sendbuf=$this->park("030A",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_garden_glamour($userid,$garden_glamour)
	{
		$pri_msg=pack("L",$garden_glamour);
		$sendbuf=$this->park("031D",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_add_offline_msg($userid,$len,$friend_id,$msg)
	{
		$pri_msg=pack("LLa160",$len,$friend_id,$msg);
		print_r($msg);
		print_r($pri_msg);
		$sendbuf=$this->park("0308",$userid,$pri_msg);
		print_r($sendbuf);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get_all_gar($userid)
	{
		$sendbuf=$this->park("0217",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "a16nick/Lnationality/Lgarden_glamour/Lplant_grade/Lvisit_count";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["visit_count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lvisit_id_$i/Lvisit_time_$i/Lvisit_opt_$i/a16visit_nick_$i";
			}

			$fmt =$fmt ."/Ltotal_present_flower/Lpresent_count";
			$recvarr=$this->unpark($recvbuf,$fmt);
			$present_count=$recvarr["present_count"];
			for ($i=0;$i<$present_count;$i++){
				$fmt = $fmt ."/Lpresent_id_$i/Lpresent_time_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	//xhx_skill
	function	xhx_skill_update_all($userid,$id,$point,$exe_num,$score,$max_score){
		$pri_msg=pack("LLLLL",$id,$point,$exe_num,$score,$max_score);
		$sendbuf=$this->park("03B6", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function	xhx_skill_del($userid,$skill_id){
		$pri_msg=pack("L",$skill_id);
		$sendbuf=$this->park("03B8", $userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_skill_get_all($userid,$b,$c ){
		$pri_msg=pack("LL",$b,$c);
		$sendbuf=$this->park("2B7",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lid_$i/Lpoint_$i/Lexe_num_$i/Lscore_$i/Lmax_score_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	//xhx_medal
	function xhx_medal_add($userid,$medal_id,$time,$flag)
	{
		$pri_msg=pack("LLL",$medal_id,$time,$flag);
		$sendbuf=$this->park("030F",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_medal_update($userid,$medal_id,$time,$flag)
	{
		$pri_msg=pack("LLL",$medal_id,$time,$flag);
		$sendbuf=$this->park("0310",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_medal_get_all($userid)
	{
		$sendbuf=$this->park("0211",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lmedal_$i/Lget_time_$i/Lflag_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_medal_del($userid,$medal){
		$pri_msg=pack("L",$medal);
		$sendbuf=$this->park("0312",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_gar
	function xhx_gar_del($userid,$garden_type)
	{
		$pri_msg=pack("L",$garden_type);
		$sendbuf=$this->park("021C",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_gar_update($userid,$garden_type,$use)
	{
	    $pri_msg=pack("LL",$garden_type,$use);
		$sendbuf=$this->park("031A",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_gar_get_all($userid)
	{
		$sendbuf=$this->park("021B",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lgarden_type_$i/Luse_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}

	}
	function xhx_gar_add($userid,$garden_type,$use)
	{
		$pri_msg=pack("LL",$garden_type,$use);
		$sendbuf=$this->park("032F",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_attire_add($userid,$attireid)
	{
		$pri_msg=pack("L",$attireid);
		$sendbuf=$this->park("330",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_attire_del_ex($userid){
		$sendbuf=$this->park("0336",$userid, $pri_msg );
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_attire_del($userid,$attireid)
	{
		$pri_msg=pack("L",$attireid);
		$sendbuf=$this->park("331",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_attire_get_all($userid,$b,$c ){
		$pri_msg=pack("LL",$b,$c);
		$sendbuf=$this->park("232",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lattireid_$i/Lattirestate_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_attire_update_state($userid,$attireid,$attirestate)
	{
		$pri_msg=pack("LL",$attireid,$attirestate);
		$sendbuf=$this->park("333",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_attire_update_flag($userid,$attireid,$flag)
	{
		$pri_msg=pack("LL",$attireid,$flag);
		$sendbuf=$this->park("0334",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_pet
	function xhx_pet_add($userid,$petid,$nick)
	{
		$pri_msg=pack("La16",$petid,$nick);
		$sendbuf=$this->park("0390",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function xhx_pet_del($userid,$petid)
	{
		$pri_msg=pack("L",$petid);
		$sendbuf=$this->park("039a",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_get_all($userid){
		$sendbuf=$this->park("0291",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lvip_flag/Lvip_begin_time/Lwonderful_val/Lwonderful_val_extra/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Lis_wonderful_$i/Lget_time_$i/a16nick_$i/Lhungry_$i/Lclean_$i/Lmood_$i/Lclose_$i/Lhealth_$i/Lstate_$i/Labnormal_time_$i/Lcare_day_$i/Lclose_changetime_$i/Lclose_max_$i/Lhouse_getid_$i/Lrandom_v_$i/Lgrow_time_$i/Lmail_add_close_time_$i/Lmail_generate_time_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	

	function xhx_pet_update_nick($userid,$petid,$nick)
	{
		$pri_msg=pack("La16",$petid,$nick);
		$sendbuf=$this->park("0392",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_update_info($userid,$petid,$hungry,$clean,$mood)
	{
		$pri_msg=pack("LLLL",$petid,$hungry,$clean,$mood);
		$sendbuf=$this->park("0393",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_update_all ($userid ,$petid,$nick,$hungry,$clean,
			$mood,$close,$health,$state,$abnormal_time,$care_day,
			$close_changetime,$close_max,$house_getid,$grow_time)
	{
		$pri_msg=pack('La16LLLLLLLLLLLL' ,$petid,$nick,$hungry,$clean,$mood,
				$close,$health,$state,$abnormal_time,$care_day,
				$close_changetime,$close_max,$house_getid,$grow_time);
		$sendbuf=$this->park("0393",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_update_value ($userid,$petid,$hungry,$clean,
			$mood,$close,$health,$state,$abnormal_time,$care_day,
			$close_changetime,$close_max,$house_getid,$grow_time,$is_wonderful,$energy,$mail_generate_time)
	{
		$pri_msg=pack('LLLLLLLLLLLLLLLL',$petid,$hungry,$clean,$mood,
				$close,$health,$state,$abnormal_time,$care_day, $close_changetime,$close_max,
				$house_getid,$grow_time,0,$is_wonderful,$mail_generate_time);
		$sendbuf=$this->park("039C",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_update_close($userid,$petid,$change_close)
	{
		$pri_msg=pack("Ll",$petid,$change_close);
		$sendbuf=$this->park("0394",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_update_health($userid,$petid,$health)
	{
		$pri_msg=pack("LL",$petid,$health);
		$sendbuf=$this->park("0395",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_goto_orphanage($userid,$petid)
	{
		$pri_msg=pack("L",$petid);
		$sendbuf=$this->park("0396",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_leave_orphanage($userid,$petid)
	{
		$pri_msg=pack("L",$petid);
		$sendbuf=$this->park("0397",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_care_pet($userid,$petid)
	{
		$pri_msg=pack("L",$petid);
		$sendbuf=$this->park("0398",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_and_petmail_get_all($userid){
		$sendbuf=$this->park("029B",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lvip_flag/Lvip_begin_time/Lwonderful_val/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpetid_$i/Lis_wonderful_$i/Lget_time_$i/a16nick_$i/Lhungry_$i/Lclean_$i/Lmood_$i/Lclose_$i/Lhealth_$i/Lstate_$i/Labnormal_time_$i/Lcare_day_$i/Lclose_changetime_$i/Lclose_max_$i/Lhouse_getid_$i/Lrandom_v_$i/Lgrow_time_$i/Lmail_add_close_time_$i/Lmail_generate_time_$i";
		}
		$fmt=$fmt."/Lpetmail_count";
		$recvarr=$this->unpark($recvbuf,$fmt);		
		$petmail_count=$recvarr["petmail_count"];
		for ($i=0;$i<$petmail_count;$i++){
			$fmt=$fmt."/Lpetmail_id_$i/Lpet_send_time_$i/Lmail_type_$i/Cpetmail_state_$i/Lreturn_mail_time_$i/Lreturn_mail_len_$i";
			$recvarr=$this->unpark($recvbuf,$fmt);		
			$mail_len=$recvarr["return_mail_len_$i"];
			$fmt=$fmt."/a$mail_len"."return_mail_$i";
		}

		return $this->unpark($recvbuf,$fmt );
	}

//xhx_friend
	function xhx_friend_add($userid,$friend_id,$friend_type,$friend_remark,$skin,$face,$eye,$eyebrow,$lip,$hair)
	{
		$pri_msg=pack("LLa16LLLLLL",$friend_id,$friend_type,$friend_remark,$skin,$face,$eye,$eyebrow,$lip,$hair);
		$sendbuf=$this->park("3E0",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_friend_get_all($userid,$begin,$count ){
		$pri_msg=pack("LL",$begin,$count);
		$sendbuf=$this->park("2E2",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lfriend_id_$i/a16remarks_$i/Lfriend_type_$i/Lskin_$i/Lface_$i/Leye_$i/Leyebrow_$i/Llip_$i/Lhair_$i";
		}
		
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_friend_del($userid,$friend_id)
	{
		$pri_msg=pack("L",$friend_id);
		$sendbuf=$this->park("3E1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_friend_update_remark($userid,$friend_id,$nick)
	{
		$pri_msg=pack("La16",$friend_id,$nick);
		$sendbuf=$this->park("03E3",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_friend_update_type($userid,$friend_id,$type)
	{
		$pri_msg=pack("LL",$friend_id,$type);
		$sendbuf=$this->park("03E4",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_friend_update_friend_info($userid,$friend_id,$friend_nick,$skin,$face,$eye,$eyebrow,$lip,$hair)
	{
		$pri_msg=pack("La16LLLLLL",$friend_id,$friend_nick,$skin,$face,$eye,$eyebrow,$lip,$hair);
		$sendbuf=$this->park("3E5",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_tool_add($userid,$toolid,$num,$max)
	{
		$pri_msg=pack("LLL",$toolid,$num,$max);
		$sendbuf=$this->park("380",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_tool_reduce($userid,$toolid,$num)
	{
		$pri_msg=pack("LL",$toolid,$num);
		$sendbuf=$this->park("381",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_tool_get_all($userid,$b,$e)
	{
		$pri_msg=pack("LL",$b,$e);
		$sendbuf=$this->park("0283",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltoolid_$i/Lnumber_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_tool_get_material($userid,$b,$e)
	{
		$pri_msg=pack("LL",$b,$e);
		$sendbuf=$this->park("0286",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltoolid_$i/Lnumber_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_tool_get_pet($userid,$b,$e)
	{
		$pri_msg=pack("LLL",800001,$b,$e);
		$sendbuf=$this->park("0287",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltoolid_$i/Lnumber_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_tool_get_furniture($userid,$b,$e)
	{
		$pri_msg=pack("LLL",500001,$b,$e);
		$sendbuf=$this->park("0287",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltoolid_$i/Lnumber_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	//xhx_task_day
	function xhx_task_day_get_list($userid)
	{
		$sendbuf=$this->park("0423",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_task_day_update_all_ex($userid,$count,$taskid1,$step1,$rate1,$time1,
			$taskid2,$step2,$rate2,$time2,$taskid3,$step3,$rate3,$time3)
	{
		$pri_msg=pack("LLLLLLLLLLLLL",$count,$taskid1,$step1,$rate1,$time1,
			$taskid2,$step2,$rate2,$time2,$taskid3,$step3,$rate3,$time3);
		$sendbuf=$this->park("0532",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_day_update_all($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0521",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_day_add($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("052F",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_day_del($userid,$taskid)
	{
		$pri_msg=pack("L",$taskid);
		$sendbuf=$this->park("0520",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_home_task_day
	function xhx_home_task_day_get_list($userid)
	{
		$sendbuf=$this->park("0458",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_home_task_day_update_all_ex($userid,$count,$taskid1,$step1,$rate1,$time1,
			$taskid2,$step2,$rate2,$time2,$taskid3,$step3,$rate3,$time3)
	{
		$pri_msg=pack("LLLLLLLLLLLLL",$count,$taskid1,$step1,$rate1,$time1,
			$taskid2,$step2,$rate2,$time2,$taskid3,$step3,$rate3,$time3);
		$sendbuf=$this->park("055A",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_home_task_day_update_all($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0556",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_home_task_day_add($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0554",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_home_task_day_del($userid,$taskid)
	{
		$pri_msg=pack("L",$taskid);
		$sendbuf=$this->park("0555",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_task_week
	function xhx_task_week_get_list($userid)
	{
		$sendbuf=$this->park("0428",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_task_week_update_all($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0526",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_week_add($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0530",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_task_week_del($userid,$taskid)
	{
		$pri_msg=pack("L",$taskid);
		$sendbuf=$this->park("0525",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_task_month
	function xhx_task_month_get_list($userid)
	{
		$sendbuf=$this->park("042d",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_task_month_update_all($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("052b",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_month_add($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0531",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_month_del($userid,$taskid)
	{
		$pri_msg=pack("L",$taskid);
		$sendbuf=$this->park("052a",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_task
	function xhx_task_get_list($userid)
	{
		$sendbuf=$this->park("02A5",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_task_update_all($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("3A3",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_add($userid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLL",$taskid,$step,$rate,$time);
		$sendbuf=$this->park("3A1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}

	function xhx_task_del($userid,$taskid)
	{
		$pri_msg=pack("L",$taskid);
		$sendbuf=$this->park("3A2",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_task_su_get_list($userid)
	{
		$sendbuf=$this->park("02AD",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_task_su_loop_get_list($userid)
	{
		$sendbuf=$this->park("0433",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt = $fmt ."/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	//xhx_tool_house
	function xhx_tool_house_get_all($userid,$begin,$count)
	{
		$pri_msg=pack("LL",$begin,$count);
		$sendbuf=$this->park("2B4",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);

		$fmt = "Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lhouseid_$i/Lpos_$i/Lopen_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}

	}
	
	function xhx_tool_house_add($userid,$id)
	{
		$pri_msg=pack("L",$id);
		$sendbuf=$this->park("3B0",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_tool_house_del($userid,$id)
	{
		$pri_msg=pack("L",$id);
		$sendbuf=$this->park("3B1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_tool_house_update_all($userid,$id,$pos,$flag)
	{
		$pri_msg=pack("LLL",$id,$pos,$flag);
		$sendbuf=$this->park("3B5",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_petmail
	function xhx_petmail_add($userid,$petid,$pet_send_time,$mail_type)
	{
		$pri_msg=pack("LLL",$petid,$pet_send_time,$mail_type);
		$sendbuf=$this->park("0370",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_petmail_del($userid,$petid,$pet_send_time)
	{
		$pri_msg=pack("LL",$petid,$pet_send_time);
		$sendbuf=$this->park("0371",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_petmail_return_mail($userid,$petid,$pet_send_time,$mail_len,$mail)
	{
		$pri_msg=pack("LLLa$mail_len",$petid,$pet_send_time,$mail_len,$mail);
		$sendbuf=$this->park("0372",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_petmail_update_state($userid,$petid,$pet_send_time,$state)
	{
		$pri_msg=pack("LLL",$petid,$pet_send_time,$state);
		$sendbuf=$this->park("0374",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_petmail_get_list($userid,$begin,$count ){
		$pri_msg=pack("LL",$begin,$count);
		$sendbuf=$this->park("0275",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lpetid_$i/Lpet_send_time_$i/Lmail_type_$i/Lstate_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_petmail_get_list_by_petid($userid,$petid){
		$pri_msg=pack("L",$petid);
		$sendbuf=$this->park("0276",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lpetid/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lpet_send_time_$i/Lmail_type_$i/Lstate_$i/Lreturn_mail_time_$i/Lreturn_mail_len_$i";
			$recvarr=$this->unpark($recvbuf,$fmt);		
			$recv_len=$recvarr["return_mail_len_$i"];
			$fmt=$fmt ."/a".$recv_len."return_mail_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	//xhx_plant
	function xhx_plant_get_all($userid,$b,$c ){
		$pri_msg=pack("LL",$b,$c);
		$sendbuf=$this->park("241",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lget_time_$i/Lplant_pos_$i/Lgardener_id_$i/Lplant_id_$i/Lplant_time_$i/Lplant_state_$i/Lplant_val_$i"
			."/Lmoisture_$i/Lnutrition_$i/Lseed_output_$i/Lflower_output_$i/Lpollen_output_$i/Lplant_quality_$i/Ltool_state_$i"
			."/Ltool_left_time_$i/Lgrass_time_$i/Lpotid_$i/Lpoll_type_$i/Lmoisture_time_$i/Cill_flag_$i/Cchangecolor_use_$i/Lpicker_count_$i/a40pick_id_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_plant_del($userid,$get_time)
	{
		$pri_msg=pack("L",$get_time);
		$sendbuf=$this->park("34B",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//最后两个1字节表示ill_flag标记和changecolor_use,都置为0
	function xhx_plant_update_all_ex($userid,$get_time,$plantpos,$gardenerid,
			$plantid,$planttime,$plantstate,$plantval,$moisture,$nutrition,
			$seedoutput,$floweroutput,$pollenoutput,$plantquality,
			$toolstate,$tool_left_time,$grass_time,$potid,$poll_type)
	{
		$pri_msg=pack("LLLLLLLLLLLLLLLLLLCC",$get_time,$plantpos,$gardenerid,
			$plantid,$planttime,$plantstate,$plantval,$moisture,$nutrition,
			$seedoutput,$floweroutput,$pollenoutput,$plantquality,
			$toolstate,$tool_left_time,$grass_time,$potid,$poll_type,0,0);
		$sendbuf=$this->park("352",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_plant_update_all($userid,$get_time,$plantpos,$gardenerid,
			$plantid,$planttime,$plantstate,$plantval,$moisture,$nutrition,
			$seedoutput,$floweroutput,$pollenoutput,$plantquality,
			$toolstate,$tool_left_time,$grass_time,$potid,$poll_type,$m_add_time,$poll_picker_list)
	{
		$pri_msg=pack("LLLLLLLLLLLLLLLLLLLa44",1,$get_time,$plantpos,$gardenerid,
			$plantid,$planttime,$plantstate,$plantval,$moisture,$nutrition,
			$seedoutput,$floweroutput,$pollenoutput,$plantquality,
			$toolstate,$tool_left_time,$grass_time,$potid,$poll_type,$m_add_time,$poll_picker_list);
		$sendbuf=$this->park("351",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_furniture_get_all
	function xhx_furniture_get_all($userid,$b,$c ){
		$pri_msg=pack("LL",$b,$c);
		$sendbuf=$this->park("2D2",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lallcount/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
		$fmt=$fmt . "/Lget_time_$i/Lfurniture_id_$i/Lpos_$i/Langle_$i/Lsize_$i/Lhouse_getid_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}

	function xhx_furniture_add($userid,$f_id,$max)
	{
		$pri_msg=pack("LL",$f_id,$max);
		$sendbuf=$this->park("3D0",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lget_time";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		return $this->unpark($recvbuf,$fmt );
		//return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_furniture_del($userid,$get_time)
	{
		$pri_msg=pack("L",$get_time);
		$sendbuf=$this->park("3d1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_furniture_update_all ($userid ,$get_time,$furniture_id,$pos,$angle,$size,$house_getid){
		$pri_msg=pack('LLLLLL',$get_time,$furniture_id,$pos,$angle,$size,$house_getid );
		$sendbuf=$this->park("3d3",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_furniture_del_id($userid,$furniture_id)
	{
		$pri_msg=pack("L",$furniture_id);
		$sendbuf=$this->park("3d4",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lget_time";
		return $this->unpark($recvbuf,$fmt );
	}

	//xhx_invi
	function xhx_invi_add($code)
	{
		$pri_msg=pack("a6",$code);
		$sendbuf=$this->park("3FA",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_invi_update($code,$flag)
	{
		$pri_msg=pack("a6L",$code,$flag);
		$sendbuf=$this->park("3FB",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_invi_get_info($code)
	{
		$pri_msg=pack("a6",$code);
		$sendbuf=$this->park("02FC", $userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$all = "Lflag";
		return $this->unpark($recvbuf,$all);
	}

	function xhx_invi_get_codes($userid)
	{
		$sendbuf=$this->park("5F9",$userid,'');
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "a6code_$i/Lflag_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	//xhx_pet_task
	function xhx_pet_task_get_list($userid)
	{
		$sendbuf=$this->park("02AB",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lpet_id_$i/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}

	function xhx_pet_task_update($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("03AA",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_add($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("03A7",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_del($userid,$petid,$taskid,$step)
	{
		$pri_msg=pack("LLL",$petid,$taskid,$step);
		$sendbuf=$this->park("03A9",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_pet_task_day
	function xhx_pet_task_day_get_list($userid)
	{
		$sendbuf=$this->park("0440",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lpet_id_$i/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}
	function xhx_pet_task_day_update($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0539",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_day_add($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0536",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_day_del($userid,$petid,$taskid,$step)
	{
		$pri_msg=pack("LLL",$petid,$taskid,$step);
		$sendbuf=$this->park("0538",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_pet_task_week
	function xhx_pet_task_week_get_list($userid)
	{
		$sendbuf=$this->park("0445",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lpet_id_$i/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}
	function xhx_pet_task_week_update($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0544",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_week_add($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0541",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_week_del($userid,$petid,$taskid,$step)
	{
		$pri_msg=pack("LLL",$petid,$taskid,$step);
		$sendbuf=$this->park("0543",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_pet_task_month
	function xhx_pet_task_month_get_list($userid)
	{
		$sendbuf=$this->park("0450",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		else
		{
			$recv_count = $recvarr["count"];
			for($i=0;$i<$recv_count;$i++)
			{
				$fmt = $fmt ."/Lpet_id_$i/Ltask_id_$i/Lstep_$i/Lrate_$i/Ltime_$i";
			}
			return $this->unpark($recvbuf,$fmt);
		}
	}
	function xhx_pet_task_month_update($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0549",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_month_add($userid,$petid,$taskid,$step,$rate,$time)
	{
		$pri_msg=pack("LLLLL",$petid,$taskid,$step,$rate,$time);
		$sendbuf=$this->park("0546",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");

	}
	function xhx_pet_task_month_del($userid,$petid,$taskid,$step)
	{
		$pri_msg=pack("LLL",$petid,$taskid,$step);
		$sendbuf=$this->park("0548",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	//xhx_msg_board	
	function xhx_msg_board_add($userid,$send_id,$nick,$msg)
	{
		$msg_len=strlen($msg);
		$format="La16La".$msg_len;
		$pri_msg=pack($format,$send_id,$nick,$msg_len,$msg);
		$sendbuf=$this->park("0361",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_msg_board_del($userid,$msg_id)
	{
		$pri_msg=pack("LL",$msg_id,0);
		$sendbuf=$this->park("0362",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_msg_board_update_flag($userid,$msg_id,$flag)
	{
		$pri_msg=pack("LL",$msg_id,$flag);
		$sendbuf=$this->park("0364",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_msg_board_get($userid ,$begin,$count,$send_id)
	{
		$in_msg=pack('LLL' ,$begin,$count,$send_id );
		$sendbuf=$this->park("0265",$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lall_count/Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt ."/Lmsg_id_$i/Lsend_id_$i/Ltime_$i/a16nick_$i/Lflag_$i/Lmsg_len_$i";
			$recvarr=$this->unpark($recvbuf,$fmt);
			$fmt=$fmt."/a".$recvarr["msg_len_$i"]."msg_$i/Lreply_len_$i";
			$recvarr=$this->unpark($recvbuf,$fmt);
			$fmt=$fmt."/a".$recvarr["reply_len_$i"]."reply_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}

	function xhx_func_tool_add($userid,$toolid,$pos)
	{
		$pri_msg=pack('LL',$toolid,$pos);
		$sendbuf=$this->park("03C0",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_func_tool_del($userid,$toolid)
	{
		$pri_msg=pack('L',$toolid);
		$sendbuf=$this->park("03C1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_func_tool_update($userid,$toolid,$pos)
	{
		$pri_msg=pack('LL',$toolid,$pos);
		$sendbuf=$this->park("03C2",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_func_tool_get_all($userid)
	{
		$sendbuf=$this->park("02C3",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt = $fmt ."/Ltoolid_$i/Lpos_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	//xhx_mail
	function xhx_mail_get_all($userid)
	{
		$sendbuf=$this->park("0254",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lmail_id_$i/Lflag_$i/Lmail_type_$i/Ltime_$i/Lfrom_$i/a16nick_$i/Lstyle_$i/Lfont_color_$i/Lattach_$i/Lnum_$i/Ltopic_len_$i/Lcontent_len_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_mail_add ($userid,$from,$nick,$style,$font_color,$topic,$content)
	{
		$topic_len=strlen($topic);
		$content_len=strlen($content);

		$format="La16LLLa".$topic_len ."La".$content_len;
		$pri_msg=pack($format,$from,$nick,$style,$font_color,$topic_len,$topic,$content_len,$content );
		$sendbuf=$this->park("0355",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_mail_del ($userid,$mail_id)
	{
		$pri_msg=pack('L',$mail_id);
		$sendbuf=$this->park("0356",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_mail_get($userid,$begin,$count)
	{
		$pri_msg=pack('LL',$begin,$count);
		$sendbuf=$this->park("0258",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lall_count/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lmail_id_$i/Lflag_$i/Lmail_type_$i/Ltime_$i/Lfrom_$i/a16nick_$i/Lattach_$i/Lnum_$i/Ltopic_len_$i";
			$recvarr=$this->unpark($recvbuf,$fmt);
			$topic_len=$recvarr["topic_len_$i"];
			$fmt=$fmt ."/a".$topic_len."topic_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_mail_get_by_id ($userid,$mail_id)
	{
		$pri_msg=pack('L',$mail_id);
		$sendbuf=$this->park("0259",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt="Lstyle/Lfont_color/Lcontent_len";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$content_len = $recvarr["content_len"];
		$fmt=$fmt ."/a".$content_len."content";
		return $this->unpark($recvbuf,$fmt);
	}

	// xhx_multi_add
	function xhx_multi_add($userid,$item_type,$item_id,$number,$max_count)
	{
		$pri_msg=pack('LLLL' ,$item_type,$item_id,$number,$max_count );
		$sendbuf=$this->park("03F6",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	// xhx_share_plant
	function xhx_share_plant_get_all($userid)
	{
		$sendbuf=$this->park("0243",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Ldest_id_$i/Lnumber_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_share_plant_add($userid,$dest_id,$number)
	{
		$pri_msg=pack('LL' ,$dest_id,$number);
		$sendbuf=$this->park("0344",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_share_plant_del($userid,$dest_id,$number)
	{
		$pri_msg=pack('LL' ,$dest_id,$number);
		$sendbuf=$this->park("0345",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_chat_across_svr($userid,$npc_id,$content)
	{
		$content_len=strlen($content);
		$format='LLa'.$content_len;
		$pri_msg=pack($format,$npc_id,$content_len,$content);
		$sendbuf=$this->park("F237",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function user_get_userid_by_email($email){
		$pri_msg=pack("A64", $email );
		$sendbuf=$this->park(4004,0, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"Luserid");
	}

	function xhx_update_vip_info($userid,$vip_flag,$vip_month,$vip_end_time,$vip_auto_charge,
		$vip_begin_time,$last_charge_chnl_id)
	{
		$pri_msg=pack('LLLLLL',$vip_flag,$vip_month,$vip_end_time,$vip_auto_charge,$vip_begin_time,$last_charge_chnl_id);
		$sendbuf=$this->park("0307",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_update_wonderful($userid,$wonderful_val)
	{
		$pri_msg=pack('L',$wonderful_val);
		$sendbuf=$this->park("03FF",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get_vip_info($userid)
	{
		$sendbuf=$this->park("02FE", $userid, $pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$all = "Lvip_flag/Lvip_month/Lvip_begin_time/Lvip_end_time/Lvip_auto_charge/Llast_charge_chnl_id";
		return $this->unpark($recvbuf,$all);
	}

	//xhx_pet_skill
	function xhx_pet_skill_add($userid,$petid,$skill_id)
	{
		$pri_msg=pack("LL",$petid,$skill_id);
		$sendbuf=$this->park("0379",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function xhx_pet_skill_del($userid,$petid,$skill_id)
	{
		$pri_msg=pack("LL",$petid,$skill_id);
		$sendbuf=$this->park("0377",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_pet_skill_get_all($userid)
	{
		$sendbuf=$this->park("0278",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lpet_id_$i/Lskill_id_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_seal_history_get($userid,$beg_time,$end_time)
	{
		$pri_msg=pack("LL",$beg_time,$end_time);
		$sendbuf=$this->park("0222",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lseal_time_$i/Lseal_flag_$i/Lseal_reason_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_get_seal_info($userid)
	{
		$sendbuf=$this->park("0223",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lseal_time/Lseal_flag";
		return $this->unpark($recvbuf,$fmt);
	}

	function kick_user_offline($userid){
		$sendbuf=$this->park("EA69",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_magic_card_get($userid)
	{
		$sendbuf=$this->park("0227",$userid,$pri_msg);
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lcard_key_$i/Lcard_id_$i/Lget_time_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}
	
	function xhx_magic_card_add($userid ,$key,$card_id,$get_time)
	{
		$pri_msg=pack('LLL' ,$key,$card_id,$get_time );
		$sendbuf=$this->park("0325",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_magic_card_del($userid ,$key)
	{
		$pri_msg=pack('L' ,$key);
		$sendbuf=$this->park("0326",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_magic_card_update($userid ,$key,$card_id,$get_time)
	{
		$pri_msg=pack('LLL' ,$key,$card_id,$get_time );
		$sendbuf=$this->park("0328",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_attack_card_add($userid,$card_id){
		$pri_msg=pack('L',$card_id);
		$sendbuf=$this->park("03f1",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function xhx_attack_card_del($userid,$card_id){
		$pri_msg=pack('L',$card_id);
		$sendbuf=$this->park("03f3",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_attack_card_get($userid)
	{
		$sendbuf=$this->park("02f4",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lcard_id_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_boss_add($userid,$boss_id){
		$pri_msg=pack('L',$boss_id);
		$sendbuf=$this->park("03c6",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	
	function xhx_boss_del($userid,$boss_id){
		
		$pri_msg=pack('L',$boss_id);
		$sendbuf=$this->park("03c5",$userid,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function xhx_boss_get($userid)
	{
		$sendbuf=$this->park("02c7",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt);
		if($recvarr && $recvarr["result"] != SUCC)
		{
			return $recvarr;
		}
		$recv_count = $recvarr["count"];
		for($i=0;$i<$recv_count;$i++)
		{
			$fmt=$fmt . "/Lboss_id_$i";
		}
		return $this->unpark($recvbuf,$fmt);
	}

	function xhx_kaixin_id_get($strid)
	{
		$pri_msg=pack('a10',$strid);
		$sendbuf=$this->park("0460",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		return $this->unpark($recvbuf, "Luserid");
	}

	function xhx_kaixin_id_get_strid($userid)
	{
		$sendbuf=$this->park("0465",$userid,$pri_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		return $this->unpark($recvbuf, "a10strid");
	}

	function xhx_send_sys_msg_to_all($online_id, $npc_id, $content)
	{
		$content_len=strlen($content);
		$format='LLa'.$content_len;
		$pri_msg=pack($format,$npc_id, $content_len, $content);
		$sendbuf=$this->park("F23A",$online_id,$pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}

	function xhx_get_spirit($userid)
	{

		$sendbuf = $this->park("02bb",$userid,"");
		$recvbuf = $this->sock->sendmsg($sendbuf);
		$fmt = "Lcnt";
		$recvarr = $this->unpark($recvbuf, $fmt);
		$cnt = $recvarr["cnt"];
		echo $cnt;
		for ($i = 0; $i < $cnt; $i++){

			$fmt = $fmt."/Lspiritid_$i/Lnum_$i/Cfollow_flag_$i";

		}
		$newarr = $this->unpark($recvbuf,$fmt);
		return $newarr;

	}
} 
?>
