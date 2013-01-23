/**
 * @file bind_conf.h
 * @brief 读取配置文件类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-01
 */
#ifndef _H_BINDFILE_CONF_H_
#define _H_BINDFILE_CONF_H_

#include <string>
#include <list>

#include "async_serv.h"

class bind_conf {
public:

    bind_conf(const std::string bind_file_path)
        : m_bind_file_path_(bind_file_path)
    {
    }

    ~bind_conf() { }

    int load_bind_conf(std::list<bind_conf_t>& bind_conf_list);

private:
    int mmap_bind_file(char ** buf);
    void build_ifs(char* tifs, const u_char* ifs0);
    void make_default_ifs();
    int str_explode(const char* ifs, char*line0, char* field[], int n);

private:
    std::string m_bind_file_path_;
    char m_default_ifs_[256];
};


#endif


