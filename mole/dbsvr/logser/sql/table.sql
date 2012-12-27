 	CREATE TABLE IF NOT EXISTS t_user_opt_20080614(
		logtime DATETIME NOT NULL,
		userid 	INT UNSIGNED NOT NULL DEFAULT '0', 
		flag 	INT UNSIGNED NOT NULL DEFAULT '0', 
		regpost INT UNSIGNED NOT NULL DEFAULT '0', 
		PRIMARY KEY  (logtime,userid)					
	)ENGINE=innodb CHARSET=utf8;
	CREATE INDEX idx on t_user_opt_20080614 (logtime,userid );
