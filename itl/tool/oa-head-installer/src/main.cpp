/**
 * =====================================================================================
 *       @file  main.cpp
 *      @brief   
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  09/03/2010 06:31:41 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason , mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <errno.h>
#include <pwd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include "i_mysql_iface.h"

#define START_RED_TIP           "\e[1m\e[31m"
#define START_GREEN_TIP         "\e[1m\e[32m"
#define START_YELLOW_TIP        "\e[1m\e[33m"
#define END_COLOR_TIP           "\e[m"

const unsigned int MAX_STR_LEN = 4096;                 

#ifdef RELEASE_VERSION
char  g_install_prefix[PATH_MAX] = "/opt/taomee/oaadmin/";
char  g_db_host[16] = "192.168.0.131";
#else 
char  g_install_prefix[PATH_MAX] = "/opt/taomee/oa/";
char  g_db_host[16] = "10.1.1.27";
#endif
char  g_cur_path[PATH_MAX] = {0};
char  g_db_user[256] = {0};
char  g_db_pass[256] = {0};
char  g_db_name[256] = "db_itl";
int   g_nobody_uid = 0;
int   g_nobody_gid = 0;
int   g_grid_service_flag = 1;

static void str_trim(char *str)
{
    if(NULL == str) {    
        return;
    }    

    char *out = str; 
    char *in = str; 

    while(*in != '\0') {    
        if(*in == ' ' || *in == '\t' || *in == '\r' || *in == '\n') {    
            in++;
        }    
        else {    
            *out++ = *in++;
        }
    }
    *out = '\0';
    return;
}

static char *get_first_name_str(const char *src, char *buf, int buf_len)
{
    int  i = 0;
    if(src == NULL || buf == NULL || buf_len <= 0) {
        return NULL;
    }

    while(*src != '\0' && *src != ',' && *src != ';' && *src != ':' && i < buf_len) {
        buf[i++] = *src++;
    }

    if(i >= buf_len) {
        buf[buf_len - 1] = '\0';
    }
    else {
        buf[i] = '\0';
    }

    if(*src == '\0') {
        return NULL;
    }

    return (char*)(src + 1);
}

static void name_quoted(char *str, char *buf, int buf_len)
{
    if(NULL == str || buf == NULL || buf_len <= 0) {    
        return;
    }    

    char *p = str;
    char  tmp_name[256] = {0};
    char *write_pos = buf;
    int   remain_len = buf_len; 
    int   len = 0;

    while((p = get_first_name_str(p, tmp_name, sizeof(tmp_name))) != NULL) { 
        if(strlen(tmp_name) > 0) {
            len = sprintf(write_pos, "'%s',", tmp_name);
            write_pos += len;
            remain_len -= len;
        }
        memset(tmp_name, 0, sizeof(tmp_name));
    }

    if(strlen(tmp_name) > 0) {
        sprintf(write_pos, "'%s'", tmp_name);
    }

    return;
}

static int check_dir(const char* dir, int  * is_exists)
{
    if(access(dir, F_OK) != 0) {
        *is_exists = 0;
        if(0 != mkdir(dir, 0755)) {
            fprintf(stderr, "%sInstall dir does not exists and create it failed,sys error:%s%s.",
                    START_RED_TIP, strerror(errno), END_COLOR_TIP);
            return -1;
        }
    }
    else {
        *is_exists = 1;
    }

    return 0;
}

static char get_yes_no_input(const unsigned short max_times)
{
    unsigned short index =0; 
    char yes_no[5]={'N'};
    while(index < max_times) {   
        ++index;
        fgets(yes_no, 4, stdin);
        __fpurge(stdin);
        if('Y' == toupper(yes_no[0]) || 'N' == toupper(yes_no[0])) {   
            break;
        }   
        else {   
            if(index < max_times) {   
                printf("Input ERROR, please input [YES] or [NO]:");
            }   
        }   
    }   
    if(index == max_times) {    
        printf("Input error %d times, take it as [Ns] input!\n", max_times);
    }   

    return yes_no[0];
}

static int get_grid_info_and_install(i_mysql_iface* db_conn, const char *grid_name_list)
{
    char   install_path[PATH_MAX] = {'\0'};
    char   file_path[PATH_MAX] = {'\0'};
    char   select_sql[MAX_STR_LEN] = {'\0'};
    char   cmd_str[MAX_STR_LEN] = {'\0'};
    int    len = 0;
    FILE  *fp = NULL;

    strncpy(install_path, g_install_prefix, sizeof(install_path));

    if(strlen(grid_name_list) <= 0) {
        sprintf(select_sql, "SELECT grid_id FROM v_grid_info ORDER BY grid_id DESC");
    }
    else {
        sprintf(select_sql, "SELECT grid_id FROM v_grid_info WHERE grid_id IN(%s)", grid_name_list);
    }

    MYSQL_ROW row = NULL;
    int  ret = db_conn->select_first_row(&row, select_sql);
    if(ret < 0) {
        fprintf(stderr, "%sQuery db error.\nSQL:[%s] db error:[%s].%s\n",
                START_RED_TIP, select_sql, db_conn->get_last_errstr(), END_COLOR_TIP);
        return -1;
    }
    else if(ret == 0) {
        fprintf(stderr, "%sNo record satisfied the conditions.\nSQL:[%s]%s\n",
                START_RED_TIP, select_sql, END_COLOR_TIP);
        return -1;
    }

    int   exist = -1;
    char  ch  = 0;
    while(row != NULL) {
        if(!row[0]) {
            goto next;
        }

        strcat(install_path, "oa-head-");
        strcat(install_path, row[0]);

        if(check_dir(install_path, &exist) != 0) {
            fprintf(stderr, "OA_HEAD for grid %s%s %sinstall failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
            goto next;
        }

        if(exist) {
            fprintf(stderr, 
                    "The OA_HEAD for grid %s%s %salready exist%s.\n%sDo you want to overwrite it?[Y|N]", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP, START_GREEN_TIP);
            ch = get_yes_no_input(3);
            if(ch != 'Y' && ch != 'y') {
                goto next;
            }
        }

        chmod(install_path, 0755);
        chown(install_path, g_nobody_uid, g_nobody_gid);

        sprintf(cmd_str, "rm -rf %s/* ;cp -rf ./install-pkg/* %s", install_path, install_path);
        if(system(cmd_str) == -1) {
            fprintf(stderr, "Copy file to [%s] failed.\n", install_path);
            fprintf(stderr, "OA_HEAD for grid %s%s%s install failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
            goto next;
        }

        sprintf(file_path, "%s/start_oa_head.sh", install_path);
        fp = fopen(file_path, "a");
        if(fp == NULL) {
            fprintf(stderr, "Open file %s failed,sys error:%s.\n", file_path, strerror(errno));
            fprintf(stderr, "OA_HEAD for grid %s%s%s install failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
            goto next;
        }

        len = sprintf(cmd_str, "./oa_head -h%s -d%s -u%s -p%s -n%s \nexit 0\n", 
                g_db_host, g_db_name, g_db_user, g_db_pass, row[0]);
        if(fwrite(cmd_str, 1, strlen(cmd_str), fp) != strlen(cmd_str)) {
            fprintf(stderr, "Modify file %s failed.\n", file_path);
            fprintf(stderr, "OA_HEAD for grid %s%s%s install failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
            fclose(fp);
            goto next;
        }
        fclose(fp);

        sprintf(cmd_str, "chown nobody.nogroup -R %s/*", install_path);
        if(system(cmd_str) == -1) {
            fprintf(stderr, "Chown failed.\n");
            fprintf(stderr, "OA_HEAD for grid %s%s%s install failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
            goto next;
        }

        fprintf(stderr, "OA_HEAD for grid %s%s%s installed success.%s\nDo you want to start it[Y/N]?", 
                START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
        ch = 0;
        ch = get_yes_no_input(3);
        if(ch != 'Y' && ch != 'y')
            goto next;

        chdir(install_path);
        if(system("./start_oa_head.sh") == -1) {
            fprintf(stderr, "Start failed.\n");
            fprintf(stderr, "OA_HEAD for grid %s%s%s start failed.%s\n", 
                    START_RED_TIP, row[0], START_YELLOW_TIP, END_COLOR_TIP);
        }
        chdir(g_cur_path);
next:
        usleep(900000);
        fprintf(stderr, "\n");
        strncpy(install_path, g_install_prefix, sizeof(install_path));
        row = db_conn->select_next_row(false);
    }
    return 0;
}

int main(int argc, char **argv)
{
    //配置数据库连接
    i_mysql_iface  *conf_db_conn = NULL;

    //grid name 列表
    char    grid_name_list[4096] = {0};
    //配置库的配置项
    unsigned short db_port = 3306;

    int     ch = 0;
    //处理命令行参数
    while(1) {   
        ch = getopt(argc, argv, "n:d:h:p:u:P:i:s:");
        if(ch == -1) {
            break;
        }

        unsigned short  tmp_port = 3306;
        switch(ch)
        {   
        case 'n':  //grid name
            if(strlen(optarg) >= sizeof(grid_name_list)) {
                fprintf(stderr, "The grid name list is too long,the max length of name is:%u.\n", 4096);
                return -1;
            }
            strcpy(grid_name_list, optarg);
            break;
        case 'd':  //db name 
            strncpy(g_db_name, optarg, sizeof(g_db_name) - 1);
            break;
        case 'h':  //db host
            strncpy(g_db_host, optarg, sizeof(g_db_host) - 1);
            break;
        case 'P':  //db port
            tmp_port = atoi(optarg);
            db_port = tmp_port <= 0 ? db_port : tmp_port;
            break;
        case 'u':   //db user 
            strncpy(g_db_user, optarg, sizeof(g_db_user) - 1);
            break;
        case 'p':  //db passwd
            strncpy(g_db_pass, optarg, sizeof(g_db_pass) - 1);
            break;
        case 'i':  //install dir
            if(access(optarg, F_OK | R_OK | W_OK | X_OK)) {
                fprintf(stderr, "Wrong install directory: %s.\n", strerror(errno));
                return -1;
            }
            strncpy(g_install_prefix, optarg, sizeof(g_install_prefix));
            break;
        case 's':  //service flag 主还是备
            if(!strcasecmp(optarg, "s")) {
                g_grid_service_flag = 2;
            }
            else if(!strcasecmp(optarg, "m")) {
                g_grid_service_flag = 1;
            }
            else {   
                fprintf(stderr, "The grid_service_flag is wrong it should be s(slave)|m(master).");
                return -1; 
            }   
            break;
           default: break;
        }
    }

    if(strlen(g_db_user) == 0 || strlen(g_db_pass) == 0) {
        fprintf(stderr, "Critical:No db_user|db_passwd argument.\n");
        fprintf(stderr, 
                "Usage:oa_head_installer -n <grid_name_list> [-d <db_name>] [-h <db_host>]\n\t\t[-P <db_port>] -u <db_user> -p <db_passwd>\n");
        return -1;
    }

    int   len = strlen(g_install_prefix);
    assert(len > 0);

    if(len < (int)sizeof(g_install_prefix) && g_install_prefix[len - 1] != '/') {
        g_install_prefix[len] = '/';
        g_install_prefix[len + 1] = '\0';
    }

    if(create_mysql_iface_instance(&conf_db_conn) != 0) {
        fprintf(stderr, "Create mysql connect instance failed.\n");
        return -1;
    }

    if(conf_db_conn->init(g_db_host, db_port, g_db_name, g_db_user, g_db_pass, "utf8") != 0) {
        fprintf(stderr, "Connect to db:[%s] of host:[%s] failed.\n", g_db_name, g_db_host);
        return -1;
    }

    //取得nobody的uid和gid
    struct passwd *pw = NULL;
    pw = getpwnam("nobody");
    if(pw != NULL) {
        g_nobody_uid = pw->pw_uid;
        g_nobody_gid = pw->pw_gid;
    }

    //取得当前目录的绝对路径
    getcwd(g_cur_path, sizeof(g_cur_path));

    char  quoted_grid_name_list[5000] = {0};

    if(strlen(grid_name_list) > 0) {
        str_trim(grid_name_list);
        //name_quoted(grid_name_list, quoted_grid_name_list, sizeof(quoted_grid_name_list));
        strcpy(quoted_grid_name_list, grid_name_list);
    }

    get_grid_info_and_install(conf_db_conn, quoted_grid_name_list);
    conf_db_conn->uninit();
    conf_db_conn->release();

    return 0;
}
