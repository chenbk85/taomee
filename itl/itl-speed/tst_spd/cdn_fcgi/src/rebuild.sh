make cleanall
rm -f *.log core.*
make
make cleanobj
mv cdn_fcgi ../
