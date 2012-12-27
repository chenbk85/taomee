#include "myinit.h"
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <arpa/inet.h>
}

my_option_t my_opt;

int my_read_conf()
{
    /*read all configuration param into global variable*/
    strcpy(my_opt.multicast_interface, config_get_strval("multicast_interface"));
    strcpy(my_opt.multicast_ip, config_get_strval("multicast_ip"));
    my_opt.multicast_port = config_get_intval("multicast_port", 0);

    strcpy(my_opt.mole_dbproxy_name, config_get_strval("mole_dbproxy"));
    //strcpy(my_opt.mole_dbproxy_ip, config_get_strval("mole_dbproxy_ip"));
    //my_opt.mole_dbproxy_port = config_get_intval("mole_dbproxy_port", 0);

    strcpy(my_opt.time_ip, config_get_strval("time_ip"));
    my_opt.time_port = config_get_intval("time_port", 0);

    strcpy(my_opt.master_ip, config_get_strval("master_ip"));
    my_opt.master_port = config_get_intval("master_port", 0);
	strcpy(my_opt.master_ser, config_get_strval("master_ip"));
	
	my_opt.dx_or_wt = config_get_intval("domain", 0);

    return 0;
}


