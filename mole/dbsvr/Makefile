#淘米网络，easyeagel@gmx.com
#这是一个最高层makefile，它使用递归的makefile方案

.PHONY : clean test tags libs dbtest

dbser:
	make -C ser/src

dbtest:
	make -C ser/src install
	cd ser/bin && ./daemon.sh restart && cd -

tags:
	ctags -R  --append=no --c++-kinds=+p --fields=+iaS --extra=+q --language-force=c++ include ser

libs:
	make -C libsrc

