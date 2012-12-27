#!/usr/bin/perl -w

use strict;
use IO::Socket;
use online_util_module;
use Test::More;

#nei wang 
#my $login_uid = $ARGV[0] || 9956539;
my $login_uid = $ARGV[0] || 4437237;

my $online_addr1 = "10.1.1.24";
my $online_port1 = "4803";
my $online_id1 = 103;

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

sub check_mailbox;
my $sendbuf;

my $seqnum = 0;

#----------------------------------test begin--------------------------------


open(ERROR_LOG_FILE, ">./error_logfile") || die ("Could not open file");
open(DEBUG_LOG_FILE, ">./debug_logfile") || die ("Could not open file");

my $now = scalar localtime(time());

if ( &init_online_sock($online_addr1, $online_port1) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT ONLINE SOCK, [ONLINE ID=$online_id1]]\n";
	exit 0;
}

if ( &login_onlineSvr($login_uid, $online_id1) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! LOGIN ONLINE, [ONLINE ID=$online_id1]]\n";
	exit 0;
}
print DEBUG_LOG_FILE "$now LOGIN ONLINE SUCCESS, [ONLINE ID=$online_id1, \
				ONLINE IP=$online_addr1, ONLINE PORT=$online_port1\n";

&battle_start();
exit 0;

sub get_user_info
{
	$sendbuf = pack("NcN4 N", 21+4, 1, 10010, $login_uid, $seqnum++, 0, 
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
	$sendbuf = pack("NcN4 a16N3a16", 21 + 44, 1, 10011, $login_uid, 0, 
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

sub chat_request
{
	$sendbuf = pack("NcN4 NNN", 21 + 12, 1, 10012, $login_uid, 0, 
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
	$sendbuf = pack("NcN4 NNN", 21 + 12, 1, 10018, $login_uid, 0, 
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
	$sendbuf = pack("NcN4 NNNa10", 21 + 22, 1, 10080, $login_uid, 0, 
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
	$sendbuf = pack("NcN4 NNa10", 21 + 18, 1, 10020, $login_uid, 0, 
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
	$sendbuf = pack("NcN4 NNNa5", 21 + 21, 1, 10021, $login_uid, 0, 
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

sub battle_start
{
	$sendbuf = pack("NcN4 N2", 21 + 8, 1, 10100, $login_uid, 1, 0, 
					100, 1) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 10100) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	
	return 0;

}
