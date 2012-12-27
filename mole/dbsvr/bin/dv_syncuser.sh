
echo "restart dv_syncuser ..."
killall -9 dv_syncuser 
cd ~/DB/ser/bin/
~/DB/bin/dv_syncuser 10.1.1.89   11002
echo "restart dv_syncuser ... ok"
