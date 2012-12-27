<?php

require_once(dirname(__FILE__).'/../common/error.inc');
require_once(dirname(__FILE__).'/./mcrypt.inc');

class AuthSession 
{
	var $_uid = 0;
	var $_time = 0;
	var $_ip = '';
	var $_port = '';
	var $_mcryptor = NULL;
	var $_key;
	private $_session_timeout = 600;	// 10 mins

	function __construct($con_key) {
		$this->_key = $con_key;
		$this->_mcryptor = new Mcrypt($this->_key);
	}

	function __destruct() {
	}

	function decode_session($session) {
		// check session length
		if (!$this->_mcryptor)
			return false;

		if ( strlen($session) != 16 ) 
			return false;
		
		//$session_array = unpack("A8session1/A8session2", $session);
	
		// decrypt session
		if ($this->_mcryptor->init() < 0) 
			return false;
	
		//$decrypted_session1 = $this->_mcryptor->decrypt($session_array[session1]);
		//$decrypted_session2 = $this->_mcryptor->decrypt($session_array[session2]);
		$decrypted = $this->_mcryptor->decrypt($session);
		$this->_mcryptor->fini();
	
		//return $this->parse($decrypted_session1.$decrypted_session2);
		return $this->parse($decrypted);
	}

	function check_uid($uid) {
		if (strncmp($uid, $this->_uid, 12) == 0)
			return true;
		else {
			dbg_log("uid: ".$uid."\tsession.uid: ".$this->_uid);
			return false;
		}
	}

	function check_time($time) {
		if ( $time < $this->_time ||
			($time-$this->_time) < $this->_session_timeout ) 
			return true;
		else {
			dbg_log("time: ".$time."\tsession.time: ".$this->_time);
			return false;
		}
	}

	function check_ip($ip) {
		if (strncmp($ip, $this->_ip, 22) == 0)
			return true;
		else {
			dbg_log("ip: ".$ip."\tsession.ip: ".$this->_ip);
			return false;
		}
	}

	function check_port($port) {
		if (strncmp($port, $this->_port, 6) == 0)
			return true;
		else {
			dbg_log("port: ".$port."\tsession.port: ".$this->_port);
			return false;
		}
	}

	// just for test
	function print_member() {
		$members =  "decrypt session: \n".
					"uid: ".$this->_uid."\n".
					"time: ".$this->_time."\n".
					"ip: ".$this->_ip."\n".
					"port: ".$this->_port."\n";
		dbg_log($members);
	}
	
	private function parse($session)
	{
		if (strlen($session) != 16)
			return false;
	
		$session_array = unpack("Nuid/Ntime/A4ip/Nport", $session);
		$this->_uid = $session_array[uid];
		$this->_time = $session_array[time];
		$this->_ip = inet_ntop($session_array[ip]);
		$this->_port = $session_array[port];

		return true;
	}

}

?>
