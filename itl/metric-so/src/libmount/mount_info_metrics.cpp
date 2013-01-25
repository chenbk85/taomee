/** 
 * ========================================================================
 * @file mount_info_metrics.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <errno.h>
#include "mount_info_metrics.h"
#include <libtaomee/log.h>

#define MOUNTS "/proc/mounts"
#define PARTITION_CMD "/sbin/blkid"

uint32_t g_mount_num = 0;
char g_mount_name[20][128];
uint32_t g_partition_num = 0;

void sort_by_name(int mount_num)
{
    int index;
    char tmp[128];
    for (int i = 0; i < mount_num - 1; i++) 
    {
        index = i;
        for (int j = i + 1; j < mount_num; j++)
        {
            if (strcmp(g_mount_name[index], g_mount_name[j]) > 0)
            {
                index = j;
            }
        }
        strcpy(tmp, g_mount_name[index]);
        strcpy(g_mount_name[index], g_mount_name[i]);
        strcpy(g_mount_name[i], tmp);
    }
}


int get_mount_name()
{
    FILE *mounts;
    char procline[256];
    char mount[128], device[128], type[32], mode[128];
    int  rc;
    int  mount_num = 0;
    mounts = fopen(MOUNTS,"r");
    if (mounts == NULL)
    {
        ERROR_LOG("Df Error: could not open mounts file %s. Are we on the right OS?\n", MOUNTS);
        return -1;
    }

    while (fgets(procline, sizeof(procline), mounts)) 
    {
        rc = sscanf(procline, "%s %s %s %s ", device, mount, type, mode);
        if (!rc) 
        {
            continue;
        }

        if (!strncmp(mode, "ro", 2)) 
        {
            continue;
        }

        if (remote_mounts(device, type)) 
        {
            continue;
        }

        if (strncmp(device, "/dev/", 5) != 0 && strncmp(device, "/dev2/", 6) != 0) 
        {
            continue;
        }

        if (strstr(mount, "/.") != 0) {
            continue;
        }

        strncpy(g_mount_name[mount_num], mount, sizeof(g_mount_name[mount_num]));
        mount_num++;
    }
    fclose(mounts);

    g_mount_num = mount_num;
    sort_by_name(mount_num);

    return 0;
}



int get_all_mounts(c_value * val)
{

    if (0 != get_mount_name())
    {
        return -1;
    }

    val->reset();
    for (uint32_t i = 0; i < g_mount_num; i++)
    {
        val->sprintf("%s%s,", val->get_string(), g_mount_name[i]);
    }
    return 0;
}

bool remote_mounts(const char * device, const char * type)
{
    /* From ME_REMOTE macro in mountlist.h:
       A file system is `remote' if its Fs_name contains a `:'
       or if (it is of type smbfs and its Fs_name starts with `//'). */
    if (NULL != strchr(device, ':'))
    {
        return true;
    }

    if (!strcmp(type, "smbfs") && device[0]=='/' && device[1]=='/')
    {
        return true;
    }

    if (!strncmp(type, "nfs", 3))
    {
        return true;
    }
        
        
    if (!strcmp(type, "autofs"))
    {
        return true;
    }
    
    if (!strncmp(type, "devpts", 6))
    {
        return true;
    }

    if (!strncmp(type, "usbfs", 5))
    {
        return true;
    }

    if (!strncmp(type, "tmpfs", 5))
    {
        return true;
    }

    if (!strncmp(type, "udev", 4))
    {
        return true;
    }

    if (!strcmp(type,"gfs"))
    {
        return true;
    }

    if (!strcmp(type,"none"))
    {
        return true;
    }

    return false;
}



