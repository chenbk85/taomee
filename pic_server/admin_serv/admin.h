#ifndef _ADMIN_H_
#define _ADMIN_H_
                
#include <libxml/tree.h>    
#include <iter_serv/dll.h>

#include "common.h"
#include "error_nbr.h"



int send_err_to_php(int fd, uint16_t cmd, uint32_t userid, int err);
int send_ok_to_php(int fd, uint16_t cmd, uint32_t userid);


#endif

