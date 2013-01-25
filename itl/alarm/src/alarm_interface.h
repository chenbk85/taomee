/** 
 * ========================================================================
 * @file alarm_interface.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-27
 * Modify $Date: 2012-07-30 13:34:33 +0800 (一, 30  7月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_ALARM_INTERFACE_H_2012_07_27
#define H_ALARM_INTERFACE_H_2012_07_27


#define POST_DATA_LEN (1024)


int send_mobile_alarm(const char * contact, const char * msg);
int send_email_alarm(const char * contact, const char * msg);
int send_rtx_alarm(const char * contact, const char * msg);



#endif
