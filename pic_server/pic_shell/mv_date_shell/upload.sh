#!/bin/sh
i=1;
while [ "$i" -le 5 ]
do
	curl -H 'Expect:' -F 'file=@/home/kathy/j.jpeg;type=image/jpeg' -F 'session_key=43e0d130dd6eb931d5ba40a03a768c79c0af62b6c0ef8543756e97dc4209e246ee5f60808febeaa38a9acc2c0e5a8612756e97dc4209e2463b3e1c78e6c07a096fac8741b20b372c756e97dc4209e246339edf4851bcebdbff0b74c3181f5e98' 'http://10.1.1.58/cgi_upload.cgi'
	i=$(($i+1))
done

while [ "$i" -le 10 ]
do
	curl -H 'Expect:' -F 'file=@/home/kathy/g.gif;type=image/gif' -F 'session_key=a79a5524f18e9313d5ba40a03a768c79c0af62b6c0ef8543756e97dc4209e246ee5f60808febeaa38a9acc2c0e5a8612756e97dc4209e2463b3e1c78e6c07a096fac8741b20b372c756e97dc4209e246339edf4851bcebdbff0b74c3181f5e98' 'http://10.1.1.58/cgi_upload.cgi'
	i=$(($i+1))
done

while [ "$i" -le 16 ]
do
	curl -H 'Expect:' -F 'file=@/home/kathy/p.png;type=image/png' -F 'session_key=43e0d130dd6eb931d5ba40a03a768c79c0af62b6c0ef8543756e97dc4209e246ee5f60808febeaa38a9acc2c0e5a8612756e97dc4209e2463b3e1c78e6c07a096fac8741b20b372c756e97dc4209e246339edf4851bcebdbff0b74c3181f5e98' 'http://10.1.1.58/cgi_upload.cgi'
	i=$(($i+1))
done

exit 0






