   CREATE TABLE IF NOT EXISTS t_validationcode(
		id  INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		code  char(16),
        PRIMARY KEY  (id)
) ENGINE=memory, CHARSET=utf8;
