#!/bin/sh
php test_filter.php 50077 1 "AA" &
php test_filter.php 50077 1 "BB" &
php test_filter.php 50077 1 "CC" &
php test_filter.php 50077 0 "DD" &
php test_filter.php 50077 0 "EE" &
php test_filter.php 50078 1 "FF" &
php test_filter.php 50078 1 "GG" &
php test_filter.php 1234567 1 "HH" &
php test_filter.php 1234567 1 "II" &
php test_filter.php 1234567 1 "JJ" &
