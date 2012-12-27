
#!/bin/sh

/usr/sbin/ab -v 4 -n $2 -c $3 \
        -p $1 \
	    -T "multipart/form-data; boundary=HTTP_Request_whathellareyoudoing" \
		http://10.1.1.7:81/cgi-bin/gate/gate.php >> ab.log
