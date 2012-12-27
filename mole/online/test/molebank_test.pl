#!/usr/bin/perl
use IO::Socket;
use Test::More 'no_plan';
use Digest::MD5  qw(md5 md5_hex md5_base64);

my $login_svr_addr = '10.1.1.5';
my $login_svr_port = '7788';

my $online_svr_addr = '10.1.1.5';
my $online_svr_port = '1201';

my $login_uid = $ARGV[0] || 50015;

#global data used to store deposit infomation
our @deposit_bean; 
our @deposit_drawout; 
our @deposit_time; 
our @deposit_interval;

#the mole's current beans
our $cur_molebean;
our $draw_interest;
our $draw_bean;

#functions related to login server
sub init_login_sock;
sub login_loginSvr;

#functions related to online server
sub init_online_sock; 
sub login_onlineSvr;

#functions related to test cases, used as utilities.
sub get_deposit_info;
sub clear_deposit_info;
sub show_deposit_info;
sub deposit_bean;
sub draw_bean;


#test suite for the mole bank subsystem
sub ts_get_deposit_info;
sub ts_deposit_bean;
sub ts_draw_bean;

#test cases for each test suite
sub tc_1001;
sub tc_1002;
sub tc_1003;
sub tc_1101;

sub tc_2001;
sub tc_2002;
sub tc_2003;
sub tc_2004;
sub tc_2101;

sub tc_3001;
sub tc_3002;
sub tc_3003;
sub tc_3101;


#------------------------------------- TEST BEGIN ------------------------------------

&init_login_sock();
&login_loginSvr();

&init_online_sock();
&login_onlineSvr();

&ts_get_deposit_info();
&ts_deposit_bean();
&ts_draw_bean();


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
	$result = sprintf("%d", $result);
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
	$md5_str = "Bye bYe crAzy B Aby in Our ProgRAm?";
	$md5_key = sprintf("%u%.11s%u", unpack("x10N", $sess_body),
						substr($md5_str, 5, 11), unpack("x3N", $sess_body));
	$digest = md5_hex($md5_key);
	$digest = substr($digest, 6, 16);
	$sendbuf = pack("NcN3 na16Na16", 55, 1, 201, $login_uid, 0, 
					49, $digest, 16, $sess_body) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

	&recv_respond_by_cmd(201);
	is($result, 0, "login online svr ok");

	return 0;
} 

sub ts_get_deposit_info
{
	&tc_1001();
SKIP: 
	{
	skip(' never do this now ', 1) if 1;
	&tc_1101();

	}
}

sub ts_deposit_bean
{
	&tc_2001();
	&tc_2002();
	&tc_2003();
	&tc_2004();

	&tc_2101();
SKIP: 
	{
	skip(' never do this now ', 1) if 1;

	}
}

sub ts_draw_bean
{
	&tc_3001();
	&tc_3002();
	&tc_3003();

	&tc_3101();
SKIP: 
	{
	skip(' never do this now ', 1) if 1;

	}
}


sub tc_1001 
{
	diag("----------------------ENTER TC_1001-----------------------");
	&get_deposit_info();
	ok($dep_count <= 5 && $dep_count >= 0, "test the basic get info cmd");

	return;
}


sub tc_1002 
{
	diag("----------------------ENTER TC_1002-----------------------");
	
	return;
}
 
sub tc_1101
{
	diag("----------------------ENTER TC_1101-----------------------");
	my $oldid = $login_uid;
	$login_uid = 0;
	&get_deposit_info();
	$login_uid = $oldid;	
}


sub tc_2001
{
	diag("----------------------ENTER TC_2001-----------------------");

	&clear_deposit_info();

	&get_cur_molebean();
	my $old_molebean = $cur_molebean;
	
	diag("deposit 2000 beans");
	&deposit_bean(2000, 1, 1); #make the test
	is( $result, 0, "deposit should success");

	&get_deposit_info();
	&get_cur_molebean();
	diag("TEST CASE 2001 [old bean=$old_molebean current bean=$cur_molebean]");
	ok( $dep_count == 1 && $deposit_bean[0] == 2000 && $cur_molebean == $old_molebean - 2000 ,
			"test the basic deposit process");
}

