#include "Options.h"
#include <string>
#include <stdio.h>
using namespace std;
	
Ini_Option_t ini;
Option::Option()
{
	memset (&ini, 0x0, sizeof (ini));
}

Option::~Option()
{
}

int Option::init(const char* config_file)
{
	if (!ini_file.open (config_file))
	{
		printf ("open config file:%s failed\n", config_file);
		return -1;
	}

	if (parse_ini () != 0) 
		return -1;
	
	return 0;
}

int	Option::parse_ini()
{
	string tmp;

	tmp = ini_file.read ("logger", "LOG_MAX_SIZE");
	if (tmp.empty ())
		return -1;
	ini.log_size = atoi (tmp.c_str());

	tmp = ini_file.read ("logger", "LOG_DIR");
	strncpy (ini.log_dir, tmp.c_str(), sizeof ini.log_dir);

	tmp = ini_file.read ("logger", "LOG_PRENAME");
	if (tmp.empty ())
		memset (ini.log_prename, 0x0, sizeof ini.log_prename);
	else
		strncpy (ini.log_prename, tmp.c_str(), sizeof ini.log_prename);

	tmp = ini_file.read ("logger", "LOG_MAX_NUM");
	if (tmp.empty ())
		return -1;
	ini.log_num = atoi (tmp.c_str());

	tmp = ini_file.read ("logger", "LOG_PRIORITY");
	if (tmp.empty ())
		return -1;
	ini.log_priority = atoi (tmp.c_str());

    tmp = ini_file.read ("misc", "MAX_CONNECTIONS");
    if (tmp.empty ())
        return -1;
    ini.max_fds = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "BIND_PORT");
	if (tmp.empty ())
		return -1;
	ini.bind_port = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "BIND_IP");
	if (tmp.empty ())
		strcpy (ini.bind_ip, "*.*.*.*");
	else
		strncpy (ini.bind_ip, tmp.c_str(), sizeof (ini.bind_ip) - 1);

	tmp = ini_file.read ("misc", "WORKER_PROC_NUM");
	if (tmp.empty ())
		return -1;
	ini.worker_proc_num = atoi (tmp.c_str());

	return 0;
}

void Option::print ()
{
	printf ("LOG_DIR:%s\n", ini.log_dir);
	printf ("LOG_MAX_SIZE:%d\n", ini.log_size);
	printf ("LOG_MAX_NUM:%d\n", ini.log_num);
	printf ("LOG_PRIORITY:%d\n", ini.log_priority);
	printf ("LOG_PRENAME:%s\n\n", ini.log_prename);

	printf ("BIND_IP:%s\n", ini.bind_ip);
	printf ("BIND_PORT:%d\n", ini.bind_port);
	printf ("WORKER_PROC_NUM:%d\n", ini.worker_proc_num);
	printf ("MAX_CONNECTIONS:%d\n\n", ini.max_fds);
}
