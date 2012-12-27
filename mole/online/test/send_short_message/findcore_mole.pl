#!/usr/bin/perl

$ip[0]="192.168.0.9";
$ip[1]="192.168.0.19";
$ip[2]="192.168.0.22";
$ip[3]="192.168.0.36";
$ip[4]="192.168.0.37";
$ip[5]="192.168.0.47";
$ip[6]="192.168.0.49";
$ip[7]="192.168.0.50";
$ip[8]="192.168.0.51";
$ip[9]="192.168.0.56";
$ip[10]="192.168.0.57";
$ip[11]="192.168.0.58";
$ip[12]="192.168.0.66";
$ip[13]="192.168.0.74";
$ip[14]="192.168.0.75";
$ip[15]="192.168.0.77";
$ip[16]="192.168.0.78";
$ip[17]="192.168.0.82";
$ip[18]="192.168.0.85";
$ip[19]="192.168.0.86";
$ip[20]="192.168.0.88";
$ip[21]="192.168.0.89";
$ip[22]="192.168.0.90";
$ip[23]="192.168.0.91";
$ip[24]="192.168.0.130";
$ip[25]="192.168.0.131";
$ip[26]="192.168.0.132";
$ip[27]="192.168.0.133";
$ip[28]="192.168.0.134";
$ip[29]="192.168.0.135";
$ip[30]="192.168.0.136";
$ip[31]="192.168.0.137";
$ip[32]="192.168.0.138";
$ip[33]="192.168.0.139";
$ip[34]="192.168.0.140";
$ip[35]="192.168.0.141";
$ip[36]="192.168.0.142";
$ip[37]="192.168.0.143";
$ip[38]="192.168.0.144";
$ip[39]="192.168.0.145";
$ip[40]="192.168.0.27";
$svr_cnt = 41;

use Expect;


$port = 56000;
$pwd ="@))(51mole\n";
$timeout = 10;
$tar_file="OnlineNewA";
$cmd_line= "taomee.*\$";
$log="./mole_core_info.log";
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
        $exp->expect($timeout,-re=>$cmd_line);
		print "here1\n";
		if (!($exp->match() =~ /$cmd_line/)) {
			exit -1;
		}
		print "here2\n";
        $exp->send("./lscore\r");
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
			$exp->send("mv -f online*/core* ./core_bak/$now\r");
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
@result = `grep \"online.*lscore\" $log`;
foreach(@result) {
	if ( $_ =~ /@.*(\.\d+)+.*:/ ) {
		$core_file_cnt++;
		push @msg_arr, "$1 ";
		print "$1\n";
	}
}

print "core file total count is $core_file_cnt\n";

if ( $core_file_cnt > 0 ) {
#james, sylar, andy, crow
	`/home/kal/project/online/test/send_short_message/sendsms 15000391951,15921954486,13761071357,13621606625,13917927682 "$core_file_cnt mole online core! @msg_arr"`;
}

exit 0;

sub connect_to_host {
    my $ip = shift;
    my $exp = Expect->spawn("ssh -p$port online\@$ip") or die "Can not spawn ssh";
    $exp->expect($timeout,"(yes/no)?", "password:");
    if($exp->match() eq "password:") {
        $exp->send($pwd);
    } elsif ($exp->match() eq "(yes/no)?") {
        $exp->send("yes\r");
        $exp->expect($timeout, "password:");
        $exp->send($pwd);
    } else {
		$exp->hard_close();
		exit -1;
	}
    return $exp;
}

