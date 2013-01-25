#!/bin/bash
time=`date +%s`
rm bin/libport.so.* -f
cp ./libextend_port.so bin/libport.so.$time
#cd ~/online/tools/reload_text
./reload -d 1 -m extend -b 10.1.1.23 -s bin/libport.so.$time
cd -

exit 0
