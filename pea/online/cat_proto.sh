#!/bin/bash


gen_proto_dir=../../gen_proto/

$gen_proto_dir/gen_proto_app/bin/cat_proto -f "user, L, 6, 4 | seq, L, 10, 4 | ret, L, 14, 4"  -c 4 -b -p $gen_proto_dir/python/pea_online_proto.py $*

