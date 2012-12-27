<?php

require_once('../common/global.inc');

class FormParser 
{
	private $_form_array;

	function __construct() {
		$this->_form_array = array();
	}

	function __destruct() {
	}

	function print_member() {
		if (is_array($this->_form_array)) 
			print_r($this->_form_array);
	}


	function parse_form() {
		global $g_base_keys;
		global $g_option_keys;

		foreach($g_base_keys as $key) {
			if (!isset($_POST[$key])) {
				echo("base_key is not set: ".$key."\n");
				return false;
			} else
				$this->_form_array[$key] = $_POST[$key];
		}

		foreach($g_option_keys as $key) {
			if (isset($_POST[$key]))
				$this->_form_array[$key] = $_POST[$key];
		}

		return true;
	}

	function get_val($key) {
		if (isset($this->_form_array[$key]))
			return $this->_form_array[$key];
		else {
			echo("key is not set: ".$key."\n");
			return -1;
		}
	}
/*
	function check_keys($cmdid) {
		switch ($cmdid) {
		case G_CMD_ADDPIC:
			foreach($g_addpic_keys as $key)
		}

	}
*/
}

?>

