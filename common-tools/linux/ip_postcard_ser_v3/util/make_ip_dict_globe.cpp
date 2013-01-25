#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <mysql/mysql.h>
#include <list>
#include <map>
#include <fstream>

struct code_t
{
    uint32_t province_code;
    uint32_t city_code;
};

struct ip_info_t
{
    uint32_t start_ip;
    uint32_t end_ip;
    uint32_t province_code;
    uint32_t city_code;
};

int main()
{
    const char* host = "10.1.1.5";
    const char* user = "root";
    const char* pass = "ta0mee";
    unsigned int port = 3306;

    MYSQL *handle;
    handle = mysql_init(0);
 
    if (!mysql_real_connect(handle, host, user, pass, 0, port, 0, CLIENT_FOUND_ROWS))
    {
        printf("connect mysql server %s failed %s\n", host, mysql_error(handle));
        return 0;
    }

    const char *sql = "select start_ip, end_ip, province_code, city_code from \
                       db_ip_distribution_v8.t_city_ip";

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

    std::list<ip_info_t> iplist;
    std::map<uint32_t, code_t> ipmap;

    MYSQL_ROW mysql_row;
    while ((mysql_row = mysql_fetch_row(result)) != 0)
    {
        ip_info_t t;
        t.start_ip = atoll(mysql_row[0]);
        t.end_ip = atoll(mysql_row[1]);
        t.province_code = atoll(mysql_row[2]);
        t.city_code = atoll(mysql_row[3]);
        iplist.push_back(t);

        code_t c;
        memset(&c, 0, sizeof(c));
        ipmap[t.start_ip] = c;
        ipmap[t.end_ip+1] = c;
    }

    mysql_free_result(result);
    mysql_close(handle);

    code_t c;
    memset(&c, 0, sizeof(c));
    ipmap[0] = c;
    ipmap[0xffffffff] = c;

    std::list<ip_info_t>::iterator itlist;
    for (itlist = iplist.begin(); itlist != iplist.end(); ++itlist)
    {
        std::map<uint32_t, code_t>::iterator it;
        it = ipmap.find((*itlist).start_ip);
        if (it == ipmap.end())
            continue;
        
        if ((*it).second.province_code == 0)
            (*it).second.province_code = (*itlist).province_code;
        if ((*it).second.city_code != (*itlist).city_code)
            (*it).second.city_code = (*itlist).city_code;

        for (; it != ipmap.end(); ++it)
        {
            if ((*it).first != (*itlist).end_ip + 1)
            {
                if ((*it).second.province_code == 0)
                    (*it).second.province_code = (*itlist).province_code;
                if ((*it).second.city_code != (*itlist).city_code)
                    (*it).second.city_code = (*itlist).city_code;
            }
            else
                break;
        }
    }

    {
        std::ofstream ofs;
        ofs.open("ipdict.dat", /*std::ios_base::binary|*/std::ios_base::trunc);
        uint32_t size = ipmap.size();
//        ofs.write((char*)&size, sizeof(size));
        ofs << size << " ";
        printf("size = %u", size);
        std::map<uint32_t, code_t>::iterator it;    
        for (it = ipmap.begin(); it != ipmap.end(); ++it)
        {
            uint32_t key = (*it).first;
            code_t code = (*it).second;
            
//            ofs.write((char*)&size, sizeof(size));
//            ofs.write((char*)&size, sizeof(size));
//            ofs.write((char*)&size, sizeof(size));
            ofs << key << " ";
            ofs << code.province_code << " ";
            ofs << code.city_code << " ";
        }
        ofs.flush();
    }

    return 0;
}


