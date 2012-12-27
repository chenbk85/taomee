t_gongfu1="ALTER TABLE t_gf_attire_%02d add duration int(10) unsigned NOT NULL default 0;"
#t_gongfu2="ALTER TABLE t_gf_attire_%02d drop primary key;"
#t_gongfu3="ALTER TABLE t_gf_attire_%02d add primary key(userid,role_regtime,gettime);"


for db in range(100):
	print "USE GF_%02d;"%(db)	
	for t in range(100):
		print t_gongfu1 %(t)
		#print t_gongfu2 %(t)
		#print t_gongfu3 %(t)
