/** 
 * ========================================================================
 * @file node.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-03
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/wait.h>


#include "xml.h"
#include "node.h"
#include "db.h"
#include "proto.h"
#include "itl_timer.h"
#include "collect_interface.h"


#define g_http_transfer SINGLETON(Chttp_transfer)

char g_node_ip_str[IP_STRING_LEN] = {0};
uint32_t g_node_ip = 0;
uint32_t g_node_id = 0;
char g_server_tag[SERVER_TAG_LEN] = {0};
uint32_t g_start_timestamp = 0;

// 系统类型
#ifdef CENTOS
const char * g_os = "centos";
#else
const char * g_os = "debian";
#endif

// 自动更新的url
const char * g_auto_update_url = config_get_strval("auto_update_url", "http://10.1.1.63/itl-auto-update/");

// metric so 的本地目录
const char * g_metric_so_dir = config_get_strval("so_dir", "./so/");

// 框架文件
c_file * g_server_bin = new c_file("./bin/itl_node");

// libnode.so
c_file * g_node_bin = new c_file(config_get_strval("plugin_file", "./bin/libnode.so"));

// bench.conf
c_file * g_bench_conf = new c_file("./conf/bench.conf");

// work.conf
c_file * g_work_conf = new c_file(config_get_strval("work_conf", "./conf/work.conf"));

// 重启脚本
c_file * g_restart_script = new c_file(config_get_strval("restart_script", "./bin/restart_node.sh"));

// 控制脚本
c_file * g_control_script = new c_file("./itl_node.sh");

bool g_collect_flag = true;
bool g_auto_update_flag = config_get_intval("enable_auto_update", 1) ? true : false;

timer_head_t g_node_event;


int get_node_ip()
{

    char node_ip_str[IP_STRING_LEN] = {0};

    if (0 != get_inside_ip(node_ip_str))
    {
        return -1;
    }


    uint32_t node_ip = ip2long(node_ip_str);

    int ret = -1;
    if (node_ip != g_node_ip)
    {
        ret = 0;
        g_node_ip = node_ip;
        STRNCPY(g_node_ip_str, node_ip_str, IP_STRING_LEN);
        DEBUG_LOG("node ip: %s, %u", g_node_ip_str, g_node_ip);

    }


    return ret;
}


int monitor_node_ip(void * owner, void * data)
{
    ADD_TIMER_EVENT(&g_node_event, monitor_node_ip, reinterpret_cast< void *>(1), get_now_tv()->tv_sec + MONITOR_NODE_IP_INTERVAL);

    if (data)
    {
        if (0 == get_node_ip())
        {
            // ip变了，直接重启node
            restart_node();
        }

    }

    return 0;
}


int restart_node()
{
    // 每次都从远程获取重启脚本
    // 好处在于能够保证本地和远程的文件一致
    // 通过控制远程文件，保证重启成功
    char url[PATH_MAX] = {0};
    snprintf(url, sizeof(url), "%s/script/restart_node.sh", g_auto_update_url);
    int ret = download_file(g_restart_script->m_full, url, NULL);
    if (ret != 0)
    {
        return ret;
    }
    // 加权限
    chmod(g_restart_script->m_full, 0755);
    if (-1 == access(g_restart_script->m_full, F_OK | R_OK | X_OK))
    {
        // 还有问题，就终止协议处理
        return NODE_ERR_DOWNLOAD_FAIL;
    }

    g_spawn_command_line_async(g_restart_script->m_full, NULL);

    return 0;
}


/**
 *@brief 读取数据的回调。
 */
size_t read_data(void * buffer, size_t size, size_t nmemb, void * user_p)
{
    return fread(buffer, size, nmemb, (FILE *)user_p);
}


