#!/bin/bash


gen_proto_dir=../../gen_proto/

$gen_proto_dir/gen_proto_app/bin/cat_proto -f "user, L, 14, 4 | seq, L, 4, 4 | ret, L, 10, 4"  -c 8 -p $gen_proto_dir/python/pea_battle_proto.py $*

