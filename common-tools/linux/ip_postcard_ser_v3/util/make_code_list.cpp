#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <mysql/mysql.h>
#include <list>
#include <map>
#include <fstream>

struct province_t
{
    uint32_t code;
    std::string name;
};

struct city_t
{
    uint32_t code;
    std::string name;
};

int main()
{
    const char* host = "10.1.1.5";
    const char* user = "root";
    const char* pass = "ta0mee";
    unsigned int port = 3306;

    MYSQL *handle;
    handle = mysql_init(0);

    mysql_options(handle,MYSQL_SET_CHARSET_NAME,"utf8");
 
    if (!mysql_real_connect(handle, host, user, pass, 0, port, 0, CLIENT_FOUND_ROWS))
    {
        printf("connect mysql server %s failed %s\n", host, mysql_error(handle));
        return 0;
    }

    {
        const char *sql = "select province_name, province_code from \
                           db_ip_distribution_v8.t_province_code";

        if (mysql_real_query(handle, sql, strlen(sql)) != 0)
        {
            printf("%s", mysql_error(handle));
            return 0;
        }

        MYSQL_RES *result = mysql_store_result(handle);
        if(result == 0)
        {
            printf("%s", mysql_error(handle));
            return false;
        }

        std::list<province_t> province_list;

        MYSQL_ROW mysql_row;
        while ((mysql_row = mysql_fetch_row(result)) != 0)
        {
            province_t t;
            t.name = mysql_row[0];
            t.code = atoll(mysql_row[1]);
            province_list.push_back(t);
        }

        mysql_free_result(result);

        //province_t
        std::ofstream ofs;
        ofs.open("province.txt", std::ios_base::trunc);
        std::list<province_t>::iterator itlist;
        for (itlist = province_list.begin(); itlist != province_list.end(); ++itlist)
        {
            ofs << (*itlist).name << "\t\t" << (*itlist).code << "\n";
        }
        ofs.flush();
    }

    {
        const char *sql = "select city_name, city_code from \
                           db_ip_distribution_v8.t_city_code";

        if (mysql_real_query(handle, sql, strlen(sql)) != 0)
        {
            printf("%s", mysql_error(handle));
            return 0;
        }

        MYSQL_RES *result = mysql_store_result(handle);
        if(result == 0)
        {
            printf("%s", mysql_error(handle));
            return false;
        }

        std::list<city_t> city_list;

        MYSQL_ROW mysql_row;
        while ((mysql_row = mysql_fetch_row(result)) != 0)
        {
            city_t t;
            t.name = mysql_row[0];
            t.code = atoll(mysql_row[1]);
            city_list.push_back(t);
        }

        mysql_free_result(result);

        //city_t
        std::ofstream ofs;
        ofs.open("city.txt", std::ios_base::trunc);
        std::list<city_t>::iterator itlist;
        for (itlist = city_list.begin(); itlist != city_list.end(); ++itlist)
        {
            ofs << (*itlist).name << "\t\t" << (*itlist).code << "\n";
        }
        ofs.flush();
    }
    mysql_close(handle);
    return 0;
}


