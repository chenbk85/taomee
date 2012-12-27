1. 开启程序：startup.sh
2. 结束程序：pkill -9 Switch
3. 监控程序：在crontab中加入* * * * *  cd 程序所在目录（如/opt/taomee/switch） && ./restart.sh >> ./log/restartlog.txt 2>/dev/null
4. 主配置文件bench.conf的各项含义：
	log_dir		./log			    #日志文件存放目录
	log_level	7			        #日志打印等级，7不打印trace日志
	log_size	32000000		    #每个日志文件的最大大小

	shmq_length	8192000			    #共享内存大小,8M
	run_mode	background		    #background表示程序将后台运行

	#server info
	bind_ip		10.1.121.222    	#Switch绑定的IP
	bind_port	4431             	#Switch绑定的端口

	server_num	500			        #online服务器总数上限
	user_num	14950000		    #米米号个数上限
	begin_user_id	50000			#用户id启始号码

	#db server
	dbproxy_ip	192.168.0.87		#DB Proxy的IP
	dbproxy_port	21001			#DB Proxy的端口

	save_login_file	./data/USR_LOGIN_FILE	#日志文件位置,保存用户最后一次登录的服务器信息

5. Switch Server目录树：
      Switch
        -- bin/*			#存放Switch的so
        -- data/			#存放用户上次登陆的服务器ID
        -- log/*			#存放日志文件
        -- bench.conf		#主配置文件
        -- Switch			#主程序
        -- restart.sh	    #监控脚本 
        -- startup.sh	    #启动脚本
        -- readme.txt		#说明文件
        -- changelog.txt	#程序更新说明
        
scp -P56000 libhomeserv.so online@192.168.0.165:/opt/taomee/homeserv
发布到 164 165 上
