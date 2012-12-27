t_gf_attire="CREATE TABLE `t_gf_attire_%02d` (\n\
  `userid` int unsigned NOT NULL,\n\
  `gettime` int unsigned NOT NULL default '0',\n\
  `attireid` int unsigned NOT NULL,\n\
  `attire_rank` int unsigned NOT NULL,\n\
  `usedflag` int unsigned NOT NULL,\n\
  PRIMARY KEY  (`userid`,`gettime`)\n\
) ENGINE=InnoDB DEFAULT CHARSET=utf8;"

for db in range(100):
	print "USE GF_%02d;"%(db)	
	for t in range(100):
		print "drop table if exists t_gf_attire_%02d;"%(t)
		print t_gf_attire %(t)
