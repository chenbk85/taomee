#!/bin/bash


gen_proto_dir=../../gen_proto/

$gen_proto_dir/gen_proto_app/bin/cat_proto -l 22 -f "seq, L, 4, 4 | ret, L, 10, 4 | user_id, L, 14, 4 | role_tm, L, 18, 4"  -c 8 -p $gen_proto_dir/python/pea_db_proto.py $*

