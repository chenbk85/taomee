#!/usr/bin/perl
use IO::Socket;
use Test::More 'no_plan';
use Digest::MD5  qw(md5 md5_hex md5_base64);

my $login_svr_addr = '10.1.1.5';
my $login_svr_port = '7777';

my $online_svr_addr = '10.1.1.5';
my $online_svr_port = '1211';

my $login_uid = $ARGV[0] || 50015;

#functions related to login server
sub init_login_sock;
sub login_loginSvr;

#functions related to online server
sub init_online_sock; 
sub login_onlineSvr;

#functions related to da rong shu task
sub become_water_bucket_cmd;
sub get_apple_pai;
sub finish_water_bucket_cmd();


#------------------------------------- TEST BEGIN ------------------------------------
#init the server
&init_login_sock();
&login_loginSvr();

&init_online_sock();
&login_onlineSvr();

#do the test
&become_water_bucket_cmd();
&get_apple_pai();

diag("------------ ALL TEST HAS BEEN DONE , PLEASE CHECK THE RESULT --------------");
#------------------------------------- TEST END ------------------------------------
sleep 60;
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
	$result = sprintf("%d", $result);
	if ( $cmdid != 101 ) {
		read($sock, $recvbuf, $length - 17) if $length != 17;
		goto again;
	} 
	#the right cmd return
 	return 0 if $length == 17; 

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

}

sub login_onlineSvr
{
	#login online server
	$md5_str = "Bye bYe crAzy B Aby in Our ProgRAm?";
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
	$result = sprintf("%d", $result);
		if ( $cmdid != 201 ) {
		read($sock, $recvbuf, $length - 17) if $length != 17;
		goto again;
	} 
	#the right cmd return
	#diag("FUNC:login_onlineSvr [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
 	return 0 if $length == 17; 
	
	read($sock, $recvbuf, $length - 17) || die "recv error $!\n";

	is($result, 0, "login online svr ok");

	return 0;
} 

sub become_water_bucket_cmd
{
	#send a request
	$sendbuf = pack("NcN3", 17, 1, 1132, $login_uid, 0) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

	#get the header
again:
	read($sock, $recvbuf, 17) || die "recv error $!\n"; #package header
	$length = unpack("N", $recvbuf); 
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf);
	$result = unpack("x13N", $recvbuf);
	$result = sprintf("%d", $result);
	if ( $cmdid != 1132 ) {
		read($sock, $recvbuf, $length - 17) if $length != 17;
		goto again;
	} 
	#the right cmd return
	diag("cmd return [uid=$uid, cmdid=$cmdid, result=$result]");
 	return 0 if $length == 17; 

	#get the body
	read($sock, $recvbuf, $length - 17);
	(my $water_uid, my $water_itmid) = unpack("NN", $recvbuf);
	diag("[water_uid=$water_uid, water_itmid=$water_itmid]");

	return 0;

}

sub get_apple_pai
{
	#get apple pai
	$sendbuf = pack("NcN3 ccN3", 31, 1, 515, $login_uid, 0,
					2, 3, 190141, 190119, 190024) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

	#get the header
again:
	read($sock, $recvbuf, 17) || die "recv error $!\n"; #package header
	$length = unpack("N", $recvbuf); 
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf);
	$result = unpack("x13N", $recvbuf);
	$result = sprintf("%d", $result);
	if ( $cmdid != 515 ) {
		read($sock, $recvbuf, $length - 17) if $length != 17;
		goto again;
	} 
	#the right cmd return
	diag("cmd return [uid=$uid, cmdid=$cmdid, result=$result]");
 	return 0 if $length == 17; 

	#get the body
	read($sock, $recvbuf, $length - 17);
	my $itmcount = unpack("N", $recvbuf);
	$recvbuf = substr($recvbuf, 4);
	for ( my $i=0; $i<$itmcount; $i++ ) {
		(my $itmid, my $itmidcnt) = unpack("NN", $recvbuf);
		diag("GET COMPOUND ITEM [itmid=$itmid, itmidcnt=$itmidcnt]");
		$recvbuf = substr($recvbuf, 8);
	}

	return 0;
}