sub tc_2002
{
	diag("----------------------ENTER TC_2002-----------------------"); &clear_deposit_info();

	&get_cur_molebean();
	my $old_molebean = $cur_molebean;

	diag("deposit 500 beans");
	&deposit_bean(500, 1, 1); #make the test
	is( $result, -11171, "deposit should fail because of deposit lt 1000 beans");

	&get_deposit_info();
	&get_cur_molebean();
	diag("TEST CASE 2002 [old bean=$old_molebean current bean=$cur_molebean]");

	ok( $dep_count == 0 && $cur_molebean == $old_molebean, 
			"test the deposit bean is lt 1000 case");
}

sub tc_2003
{
	diag("----------------------ENTER TC_2003-----------------------");
	&get_cur_molebean;
	my $old_molebean = $cur_molebean;

	diag("TEST CASE 2003 deposit many many beans");
	&deposit_bean($cur_molebean + 1000, 0, 2);
	is( $result ,-11117 , "deposit should fail because of cur mole bean is not enough");

	&get_cur_molebean;
	is($old_molebean, $cur_molebean, "test the cur mole bean is not enough"); 
}

sub tc_2004
{
	diag("----------------------ENTER TC_2004-----------------------");
	&clear_deposit_info();

	&get_cur_molebean();
	my $old_molebean = $cur_molebean;

	my $i;
	for ($i=0; $i<6; $i++ ) {
		&deposit_bean(2000, 1, 1); #make the test
		sleep(2);
	}

	is( $result , -11170, "test the deposit count gt 5 case");

}

sub tc_2101
{
	diag("----------------------ENTER TC_2101-----------------------");
	&clear_deposit_info();

	&get_cur_molebean();
	my $old_molebean = $cur_molebean;

	&deposit_bean(2000, 1, 1); #make the test
	is( $result , 0, "test deposit occur in same second");

	&deposit_bean(2000, 1, 1); #make the test
	is( $result , -11174, "test deposit occur in same second");
}

sub tc_3001
{
	diag("----------------------ENTER TC_3001-----------------------");
	&clear_deposit_info();

	diag("deposit 2000 beans");
	&deposit_bean(2000, 0, 1); #make the test
	sleep(65);

	&get_cur_molebean();
	my $old_molebean = $cur_molebean;

	&get_deposit_info();
	&draw_bean( $deposit_time[0]);
	diag("TEST CASE 3001 [interest=$draw_interest drawbeans=$draw_bean]"); 
	is( $result , 0, "test the basic draw process");

	&get_cur_molebean();

	ok( $cur_molebean - $old_molebean > 2000, 
			"draw bean success, interest should be calculated ");
}

sub tc_3002
{
	diag("----------------------ENTER TC_3002-----------------------");
	&clear_deposit_info();
	diag("deposit 2000 beans");
	&deposit_bean(2000, 0, 1);

	&get_deposit_info();
	&draw_bean($deposit_time[0]);

	is($result, -11172, "test the draw when drawout is not permitted case");
}

sub tc_3003
{
	diag("----------------------ENTER TC_3003-----------------------");
	&clear_deposit_info();
	diag("deposit 2000 beans");
	&deposit_bean(2000, 1, 2);

	&get_deposit_info();
	&draw_bean($deposit_time[0]);

	is($result, 0, "test the draw permitted case");
	is($draw_bean, 2000, "draw the correct bean with no interest");
	is($draw_interest, 0, "should not have interest");
}
 
sub tc_3101
{
	diag("----------------------ENTER TC_3101-----------------------");
	&clear_deposit_info();
	diag("deposit 2000 beans");
	&deposit_bean(2000, 1, 1);

	&get_deposit_info();
	&draw_bean($deposit_time[0]);
	is($result, 0, "test draw beans in a second");

	&draw_bean($deposit_time[0]);
	is($result, -11173, "test draw beans in a second");
	
}

