#!/usr/bin/perl

package online_util_module;

use IO::Socket;
use Test::More 'no_plan';
use Digest::MD5  qw(md5 md5_hex md5_base64);
require Exporter;
our @ISA =qw(Exporter);
our @EXPORT =qw($main_login_sock $mole_login_sock $online_sock $recvbuf $length $cmdid $uid $result $ver
		init_main_login_sock login_main_loginSvr init_mole_login_sock get_mole_ranged_svr_list 
		get_mole_recommended_svr_list
		init_online_sock login_onlineSvr recv_respond_by_cmd);
our @version = 1.0;


our $main_login_sock = 0;
our $mole_login_sock = 0;
our $online_sock = 0;
our $length = 0;
our $cmdid;
our $uid;
our $result;
our $recvbuf;
our $sess_body;
our $ver = 0x30;

#functions related to login server
sub init_main_login_sock;
sub login_main_loginSvr;

#functions related to mole login server
sub init_mole_login_sock;
sub get_mole_ranged_svr_list;
sub get_mole_recommended_svr_list;

#functions related to online server
sub init_online_sock; 
sub login_onlineSvr;

sub recv_respond_by_cmd;


sub init_main_login_sock
{
	die("param error") if @_ != 2;
	(my $login_svr_addr, my $login_svr_port) = ($_[0], $_[1]);
	$main_login_sock = IO::Socket::INET->new(
								PeerAddr => $login_svr_addr,
								PeerPort => $login_svr_port,
								Proto => 'tcp',
								Type => SOCK_STREAM);
	if ( $main_login_sock == 0 ) {
		diag("Can't connect: $!\n");
		return -1;
	}
	return 0;
}

sub login_main_loginSvr
{
	die("param error") if @_ != 1;
	my $login_uid = $_[0];

#login
	#send a request
	$sendbuf = pack("NcN3 a32N3", 61, 1, 104, $login_uid, 0, 'a420384997c8a1a93d5a84046117c2aa', 9, 1, 0);
	if ( send($main_login_sock, $sendbuf, 0) == 0 ) {
		diag("send error $!\n");
		return -1;
	}

	#recv the respond
	if ( &recv_respond_by_cmd($main_login_sock, 104) != 0 ) {
		return -1;
	}

	if ( $result != 0 ) {
		return -1;
	}
	$sess_body = unpack("a16", $recvbuf);
	return 0;
}

sub init_mole_login_sock
{
	die("param error") if @_ != 2;
	(my $login_svr_addr, my $login_svr_port) = ($_[0], $_[1]);
	$mole_login_sock = IO::Socket::INET->new(
								PeerAddr => $login_svr_addr,
								PeerPort => $login_svr_port,
								Proto => 'tcp',
								Type => SOCK_STREAM);
	if ( $mole_login_sock == 0 ) {
		diag("Can't connect: $!\n");
		return -1;
	}
	return 0;
}


sub init_online_sock
{
	die("param error") if @_ != 2;
	(my $online_svr_addr, my $online_svr_port) = ($_[0], $_[1]);
	$online_sock = IO::Socket::INET->new(
								PeerAddr => $online_svr_addr,
								PeerPort => $online_svr_port,
								Proto => 'tcp',
								Type => SOCK_STREAM);
	if ( $online_sock == 0 ) {
		diag("Can't connect: $!\n");
		return -1;
	}
	return 0;

}

sub login_onlineSvr
{
	die("param error") if @_ != 2;
	(my $login_uid, my $onlineid) = ( $_[0], $_[1]);

	#login online server
	$md5_str = "fREd hAo crAzy BAby in Our ProgRAm?";
	$md5_key = sprintf("%u%.11s%u", unpack("x10N", $sess_body),
						substr($md5_str, 5, 11), unpack("x3N", $sess_body));
	$digest = md5_hex($md5_key);
	$digest = substr($digest, 6, 16);
	$sendbuf = pack("NcN3 na16Na16", 55, $ver++, 201, $login_uid, 0, 
					$onlineid, $digest, 16, $sess_body) ;
	if ( send($online_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($online_sock, 201) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}
	return 0;
} 

sub get_mole_recommended_svr_list
{
	die("param error") if @_ != 1;
	$login_uid = $_[0];
	#send a request
	$sendbuf = pack("NcN3 a16N", 37, 1, 105, $login_uid, 0, $sess_body, 0);
	if ( send($mole_login_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($mole_login_sock, 105) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}

	return 0;
}

sub get_mole_ranged_svr_list
{
	die("param error") if @_ != 3;
	(my $login_uid, my $startid, my $endid) = ( $_[0], $_[1], $_[2]);
	#send a request
	$sendbuf = pack("NcN3 N3", 29, 1,106, $login_uid, 0, $startid, $endid, 0) ;
	if ( send($mole_login_sock, $sendbuf, 0) == 0 ) {
		return -1;
	}

	if ( &recv_respond_by_cmd($mole_login_sock, 106) != 0 ) {
		return -1;
	}

	if ($result != 0 ) {
		return -1;
	}

	return 0;
}


sub recv_respond_by_cmd
{
	die("param error") if @_ != 2;
	my $sock = $_[0];
	my $expect_cmd = $_[1];

	return -1 if $sock == 0;
again:	
	if ( read($sock, $recvbuf, 17) == 0 ) {
		diag("recv error $!\n");
		return -1;
	}
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);

	if ( $cmdid != $expect_cmd ) {
		if ( $length == 17 ) {
			goto again;
		}
		read($sock, $recvbuf, $length - 17) || return -1;
		goto again;
	}
	if ( $length == 17 ) {
		return 0;
	}
	read($sock, $recvbuf, $length - 17) || return -1;
	return 0;
}

1;
__END__

