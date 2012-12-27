set pwd "ta0mee@nh\r"
set ept "#"
set ip "114.80.98.29"
set timeout 15

spawn ./scp
expect {
        -re "password:" { send $pwd; expect { 
                            -re  $ept { }
                            -re  "password:" { send $pwd; expect $ept } 
                          }
        }
        -re $ept { }     -re "(yes/no).*" { send "yes\r"; expect "password:"; send $pwd; expect $ept     } 
        timeout { send_user "$ip connection time out\r\n" ; close ; return }
}

spawn ssh -p56000 -lroot "$ip"
expect {
        -re "password:" { send $pwd; expect { 
                        -re  "#" { }
                        -re  "password:" { send $pwd; expect "#" } 
                }
        }
        -re "#" { }
        -re "(yes/no).*" { send "yes\r"; expect "password:"; send $pwd; expect "#"   }
        timeout { send_user "$ip connection time out\r\n" ; close ; return }
}

send "cd /opt/taomee/gameservtest; mv StableGameserv GameservTest -f; rm ./log/* -f; ./startup.sh; logout\r"
expect ":~#" 
