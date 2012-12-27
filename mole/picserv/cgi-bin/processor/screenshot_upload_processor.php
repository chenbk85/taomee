<?php 

/**************************************************************************
 * POST FORM:
--HTTP_Request_whathellareyoudoing
Content-Disposition: form-data; name="screenshot"; filename="screenshot.jpg"
Content-Type: image/jpeg
 *
**************************************************************************/
require_once('../common/error.inc');
require_once('../auth/auth_session.inc');

// make a note of the directory that will recieve the uploaded file 
$screenshot_dir = SCREENSHOT_TMP_DIR;
$servip = $_SERVER['SERVER_ADDR'];
$port = $_SERVER['SERVER_PORT'];

// ==1==. parse http request
if (!isset($_SERVER[HTTP_USER_ID]))
	error("no user id");

$uid = $_SERVER[HTTP_USER_ID];
// post data is refered by $HTTP_RAW_POST_DATA
if (!isset($HTTP_RAW_POST_DATA))
	error($uid."no post data");
// content_length is set && content_length > 32
if (!(isset($_SERVER[CONTENT_LENGTH]) && $_SERVER[CONTENT_LENGTH] > 32))
	error($uid."content length error");

$post_array = unpack('A16session/A*image', $HTTP_RAW_POST_DATA);
// end parse http request

// ==2==.auth
$screen_key = 'pIc:Svr@';
$author = new AuthSession($screen_key);
$author->decode_session($post_array['session']);
if ( !$author->check_uid($uid) ||
	 !$author->check_time(time()) ||
	 !$author->check_ip($servip) ||
	 !$author->check_port($port) ) {
	dbg_log("uid: ".$uid."\t"."session: ".bin2hex($post_array['session']));
	error($uid."\tauth error");
}



// ==3==.create file
$now = @time(); 
$rand_dir = $screenshot_dir.'/'.rand(0,100);
if (!is_dir($rand_dir))
	mkdir($rand_dir, 0775);

while(file_exists($saved_file = $rand_dir.'/'.$now.'_'.$uid.".jpg")) 
{ 
    $now++; 
} 

$fp = fopen($saved_file, "w+");
if (!$fp)
	error($uid."fopen failed: ".$saved_file);
fwrite($fp, $post_array[image]);
fclose($fp);

// ==4==.create response
$relative_path = substr($saved_file, strlen(PICSERVER_ROOT));
$saved_url = $servip.':'.$port.$relative_path;

echo $saved_url;


?>
