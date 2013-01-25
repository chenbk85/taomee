/**
 *============================================================
 *  @file        dll.h
 *  @brief      Define the interfaces that a .so must implement to make use of AsyncServ
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef ITERATIVE_SERVER_DLL_H_
#define ITERATIVE_SERVER_DLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

/**
 * @struct IterServInterface
 * @brief To make use of IterServ, a .so must implement the interfaces held by this structure
 *
 */
typedef struct IterServInterface {
	void*	handle; // Hold the handle returned by dlopen

	/*!
	  *  Called each time on a new connection accepted. Optional interface.\n
	  *   You can do something (ie. allocate memory) to the newly accepted sockfd here.\n
	  *   You must return 0 on success, -1 otherwise.
	  */
	int		(*on_conn_accepted)(int sockfd);
	/*!
	  *  Called each time on a connection closed. Optional interface.\n
	  *   You can do something (ie. deallocate memory) to the closed sockfd here.\n
	  */
	void	(*on_conn_closed)(int sockfd);
	/*!
	  * Called to process packages from clients. Called once for each package. \n
	  * Return non-zero if you want to close the client connection from which the `pkg` is sent,
	  * otherwise returns 0.
	  */
	int		(*on_pkg_received)(int sockfd, void* pkg, int pkglen);
	/*!
	  *  Called to process multicast packages from the specified `mcast_ip` and `mcast_port`. \n
	  *   Called once for each package. Optional interface.
	  */
	void	(*proc_mcast_pkg)(const void* data, int len);

	/*!
	  * Called each time when all the packages from clients have been processed. Optional interface.
	  * Calling interval of this interface is no much longer than 100ms at maximum.
	  */
	void	(*proc_events)();

	/*!
	  * Called only once at server startup. Optional interface.\n
	  * You should initialize your service program (allocate memory, create objects, etc) here. \n
	  * You must return 0 on success, -1 otherwise.
	  */
	int 	(*init_service)();
	/*!
	  * Called only once at server stop. Optional interface.\n
	  * You should finalize your service program (release memory, destroy objects, etc) here. \n
	  * You must return 0 if you have finished finalizing the service, -1 otherwise.
	  */
	int 	(*fini_service)();
	/*!
	  * This interface will be called on data ready to be processed.\n
	  * You must return 0 if you cannot yet determine the length of the incoming package,
	  * return -1 if you find that the incoming package is invalid and IterServ will close the connection,
	  * otherwise, return the length of the incoming package. Note, the package should be no larger than 8192 bytes.
	  */
	int		(*get_pkg_len)(int fd, const void* avail_data, int avail_len);
} iter_serv_if_t;

extern iter_serv_if_t dll;

int  register_plugin(const char* file_name);
void unregister_plugin();

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // ITERATIVE_SERVER_DLL_H_
