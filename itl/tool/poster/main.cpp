#include "http_transfer.h"
#include "stdio.h"
#include "stdlib.h"
/** 
* @brief  ±®æØ≤‚ ‘
* @return  void  
**/
void report_alarm()
{
    Chttp_transfer  http_transfer;
    char  post_data[1024 * 2] = {'\0'};
    int   i = 0;

    //cmd=20001&host_info=<host_ip>&metric_info=<metric_name>;<current_val>;<threshold_val>&alarm_info=<"warning"|"critical">; <alarm_expr>;<alarm_level>
    while(1)
    {
        snprintf(post_data, sizeof(post_data),
                "cmd=20001&host_info=10.1.1.28&metric_info=system_cpu;12;10.10&alarm_info=warning");
        http_transfer.http_post("http://10.1.1.28/auto-update/index.php", post_data);

        sleep(200);

        //snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.44&metric_info=system_cpu");
        //http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);

        sleep(200);

        snprintf(post_data, sizeof(post_data),
                "cmd=20001&host_info=10.1.1.44&metric_info=user_cpu;12;10.10&alarm_info=warning");
        http_transfer.http_post("http://10.1.1.28/auto-update/index.php", post_data);

        sleep(58);

        //snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.28&metric_info=system_cpu");
        //http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);
        //
        snprintf(post_data, sizeof(post_data),
                "cmd=20001&host_info=10.1.1.27&metric_info=user_cpu;12;10.10&alarm_info=critical");
        http_transfer.http_post("http://10.1.1.28/auto-update/index.php", post_data);


        sleep(80);


        //snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.44&metric_info=user_cpu");
        //http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);


        snprintf(post_data, sizeof(post_data), "cmd=20003&host_info=%s", "10.1.1.28");
        http_transfer.http_post("http://10.1.1.28/auto-update/index.php", post_data);

        sleep(100);

        if(++i%2 == 0)
        {
            snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.44&metric_info=user_cpu");
            http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);
        }
        else if(i%3 == 0)
        {
            snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.27&metric_info=user_cpu");
            http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);

        }
        else if(i%5 == 0)
        {
            snprintf(post_data , sizeof(post_data), "cmd=20002&host_info=10.1.1.28&metric_info=system_cpu");
            http_transfer.http_post("http://10.1.1.28/auto-update/index.php" , post_data);
        }
        else
        {
            snprintf(post_data, sizeof(post_data), "cmd=20004&host_info=%s", "10.1.1.28");
            http_transfer.http_post("http://10.1.1.28/auto-update/index.php", post_data);
        }
        sleep(80);

    }
    return ;
}

int main()
{
    daemon(1,1);
    report_alarm();


}
