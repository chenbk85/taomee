1. 开启程序：startup.sh
2. 结束程序：pkill -9 程序名(如CacheSvr)

3. 主配置文件bench.conf的各项含义：
      log_dir        ./log           #日志文件存放目录
      log_level      8               #日志打印等级
      log_size       104857600       #每个日志文件的最大大小

      shmq_length    8192000         #共享内存大小，8M
      run_mode       background      #background表示程序将后台运行

      pkg_timeout     5              #接收进程和处理进程的超时时间
      worker_num      200            #子进程的数目
      max_user_id     100000000      #最大用户数
      max_game_id     50             #最大游戏数
      timer_step_time 1800           #和数据库的同步时间间隔
      
      send_sem_key     81111         #信号量key值
      recv_sem_key     82222         #信号量key值

      dbproxy_ip       10.1.1.5      #数据库的IP地址
      dbproxy_port     21001         #数据库的端口号

      game_score_file  ./data/game_score_file #存储游戏分数的文件
      bind_file		./conf/bind.conf      #服务器IP和端口号配置文件
      game_id_file	./conf/game_id_file.conf #存储游戏ID号和FLAG的配置文件
      timestamp_file  ./data/timestamp_file  #保存时间戳的文件
   
       
4. 配置文件bind.conf的各项含义：
      ip              #服务器的IP
      port            #服务器的端口
      type            #协议类型
      idle timeout    #超时时间
      

5. Cache Server目录树：
      cache_svr
        -- conf/               #存放cache server配置文件
            -- bench.conf      #主配置文件
            -- bind.conf       #服务器IP和端口配置文件
            -- game_id_file.conf #游戏ID和FLAG配置文件
        -- log/*               #存放日志文件        
        -- bin/                #共享库文件
        -- startup.sh          #启动脚本
        -- doc/
            -- README           #说明文件 
            -- changelog        #程序更新说明
            -- html/            #函数说明文件
        -- data/
            -- game_score_file  #分数文件
            -- timestamp_file   #时间戳文件
        -- pkg/                 #操作数据库头文件
        