#ifndef OPTIONS_H
#define OPTIONS_H

#include "Ini_File.h"

struct Ini_Option_t
{
	char	log_dir[128];
	char	log_prename[32];
	int 	log_size;
	int		log_num;
	short	log_priority;

	char 	bind_ip[16];	
	u_short bind_port;

	int		worker_proc_num;
	int		max_fds;
};

class Option
{
public:
	Option();
	~Option();

	int init(const char* config_file);
	void print();
protected:
	int	parse_ini();

	IniFile ini_file;
};

extern Ini_Option_t ini;
extern Option option;
#endif

