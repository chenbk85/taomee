/**
 * @file bind_conf.cpp
 * @brief 读取配置文件类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-01
 */
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <alloca.h>
#include <string.h>
#include <stdlib.h>

#include "bind_conf.h"

int bind_conf::mmap_bind_file(char **buf)
{
    int fd;
    int len;
    int ret_code = -1;

    fd = ::open(m_bind_file_path_.c_str(), O_RDONLY);
    if (fd < 0)
        return -1;

    len = ::lseek(fd, 0L, SEEK_END);
    ::lseek(fd, 0L, SEEK_SET);

    *buf = (char *) ::mmap(0, len + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*buf == MAP_FAILED) {

    } else {
        ::read(fd, *buf, len);
        (*buf)[len] = '\0';
        ret_code = len + 1;
    }

    ::close(fd);
    return ret_code;
}

int bind_conf::load_bind_conf(std::list<bind_conf_t>& bind_conf_list)
{
    int len;
    char* buf;

    len = mmap_bind_file(&buf);
    if (len <= 0)
        return -1;

    char* field[3];
    char* start = buf;
    len = ::strlen(buf);
    while (start < buf + len) {
        char* end = ::strchr(start, '\n');
        if (end)
            *end = '\0';

        if (*start != '#' && str_explode(NULL, start, field, 3) == 3) {
            bind_conf_t bc;

            bc.ip_addr = field[0];
            bc.port = atoi(field[1]);

            if (!::strcasecmp(field[2], "tcp")) {
                bc.type =TYPE_TCP;
            } else if (!::strcasecmp(field[2], "udp")) {
                bc.type = TYPE_UDP;
            } else {
                break;
            }

            bind_conf_list.push_back(bc);
        }
        start = end + 1;
    }

    ::munmap(buf, len);

    return 0;
}

void bind_conf::build_ifs(char *tifs, const u_char *ifs0)
{
    const u_char *ifs = ifs0;
    ::memset(tifs, 0, 256);
    while (*ifs) {
        tifs[*ifs++] = 1;
    }
}

void bind_conf::make_default_ifs()
{
    ::memset(m_default_ifs_, 0, 256);
    m_default_ifs_[9] = 1;
    m_default_ifs_[10] = 1;
    m_default_ifs_[13] = 1;
    m_default_ifs_[32] = 1;
}

int bind_conf::str_explode(const char *ifs, char *line0, char *field[], int n)
{
    char *line = line0;
    int i;

    make_default_ifs();
    if (ifs == NULL) {
        ifs = m_default_ifs_;
    } else if (*ifs) {
        char *implicit_ifs = (char*) ::alloca(256);
        build_ifs(implicit_ifs, (const u_char*)ifs);
        ifs = implicit_ifs ;
    }

    i = 0;
    while (1) {
        while (ifs[(u_char)*line]) {
            line++;
        }
        if (!*line)
            break;

        field[i++] = line;
        if (i >= n) {
            line += ::strlen(line) - 1;
            while (ifs[(u_char)*line]) {
                line--;
            }
            line[1] = '\0';
            break;
        }
        while (*line && !ifs[(u_char)*line]) {
            line++;
        }
        if (!*line)
            break;

        *line++ = '\0';
    }

    return i;
}

