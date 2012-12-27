t_gongfu="CREATE TABLE `t_gongfu_%01d` (\n\
  `userid` int unsigned NOT NULL,\n\
  `flag`   int unsigned NOT NULL,\n\
  `regtime` int unsigned NOT NULL,\n\
  `nick` char(16) NOT NULL,\n\
  `vip` int unsigned NOT NULL default '0',\n\
  `color` int unsigned NOT NULL default '0',\n\
  `xiaomee` int unsigned NOT NULL,\n\
  `Ol_count` int unsigned NOT NULL,\n\
  `Ol_today` int unsigned NOT NULL,\n\
  `Ol_last` int unsigned NOT NULL,\n\
  `Ol_time` int unsigned NOT NULL,\n\
  PRIMARY KEY  (`userid`)\n\
) ENGINE=InnoDB DEFAULT CHARSET=utf8;"

for db in range(100):
	print "drop database if exists GF_%02d;"%(db)
	print "create database GF_%02d;"%(db)
	print "USE GF_%02d;"%(db)	
	for t in range(10):
		print t_gongfu %(t)
