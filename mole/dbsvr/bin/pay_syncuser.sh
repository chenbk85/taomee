
echo "restart pay_syncuser ..."
killall -9 pay_syncuser 
cd ~/DB/payser/bin
pay_syncuser 127.0.0.1 21001 13001
echo "restart pay_syncuser ... ok"
