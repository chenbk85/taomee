#!/bin/bash
if [ $# != 3 ]
then
	echo "usage: ./test_login userid passwd startid"
	echo "for example, [./test_login xxxxx xxxxxx 0]"
	exit -1
fi

{
python ./protocol.py $1 $2 $3 >>./data/result
}&

exit 0

