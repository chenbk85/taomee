	CREATE TABLE IF NOT EXISTS t_sysarg_type_buf( 
		type    		INT UNSIGNED NOT NULL DEFAULT 0,
		type_buf		VARCHAR(100),
		PRIMARY KEY (type)
		) ENGINE=innodb, CHARSET=utf8;
