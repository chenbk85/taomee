<?php

require_once('../auth/auth_session.inc');

// init data to be encrypted.
$uid = pack('N', 50000);
$time = pack('N', time());
$ip = inet_pton("10.1.1.7");
$port = pack('N', 80);
$session = $uid.$time.$ip.$port;

// encrypt session
$mcryptor = new Mcrypt();
$en_session = $mcryptor->encrypt($session);
//delete $mcryptor;

// auth session
$author = new AuthSession();
$author->decode_session($en_session);
$author->print_member();
//delete $author;

?>
