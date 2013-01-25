#!/usr/bin/perl -w

use strict;
use IO::Socket;
use online_util_module;
use Test::More;

#nei wang 
#my $login_uid = $ARGV[0] || 4437237;
#my $main_login_svr_addr = '10.1.1.5';
#my $main_login_svr_port = '7788';
#my $mole_login_svr_addr = '10.1.1.5';
#my $mole_login_svr_port = '7788';

#wai wang 
#my $login_uid = $ARGV[0] || 88693; #jim's number
my $login_uid = $ARGV[0] || 37139509;
my $main_login_svr_addr = '114.80.98.17';
my $main_login_svr_port = '1863';
my $mole_login_svr_addr = '114.80.98.17';
my $mole_login_svr_port = '1863';

my $monitor_server_ip = '127.0.0.1';
my $monitor_sock; 
my $monitor_server_port = '7589';

my $sendbuf;
my $MAX_FILE_SIZE = 500*1024*1024;

my @data;
my $index;
my $MAX_ONLINE_ID = 1200;
my $online_buf_cnt = 0;
my @online_buf_id;
my @online_buf_addr;
my @online_buf_port;

sub walk_cmd_test;
sub enter_map_cmd_test;
sub get_item_cmd_test;

sub get_1_online;

sub send_to_monitor_server;

#----------------------------------test begin--------------------------------


open(ERROR_LOG_FILE, ">>/opt/taomee/monitor_online_mole/error_logfile") || die ("Could not open file");
open(DEBUG_LOG_FILE, ">>/opt/taomee/monitor_online_mole/debug_logfile") || die ("Could not open file");

&init_monitor_server();

my $now = scalar localtime(time());

@data = (0, 0, 0, 0, 0, 0); #6 data, must change when test add, pack func must change too.

if ( &init_main_login_sock($main_login_svr_addr, $main_login_svr_port) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT MAIN LOGIN SOCK, $!\n";
	&send_data(); 
	goto exit_point;
}
if ( &login_main_loginSvr($login_uid) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! LOGIN MAIN LOGIN SOCK, $!\n";
	&send_data();
	goto exit_point;
}

if ( &init_mole_login_sock($mole_login_svr_addr, $mole_login_svr_port) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! INIT MAIN LOGIN SOCK, $!\n";
	&send_data();
	goto exit_point;
}
if ( &get_mole_recommended_svr_list($login_uid) != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! GET RECOMMENDED SVR LIST, $!\n";
	&send_data();
	goto exit_point;
}
$data[1] = 1; #test mole recommended svr list pass
print DEBUG_LOG_FILE "$now GET RECOMMENDED SVR LIST SUCCESS\n";

#get the specific online
if ( &get_1_online() != 0 ) {
	print ERROR_LOG_FILE "$now ERROR! GET RANGED SVR LIST, $!\n";
	&send_data();
	goto exit_point;
} else {
	print DEBUG_LOG_FILE "$now GET RANGED SVR LIST FROM $index SUCCESS, $online_buf_cnt onlines get\n";
}

#-----------------------make the traverse----------------
&traverse_online();

exit_point:	
if ( $main_login_sock != 0 ) {
	close $main_login_sock; $main_login_sock = 0;
}
if ( $mole_login_sock != 0 ) {
	close $mole_login_sock; $mole_login_sock = 0;
}
close $monitor_sock;
exit 0;

#----------------------------------test end----------------------------------

sub walk_cmd_test
{
	#send a request
    $sendbuf = pack("NcN3 NNN", 29, $ver++, 303, $login_uid, 0,
					100, 200, 0);
    if ( send($online_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 303) != 0 ) {
		return -1;
	}
	return 0;
}

sub enter_map_cmd_test
{
	#send a request
    $sendbuf = pack("NcN3 ", 17, $ver++, 402, $login_uid, 0);
    if ( send($online_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 402) != 0 ) {
		return -1;
	}

	#send a request
    $sendbuf = pack("NcN3 NNNN", 33, $ver++, 401, $login_uid, 0,
					15, 0, 0, 0);
    if ( send($online_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 401) != 0 ) {
		return -1;
	}
	return 0;

}

sub get_item_cmd_test
{
	#send a request
    $sendbuf = pack("NcN3 NNc", 26, $ver++, 507, $login_uid, 0,
					$login_uid, 1, 0);
    if ( send($online_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 507) != 0 ) {
		return -1;
	}
	my $retuid = unpack("N", $recvbuf);
	if ( $retuid != $login_uid ) {
		diag("ret pkg error\n");
		return -1;
	}
	return 0;

}

