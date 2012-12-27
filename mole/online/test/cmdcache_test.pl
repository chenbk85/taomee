#!/usr/bin/perl
use IO::Socket;
use Test::More 'no_plan';
use Digest::MD5  qw(md5 md5_hex md5_base64);

my $login_svr_addr = '10.1.1.5';
my $login_svr_port = '7777';

my $online_svr_addr = '10.1.1.5';
my $online_svr_port = '1249';

my $login_uid = $ARGV[0] || 50015;



#functions related to login server
sub init_login_sock;
sub login_loginSvr;

#functions related to online server
sub init_online_sock; 
sub login_onlineSvr;

#functions to do the test
sub send_cmd_nobody;
sub recv_cmd_nobody;


#------------------------------------- TEST BEGIN ------------------------------------

&init_login_sock();
&login_loginSvr();

&init_online_sock();
&login_onlineSvr();

diag("the 1205 cmd must be send only once");
&send_cmd_nobody(1205); #recv nothing forever

for( $i=0; $i<10; $i++ ) {
	&send_cmd_nobody(1206);
	&send_cmd_nobody(1207);
	&send_cmd_nobody(1208);
	&send_cmd_nobody(1209);
}

sleep 6;
&recv_cmd_nobody() while (1);




diag("------------ ALL TEST HAS BEEN DONE , PLEASE CHECK THE RESULT --------------");
#------------------------------------- TEST END ------------------------------------
exit(0);


sub init_login_sock
{
	$sock = IO::Socket::INET->new(
								PeerAddr => $login_svr_addr,
								PeerPort => $login_svr_port,
								Proto => 'tcp',
								Type => SOCK_STREAM) || die "Can't connect: $!\n";
}

sub login_loginSvr
{
#login
	#send a request
	$sendbuf = pack("NcN3 a32N", 53, 1, 101, $login_uid, 0, 'a420384997c8a1a93d5a84046117c2aa', 9) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

	#get the header
again:
	read($sock, $recvbuf, 17) || die "recv error $!\n"; #package header
	$length = unpack("N", $recvbuf); 
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf);
	$result = unpack("x13N", $recvbuf);
	goto again if ($cmdid != 101 );

	is($result, 0, "login login_svr ok");

	#get the body
	read($sock, $recvbuf, $length - 17);
	$sess_body = unpack("x4a16", $recvbuf);
	return 0;
}


sub init_online_sock
{
#	open (mole_bank_log, ">./mole_bank_log") || die ("Could not open file"); 
	$sock = IO::Socket::INET->new(
								PeerAddr => $online_svr_addr,
								PeerPort => $online_svr_port,
								Proto => 'tcp',
								Type => SOCK_STREAM) || die "Can't connect: $!\n";
#	select mole_bank_log;
#	$| = 1;
#	select STDOUT;
}

sub login_onlineSvr
{
	#login online server
#$md5_str = "Bye bYe crAzy B Aby in Our ProgRAm?";
	$md5_str = "bAbY mOlE Go tO heLl!";
	$md5_key = sprintf("%u%.11s%u", unpack("x10N", $sess_body),
						substr($md5_str, 5, 11), unpack("x3N", $sess_body));
	$digest = md5_hex($md5_key);
	$digest = substr($digest, 6, 16);
	$sendbuf = pack("NcN3 na16Na16", 55, 1, 201, $login_uid, 0, 
					49, $digest, 16, $sess_body) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

again:
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	diag("FUNC:login_onlineSvr [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
	read($sock, $recvbuf, $length - 17) || die "recv error $!\n";
	goto again if ($cmdid != 201 );

	is($result, 0, "login online svr ok");

	return 0;
} 

#--------------------------------------- UITILITIES -------------------------------------------
sub send_cmd_nobody
{
	( @_ == 1 ) || die("param error");
	my $cmd = $_[0];
	$sendbuf = pack("NcN3 ", 17, 1, $cmd, $login_uid, 0) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";
	diag("FUNC:send_cmd_nobody [sendcmdid=$cmd]");
}

sub recv_cmd_nobody
{
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:recv_cmd_nobody [recvlen=$length] [recvcmdid=$cmdid] [uid=$uid] [result=$result]");
	return;
}
