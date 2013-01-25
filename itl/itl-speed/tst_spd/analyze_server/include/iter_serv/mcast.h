/**
 *============================================================
 *  @file        mcast.h
 *  @brief      Define the interfaces to send multicast datagram
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef ITERATIVE_SERVER_MCAST_H_
#define ITERATIVE_SERVER_MCAST_H_

#ifdef __cplusplus
extern "C" {
#endif

int create_mcast_socket();

/**
  * @brief Send out a mcast datagram to the specified `mcast_ip` and `mcast_port`.
  *           Note, length of the data should be no longer than 8192.
  *
  * @param const void* data,  data to send.
  * @param int len,  length of `data`.
  *
  * @return int, number of characters sent on success, -1 on error
  */
int send_mcast_pkg(const void* data, int len);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // ITERATIVE_SERVER_MCAST_H_