sub init_monitor_server
{
	$monitor_sock = IO::Socket::INET->new(
	    PeerAddr => $monitor_server_ip,
	    PeerPort => $monitor_server_port,
	    Proto => 'tcp',
	    Type => SOCK_STREAM);
	if ( $monitor_sock == 0 ) {         
	  diag("Can't connect: $!\n");
	  die("connect monitor server error");
	 }   
}
sub send_to_monitor_server 
{
	#send a request
    $sendbuf = pack("N6", @data);
    if ( send($monitor_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

}
 
sub get_1_online 
{
	$index = int(rand($MAX_ONLINE_ID - 1));
	$index = 1 if $index == 0;
	if ( &get_mole_ranged_svr_list($login_uid, $index, $index) != 0 ) {
		return -1;
	}
	#update the online buffer
	$online_buf_cnt = unpack("N", $recvbuf);
	$recvbuf = substr($recvbuf, 4); 
	for (my $i = 0; $i < $online_buf_cnt; $i++) { 
		($online_buf_id[$i], $online_buf_addr[$i], $online_buf_port[$i]) = unpack("Nx4A16nx4", $recvbuf);
		$recvbuf = substr($recvbuf, 30);
	}
	return 0;
}

sub send_data
{
	$data[0] = $login_uid;

	if ( &send_to_monitor_server() != 0 ) {
	}

	ERROR_LOG_FILE->flush();
	DEBUG_LOG_FILE->flush();
	if ( (stat(DEBUG_LOG_FILE))[7] > $MAX_FILE_SIZE ) {
		close DEBUG_LOG_FILE;
		open(DEBUG_LOG_FILE, ">./debug_logfile") || die ("Could not open file");
	}
	if ( (stat(ERROR_LOG_FILE))[7] > $MAX_FILE_SIZE ) {
		close ERROR_LOG_FILE;
		open(ERROR_LOG_FILE, ">./error_logfile") || die ("Could not open file");
	}
}

sub traverse_online
{
	my $i = 0;
	my $now;
	for ( $i=0; $i<$online_buf_cnt; $i++ ) {
		($data[2], $data[3], $data[4], $data[5]) = (0, 0, 0, 0);
		$now = scalar localtime(time());
		if ( &init_online_sock($online_buf_addr[$i], $online_buf_port[$i]) != 0 ) {
			print ERROR_LOG_FILE "$now ERROR! INIT ONLINE SOCK, [ONLINE ID=$online_buf_id[$i]], $!\n";
			goto next_loop;
		}
		if ( &login_onlineSvr($login_uid, $online_buf_id[$i]) != 0 ) {
			print ERROR_LOG_FILE "$now ERROR! LOGIN ONLINE, [ONLINE ID=$online_buf_id[$i]], $!\n";
			goto next_loop;
		}
		$data[2] = 1; #test login online pass
		print DEBUG_LOG_FILE "$now LOGIN ONLINE SUCCESS, [ONLINE ID=$online_buf_id[$i], \
	ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]]\n";
	
		if ( &walk_cmd_test() != 0 ) {
			print ERROR_LOG_FILE "$now ERROR! ONLINE WALK CMD, [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]], $!\n";
		} else {
			$data[3] = 1; #test walk cmd pass
			print DEBUG_LOG_FILE "$now TEST WALK CMD SUCCESS , [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]]\n";
		}
	
		if ( &enter_map_cmd_test() != 0 ) {
			print ERROR_LOG_FILE "$now ERROR! ONLINE ENTER MAP CMD, [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]],$!\n";
		} else {
			$data[4] = 1; #test enter map cmd pass
			print DEBUG_LOG_FILE "$now TEST ENTER MAP CMD SUCCESS, [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]]\n";
		}
		if ( &get_item_cmd_test() != 0 ) {
			print ERROR_LOG_FILE "$now ERROR! GET ITEM CMD, [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]], $!\n";
		} else {
			$data[5] = 1; #test get item cmd pass
			print DEBUG_LOG_FILE "$now TEST GET ITEM CMD SUCCESS, [ONLINE ID=$online_buf_id[$i], ONLINE IP=$online_buf_addr[$i], ONLINE PORT=$online_buf_port[$i]]\n";
		}

next_loop:
		&send_data();
		if ( $online_sock != 0 ) {
			close $online_sock; $online_sock = 0;
		}
	}
}