int upload_file(const char * src, const char * dst)
{
    FILE * fp = fopen(src, "rb");
    if (NULL == fp)
    {
        return NODE_ERR_ACCESS;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    CURL * cur = curl_easy_init();
    if (NULL == cur)
    {
        fclose(fp);
        return NODE_ERR_UNKNOWN;
    }


    curl_easy_setopt(cur, CURLOPT_URL, dst);
    // curl_easy_setopt(cur, CURLOPT_USERPWD, ""); 
    curl_easy_setopt(cur, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(cur, CURLOPT_READFUNCTION, &read_data);
    curl_easy_setopt(cur, CURLOPT_READDATA, fp);
    curl_easy_setopt(cur, CURLOPT_INFILESIZE_LARGE, file_size);
    char error_buf[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(cur, CURLOPT_ERRORBUFFER, error_buf);

    if (CURLE_OK != curl_easy_perform(cur))
    {
        DEBUG_LOG("curl perform error: %s", error_buf);
        fclose(fp);
        curl_easy_cleanup(cur);

        return NODE_ERR_UPLOAD_FILE;
    }

    fclose(fp);
    curl_easy_cleanup(cur);
    return 0;
}


int download_file(const char * dst, const char * src, const char * md5)
{

    if (!(*src) || !(*dst))
    {
        return NODE_ERR_REQ_DATA;
    }


    char temp_file[PATH_MAX] = {0};
    snprintf(temp_file, PATH_MAX, "%sXXXXXX", dst);
    mktemp(temp_file);

    g_http_transfer.download_file(src, temp_file);

    DEBUG_LOG("downloaded %s to %s", src, dst);

    if (NULL != md5)
    {
        char temp_md5[MD5_LEN + 1] = {0};
        if (0 != get_file_md5(temp_file, temp_md5))
        {
            ERROR_LOG("download file %s to %s failed", src, dst);
            return NODE_ERR_DOWNLOAD_FAIL;
        }

        if (0 != strncmp(md5, temp_md5, MD5_LEN))
        {
            ERROR_LOG("download file %s to %s, \noriginal md5: %s, actual md5: %s, mismatch", 
                    src, dst, md5, temp_md5);
            remove(temp_file);
            return NODE_ERR_MD5_MISMATCH;
        }


    }

    int ret = rename(temp_file, dst);
    if (0 != ret)
    {
        remove(temp_file);
        if (EBUSY == errno)
        {
            return NODE_ERR_FILE_BUSY;
        }
        else if (EACCES == errno)
        {
            return NODE_ERR_ACCESS;
        }
        else
        {
            return NODE_ERR_UNKNOWN;
        }
    }

    return 0;
}


int download_check_link(const char * dst, const char * src, const char * md5, check_file_func_t cb)
{
    char new_file[PATH_MAX] = {0};
    snprintf(new_file, sizeof(new_file), "%s.%s", 
            dst, 
            timestamp2compact_str(get_now_tv()->tv_sec));

    int ret = download_file(new_file, src, md5);
    if (0 != ret)
    {
        return ret;
    }

    if (NULL != cb)
    {
        ret = cb(new_file);
        if (0 != ret)
        {
            return ret;
        }
    }

    const char * new_file_name = get_filename(new_file);

    ret = link_file(new_file_name, dst);
    if (0 != ret)
    {
        return NODE_ERR_LINK_FILE;
    }

    return 0;
}

int download_metric_so(const char * filename)
{
    char so_url[PATH_MAX] = {0};
    snprintf(so_url, sizeof(so_url), "%s/so/%s/%s",
            g_auto_update_url, g_os, filename);
    char target_so[PATH_MAX] = {0};
    snprintf(target_so, sizeof(target_so), "%s/%s",
            g_metric_so_dir, filename);

    return download_check_link(target_so, so_url, NULL, NULL);
}


int link_file(const char * src, const char * dst)
{
    int ret = symlink(src, dst);
    if (0 != ret)
    {
        if (EEXIST == errno)
        {
            // 目标已经存在，需要处理
            char old_link[PATH_MAX] = {0};
            ssize_t read_size = readlink(dst, old_link, PATH_MAX);
            if (-1 == read_size)
            {
                ERROR_LOG("readlink %s failed, %s", dst, strerror(errno));
                return -1;
            }

            ret = unlink(dst);
            if (0 != ret)
            {
                ERROR_LOG("unlink %s failed, %s", dst, strerror(errno));
                return -1;
            }

            ret = symlink(src, dst);
            if (0 != ret)
            {
                ERROR_LOG("symlink %s -> %s failed, %s", src, dst, strerror(errno));
                // 恢复old_link
                unlink(dst);
                symlink(old_link, dst);
                return -1;
            }
        }
        else
        {
            ERROR_LOG("symlink %s -> %s failed, %s", src, dst, strerror(errno));
            return -1;
        }
    }

    return 0;
}


bool check_update()
{

    c_file * bin[] = {g_server_bin, g_node_bin};
    check_file_func_t bin_check[] = {NULL, check_so};

    c_file * conf[] = {g_bench_conf, g_work_conf};

    c_file * script[] = {g_control_script};


    static char post_data[1024] = {0};
    post_data[0] = 0;

#define WRITE(fmt, ...) snprintf(post_data + strlen(post_data), sizeof(post_data), fmt, __VA_ARGS__)

    WRITE("cmd=10001&ip=%s&os=%s", g_node_ip_str, g_os);


    uint32_t bin_num = array_elem_num(bin);
    if (0 != bin_num)
    {
        WRITE("%s", "&bin=");
        for (uint32_t i = 0; i < bin_num; i++)
        {
            WRITE("%s;%s;", bin[i]->m_filename, bin[i]->m_md5);
        }
    }


    uint32_t conf_num = array_elem_num(conf);
    if (0 != conf_num)
    {
        WRITE("%s", "&conf=");
        for (uint32_t i = 0; i < conf_num; i++)
        {
            WRITE("%s;%s;", conf[i]->m_filename, conf[i]->m_md5);
        }
    }

    uint32_t script_num = array_elem_num(script);
    if (0 != script_num)
    {
        WRITE("%s", "&script=");
        for (uint32_t i = 0; i < script_num; i++)
        {
            WRITE("%s;%s;", script[i]->m_filename, script[i]->m_md5);
        }
    }

    uint32_t so_num = so_file_mgr.size();
    if (0 != so_num)
    {

        WRITE("%s", "&so=");
        container_for_each(so_file_mgr, it)
        {
            metric_so_t * p_so = it->second;
            WRITE("%s;%s;", p_so->file.m_filename, p_so->file.m_md5);

        }

    }
#undef WRITE

    DEBUG_LOG("post data: %s", post_data);

    g_http_transfer.http_post(g_auto_update_url, post_data);


    std::string back = g_http_transfer.get_post_back_data();
    DEBUG_LOG("back: %s", back.c_str());



    bool need_restart = false;
    xmlDocPtr doc = xmlParseMemory(back.c_str(), back.length());
    if (!doc)
    {
        // 失败的
        return need_restart;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        xmlFreeDoc(doc);
        return need_restart;
    }


    const char * filename = NULL;
    char src_url[PATH_MAX] = {0};
    char src_filename[PATH_MAX] = {0};
    char new_md5[MD5_LEN + 1] = {0};

    for (uint32_t i = 0; i < bin_num; i++)
    {
        filename = bin[i]->m_filename;
        xmlnode_for_each(root, file_node)
        {
            if (!is_xmlnode_name(file_node, filename))
            {
                continue;
            }

            if (0 != get_xml_prop_string(new_md5, file_node, "md5"))
            {
                continue;
            }

            if (0 != get_xml_prop_string(src_filename, file_node, "download_filename"))
            {
                continue;
            }

            snprintf(src_url, sizeof(src_url), "%s/%s", g_auto_update_url, src_filename);

            if (0 == download_check_link(bin[i]->m_full, src_url, new_md5, bin_check[i]))
            {
                // 千万要记得加x权限
                chmod(bin[i]->m_full, 0755);
                need_restart = true;
            }

        }

    }


    for (uint32_t i = 0; i < conf_num; i++)
    {
        filename = conf[i]->m_filename;
        xmlnode_for_each(root, file_node)
        {
            if (!is_xmlnode_name(file_node, filename))
            {
                continue;
            }

            if (0 != get_xml_prop_string(new_md5, file_node, "md5"))
            {
                continue;
            }

            if (0 != get_xml_prop_string(src_filename, file_node, "download_filename"))
            {
                continue;
            }

            snprintf(src_url, sizeof(src_url), "%s/%s", g_auto_update_url, src_filename);

            remove(conf[i]->m_full);
            if (0 == download_check_link(conf[i]->m_full, src_url, new_md5, NULL))
            {
                need_restart = true;

            }
        }

    }

    for (uint32_t i = 0; i < script_num; i++)
    {
        filename = script[i]->m_filename;
        xmlnode_for_each(root, file_node)
        {
            if (!is_xmlnode_name(file_node, filename))
            {
                continue;
            }

            if (0 != get_xml_prop_string(new_md5, file_node, "md5"))
            {
                continue;
            }

            if (0 != get_xml_prop_string(src_filename, file_node, "download_filename"))
            {
                continue;
            }

            snprintf(src_url, sizeof(src_url), "%s/%s", g_auto_update_url, src_filename);

            if (0 == download_file(script[i]->m_full, src_url, new_md5))
            {
                // 要记得加x权限
                chmod(script[i]->m_full, 0755);
                get_file_md5(script[i]->m_full, script[i]->m_md5);

            }

        }

    }

    container_for_each(so_file_mgr, it)
    {
        metric_so_t * p_so = it->second;
        filename = p_so->file.m_filename;
        xmlnode_for_each(root, file_node)
        {
            if (!is_xmlnode_name(file_node, filename))
            {
                continue;
            }

            if (0 != get_xml_prop_string(new_md5, file_node, "md5"))
            {
                continue;
            }

            if (0 != get_xml_prop_string(src_filename, file_node, "download_filename"))
            {
                continue;
            }

            snprintf(src_url, sizeof(src_url), "%s/%s", g_auto_update_url, src_filename);

            if (0 == download_check_link(p_so->file.m_full, src_url, new_md5, check_so))
            {
                need_restart = true;
            }

        }

    }

    xmlFreeDoc(doc);

    return need_restart;
}


int check_update_timely(void * owner, void * data);


int init_check_update()
{
    check_update_timely(NULL, NULL);
    return 0;
}

int fini_check_update()
{
    check_update_timely(NULL, (void *)1);
    return 0;
}

int check_update_timely(void * owner, void * data)
{
    if (g_auto_update_flag)
    {
        if (check_update())
        {
            restart_node();
        }
    }


    if (!data)
    {

        ADD_TIMER_EVENT(&g_node_event, check_update_timely, NULL, get_now_tv()->tv_sec + config_get_intval("check_update_interval", ITL_NODE_CHECK_UPDATE_INTERVAL));
    }

    return 0;
}


int check_so(const char * new_so)
{

    // 检查新so的可加载性
    void * handle = dlopen(new_so, RTLD_NOW);
    if (NULL == handle)
    {
        ERROR_LOG("load new so %s failed, err: %s", new_so, dlerror());
        return NODE_ERR_OPEN_SO;
    }

    dlclose(handle);

    return 0;
}


int do_command(const char * cmd)
{

    gint status = 0;
    gboolean ret = g_spawn_command_line_sync(cmd, NULL, NULL, &status, NULL);
    if (!ret)
    {
        return NODE_ERR_FORK;
    }

    if (-1 == status)
    {
        return NODE_ERR_FORK;
    }
    else
    {
        return NODE_SUCC;
    }


}
