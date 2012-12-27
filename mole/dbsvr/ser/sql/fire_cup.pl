#!/usr/bin/perl -w

use DBI;
use Data::Dumper;
use strict;
my $dbh = DBI->connect("DBI:mysql:database=SYSARG_DB;host=localhost", "root", "ta0mee", {'RaiseError' => 1}) or die;

my $date=`date "+%Y%m%d"`;
$date-=1;
my $sth = sprintf "SELECT type, value FROM SYSARG_DB.t_sys_arg where type>=100 and type<=104 order by type ASC";
my $team_num=$dbh->selectall_arrayref($sth);
for(my $i=0; $i<5; $i++) {
	my $sth = sprintf "update SYSARG_DB.t_sysarg_team_medal set num = $team_num->[$i][1] where\
			  team = $team_num->[$i][0]-99 and date=$date order by team ASC";
	$dbh->do($sth);
}

$date=`date "+%Y%m%d"`;
$date-=1;
print $date;
$sth = sprintf "SELECT team, gold, silver, copper, num FROM SYSARG_DB.t_sysarg_team_medal where date=%d order by team ASC", $date;
my $array_temp=$dbh->selectall_arrayref($sth);
for (my $i=0; $i<5; $i++) {
	for(my $j=1; $j<4; $j++) {
	$array_temp->[$i][$j] = $array_temp->[$i][$j] / $array_temp->[$i][4];
	}
}
print Dumper($array_temp);
for(my $i = 0; $i < 5; $i++) {
	$sth = sprintf "insert into SYSARG_DB.t_sysarg_team_medal values(0, $array_temp->[$i][0] + 5,\
	   $array_temp->[$i][1] * 100, $array_temp->[$i][2] * 100, $array_temp->[$i][3]*100, 0)";
	my $result = $dbh->do($sth);
}

$sth = sprintf "SELECT team, gold, silver, copper, num FROM SYSARG_DB.t_sysarg_team_medal where team>=6 and team<=10\
	           order by gold ASC, silver ASC, copper ASC";
my $array=$dbh->selectall_arrayref($sth);
$sth = sprintf "delete from SYSARG_DB.t_sysarg_team_medal where team>=6 and team<=10";
my $rows=$dbh->do($sth);
print Dumper($array);

$sth = sprintf "update SYSARG_DB.t_sys_arg set value=0 where type>=115 and type<=119";
$rows = $dbh->do($sth);

$sth = "SELECT type, value FROM SYSARG_DB.t_sys_arg WHERE type>=110 AND type<=119 order by type ASC";
my $array_npc=$dbh->selectall_arrayref($sth);
for(my $i=0; $i<5; $i++) {
	for(my $j=0; $j<5; $j++) {
		if (($array->[$i][0] - 5) == ($array_npc->[$j][0] - 109)) {
			$array_npc->[$j][1] += $i + 1;
			if ($array_npc->[$j][1] == 33) {
				$array_npc->[$j][1] += 1;
				$array_npc->[$j + 5][1] = 1;
			}
			if ($array_npc->[$j][1] == 3) {
				$array_npc->[$j][1] += 3;
				$array_npc->[$j + 5][1] = 2;
			}
			if ($array_npc->[$j][1] == 13 || $array_npc->[$j][1]==48 || $array_npc->[$j][1]==69) {
				$array_npc->[$j][1] += 2;
				$array_npc->[$j + 5][1] = 3;
			}

			if ($array_npc->[$j][1] == 22) {
				$array_npc->[$j][1] += 4;
				$array_npc->[$j + 5][1] = 4;
			}
			if ($array_npc->[$j][1] == 26 || $array_npc->[$j][1] == 80) {
				$array_npc->[$j][1] -= 2;
				$array_npc->[$j + 5][1] = 5;
			}

			if ($array_npc->[$j][1] == 17 || $array_npc->[$j][1] == 29 || $array_npc->[$j][1]==45 || $array_npc->[$j][1] == 59 || $array_npc->[$j][0] == 75) {
				$array_npc->[$j + 5][1] = 6;
			}
		}
	}
}

for(my $i=0; $i<10; $i++) {
	$sth = sprintf "update SYSARG_DB.t_sys_arg  set value = $array_npc->[$i][1] where type = $array_npc->[$i][0]";
	$rows = $dbh->do($sth);
	print $sth;
}


$sth = sprintf "SELECT type, value FROM SYSARG_DB.t_sys_arg where type>=100 and type<=104 order by type ASC";
$team_num=$dbh->selectall_arrayref($sth);

$date=`date "+%Y%m%d"`;
my $gold_add = 0;
for(my $i=0; $i<5; $i++) {
	if ($array_npc->[$i + 5][1] == 6) {
		$gold_add=$team_num->[$i][1];
		$sth = sprintf "update SYSARG_DB.t_sysarg_team_medal set gold =gold+$gold_add where team=$i + 1 and date =$date";
		$rows = $dbh->do($sth);
	}
}

$sth = "SELECT type, value FROM SYSARG_DB.t_sys_arg WHERE type>=120 AND type<=129 order by type ASC";
my $array_task=$dbh->selectall_arrayref($sth);
my $task_count=0;
for (my $i = 0; $i < 5; $i++) {
	$task_count=0;
	for(;;) {
		my $index = int(rand(7));
		if ($index > 6) {
			$index = 6;
		}
		if (($array_task->[$i + 5][1] & (1 << $index)) == 0) {
			$array_task->[$i][1] = $index + 1;
			$array_task->[$i + 5][1] |= (1 << $index);
			last;
		}
		$task_count++;
		if ($task_count > 100000) {
			last;
		}
	}
}

for(my $i=0; $i<10; $i++) {
	$sth = sprintf "update SYSARG_DB.t_sys_arg  set value = $array_task->[$i][1] where type=$array_task->[$i][0]";
	my $rows = $dbh->do($sth);
	print "$rows row(s) affected ";
}


