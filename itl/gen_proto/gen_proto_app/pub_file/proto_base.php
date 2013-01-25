<?php
define( "SUCC" ,							0	);
function print_dbret( $arr ){
    $arr["pri_msg"]="";
    print_r($arr);
};


function buf_format($buf,$size){
    $buf_size=strlen($buf);
    if ($buf_size==$size){
        return $buf;
    }else if ($buf_size<$size )	{
        $ex_str="";
        $ex_len=$size-$buf_size;
        $ex_char=pack("C",0);

        for($i=0;$i<$ex_len;$i++ ) {
            $ex_str.=$ex_char;
        }
        return $buf.$ex_str;
    }else{
        return  substr($buf,0,$size );
    }
}	
/*
a - NUL-padded string
A - SPACE-padded string
h - Hex string, low nibble first
H - Hex string, high nibble first
c - signed char
C - unsigned char
s - signed short (always 16 bit, machine byte order)
S - unsigned short (always 16 bit, machine byte order)
n - unsigned short (always 16 bit, big endian byte order)
v - unsigned short (always 16 bit, little endian byte order)
i - signed integer (machine dependent size and byte order)
I - unsigned integer (machine dependent size and byte order)
l - signed long (always 32 bit, machine byte order)
L - unsigned long (always 32 bit, machine byte order)
N - unsigned long (always 32 bit, big endian byte order)
V - unsigned long (always 32 bit, little endian byte order)
f - float (machine dependent size and representation)
d - double (machine dependent size and representation)
 */
class Cbyte_array
{
    private $is_read_mode;
    private $m_buf;
    private $m_size;//只在read 使用
    private $m_postion;//只在read 使用
    private $m_is_bigendian;//是否是大端
    private $pack_fmt_map = 
        array(
            // 小端 大端
            "uint32" => array ("V","N"),
            "int32" => array ("l","L"),
            "uint16" => array ("v","n"),
            "int16" => array ("s","n"), //int16 大端 不存在,只好使用"uint16" 大端,代码部分特别处理
        );


    public function get_frm_char($type_str)
    {
        $arr = $this->pack_fmt_map[$type_str];
        if ($arr)
        {
            if ($this->m_is_bigendian) 
            {
                return $arr[1];
            }
            else
            {
                return $arr[0];

            }
        }
        else
        {
            return "";
        }
    }	

    public function is_end()
    {
        return $this->m_postion == $this->m_size;
    }

    public function get_buf()
    {
        return ($this->m_buf);
    }

    public function set_is_bigendian($value)
    {
        $this->m_is_bigendian = $value;
    }	

    public function read_single_value(&$value, $item_size, $unpack_fmtstr)
    {
        if (! $this->is_read_mode)
        {
            return false;
        }
        if ($this->m_postion + $item_size > $this->m_size)
        {
            return false;
        }
        else
        {
            $arr = unpack($unpack_fmtstr, substr($this->m_buf, $this->m_postion, $item_size));
            $value = $arr[1];
            $this->m_postion += $item_size;
            return true;
        }
    }

    public function write_single_value($value, $unpack_fmtstr)
    {
        if ($this->is_read_mode)
        {
            return false;
        }

        $this->m_buf .= pack($unpack_fmtstr, $value);
        return true;
    }

    public function write_buf($buf, $size)
    {
        if ($this->is_read_mode)
        {
            return false;
        }
        $this->m_buf .= buf_format($buf, $size);
        return true;
    }

    public function read_buf(&$buf, $len)
    {
        if (! $this->is_read_mode)
        {
            return false;
        }
        if ($this->m_postion + $len > $this->m_size)
        {
            return false;
        }
        else
        {
            $buf = substr($this->m_buf, $this->m_postion, $len);
            $this->m_postion += $len;
            return true;
        }
    }

    public function read_int32(&$value)
    {
        return $this->read_single_value($value, 4, $this->get_frm_char("int32"));
    }

    public function read_int16(&$value ){
        $ret=$this->read_single_value($value,2, $this->get_frm_char("int16") );
        if ($value>=0x8000){
            $value=-($value-0x8000);
        }
        return $ret;
    }

    public function read_int8(&$value ){
        return $this->read_single_value($value,1,"c");
    }

