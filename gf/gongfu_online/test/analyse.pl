#!/usr/bin/perl -w

#`sort -k 2.5 totalfailed -o totalfailed_sort`;
#`sort -k 2.5 totalsuccess -o totalsuccess_sort`;

open(failed_sort, "<./totalfailed_sort") || die("cannot open sorted failed file");
open(success_sort, "<./totalsuccess_sort") || die("cannot open sorted success file");

open(failed_result, ">./failed_result") || die("cannot open failed_result");

@uid_succ_arr;
$i = 0; 
while ($line_succ = <success_sort>) {
	$line_succ =~ m/\[([0-9:]+)\].*uid=(\d+)\sip=(\w+)/i;
	$uid_succ_arr[$i++] = $2;
}
$uid_succ_arr_cnt = $i;

$final_fail_cnt = 0;
$final_succ_cnt = 0;
$final_fail_uiderr_cnt = 0;
$final_fail_passwd_cnt = 0;
$pre_uid = -1;
$uid_cnt = 0;
$i = 0;
while ($line_failed = <failed_sort> ) {
	$line_failed =~ m/\[([0-9:]+)\].*uid=(\d+)\sdberr=(\d+)\sip=(\w+)/i;
	$time = $1;
	$uid = $2;
	$dberr = $3;
	$ip = $4;

	if ($uid ne $pre_uid) { 
		if ( $pre_uid != -1 ) {
			print failed_result "uid=$pre_uid failed $uid_cnt times\n";
			#check the success uid
			while ( $i < $uid_succ_arr_cnt ) {
				if ( $pre_uid lt $uid_succ_arr[$i] ) {
					print failed_result "uid=$pre_uid finally login failed\n";
					if ( $dberr == 1105 ) {
						$final_fail_uiderr_cnt++;
					}
					if ( $dberr == 1103 ) {
						$final_fail_passwd_cnt++;
					}
					$final_fail_cnt++;
					last;
				} elsif ( $pre_uid eq $uid_succ_arr[$i] ) {
					print failed_result "uid=$pre_uid finally login succeeded\n";
					$final_succ_cnt++;
					$i++;
					last;
				} else {
					$i++;
				}
			}
		}
		$uid_cnt = 1;
	} else {
		$uid_cnt++;
	}
	$pre_uid = $uid;
}

$total = $final_succ_cnt + $final_fail_cnt;
$fail_per = $final_fail_cnt/$total;
$succ_per = $final_succ_cnt/$total;
$fail_err_uid = $final_fail_uiderr_cnt/$final_fail_cnt;
$fail_err_passwd = $final_fail_passwd_cnt/$final_fail_cnt;
print "final success percent: $succ_per\n";
print "final fail percent: $fail_per\n";
print "uid fail percent: $fail_err_uid\n";
print "passwd fail percent: $fail_err_passwd\n";

