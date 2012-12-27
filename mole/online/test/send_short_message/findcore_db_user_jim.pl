#!/usr/bin/perl

$ip[0]="192.168.0.3";
$ip[1]="192.168.0.5";
$ip[2]="192.168.0.30";
$ip[3]="192.168.0.31";
$ip[4]="192.168.0.33";
$ip[5]="192.168.0.34";
$ip[6]="192.168.0.43";
$ip[7]="192.168.0.44";
$ip[8]="192.168.0.67";
$ip[9]="192.168.0.68";
$ip[10]="192.168.0.69";
$ip[11]="192.168.0.70";
$ip[12]="192.168.0.71";
$ip[13]="192.168.0.72";
$ip[14]="192.168.0.73";
$ip[15]="192.168.0.79";
$ip[16]="192.168.0.40";
$ip[17]="192.168.0.42";
$ip[18]="192.168.0.62";
$ip[19]="192.168.0.32";
$ip[20]="192.168.0.41";
$ip[21]="192.168.0.61";
$ip[22]="192.168.0.54";
$ip[23]="192.168.0.55";
$ip[24]="192.168.0.4";
$ip[25]="192.168.0.59";
my $svr_cnt = 26;

use Expect;


$port = 56000;
$pwd ="xcwen142857\n";
$timeout = 10;
$cmd_line= "jim.*\$";
$log="./jim_db_core_info.log";
my $core_file_cnt = 0;


`>$log`;
open LOG, ">>", "$log";
$Expect::Log_Stdout = 0;


for(my $i = 0; $i < $svr_cnt; $i++) {
    $pid[$i] = fork();
    if ($pid[$i] > 0) {
        next;
    }
    elsif($pid[$i] eq 0) {
#work process!
        $exp = connect_to_host($ip[$i]);
		print "here1\n";
        $exp->expect($timeout,-re=>$cmd_line);
		if (!($exp->match() =~ /$cmd_line/)) {
			exit -1;
		}
		print "here2\n";
        $exp->send("ls -lih DB/ser/bin/core*\r");
        $exp->expect($timeout,"directory");
        if($exp->match() eq "directory") {
        } else {
			print "here3\n";
            $exp->log_file($log);
		    $exp->print_log_file($exp->before());
			$exp->print_log_file("$ip[$i] has core files");
			$exp->log_file(undef); #close log
			my $now = scalar localtime(time());
			$now =~ s/\s/_/g;
			$now =~ s/:/_/g;
			$exp->send("mkdir ./core_bak/\r");
			$exp->expect(1,-re=>".*");
			$exp->send("mkdir ./core_bak/$now\r");
			$exp->expect(1,-re=>".*");
			$exp->send("mv -f DB/ser/bin/core* ./core_bak/$now\r");
			$exp->expect(1,-re=>".*");
        }

        $exp->expect($timeout,-re=>$cmd_line);
        $exp->send("logout\r");
        $exp->soft_close();

        exit;
    }
}


for($i = 0; $i < $svr_cnt; $i++) {
    waitpid($pid[$i],0);
}


sendmsg:
my @msg_arr = ();
@result = `grep "has core" $log`;
foreach(@result) {
	if ( $_ =~ /(\.\d+)+\shas core/ ) {
		$core_file_cnt++;
		push @msg_arr, "$1 ";
		print "$1\n";
	}
}

print "core file total count is $core_file_cnt\n";

if ( $core_file_cnt > 0 ) {
	#jim, millar, andy, crow
#	`/home/kal/project/online/test/send_short_message/sendsms 13585574642,15202115826,13761071357,13621606625,13917927682 "$core_file_cnt db core! @msg_arr"`;
	`/home/kal/project/online/test/send_short_message/sendsms 13917927682 "$core_file_cnt db core! @msg_arr"`;
}

exit 0;

sub connect_to_host {
    my $ip = shift;
    my $exp = Expect->spawn("ssh -p$port jim\@$ip") or die "Can not spawn ssh";
    $exp->expect($timeout,"(yes/no)?", "password:");
    if($exp->match() eq "password:") {
        $exp->send($pwd);
    } elsif ($exp->match() eq "(yes/no)?") {
        $exp->send("yes\r");
        $exp->expect($timeout, "password:");
        $exp->send($pwd);
    } else {
		$exp->hard_close();
	}
    return $exp;
}

