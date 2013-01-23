<?php
require 'socket_handler.class.php';

class random_request_file
{
    private $socket_;

    private $file_list_ = array();

    private $error_ = '';

    private function set_error_($error)
    {
        $this->error_ = $error;
    }

    public function get_error()
    {
        return $this->error_;
    }

    public function __construct()
    {
        $this->socket_ = new socket_handler();
    }

    public function init($host, $port, $file_list)
    {
        $ret = $this->socket_->connect($host, $port);

        if (false == $ret)
        {
            $this->set_error_('fail to connect ' . $host . ':' . $port);
            return false;
        }

        $this->file_list_ = $file_list;
        return true;
    }

    public function run()
    {
        $times_request = 0;
        $header_len = 2 * 2 + 2 * 4;

        while (true)
        {
            $index = rand(0, count($this->file_list_) - 1);
            $filename = $this->file_list_[$index]['name'];
            $md5 = $this->file_list_[$index]['md5'];

            $length = $header_len + strlen($filename);
            $user_id = rand(0, 9);
            $package = pack('SSLL', 0x1001, 0, $length, $user_id) . $filename;
            echo "\n" . $filename;
            $this->socket_->send($package);

            $content = '';

            for (;;)
            {
                $data = $this->socket_->receive();

                if (false === $data)
                {
                    $this->set_error_('fail to receive');
                    return false;
                }

                $content .= $data;

                if (strlen($content) >= $header_len)
                {
                    $up = unpack('Scmd/Sstatus/Llength/Luser_id', $content);

                    if (strlen($content) >= $up['length'])
                    {
                        $file_content = substr($content,
                                               $header_len,
                                               $up['length'] - $header_len);
                        $saveas = './download/'
                                  . $filename . '_' . strtotime('now');
                        file_put_contents($saveas, $file_content);

                        if ($md5 != strtolower(md5($file_content)))
                        {
                            $this->set_error_(
                                'md5 not match! file ' . $saveas
                                . '[' . md5($file_content)
                                . '<>' . $md5 . ']');
                            return false;
                        }

                        break;
                    }
                }
            }
        }
    }
}
