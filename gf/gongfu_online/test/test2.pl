#!/usr/bin/perl -w

use strict;
use IO::Socket;
use online_util_module;
use Test::More;

#nei wang 
#my $login_uid = $ARGV[0] || 9956539;
my $login_uid = $ARGV[0] || 4437237;
my $main_login_svr_addr = '10.1.1.5';
my $main_login_svr_port = '8888';
my $dudu_login_svr_addr = '10.1.1.5';
my $dudu_login_svr_port = '8888';

my $online_addr1 = "10.1.1.5";
my $online_port1 = "1704";
my $online_id1 = 4;

#wai wang 
#my $login_uid = $ARGV[0] || 88693; #jim's number
#my $login_uid = $ARGV[0] || 37139509;
#my $main_login_svr_addr = '114.80.99.74';
#my $main_login_svr_port = '1863';
#my $dudu_login_svr_addr = '114.80.99.73';
#my $dudu_login_svr_port = '1863';


sub get_user_info;
sub mod_user_info;
sub get_pic_ip;
sub chat_request;
sub get_users_status;
sub tmp_chat;
sub add_frd;
sub reply_add_frd;
sub mod_url;
sub mod_info;

my $sendbuf;



#----------------------------------test begin--------------------------------


open(ERROR_LOG_FILE, ">./error_logfile") || die ("Could not open file");
open(DEBUG_LOG_FILE, ">./debug_logfile") || die ("Could not open file");

my $now = scalar localtime(time());

if ( &init_main_login_sock($main_login_svr_addr, $main_login_svr_port) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT MAIN LOGIN SOCK\n";
	exit 0;
}
if ( &login_main_loginSvr($login_uid) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! LOGIN MAIN LOGIN SOCK\n";
	close $main_login_sock;
	exit 0;
}


print DEBUG_LOG_FILE "$now MAIN LOGIN SUCCESS\n";

if ( &init_dudu_login_sock($dudu_login_svr_addr, $dudu_login_svr_port) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT MAIN LOGIN SOCK\n";
	exit 0;
}
if ( &get_dudu_recommended_svr_list($login_uid) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! GET RECOMMENDED SVR LIST\n";
	close $dudu_login_sock;
	exit 0;
}
my $count = unpack("N", $recvbuf);
print DEBUG_LOG_FILE "$now GET RECOMMENDED SVR LIST SUCCESS, count is $count\n";

if ( &create_role("abcd", 0) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! create role\n";
	close $main_login_sock;
	exit 0;
}
print DEBUG_LOG_FILE "$now create role SUCCESS\n";
if ( &init_online_sock($online_addr1, $online_port1) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT ONLINE SOCK, [ONLINE ID=$online_id1]]\n";
	exit 0;
}

$now = scalar localtime(time());
if ( &login_onlineSvr($login_uid, $online_id1) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! LOGIN ONLINE, [ONLINE ID=$online_id1]]\n";
	exit 0;
}
print DEBUG_LOG_FILE "$now LOGIN ONLINE SUCCESS, [ONLINE ID=$online_id1, \
				ONLINE IP=$online_addr1, ONLINE PORT=$online_port1\n";
print DEBUG_LOG_FILE "$now CMD=10001, RES=$result\n";

&get_user_info();

&get_pic_ip();

&chat_request();

sleep 60;

exit 0;


sub get_user_info
{
	$sendbuf = pack("NcN3 N", 21, 1, 10010, $login_uid, 0, 
					$login_uid) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10010) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	(my $uid, my $flag, my $regtime, my $birthday, my $nick, my $xiaomee, my $exp) = unpack("N4a16N2", $recvbuf);
	print DEBUG_LOG_FILE "$uid, $flag,$regtime, $birthday, $nick, $xiaomee, $exp ";
	$recvbuf = substr($recvbuf, 40);
	my $signlen = unpack("N", $recvbuf);
	$recvbuf = substr($recvbuf, 4);
	my $sign = unpack("a$signlen", $recvbuf);
	print DEBUG_LOG_FILE "$sign\n";

	return 0;

}

sub mod_user_info 
{
	$sendbuf = pack("NcN3 a16N3a16", 61, 1, 10011, $login_uid, 0, 
					"昵称",  -2000, 500, 16, "this is my sign") ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10011) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub get_pic_ip
{
	$sendbuf = pack("NcN3 N2", 25, 1, 10040, $login_uid, 0, 
					4437237, 1) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10040) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub chat_request
{
	$sendbuf = pack("NcN3 NNN", 17 + 12, 1, 10012, $login_uid, 0, 
					2, $login_uid, 50438) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}
	if ( &recv_respond_by_cmd($online_sock, 10012) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}

	return 0;

}

sub get_users_status  
{
	$sendbuf = pack("NcN3 NNN", 17 + 12, 1, 10018, $login_uid, 0, 
					2, 50033, 50098) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10018) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub tmp_chat  
{
	$sendbuf = pack("NcN3 NNNa10", 17 + 22, 1, 10080, $login_uid, 0, 
					$login_uid, 123123, 4, "fuck") ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10080) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub add_frd  
{
	$sendbuf = pack("NcN3 NNa10", 17 + 18, 1, 10020, $login_uid, 0, 
					50438, 4, "hihi") ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10020) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub reply_add_frd  
{
	$sendbuf = pack("NcN3 NNNa5", 17 + 17, 1, 10021, $login_uid, 0, 
					50238, 2, 4, "hihi") ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10021) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}

sub mod_info 
{
	$sendbuf = pack("NcN3 ", 17, 1, 10041, $login_uid, 0) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10041) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}
sub mod_url  
{
	$sendbuf = pack("NcN3 ", 17, 1, 10042, $login_uid, 0) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10042) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}