#--------------------------------------- UITILITIES -------------------------------------------
sub get_deposit_info
{
	$sendbuf = pack("NcN3 ", 17, 1, 1201, $login_uid, 0) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";
	
again:
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:get_deposit_info [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
	if ( $length == 17 ) {
		return;
	}

	read($sock, $recvbuf, $length - 17);
	goto again if ( $cmdid != 1201 );

	$dep_count = unpack("x4N", $recvbuf);
	$recvbuf = substr($recvbuf, 8);
	diag("FUNC:get_deposit_info [deposit count=$dep_count]"); 
	for ($i=0; $i<$dep_count; $i++) {
		($deposit_bean[$i], $deposit_drawout[$i], $deposit_time[$i], $deposit_interval[$i]) = unpack("N4", $recvbuf);
		$recvbuf = substr($recvbuf, 16);
	}
}

sub clear_deposit_info
{
again:
	&get_deposit_info(); #this may be dup, but need it in case working copy data is garbige

	for ($i=0; $i<$dep_count; $i++) {
		&draw_bean($deposit_time[$i]);
	}
	&get_deposit_info(); #this may be dup, but need it in case working copy data is garbige
	if ( $dep_count > 0 ) {
		diag("waiting to clear the deposit info");
		sleep 65;
		goto again;
	}
	return;
}
 
sub show_deposit_info
{
	print mole_bank_log "show deposit info-----------------------\n";
	print mole_bank_log "[count=$dep_count, ";
	for ($i=0; $i<$dep_count; $i++) {
		print mole_bank_log "bean=$deposit_bean[$i], drawout=$deposit_drawout[$i], 
			deposit time=$deposit_time[$i], interval=$deposit_interval[$i]]\n";
	}
	return;
}
 
sub deposit_bean
{
	( @_ == 3 ) or die "invoke func error"; 
	my $beans = $_[0];
	my $drawout = $_[1];
	my $interval = $_[2];

	$sendbuf = pack("NcN3 N3", 29, 1, 1202, $login_uid, 0, 
					$beans, $drawout, $interval) ;
	send($sock, $sendbuf, 0) || die "send error $!\n";

again:
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:deposit_bean [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
	if ( $length == 17 ) {
		return;
	}

	read($sock, $recvbuf, $length - 17);
	goto again if ( $cmdid != 1202 );


	return;
}

sub draw_bean
{
	( @_ == 1 ) or die("param error");

	my $dep_time = $_[0];		
	$sendbuf = pack("NcN3 N", 21, 1, 1203, $login_uid, 0, $dep_time);
	send($sock, $sendbuf, 0) || die "send error $!\n";

again:
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:draw_bean [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
	if ( $length == 17 ) {
		return;
	}

	read($sock, $recvbuf, $length - 17) || die "recv error $!\n";
	goto again if ( $cmdid != 1203 );

	($draw_interest, $draw_bean) = unpack("NN", $recvbuf);
	diag("FUNC:draw_bean [interest=$draw_interest drawbean=$draw_bean]");
	return;
}

sub get_cur_molebean
{
	$sendbuf = pack("NcN3 ", 17, 1, 10013, $login_uid, 0);
	send($sock, $sendbuf, 0) || die "send error $!\n";

again:	
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:get_cur_molebean [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");
	if ( $length == 17 ) {
		return;
	}

	read($sock, $recvbuf, $length - 17) || die "recv error $!\n";
	goto again if ( $cmdid != 10013 );

	$cur_molebean = unpack("N", $recvbuf);
	diag("FUNC:get_cur_molebean get current mole bean success [current bean=$cur_molebean]");
	return;
}

sub recv_from_online_by_cmd
{
	die("param error") if @_ != 1;
	$expect_cmd = $_[0];
again:	
	read($sock, $recvbuf, 17) || die "recv error $!\n";
	$length = unpack("N", $recvbuf);
	$cmdid = unpack("x5N", $recvbuf);
	$uid = unpack("x9N", $recvbuf); 
	$result = unpack("x13N", $recvbuf); 
	$result = sprintf("%d", $result);
	diag("FUNC:recv_from_online_by_cmd [recvlen=$length] [cmdid=$cmdid] [uid=$uid] [result=$result]");

	if ( $cmdid != $expect_cmd ) {
		if ( $length == 17 ) {
			goto again;
		}
		read($sock, $recvbuf, $length - 17) || die "recv error $!\n";
		goto again;
	}
	if ( $length == 17 ) {
		return;
	}
	read($sock, $recvbuf, $length - 17) || die "recv error $!\n";
	return;
}
