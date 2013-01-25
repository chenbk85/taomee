/** 
 * ========================================================================
 * @file alarm_interface.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-27
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#include <string.h>
#include "alarm_interface.h"
#include "itl_common.h"



#define g_http SINGLETON(Chttp_transfer)

enum
{
    MSG_ID_MOBILE = 10001,
    MSG_ID_EMAIL = 10002,
    MSG_ID_RTX = 10003,
};


char g_post_data[512] = {0};

const char * g_alarm_url = config_get_strval("alarm_url", "http://10.1.1.63/itl-server/alarm-server/index.php");


int send_rtx_alarm(const char * contact, const char * msg)
{

    snprintf(g_post_data, POST_DATA_LEN, 
            "cmd=%u&contact=%s&msg=%s",
            MSG_ID_RTX, contact, msg);

    INFO_LOG("\trtx alarm: %s?%s", g_alarm_url, g_post_data);
    g_http.http_post(g_alarm_url, g_post_data);

    return 0;
}


int send_email_alarm(const char * contact, const char * msg)
{
    snprintf(g_post_data, POST_DATA_LEN, 
            "cmd=%u&contact=%s&msg=%s",
            MSG_ID_EMAIL, contact, msg);

    INFO_LOG("\temail alarm: %s?%s", g_alarm_url, g_post_data);
    g_http.http_post(g_alarm_url, g_post_data);

    return 0;
}


int send_mobile_alarm(const char * contact, const char * msg)
{
    snprintf(g_post_data, POST_DATA_LEN, 
            "cmd=%u&contact=%s&msg=%s",
            MSG_ID_MOBILE, contact, msg);

    INFO_LOG("\tmobile alarm: %s?%s", g_alarm_url, g_post_data);
    g_http.http_post(g_alarm_url, g_post_data);

    return 0;
}
