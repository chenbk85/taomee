LIB="-ldl -ltaomee"
CFLAGS="-O2 -fPIC -shared -D_GNU_SOURCE" 
INCLUDE="-I./ -I../src"

g++ $INCLUDE $CFLAGS -c bench.c
g++ $INCLUDE $CFLAGS -o ../bin/bench.so bench.o $LIB

g++ $INCLUDE $CFLAGS -c load_presure.c
g++ $INCLUDE $CFLAGS -o ../bin/load_presure.so load_presure.o $LIB

#g++ $INCLUDE $CFLAGS -c proxy.c
#g++ $INCLUDE $CFLAGS -o ../bin/proxy.so proxy.o $LIB

#g++ $INCLUDE $CFLAGS -c stat_proxy.c
#g++ $INCLUDE $CFLAGS -o ../bin/stat_proxy.so stat_proxy.o $LIB

rm -fr *.o
