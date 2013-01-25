/** 
 * ========================================================================
 * @file log_archive.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <set>


#include <libtaomee/log.h>
#include "container.h"
#include "itl_util.h"
#include "log_archive.h"


timer_head_t g_log_archive_timer;

// tar时是否删除源文件
bool g_remove_file = false;
// log路径
const char * g_log_path = NULL;
// 每天开始tar的时间，离当天0点的秒数
uint32_t g_begin_offset = 0;
// 保留几个tar文件
uint32_t g_keep_tar_num = 7;

const char * get_archive_name(uint32_t date)
{
    static char file[PATH_MAX] = {0};
    snprintf(file, sizeof(file), 
            "%s/log_%u.tar", 
            g_log_path, 
            date);

    return file;
}

int get_date(uint32_t time, uint32_t * p_date)
{
    struct tm tm;
    time_t ts = (time_t)time;
    if (NULL == localtime_r(&ts, &tm))
    {
        return -1;
    }

    *p_date = (uint32_t)((tm.tm_year + 1900) * 1e4 + (tm.tm_mon + 1) * 1e2 + tm.tm_mday);
    return 0;
}


int archive_log(uint32_t date)
{
    DEBUG_LOG("archive log, date: %u", date);

    const char * archive_file = get_archive_name(date);

    char cmd[PATH_MAX];
    if (g_remove_file)
    {
        snprintf(cmd, sizeof(cmd), 
                "sh -c '/bin/tar -czPf %s %s/*%u* --exclude=*.tar --remove-files'", 
                archive_file,
                g_log_path, 
                date);

    }
    else
    {

        snprintf(cmd, sizeof(cmd), 
                "sh -c '/bin/tar -czPf '%s' '%s/*%u*' --exclude=*.tar'", 
                archive_file,
                g_log_path, 
                date);
    }
    DEBUG_LOG("cmd: %s", cmd);

    GError * err = NULL;

    gboolean ret = g_spawn_command_line_async(cmd, &err);
    if (!ret)
    {
        ERROR_LOG("%s", err->message);
        g_error_free(err);
        return -1;
    }
    else
    {
        return 0;
    }
}


int remove_expired_tar()
{
    std::set< uint32_t > date_set;
    struct dirent * p_de = NULL;
    DIR * p_dir = opendir(g_log_path);
    if (NULL == p_dir)
    {
        ERROR_LOG("opendir %s failed (%s)", g_log_path, strerror(errno));
        return -1;
    }

    while (NULL != (p_de = readdir(p_dir)))
    {
        if (DT_REG != p_de->d_type)
        {
            continue;
        }

        uint32_t date = 0;
        int ret = sscanf(p_de->d_name, "log_%u.tar", &date);
        if (1 != ret)
        {
            continue;
        }

        if (0 == date)
        {
            continue;
        }

        date_set.insert(date);
    }



    uint32_t i = date_set.size();
    set_for_each(date_set, it)
    {
        if (i <= g_keep_tar_num)
        {
            break;
        }
        const char * file = get_archive_name(*it);
        remove(file);
        i--;
    }
    return 0;
}


int initial_archive()
{
    std::set< uint32_t > date_set;
    uint32_t now = get_now_tv()->tv_sec;
    uint32_t day_ts = get_day_timestamp(now);

    struct dirent * p_de = NULL;
    DIR * p_dir = opendir(g_log_path);
    if (NULL == p_dir)
    {
        ERROR_LOG("opendir %s failed (%s)", g_log_path, strerror(errno));
        return -1;
    }

    while (NULL != (p_de = readdir(p_dir)))
    {
        if (DT_REG != p_de->d_type)
        {
            continue;
        }
        char file[PATH_MAX];
        snprintf(file, sizeof(file), "%s/%s", g_log_path, p_de->d_name);
        struct stat file_stat;
        if (0 != stat(file, &file_stat))
        {
            ERROR_LOG("stat %s failed (%s)", file, strerror(errno));
            continue;
        }

        uint32_t modify_time = file_stat.st_mtime;
        if (modify_time >= day_ts)
        {
            continue;

        }


        uint32_t date = 0;
        if (0 != get_date(modify_time, &date))
        {
            continue;
        }


        date_set.insert(date);
    }

    closedir(p_dir);

    set_for_each(date_set, it)
    {
        uint32_t date = *it;
        const char * archive_file = get_archive_name(date);
        if (-1 == access(archive_file, F_OK))
        {
            archive_log(date);
        }
    }

    return 0;
}


int archive_log_timely(void * owner, void * data)
{
    uint32_t now = get_now_tv()->tv_sec;
    uint32_t day_time = get_day_timestamp(now);
    // 下一次是在明天的这个时间
    uint32_t next_time = day_time + DAY_SECONDS + g_begin_offset;


    DEBUG_LOG("next time of log archive: %s", timestamp2str(next_time));
    ADD_TIMER_EVENT(&g_log_archive_timer, archive_log_timely, (void *)1, next_time);

    if (data)
    {
        remove_expired_tar();


        uint32_t date = 0;
        get_date(day_time - 1, &date);
        archive_log(date);
    }
    else
    {
        initial_archive();
    }

    return 0;
}




int init_log_archive(const char * log_dir, uint32_t begin_offset, bool remove_file, uint32_t keep_tar_num)
{
    if (NULL == log_dir)
    {
        return -1;
    }

    while (begin_offset > DAY_SECONDS)
    {
        begin_offset -= DAY_SECONDS;
    }

    g_remove_file = remove_file;
    g_log_path = log_dir;
    g_begin_offset = begin_offset;
    g_keep_tar_num = keep_tar_num;


    archive_log_timely(NULL, NULL);

    return 0;
}


int fini_log_archive()
{
    if (NULL != g_log_path)
    {
        REMOVE_TIMERS(&g_log_archive_timer);

    }
    return 0;
}
