#!/usr/bin/perl

$ip[0]="192.168.2.66";
$ip[1]="192.168.2.69";
$ip[2]="192.168.2.71";
$ip[3]="192.168.2.72";
$ip[4]="192.168.2.70";
my $svr_cnt = 5;

use Expect;


$port = 56000;
$pwd ="@))(seer\n";
$timeout = 10;
$cmd_line= "taomee.*\$";
$log="./seer_core_info.log";
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
		$exp->send("ls -lih Online*/core*\r");
        $exp->expect($timeout,"directory");
        if($exp->match() eq "directory") {
        } else {
			print "here3\n";
            $exp->log_file($log);
		    $exp->print_log_file($exp->before());
			$exp->log_file(undef); #close log
			my $now = scalar localtime(time());
			$now =~ s/\s/_/g;
			$now =~ s/:/_/g;
			$exp->send("mkdir ./core_bak\r");
			$exp->expect(1,-re=>".*");
			$exp->send("mkdir ./core_bak/$now\r");
			$exp->expect(1,-re=>$cmd_line);
			$exp->send("mv -f ./Online*/core* ./core_bak/$now\r");
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
@result = `grep "online" $log`;
foreach(@result) {
	if ( $_ =~ /@.*(\.\d+)+.*:/ ) {
		$core_file_cnt++;
		push @msg_arr, "$1 ";
		print "$1\n";
	}
}

print "core file total count is $core_file_cnt\n";

if ( $core_file_cnt > 0 ) {
	#zog, evan, andy,crow
	`/home/kal/project/online/test/send_short_message/sendsms 13120781518,13916174663,13761071357,13621606625,13917927682 "$core_file_cnt seer online core! @msg_arr"`;
}

exit 0;

sub connect_to_host {
    my $ip = shift;
    my $exp = Expect->spawn("ssh -p$port online\@$ip") or die "Can not spawn ssh";
    $exp->expect($timeout,"(yes/no)?", "password:",
				[timeout => sub { $exp->soft_close(); exit;}]);
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

