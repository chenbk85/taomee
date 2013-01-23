/**
 * @file process_info.h
 * @brief 获取进程信息类
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-07
 */

#ifndef _H_PROCESS_INFO_H
#define _H_PROCESS_INFO_H

#include <sys/types.h>

#include <string>


class process_info {
public:
    /**
     * @brief 获得uid
     * @return uid
     */
    static uid_t uid();

    /**
     * @brief 获得username
     * @return username
     */
    static std::string username();

    /**
     * @brief 获得hostname
     * @return hostname
     */
    static std::string hostname();

    /**
     * @brief 根据pid获得进程当前状态
     * @return 进程当前状态
     */
    static std::string get_proc_status(pid_t pid);

    /**
     * @brief 获得进程当前所打开的fd数
     * @return 当前所打开的fd数目
     */
    static int get_opened_files(pid_t pid);
};
#endif
