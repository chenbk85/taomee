/**
 * @file process_info.cpp
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-07
 */
#include <iostream>

#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include "process_info.h"

static int fd_dir_filter(const struct dirent* d)
{
  if (::isdigit(d->d_name[0])) {
      return 1;
  }
  return 0;
}

uid_t process_info::uid()
{
    return ::getuid();
}

std::string process_info::username()
{
    struct passwd pwd;
    struct passwd* result = NULL;
    char buf[8192];
    const char* name = "unknownuser";

    ::getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);
    if (result) {
        name = pwd.pw_name;
    }

    return name;
}

std::string process_info::hostname()
{
    char buf[64] = "unknownhost";
    buf[sizeof(buf)-1] = '\0';
    ::gethostname(buf, sizeof(buf));
    return buf;
}

std::string process_info::get_proc_status(pid_t pid)
{
    std::string result_string;
    char buf[PATH_MAX] = {0};
    ::sprintf(buf, "/proc/%d/status", pid);
    FILE* fp = ::fopen(buf, "r");
    if (fp) {
        while (!::feof(fp)) {
            size_t n = ::fread(buf, 1, sizeof(buf), fp);
            if (n > 0) {
                result_string.append(buf, n);
            }
        }
    ::fclose(fp);
    }

    return result_string;
}

int process_info::get_opened_files(pid_t pid)
{
    std::string result_string;
    char buf[PATH_MAX] = {0};
    ::sprintf(buf, "/proc/%d/fd", pid);

    struct dirent** namelist;
    int n = ::scandir(buf, &namelist, fd_dir_filter, alphasort);
    int opened_files = n;

    if (n < 0) {
        return -1;
    } else {
        while (--n) {
            ::free(namelist[n]);
        }
        ::free(namelist);
    }

    return opened_files;
}

