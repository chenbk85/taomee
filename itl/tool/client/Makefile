GEN_PROTO = ../../gen_proto/

all: gen_node




.PHONY : all clean

clean:
	rm -rf *.pyc

gen: gen_node gen_head gen_alarm gen_rrd gen_db




gen_node:
	$(GEN_PROTO)/getxml.sh itl_node
	cp $(GEN_PROTO)/python/itl_node_proto.py ./

gen_head:
	$(GEN_PROTO)/getxml.sh itl_head
	cp $(GEN_PROTO)/python/itl_head_proto.py ./

gen_alarm:
	$(GEN_PROTO)/getxml.sh itl_alarm
	cp $(GEN_PROTO)/python/itl_alarm_proto.py ./

gen_db:
	$(GEN_PROTO)/getxml.sh itl_db
	cp $(GEN_PROTO)/python/itl_db_proto.py ./

gen_rrd:
	$(GEN_PROTO)/getxml.sh itl_rrd
	cp $(GEN_PROTO)/python/itl_rrd_proto.py ./


rebuild: clean all

c: clean

r: rebuild

g: gen
