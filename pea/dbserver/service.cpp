#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "service.hpp"
#include "mysql_iface.h"
#include "proto.hpp"
#include "Croute_func.hpp"
#include <libtaomee++/utils/strings.hpp> 





static mysql_interface *g_db = NULL;
static Croute_func     *route_func = NULL;

int handle_init (int argc, char **argv, int pid_type)
{
    switch (pid_type) {
        case PROC_MAIN:
            boot_log(0,0,"------------boot-------------");
            return 0;

        case PROC_WORK:
            srandom(getpid());
            g_db=new mysql_interface(config_get_strval("DB_IP"),
                    config_get_strval("DB_USER"),
                    config_get_strval("DB_PASSWD"),
                    config_get_intval("DB_PORT",3306),
                    config_get_strval("DB_UNIX_SOCKET")
                    );
            /*g_cp=new Cclientproto(config_get_strval("ROUTEDB_IP"),
              config_get_intval("ROUTEDB_PORT",0 )
              );*/

            g_db->set_is_log_debug_sql( config_get_intval("IS_LOG_DEBUG_SQL",0 )  );
            route_func=new Croute_func(g_db);
            return 0;
        case PROC_CONN:
            DEBUG_LOG( "------------PROC_CONN---------");
            return 0;
        default:
            ERROR_LOG ("invalid pid_type=%d", pid_type);
            return -1;
    }
}


int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
    int reallen;
    if (length >= 4)
    {
        reallen=*((uint32_t* )buffer);
        if (reallen<=8192) {
            return reallen;
        }else{
            return -1;
        }
    }else return 0;

}

void hex_printf(char *buf,int len)
{	
    char tmpbuf[PROTO_MAX_SIZE] = "";
    char *pcur = tmpbuf;
    if (len > (PROTO_MAX_SIZE/3-100))
        return;
    for (int i=0;i<len;i++)
    {
        sprintf(pcur,"%02x ",buf[i]&0xff);
        pcur += 3;
    }
    DEBUG_LOG("hex printf: %s \n",tmpbuf);

}


int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
    hex_printf(recvbuf, rcvlen);

    int ret= route_func->deal(recvbuf, rcvlen, sendbuf, sndlen);

    if (ret!=SUCC){
        //当处理出错时,标准返回  没有私有域
        SET_STD_ERR_BUF (sendbuf, sndlen,(PROTO_HEADER *)recvbuf,ret);
    }

    DEBUG_LOG("O:%d:%d",ret, *sndlen);
    return SUCC;
}

void handle_fini(int pid_type)
{
    switch (pid_type)
    {
        case PROC_MAIN:
            break;

        case PROC_WORK:
            break;

        default:
            break;
    }
}


int handle_open(char **send_buf, int *len, const skinfo_t* sk)
{
    return 0;
}

void handle_close(const skinfo_t* sk)
{

}

int handle_filter_key (const char* buf, int len, uint32_t* key)
{
    return -1;
}
