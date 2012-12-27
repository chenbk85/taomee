<?php
require_once('../common/error.inc');

class Mcrypt
{
	// set default 
	var $_key;
	var $_algorithm = 'des';
	var $_mode = 'ecb';
	static $_td = false;

	function __construct($con_key) {
		$this->_key = $con_key;
		$this->setDefault();
	}

	function __destruct() {
		if ($this->_td) 
			$this->fini();
	}

	// return a negative value on error
	function init() {
	    // Open the cipher 
		$this->_td = mcrypt_module_open($this->_algorithm, '', $this->_mode, '');
		if (!$this->_td)
			return false;
	
	    // Create the IV and determine the keysize length, used MCRYPT_RAN on Windows instead 
		$size = mcrypt_enc_get_iv_size($this->_td);
		$iv = $this->my_mcrypt_create_iv($size);
	    //$iv = mcrypt_create_iv($size, MCRYPT_DEV_RANDOM);
		
	   	return mcrypt_generic_init($this->_td, $this->_key, $iv);
	}

	function fini() {
		if ($this->_td) {
			mcrypt_generic_deinit($this->_td);
			mcrypt_module_close($this->_td);
		}
		$this->_td = false;
	}

	function setMode($algorithm, $mode) {
		$this->_algorithm = $algorithm;
		$this->_mode = $mode;
	}

	function setKey($key) {
		$this->_key = $key;
	}

	function getKey() {
		return $this->_key;
	}

	function encrypt($data) {
		if ($this->_td == false) {
			if ($this->init() < 0) 
				return false;
		}

		return mcrypt_generic($this->_td, $data);
	}

	function decrypt($encrypted) {
		if ($this->_td == false) {
			if ($this->init() < 0)
				return false;
		} 
	
		return mdecrypt_generic($this->_td, $encrypted);
	}

	private function setDefault() {
		$_key = 'trust me';
		$_algorithm = 'des';
		$_mode = 'ecb';
		$_td = false;
	}

	private function my_mcrypt_create_iv($size) {
		$iv = '';
		for ($i =0; $i < $size; $i++) {
			$iv .= chr(rand(0,255));
		}
		return $iv;
	}
}

?>
