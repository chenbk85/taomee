<?php
#changelog
#09-05-14 :修正appeal_get_appeal, appeal_get_list没有返回telphone的错误;
#09-05-12 :调整appeal_add_appeal,appeal_get_appeal, appeal_get_list;：分别加入两个字段
#09-05-04 :设置协议超时时间 socket_set_option($socket,SOL_SOCKET,SO_REUSEADDR,1);
#09-04-27 :change user_get_userid_by_email

require_once("mole2_db_proto.php");
class Cmole2_proto extends  Cmole2_db_proto {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }
//===========================KICK PLAYER=========================
	function kick_user_offline($userid){
		$cmdid = "F618";
		$msg = pack("LL",$userid,0);
		$sendbuf=$this->park($cmdid,$adminid,$msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}
	function send_user_msg($userid,$msg){
		$cmdid = "F234";
                $msglen=strlen($msg);
		$msg = pack("Na16NNa".$msglen,$userid,"admin",$userid,$msglen,$msg);
		$climsg = pack("LNnNNN",$userid,18+strlen($msg),1010,$userid,0,0).$msg;
		$sendbuf=$this->park($cmdid,$adminid,$climsg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}
	function send_mail_notify($userid){
		$cmdid = "F23B";
		$msg = pack("LLL",$userid,1956,3);
		$sendbuf=$this->park($cmdid,$adminid,$msg);
		return $this->sock->sendmsg_without_returnmsg($sendbuf);
	}
//===========================BAN PLAYER==========================
	function pp_set_punish_flag ($userid, $ban_flag, $on){
		if($on == 0) $ban_flag = 0;
		$cmdid='09C9';
		$in_msg=pack('L' ,$ban_flag );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_get_ban_flag ($userid ){
		$cmdid='08CE';
		$in_msg='';
		$out_msg='Lflag';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//============================SEND MAIL==========================
	function user_mail_send_set ($userid ,$flag,$type,$themeid,$senderid,$sender_nick,$title,$message,$items,$ids){
		$cmdid='09B9';
		$in_msg=pack('LLLLa64a64a256a64a32' ,$flag,$type,$themeid,$senderid,$sender_nick,$title,$message,$items,$ids );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//===========================BATTLE INFO=========================
	function user_battle_info_set ($userid ,$all_score,$day_score,$update_time,$person_win,$person_fail,$team_win,$team_fail){	
		$cmdid='09B5';
		$in_msg=pack('LLLLLLL' ,$all_score,$day_score,$update_time,$person_win,$person_fail,$team_win,$team_fail );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}	
//===========================PLAYER INFO=========================
	function mole2_login ($userid ){
		$cmdid='0801';
		$in_msg='';
		$out_msg='a16nick/a32signature/Lflag/Lcolor/Lregister_time/Crace/Cprofesstion/Ljoblevel/Lhonor/Lxiaomee/Llevel/Lexperience/Sphysique/Sstrength/Sendurance/Squick/Sintelligence/Sattr_addition/Lhp/Lmp/Linjury_state/Cin_front/Lmax_attire/Lmax_medicine/Lmax_stuff';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_login ($userid ){
		$cmdid='081A';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lvip/Lfriend_cnt/Lblack_cnt';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Luserid_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
 	function mole2_user_user_login_ex ($userid ){
		$cmdid='0838';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='a16nick/a32signature/Lflag/Lcolor/Lregister_time/Crace/Cprofesstion/Ljoblevel/Lhonor/Lxiaomee/Llevel/Lexperience/Sphysique/Sstrength/Sendurance/Squick/Sintelligence/Sattr_addition/Lhp/Lmp/Linjury_state/Cin_front/Lmax_attire/Lmax_medicine/Lmax_stuff/Ltime/Laxis_x/Laxis_y/Lmapid/Ldaytime/Lfly_mapid/Lexpbox/Lenergy/Lskill_expbox/Lflag_ex/Lwinbossid/Lparent_id/Lall_score/Lday_score/Lupdate_time/Lfriends_cnt/Lall_pet_cnt/Lskill_count/Lpet_count/Lattire_on_body_count/Lattire_in_grid_count/Litem_count/Ltask_recv_count/Ltitle_count';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}
		return $recvarr;
	}
	function mole2_user_user_create_role ($userid ,$parent,$nick,$signature,$color,$professtion){
		$cmdid='0902';
		$in_msg=pack('La16a32LC' ,$parent,$nick,$signature,$color,$professtion );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_update_color ($userid ,$color){
		$cmdid='0905';
		$in_msg=pack('L' ,$color );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_update_nick ($userid ,$nick){
		$cmdid='0903';
		$in_msg=pack('a16' ,$nick );
		$out_msg='a16nick';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_update_signature ($userid ,$signature){
		$cmdid='0904';
		$in_msg=pack('a32' ,$signature );
		$out_msg='a32signature';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_update_professtion ($userid ,$professtion){
		$cmdid='0906';
		$in_msg=pack('C' ,$professtion );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_xiaomee ($userid ,$xiaomee){
		$cmdid='0907';
		$in_msg=pack('l' ,$xiaomee );
		$out_msg='Lxiaomee/ladd_value';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_honor ($userid ,$honor){
		$cmdid='0908';
		$in_msg=pack('c' ,$honor );
		$out_msg='Chonor';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_exp ($userid ,$experience){
		$cmdid='0909';
		$in_msg=pack('l' ,$experience );
		$out_msg='Lexperience';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_attr_addition ($userid ,$attr_addition){
		$cmdid='090B';
		$in_msg=pack('s' ,$attr_addition );
		$out_msg='Sattr_addition';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_hp_mp ($userid ,$hp,$mp){
		$cmdid='090C';
		$in_msg=pack('ll' ,$hp,$mp );
		$out_msg='Lhp/Lmp';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_four_attr ($userid ,$earth,$water,$fire,$wind){
		$cmdid='090D';
		$in_msg=pack('cccc' ,$earth,$water,$fire,$wind );
		$out_msg='Cearth/Cwater/Cfire/Cwind';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_level ($userid ,$level){
		$cmdid='0908';
		$in_msg=pack('L' ,$level );
		$out_msg='Llevel';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_set_hp_mp ($userid ,$hp,$mp){
		$cmdid='0939';
		$in_msg=pack('LL' ,$hp,$mp );
		$out_msg='Lhp/Lmp';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function proto_get_expbox ($userid ){
		$cmdid='08CC';
		$in_msg='';
		$out_msg='Lexpbox';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function proto_set_expbox ($userid ,$expbox){
		$cmdid='09CD';
		$in_msg=pack('L' ,$expbox );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_user_flag_set ($userid ,$flag){
		$cmdid='0994';
		$in_msg=pack('L' ,$flag );
		$out_msg='Lflag';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_set_pos ($userid ,$pos){
		$cmdid='0908';
		$in_msg=pack('C' ,$pos );
		$out_msg='Cpos';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_add_base_attr ($userid ,$physique,$strength,$endurance,$quick,$intelligence,$attr_addition){
		$cmdid='090A';
		$in_msg=pack('SSSSSS' ,$physique,$strength,$endurance,$quick,$intelligence,$attr_addition );
		$out_msg='Sphysique/Sstrength/Sendurance/Squick/Sintelligence/Sattr_addition';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//===========================OFFLINE MSG=========================
	function mole2_user_user_get_offline_msg ($userid ){
		$sendbuf=$this->park("0821",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lmsglen/Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		$msg_count=$recvarr["count"];
		for($i=0;$i<$msg_count;$i++){
			$fmt=$fmt ."/Lmsglen_$i";
			$recvarr=$this->unpark($recvbuf, $fmt);
			$len=$recvarr["msglen_$i"];
			$fmt=$fmt."/a$len"."msg_$i";
		}
		return $this->unpark($recvbuf,$fmt );
	}
	function mole2_user_user_add_offline_msg ($userid, $msg ){
		$msglen=strlen($msg);
		$pri_msg=pack("La".$msglen,$msglen,$msg);
		$sendbuf=$this->park("0920",$userid, $pri_msg);
		return $this->unpark($this->sock->sendmsg($sendbuf),"");
	}
	function mole2_user_user_update_offline_msg ($userid ,$msglen,$count,$msg){
		$cmdid='0922';
		$in_msg=pack('LLa2000' ,$msglen,$count,$msg );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//=========================FRIEND & BLACK========================
	function mole2_user_user_add_friend ($userid ,$uid){
		$cmdid='0923';
		$in_msg=pack('L' ,$uid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_del_friend ($userid ,$uid){
		$cmdid='0924';
		$in_msg=pack('L' ,$uid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_get_friend_list ($userid ){
		$sendbuf=$this->park("0825",$userid, "");
		$recvbuf=$this->sock->sendmsg($sendbuf);	
		$fmt="Lcount";
		$recvarr=$this->unpark($recvbuf,$fmt );		
		if ($recvarr && $recvarr["result"]!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr["count"];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt . "/Lfriendid_$i";
		}
		return $this->unpark($recvbuf,$fmt );

	}
	function mole2_user_user_check_is_friend ($userid ,$uid){
		$cmdid='0826';
		$in_msg=pack('L' ,$uid );
		$out_msg='Lisfriend';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_get_black_list ($userid ){
		$cmdid='0829';
		$in_msg='';
 		$sendbuf = $this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$out_msg='Lcount';
		$recvarr = $this->unpark($recvbuf,$out_msg);
		if ($recvarr && $recvarr['result']!=SUCC){
            return $recvarr;
        }

        //SUCC
        $recv_count = $recvarr['count'];
        for ($i = 0; $i < $recv_count; $i++){
            $out_msg = $out_msg .'/'. "Luserid_$i";
        }
        return $this->unpark( $recvbuf,$out_msg);		
	}	
	function mole2_user_user_add_blackuser ($userid ,$blackid){
		$cmdid='0927';
		$in_msg=pack('L' ,$blackid);
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_user_del_blackuser ($userid ,$blackid){
		$cmdid='0928';
		$in_msg=pack('L' ,$blackid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//=============================SKILL=============================
	function mole2_user_skill_add ($userid ,$skillid,$level){
		$cmdid='0940';
		$in_msg=pack('LC' ,$skillid,$level );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_skill_get_list ($userid ){
		$cmdid='0841';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$out_msg="Lcount";
		$recvarr=$this->unpark($recvbuf,$out_msg );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$out_msg = $out_msg ."/Lskillid_$i/Clevel_$i/Lexp_$i";
		}
		return $this->unpark( $recvbuf,$out_msg);
	}
	function mole2_user_skill_add_level ($userid ,$skillid,$level){
		$cmdid='0942';
		$in_msg=pack('Lc' ,$skillid,$level );
		$out_msg='Lskillid/Clevel';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_skill_del ($userid ,$skillid){
		$cmdid='0943';
		$in_msg=pack('L' ,$skillid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_skill_add_exp ($userid ,$skillid,$exp){
		$cmdid='0944';
		$in_msg=pack('Ll' ,$skillid,$exp );
		$out_msg='Lskillid/Lexp';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//=============================ATTIRE============================
	function mole2_user_attire_add ($userid ,$xiaomee,$attire_id,$gridid,$level,$mduration,$duration,$hpmax,$mpmax,$atk,$matk,$def,$mdef,$speed,$spirit,$resume,$hit,$dodge,$crit,$fightback,$rpoison,$rlithification,$rlethargy,$rinebriation,$rconfusion,$roblivion,$hole_cnt,$hole1,$hole2,$hole3){
		$cmdid='0930';
		$in_msg=pack('lLLSSSLLSSSSSSSSSSSSSSSSSLLLL' ,$xiaomee,$attire_id,$gridid,$level,$mduration,$duration,$hpmax,$mpmax,$atk,$matk,$def,$mdef,$speed,$spirit,$resume,$hit,$dodge,$crit,$fightback,$rpoison,$rlithification,$rlethargy,$rinebriation,$rconfusion,$roblivion,$hole_cnt,$hole1,$hole2,$hole3 );
		$out_msg='Lxiaomee_all/lxiaomee_change/Lgettime/Lattire_id/Lgridid/Slevel/Smduration/Sduration/Lhpmax/Lmpmax/Satk/Smatk/Sdef/Smdef/Sspeed/Sspirit/Sresume/Shit/Sdodge/Scrit/Sfightback/Srpoison/Srlithification/Srlethargy/Srinebriation/Srconfusion/Sroblivion/Lhole_cnt/Lhole1/Lhole2/Lhole3';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_attire_get ($userid ,$gettime){
		$cmdid='0832';
		$in_msg=pack('L' ,$gettime );
		$out_msg='Lgettime/Lattire_id/Lgridid/Slevel/Sduration/Lhpmax/Lmpmax/Satk/Smatk/Sdef/Smdef/Sspeed/Sspirit/Sresume/Shit/Sdodge/Scrit/Sfightback/Srpoison/Srlithification/Srlethargy/Srinebriation/Srconfusion/Sroblivion';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function user_attire_attrib_set ($userid ,$gettime,$attire_id,$gridid,$level,$mduration,$duration,$hpmax,$mpmax,$atk,$matk,$def,$mdef,$speed,$spirit,$resume,$hit,$dodge,$crit,$fightback,$rpoison,$rlithification,$rlethargy,$rinebriation,$rconfusion,$roblivion,$hole_cnt,$hole1,$hole2,$hole3){
		$cmdid='0950';
		$in_msg=pack('LLLSSSLLSSSSSSSSSSSSSSSSSLLLL' ,$gettime,$attire_id,$gridid,$level,$mduration,$duration,$hpmax,$mpmax,$atk,$matk,$def,$mdef,$speed,$spirit,$resume,$hit,$dodge,$crit,$fightback,$rpoison,$rlithification,$rlethargy,$rinebriation,$rconfusion,$roblivion,$hole_cnt,$hole1,$hole2,$hole3 );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_attire_del ($userid ,$gettime){
		$cmdid='0934';
		$in_msg=pack('L' ,$gettime );
		$out_msg='Lgettime';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_attire_get_list ($userid ){
		$cmdid='0831';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lgettime_$i/Lattire_id_$i/Lgridid_$i/Slevel_$i/Sduration_$i/Lhpmax_$i/Lmpmax_$i/Satk_$i/Smatk_$i/Sdef_$i/Smdef_$i/Sspeed_$i/Sspirit_$i/Sresume_$i/Shit_$i/Sdodge_$i/Scrit_$i/Sfightback_$i/Srpoison_$i/Srlithification_$i/Srlethargy_$i/Srinebriation_$i/Srconfusion_$i/Sroblivion_$i/Lhole_cnt_$i/Lhole1_$i/Lhole2_$i/Lhole3_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function mole2_user_attire_get_list_ex ($userid ){
		$cmdid='0835';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lgettime_$i/Lattire_id_$i/Lgridid_$i/Slevel_$i/Sduration_$i/Lhpmax_$i/Lmpmax_$i/Satk_$i/Smatk_$i/Sdef_$i/Smdef_$i/Sspeed_$i/Sspirit_$i/Sresume_$i/Shit_$i/Sdodge_$i/Scrit_$i/Sfightback_$i/Srpoison_$i/Srlithification_$i/Srlethargy_$i/Srinebriation_$i/Srconfusion_$i/Sroblivion_$i/Lhole_cnt_$i/Lhole1_$i/Lhole2_$i/Lhole3_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function user_attire_in_storage_list_get ($userid ){
		$cmdid='08B4';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lgettime_$i/Lattire_id_$i/Lgridid_$i/Slevel_$i/Smduration_$i/Sduration_$i/Lhpmax_$i/Lmpmax_$i/Satk_$i/Smatk_$i/Sdef_$i/Smdef_$i/Sspeed_$i/Sspirit_$i/Sresume_$i/Shit_$i/Sdodge_$i/Scrit_$i/Sfightback_$i/Srpoison_$i/Srlithification_$i/Srlethargy_$i/Srinebriation_$i/Srconfusion_$i/Sroblivion_$i/Lhole_cnt_$i/Lhole1_$i/Lhole2_$i/Lhole3_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
//==============================ITEM===============================
	function mole2_user_item_get_list ($userid ){
		$cmdid='0871';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Litemid_$i/Lcount_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function user_item_in_storage_list_get ($userid ){
		$cmdid='08B3';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Litemid_$i/lcount_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function mole2_user_item_add ($userid ,$itemid,$count){
		$cmdid='0970';
		$in_msg=pack('LLL' ,0,$itemid,$count );
		$out_msg='Ltype/Litemid/Lcount';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function item_storage_add ($userid ,$itemid,$count){
		$cmdid='09B7';
		$in_msg=pack('Ll' ,$itemid,$count );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_item_ranged_get_list ($userid ,$start_id,$end_id){
		$cmdid='0874';
		$in_msg=pack('LL' ,$start_id,$end_id );

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Litemid_$i/Lbag_count_$i/Lstore_count_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
//===============================PET===============================
	function mole2_user_pet_add ($userid ,$pettype,$race,$flag,$nick,$level,$exp,$physiqueinit,$strengthinit,$enduranceinit,$quickinit,$iqinit,$physiqueparam,$strengthparam,$enduranceparam,$quickparam,$iqparam,$physiqueadd,$strengthadd,$enduranceadd,$quickadd,$iqadd,$attr_addition,$hp,$mp,$earth,$water,$fire,$wind){
		$cmdid='090E';
		$in_msg=pack('LCLa16LLSSSSSSSSSSSSSSSSLLCCCCLL',$pettype,$race,$flag,$nick,$level,$exp,$physiqueinit,$strengthinit,$enduranceinit,$quickinit,$iqinit,$physiqueparam,$strengthparam,$enduranceparam,$quickparam,$iqparam,$physiqueadd,$strengthadd,$enduranceadd,$quickadd,$iqadd,$attr_addition,$hp,$mp,$earth,$water,$fire,$wind,0,10000);
		$out_msg='Lpetid/Lpettype/Crace/Lflag/a16nick/Llevel/Lexp/Sphysiqueinit/Sstrengthinit/Senduranceinit/Squickinit/Siqinit/Sphysiqueparam/Sstrengthparam/Senduranceparam/Squickparam/Siqparam/Sphysique_add/Sstrength_add/Sendurance_add/Squick_add/Siq_add/Sattr_addition/Lhp/Lmp/Cearth/Cwater/Cfire/Cwind';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_pet_get_list ($userid ){
		$cmdid='0810';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lgettime_$i/Lpettype_$i/Crace_$i/Lflag_$i/a16nick_$i/Llevel_$i/Lexp_$i/Sphysiqueinit_$i/Sstrengthinit_$i/Senduranceinit_$i/Squickinit_$i/Siqinit_$i/Sphysiqueparam_$i/Sstrengthparam_$i/Senduranceparam_$i/Squickparam_$i/Siqparam_$i/Sphysique_add_$i/Sstrength_add_$i/Sendurance_add_$i/Squick_add_$i/Siq_add_$i/Sattr_addition_$i/Lhp_$i/Lmp_$i/Cearth_$i/Cwater_$i/Cfire_$i/Cwind_$i/Linjury_state_$i/Llocation_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function mole2_user_pet_get_in_store_list ($userid ){
		$cmdid='0851';
		$in_msg='';

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lgettime_$i/Lpettype_$i/Crace_$i/Lflag_$i/a16nick_$i/Llevel_$i/Lexp_$i/Sphysiqueinit_$i/Sstrengthinit_$i/Senduranceinit_$i/Squickinit_$i/Siqinit_$i/Sphysiqueparam_$i/Sstrengthparam_$i/Senduranceparam_$i/Squickparam_$i/Siqparam_$i/Sphysique_add_$i/Sstrength_add_$i/Sendurance_add_$i/Squick_add_$i/Siq_add_$i/Sattr_addition_$i/Lhp_$i/Lmp_$i/Cearth_$i/Cwater_$i/Cfire_$i/Cwind_$i/Linjury_state_$i/Llocation_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function mole2_user_pet_set ($userid ,$gettime,$pettype,$race,$flag,$nick,$level,$exp,$physiqueinit,$strengthinit,$enduranceinit,$quickinit,$iqinit,$physiqueparam,$strengthparam,$enduranceparam,$quickparam,$iqparam,$physique_add,$strength_add,$endurance_add,$quick_add,$iq_add,$attr_addition,$hp,$mp,$earth,$water,$fire,$wind,$injury_state,$location){
		$cmdid='0952';
		$in_msg=pack('LLCLa16LLSSSSSSSSSSSSSSSSLLCCCCLL' ,$gettime,$pettype,$race,$flag,$nick,$level,$exp,$physiqueinit,$strengthinit,$enduranceinit,$quickinit,$iqinit,$physiqueparam,$strengthparam,$enduranceparam,$quickparam,$iqparam,$physique_add,$strength_add,$endurance_add,$quick_add,$iq_add,$attr_addition,$hp,$mp,$earth,$water,$fire,$wind,$injury_state,$location );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_user_pet_delete ($userid ,$petid){
		$cmdid='0953';
		$in_msg=pack('L' ,$petid );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//==============================PET SKILL============================
	function mole2_pet_skill_list_get ($userid ,$petid){
		$cmdid='0846';
		$in_msg=pack('L' ,$petid );

		$sendbuf=$this->park($cmdid,$userid,$in_msg);
		$recvbuf=$this->sock->sendmsg($sendbuf);
		$fmt='Lpetid/Lcount';
		$recvarr=$this->unpark($recvbuf,$fmt );
		if ($recvarr && $recvarr['result']!=SUCC){
			return $recvarr;
		}

		//SUCC
		$recv_count=$recvarr['count'];
		for ($i=0;$i<$recv_count;$i++){
			$fmt=$fmt .'/'. "Lskillid_$i/Clevel_$i/Lexp_$i";
		}
		return $this->unpark( $recvbuf,$fmt);
	}
	function mole2_pet_skill_add ($userid ,$petid,$skillid,$level){
		$cmdid='0945';
		$in_msg=pack('LLC' ,$petid,$skillid,$level );
		$out_msg='Lpetid/Lskillid/Clevel';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_pet_skill_delete ($userid ,$petid,$skillid,$level){
		$cmdid='0949';
		$in_msg=pack('LLC' ,$petid,$skillid,$level );
		$out_msg='';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//=================================TASK==============================
    function mole2_task_info_list_get ($userid ){
        $cmdid='08C3';
        $in_msg='';
        $sendbuf = $this->park($cmdid, $userid, $in_msg);
        $recvbuf = $this->sock->sendmsg($sendbuf);
        $fmt    = 'Lon_cnt/Lother_cnt';
        $recvarr=$this->unpark($recvbuf, $fmt);
        if ($recvarr && $recvarr['result'] != SUCC) {
            return $recvarr;
        }
        $count = $recvarr['on_cnt'];
        for ($i = 0; $i < $count; $i++) {
            $fmt    = $fmt."/Lon_taskid_$i/Lon_node_$i/Lon_state_$i/Lon_optdate_$i/Lon_fin_time_$i/Lon_fin_num_$i/a64on_cli_buf_$i/a20on_ser_buf_$i";
        }
        $count = $recvarr['other_cnt'];
        for ($i = 0; $i < $count; $i++) {
            $fmt    = $fmt."/Lot_taskid_$i/Lot_state_$i/Lot_optdate_$i/Lot_fin_time_$i/Lot_fin_num_$i";
        }
        return $this->unpark($recvbuf, $fmt);
    }
	function mole2_task_set ($userid ,$taskid,$node,$state,$optdate,$fin_time,$fin_num,$cli_buf,$ser_buf){
		$cmdid='09C0';
		$in_msg=pack('LLLLLLa64a20' ,$taskid,$node,$state,$optdate,$fin_time,$fin_num,$cli_buf,$ser_buf );
		$out_msg='Ltaskid/Lnode/Lstate/Loptdate/Lfin_time/Lfin_num/a64cli_buf/a20ser_buf';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
	function mole2_task_delete ($userid ,$taskid){
		$cmdid='09C6';
		$in_msg=pack('L' ,$taskid );
		$out_msg='Ltaskid';
		return  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);
	}
//============================2125 ADD ITEM==========================
	function proto_user_add_items($userid,$itemid,$count) {
		return $this->item_storage_add($userid,$itemid,$count);
	}

}
?>	
