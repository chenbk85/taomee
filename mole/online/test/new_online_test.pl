#!/usr/bin/perl 

use IO::Socket;
use online_util_module;
use Test::More;

my $login_uid = $ARGV[0] || 50015;
my $online_id = 300;
my $server_ip = '10.1.1.5';
my $server_port = '7589';

sub walk_cmd_test;
sub enter_map_cmd_test;


sub send_to_server;

#----------------------------------test begin--------------------------------

if ( &init_login_sock() != 0 ) {
	exit -1;
}
if ( &login_loginSvr($login_uid) != 0 ) {
	exit -1;
}

if ( &init_online_sock() != 0 ) {
	exit -1;
}
if ( &login_onlineSvr($login_uid, $online_id) != 0 ) {
	exit -1;
}

if ( &walk_cmd_test() != 0 ) {
	exit -1;
}
if ( &enter_map_cmd_test() != 0 ) {
	exit -1;
}

for ( $i=0; $i<5; $i++ ) {
	if ( &send_to_server() != 0 ) {
		exit -1;
	}
}

sleep 60;
#----------------------------------test end----------------------------------

sub walk_cmd_test
{
	#send a request
    $sendbuf = pack("NcN3 NNN", 29, 1, 303, $login_uid, 0,
					100, 200, 0);
    if ( send($sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd(303) != 0 ) {
		return -1;
	}
	is ( $result, 0, "walk cmd test ok");
	return 0;
}

sub enter_map_cmd_test
{
	#send a request
    $sendbuf = pack("NcN3 ", 17, 1, 402, $login_uid, 0);
    if ( send($sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd(402) != 0 ) {
		return -1;
	}
	is ( $result, 0, "leave map test ok");

	#send a request
    $sendbuf = pack("NcN3 NNNN", 33, 1, 401, $login_uid, 0,
					15, 0, 0, 0);
    if ( send($sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	if ( &recv_respond_by_cmd(401) != 0 ) {
		return -1;
	}
	is ( $result, 0, "enter map test ok");
	return 0;

}

sub send_to_server 
{
	$sock = IO::Socket::INET->new(
	    PeerAddr => $server_ip,
	    PeerPort => $server_port,
	    Proto => 'tcp',
	    Type => SOCK_STREAM);
	if ( $sock == 0 ) {         
	  diag("Can't connect: $!\n");
	  return -1;
	 }   

	#send a request
    $sendbuf = pack("N5", 1,1,1,1,1);
    if ( send($sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

}
