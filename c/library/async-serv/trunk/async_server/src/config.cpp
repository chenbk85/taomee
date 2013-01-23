#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <map>
#include <string>
#include "config.h"
#include "log.h"

static std::map<std::string, std::string> g_config_map;

bool parse_config_file(const char * file_name, bool overwrite_flag)
{
    int fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        return false;
    }

    int len = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);
    char *data = (char *)malloc(len + 1);
    if (data == NULL)
    {
        return false;
    }

    if (read(fd, data, len) == -1)
    {
        close(fd);
        return false;
    }

    data[len] = 0;
    char *start = data;
    char *end;
    while (data + len > start)
    {
        end = strchr(start, '\n');
        if (end)
        *end = 0;

        if (*start != '#')
        {
            char *key;
            char *val;
            key = strtok(start, "= \t");
            val = strtok(NULL, "= \t");
            if (key != NULL && val != NULL)
            {
                if (strncmp("include", key, 8))
                {
                    std::string str_key = key;
                    std::string str_val = val;
                    if (g_config_map.find(str_key) == g_config_map.end())
                    {
                        g_config_map[str_key] = str_val;
                    }
                    else
                    {
                        if (overwrite_flag)
                        {
                            g_config_map[str_key] = str_val;
                        }
                        else
                        {
                            BOOT_LOG(-1, "config %s same key: %s", file_name, key);
                        }
                    }

                }
                else
                {
                    bool ret = parse_config_file(val, overwrite_flag);
                    if (!ret)
                    {
                        free(data);
                        close(fd);
                        return false;
                    }
                }
            }
        }

        if (end)
        {
            start = end + 1;
        }
        else
        {
            break;
        }
    }

    free(data);
    close(fd);

    return true;
}


bool load_config_file(const char * file_name)
{
    g_config_map.clear();

    return parse_config_file(file_name, false);
}

bool reload_config_file(const char * file_name)
{
    return parse_config_file(file_name, true);
}

extern "C" int config_get_intval(const char *key, int val)
{
    std::map<std::string, std::string>::iterator it;
    it = g_config_map.find(key);
    if (it == g_config_map.end())
        return val;

    return atoi((*it).second.c_str());
}


extern "C" const char *config_get_strval(const char *key, const char *val)
{
    std::map<std::string, std::string>::iterator it;
    it = g_config_map.find(key);
    if (it == g_config_map.end())
        return val;

    return (*it).second.c_str();
}