    public function write_int32($value ){
        return $this->write_single_value($value,$this->get_frm_char("int32") );
    }

    public function write_int16($value ){
        if ($value<0){
            $value=0xFFFF+$value+1;
        }
        return $this->write_single_value($value,$this->get_frm_char("int16") );
    }

    public function write_int8($value){
        return $this->write_single_value($value,"c");
    }

    public function write_double($value){
        return $this->write_single_value($value,"d");
    }

    public function read_double(&$value ){
        return $this->read_single_value($value,8,"d");
    }



    public function read_uint32(&$value ){
        return $this->read_single_value($value,4,$this->get_frm_char("uint32"));
    }

    public function read_uint16(&$value ){
        return $this->read_single_value($value,2,$this->get_frm_char("uint16"));
    }

    public function read_uint8(&$value ){
        return $this->read_single_value($value,1,"C");
    }

    public function write_uint32($value ){
        return $this->write_single_value($value,$this->get_frm_char("uint32"));
    }

    public function write_uint16($value ){
        return $this->write_single_value($value,$this->get_frm_char("uint16"));
    }

    public function write_uint8($value){
        return $this->write_single_value($value,"C");
    }


    public function read_uint64(&$value ){
        return $this->read_buf($value,8);
    }

    public function write_uint64($value){
        return $this->write_buf("",8);
    }

    public function read_int64(&$value ){
        return $this->read_buf($value,8);
    } 

    public function write_int64($value){
        return $this->write_buf("",8);
    }




    public function init_read_mode($buf)
    {
        $this->is_read_mode=true;
        $this->m_buf=$buf;
        $this->m_size=strlen($buf);
        $this->m_postion=0;
    }

    public function init_write_mode()
    {
        $this->is_read_mode=false;
        $this->m_buf="";
    }

};


function hex2bin_pri($hexdata) {
    $bindata = '';
    for($i=0; $i < strlen($hexdata); $i += 2) {
        $bindata .= chr(hexdec(substr($hexdata, $i, 2)));
    }
    return $bindata;
}

function des($key,$msg) 
{
    $td = mcrypt_module_open('des', '', 'ecb', ''); 
    $iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_RAND); 
    mcrypt_generic_init($td, $key, $iv); 
    $data = mcrypt_generic($td, $msg); 
    mcrypt_generic_deinit($td); 
    mcrypt_module_close($td); 
    return $data; 
}


class proto_socket
{
    //private:
    var $socket; //socket 句柄
    //var $debug = 1;
    function __construct($ip, $port)
    {
        $address = gethostbyname($ip);
        if (($this->socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP)) < 0)
        {
            trigger_error("Couldn't create socket: " . socket_strerror(socket_last_error()) . "\n");
        }
        $result = socket_connect($this->socket, $address, $port);
        $timeout = array("sec" => 60, "usec" => 0);
        socket_set_option($this->socket, SOL_SOCKET, SO_RCVTIMEO, $timeout);
        socket_set_option($this->socket, SOL_SOCKET, SO_SNDTIMEO, $timeout);
        #socket_connect socketfd, SOL_SOCKET, SO_RCVTIMEO, 

    }
    function log_msg($msg)
    {
        global $_SESSION;
        $log_pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid",$msg);
        if ( ($log_pkg_arr["commandid"] & 0x0100) ==0x0100) {
            //dbg_log( $_SESSION["adminid"].":".  dechex($log_pkg_arr["commandid"] ).
            //		":".$log_pkg_arr["userid"].":".bin2hex($msg));
        }
    }

    function sendmsg($msg)
    {
        //$this->log_msg($msg);
        socket_write($this->socket, $msg, strlen($msg));
        $buf = socket_read($this->socket, 8192);
        $pkg_arr = @unpack("Llen", $buf);
        $len = $pkg_arr["len"];
        #echo $proto_len;
        while ($len != strlen($buf))
        {
            $buf .=	socket_read($this->socket, 4096);
        }
        return $buf;
    }

    function sendmsg_without_returnmsg($msg)
    {
        //$this->log_msg($msg);
        $result = socket_write($this->socket, $msg, strlen($msg));
        if ($result)
        {
            return array("ret"=>0);
        }else
            return array("ret"=>1003);
    }

    function close()
    {
        socket_close($this->socket);
    }

    function __destruct()
    {
    }
} 


