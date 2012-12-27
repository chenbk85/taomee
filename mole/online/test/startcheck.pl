#!/usr/bin/perl

if ( fork() == 0 ) {
	`./molebank_test.pl 50015 1>check1 2>&1`;
	exit 0;
}
`./molebank_test.pl 50220 1>check2 2>&1`;
exit 0;



