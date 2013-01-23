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

        while (true)
        {
            $index = rand(0, count($this->file_list_) - 1);
            $filename = $this->file_list_[$index]['name'];
            $md5 = $this->file_list_[$index]['md5'];

            $this->socket_->send('get ' . $filename . "\r\n");
            $content = '';
            $stars = 0;

            for (;;)
            {
                $data = $this->socket_->receive();

                if (false === $data)
                {
                    $this->set_error_('fail to receive');
                    return false;
                }

                if (strlen($data))
                {
                    $content .= $data;

                    if (strlen($content) >= 10)
                    {
                        if ('*********' == substr($content, -9))
                        {
                            $result = substr($content, -10, 1);

                            if (0 == $result)
                            {
                                $saveas = './download/'
                                          . $filename . '_' . strtotime('now');
                                $file_content = substr($content,
                                                       0,
                                                       strlen($content) - 10);

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
                            else
                            {
                                $this->set_error_('server[' . $result . ']');
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
}
