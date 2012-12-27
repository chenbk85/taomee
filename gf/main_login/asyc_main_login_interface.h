#ifndef ASYC_MAIN_LOGIN_INTERFACE_H
#define ASYC_MAIN_LOGIN_INTERFACE_H
#include "asyc_main_login_type.h"
extern "C" {
/**
  * @brief handle pkg recv from server.
  *
  * @param int fd, the filedescripter connect to server
  * @param void* data, pkg pointer  
  * @param int len, pkg length 
  * @return 
  */
void m_process_serv_return(int fd, void* data, int len);

/**
  * @brief process when server connection closed
  *
  * @param int fd, the filedescripter connect to server
  *
  * @return 
  */
void m_process_serv_fd_closed(int fd);

/**
  * @brief process pkg from client
  *
  * @param usr_info_t* p, contains plays' info
  * @param uint8_t *body, pkg pointer
  * @param int bodylen,   pkg length
  *
  * @return if values 0 just go on else connection will be closed
  */
int m_process_client_cmd(usr_info_t* p, uint8_t* body, int bodylen);

/**
  * @brief calculate the package length recv
  *
  * @param int fd, the filedescripter connection used;
  * @param void* avail_data, the package pointer
  * @param int avail_len, the package length 
  *
  * @return the result of package length
  */
int m_process_pkg_len(int fd, const void* avail_data, int avail_len);

/**
  * @brief business service initialization
  *
  * @return 0, sucess; else false;
  */
int m_process_service_init();
}
#endif