class Cproto_base
{
    var $sock; 
    var $header_len;
    var $unpack_header_fmt;

    function __construct($server_ip, $server_port)
    {
        $this->sock = new proto_socket ($server_ip, $server_port);
        $this->set_header_len(18);
        $this->set_unpack_header_fmt("Llen/Lseq/Scmd/Lret/Luid");
    }

    function __destruct()
    {
        if ($this->sock)
        {
            $this->sock->close(); 
        }
    }

    function set_header_len($len)
    {
        $this->header_len = $len;
    }

    function set_unpack_header_fmt($fmt)
    {
        $this->unpack_header_fmt = $fmt;
    }



    //function park($cmdid,$userid,$private_msg){
    //return $this->pack($cmdid,$userid,$private_msg);
    //}
    //function pack($cmdid,$userid,$private_msg){
    //global $_SESSION;
    ////18：报文头部长度
    //$pkg_len=18+strlen($private_msg) ;
    //$result=0;
    //$proto_id=$this->adminid;
    //if ($proto_id==0){
    //$proto_id=$_SESSION["adminid" ];
    //}
    //return pack("L2SL2",$pkg_len,$proto_id,hexdec($cmdid),$result,$userid)
    //.$private_msg;
    //}

    function pack_header($cmd, $uid, $body_len)
    {
        //18：报文头部长度
        $len = $this->header_len + $body_len;
        $ret = 0;
        $seq = 0;

        return pack("L2SL2", $len, $seq, $cmd, $ret, $uid);
    }

    function pack($cmd, $uid, $body, $md5_tag = 0)
    {
        return ($this->pack_header($cmd, $uid, strlen($body))) . $body;
    }

    //function unpark($sockpkg, $private_fmt=""){ 
    //return $this->unpack($sockpkg, $private_fmt );
    //}

    //function unpack($sockpkg, $private_fmt=""){
    ////echo bin2hex($sockpkg);
    //$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommand_id/Lresult/Luser_id",$sockpkg);
    ////$tmp_len=$pkg_arr["proto_len"];
    //if ($private_fmt!="" && $pkg_arr["result"]==0){//成功
    //$pkg_arr=@unpack("Lproto_len/Lproto_id/Scommandid/Lresult/Luserid/".$private_fmt, 
    //$sockpkg);
    //}	 
    //if ($pkg_arr){
    //return $pkg_arr;
    //}else{
    //return array("result"=>1010);
    //}	
    //}
    function unpack($pkg)
    {
        //echo bin2hex($sockpkg);
        $pkg_arr = @unpack($this->unpack_header_fmt, $pkg);
        if (!$pkg_arr)
        {
            return $pkg_arr;
        }

        $pkg_arr["body"] = "";

        if ($pkg_arr["ret"] == 0)
        {
            //成功
            $pkg_arr["body"] = substr($pkg, $this->header_len);
        }

        return $pkg_arr;
    }



    function send_cmd($cmdid, $userid, $in_obj, $out_obj, $md5_tag=0)
    {
        $ba = new Cbyte_array();
        $ba->init_write_mode();
        if ($in_obj)
        {
            $in_obj->write_to_buf($ba);
        }

        $sendbuf = $this->pack($cmdid, $userid, $ba->get_buf(), $md5_tag);
        $ret_arr = $this->unpack($this->sock->sendmsg($sendbuf));
        if ($ret_arr["ret"])
        {
            return $ret_arr;
        }

        if ($out_obj)
        {
            $ba->init_read_mode($ret_arr["body"]);
            //读取失败
            if (!$out_obj->read_from_buf($ba))
            {
                return $ret_arr;
            }
            //数据过多
            if (!$ba->is_end())
            {
                return $ret_arr;
            }
        }
        else
        {
            //不需要返回

            //但又有私有报文
            if ($ret_arr["body"] != "")
            {
                return $ret_arr;
            }
        }

        $ret_arr["ret"] = 0;
        $ret_arr["body"] = $out_obj;

        #合并数组返回
        return $ret_arr;
    }

};


?>