int get_mount_info(const char * args, int kind, c_value * val)
{
    struct statvfs svfs;
    char buf[64] = {0};
    char ar_buf[64];
    int r;
    *val = 0;
    strcpy(ar_buf, args); 
    int len = strlen(ar_buf); 
    if(len>=2 && ar_buf[len-1] =='/') 
    {//去掉最后一个'/' 
        ar_buf[len-1] = 0; 
    } 

    r = get_mount_basic_info(ar_buf, mount_dir_by_mount, buf);
    if (0 != r)
    {
        return -1;
    }

    r = statvfs(buf, &svfs);
    if (0 != r)
    {
        ERROR_LOG("get mount info by mount[%s:%s] error : %s", args, buf, strerror(errno));
        return -1;
    }

    switch (kind) 
    {
        case disk_total_by_mount:
            *val = (uint64_t)(svfs.f_bsize) * (uint64_t)(svfs.f_blocks);
            break;
        case disk_free_by_mount:
            *val = (uint64_t)(svfs.f_bsize) * (uint64_t)(svfs.f_bavail);
            break;
        case inodes_total_by_mount:
            *val = (uint64_t)(svfs.f_files);
            break;
        case inodes_free_by_mount:
            *val = (uint64_t)(svfs.f_favail);
            break;
        default :
            ERROR_LOG("get mount info by mount error : type = %u", kind);
            return -1;
    }
    return 0;
}

int get_mount_basic_info(const char * args, int kind, char * buf)
{
    FILE *mounts;
    char procline[256];
    char mount[128], device[128], type[32], mode[128];
    int  rc;
    buf[0] = 0;
    mounts = fopen(MOUNTS,"r");
    if(mounts == NULL) {
        ERROR_LOG("Df Error: could not open mounts file %s. Are we on the right OS?\n", MOUNTS);
        return -1;
    }
    while (fgets(procline, sizeof(procline), mounts)) {
        rc = sscanf(procline, "%s %s %s %s ", device, mount, type, mode);
        if (!rc) {
            continue;
        }
        switch (kind) {
            case mount_device_by_mount:
                if((strcmp(args, mount) == 0) || (strcmp(args, device) == 0)) {
                    strncpy(buf, device, strlen(device) + 1);
                    fclose(mounts);
                    return 0;
                } else {
                    break;
                }
            case mount_dir_by_mount:
                if((strcmp(args, mount) == 0) || (strcmp(args, device) == 0)) {
                    strncpy(buf, mount, strlen(mount) + 1);
                    fclose(mounts);
                    return 0;
                } else {
                    break;
                }
            case mount_type_by_mount:
                if((strcmp(args, mount) == 0) || (strcmp(args, device) == 0)) {
                    strncpy(buf, type, strlen(type) + 1);
                    fclose(mounts);
                    return 0;
                } else {
                    break;
                }
            default:
                ERROR_LOG("get mount info type error : %u", kind);
                fclose(mounts);
                return -1;
        }
    }
    //not found
    ERROR_LOG("can not find device or mount dir : %s", args);
    fclose(mounts);
    return -1;
}


int get_partition_info()
{
    FILE * fp = popen(PARTITION_CMD, "r");
    if (!fp)
    {
        ERROR_LOG("popen failed, %s", PARTITION_CMD);
        return -1;
    }

    uint32_t num = 0;
    static char buf[1024] = {0};
    while (fgets(buf, sizeof(buf), fp))
    {
        char * p_type = strstr(buf, " TYPE=");
        if (NULL == p_type)
        {
            continue;
        }

        // 跳过 TYPE="
        p_type += 7;

        // 白名单过滤
        if (0 != strncmp(p_type, "ext3", 4)
            && 0 != strncmp(p_type, "ext4", 4)
            && 0 != strncmp(p_type, "xfs", 3))
        {
            continue;
        }

        char * p_name = buf;
        const char * str_1 = "/dev/mapper/vgtaomee";
        if (0 == strncmp(p_name, str_1, strlen(str_1)))
        {
            continue;
        }

        num++;
    }

    pclose(fp);

    g_partition_num = num;
    return 0;
}

int get_mount_num(uint32_t * p_num)
{
    if (0 != get_mount_name())
    {
        return -1;
    }

    *p_num = g_mount_num;
    return 0;
}

int get_partition_num(uint32_t * p_num)
{
    if (0 != get_partition_info())
    {
        return -1;
    }

    *p_num = g_partition_num;
    return 0;
}


int all_partitions_mounted_func(c_value * v)
{
    uint32_t mount_num = 0;
    if (0 != get_mount_num(&mount_num))
    {
        return -1;
    }

    uint32_t partition_num = 0;
    if (0 != get_partition_num(&partition_num))
    {
        return -1;
    }

    if (partition_num != mount_num)
    {
        *v = 2;
    }
    else
    {
        *v = 1;
    }

    return 0;
}
