/** 
 * ===============================================================
 * @file mkdir_p.cpp
 * @brief 抽取了mkdir源代码中递归建立多级目录部分，形成makedir函数
 * 实现了mkdir -p的效果
 * @author smyang（杨思敏）, smyang@taomee.com
 * @version 1.0.0
 * @date 2010-09-08
 * copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ===============================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include "mkdir_p.h"


#define ISSLASH(F)                  ((F) == '/')
#define IS_ABSOLUTE_FILE_NAME(F)    (ISSLASH((F)[0]))


/* Options for savewd_chdir.  */
enum
  {
    /* Do not follow symbolic links, if supported.  */
    SAVEWD_CHDIR_NOFOLLOW = 1,

    /* The directory should be readable, so fail if it happens to be
       discovered that the directory is not readable.  (Unreadable
       directories are not necessarily diagnosed, though.)  */
    SAVEWD_CHDIR_READABLE = 2,

    /* Do not chdir if the directory is readable; simply succeed
       without invoking chdir if the directory was opened.  */
    SAVEWD_CHDIR_SKIP_READABLE = 4
  };


/* Ensure that the ancestor directories of FILE exist, using an
   algorithm that should work even if two processes execute this
   function in parallel.  Modify FILE as necessary to access the
   ancestor directories, but restore FILE to an equivalent value
   if successful.

   WD points to the working directory, using the conventions of
   savewd.

   Create any ancestor directories that don't already exist, by
   invoking MAKE_DIR (FILE, COMPONENT, MAKE_DIR_ARG).  This function
   should return 0 if successful and the resulting directory is
   readable, 1 if successful but the resulting directory might not be
   readable, -1 (setting errno) otherwise.  If COMPONENT is relative,
   it is relative to the temporary working directory, which may differ
   from *WD.

   Ordinarily MAKE_DIR is executed with the working directory changed
   to reflect the already-made prefix, and mkancesdirs returns with
   the working directory changed a prefix of FILE.  However, if the
   initial working directory cannot be saved in a file descriptor,
   MAKE_DIR is invoked in a subprocess and this function returns in
   both the parent and child process, so the caller should not assume
   any changed state survives other than the EXITMAX component of WD,
   and the caller should take care that the parent does not attempt to
   do the work that the child is doing.

   If successful and if this process can go ahead and create FILE,
   return the length of the prefix of FILE that has already been made.
   If successful so far but a child process is doing the actual work,
   return -2.  If unsuccessful, return -1 and set errno.  */


int savewd_chdir(char const *dir, int options, int open_result[2])
{
    int fd = -1;
    int result = 0;

    /* Open the directory if requested, or if avoiding a race condition
       is requested and possible.  */
    if (open_result || (options & SAVEWD_CHDIR_NOFOLLOW))
    {
        fd = open (dir, (O_RDONLY | O_DIRECTORY | O_NOCTTY | O_NONBLOCK | (options & SAVEWD_CHDIR_NOFOLLOW ? O_NOFOLLOW : 0)));

        if (open_result)
        {
            open_result[0] = fd;
            open_result[1] = errno;
        }

        if (fd < 0 && (errno != EACCES || (options & SAVEWD_CHDIR_READABLE)))
        {
            result = -1;
        }
    }

    if (result == 0 && ! (0 <= fd && options & SAVEWD_CHDIR_SKIP_READABLE))
    {
        result = (fd < 0 ? chdir (dir) : fchdir (fd));
    }

    if (0 <= fd && ! open_result)
    {
        int e = errno;
        close (fd);
        errno = e;
    }

    return result;
}


int mkancesdirs(char * file)
{
    /* Address of the previous directory separator that follows an
       ordinary byte in a file name in the left-to-right scan, or NULL
       if no such separator precedes the current location P.  */
    char * sep = NULL;

    /* Address of the leftmost file name component that has not yet
       been processed.  */
    char * component = file;

    char * p = file;
    char c;
    char made_dir = 0;

    /* Scan forward through FILE, creating and chdiring into directories
       along the way.  Try MAKE_DIR before chdir, so that the procedure
       works even when two or more processes are executing it in
       parallel.  Isolate each file name component by having COMPONENT
       point to its start and SEP point just after its end.  */

    while ((c = *p++))
    {
        if (ISSLASH (*p))
        {
            if (! ISSLASH (c))
            {
                sep = p;
            }
        }
        else if (ISSLASH (c) && *p && sep)
        {
            /* Don't bother to make or test for "." since it does not
               affect the algorithm.  */
            if (! (sep - component == 1 && component[0] == '.'))
            {
                int make_dir_errno = 0;
                int savewd_chdir_options = 0;
                int chdir_result;

                /* Temporarily modify FILE to isolate this file name
                   component.  */
                *sep = '\0';

                /* Invoke MAKE_DIR on this component, except don't bother
                   with ".." since it must exist if its "parent" does.  */
                if (sep - component == 2 && component[0] == '.' && component[1] == '.')
                {
                    made_dir = 0;
                }
                else
                {
                    switch (mkdir (component, (S_IRWXU | S_IRWXG | S_IRWXO)))
                    {
                        case -1:
                            make_dir_errno = errno;
                            break;

                        case 0:
                            savewd_chdir_options |= SAVEWD_CHDIR_READABLE;
                            /* Fall through.  */
                        case 1:
                            made_dir = 1;
                            break;
                    }
                }

                if (made_dir)
                {
                    savewd_chdir_options |= SAVEWD_CHDIR_NOFOLLOW;
                }

                chdir_result = savewd_chdir (component, savewd_chdir_options, NULL);

                /* Undo the temporary modification to FILE, unless there
                   was a failure.  */
                if (chdir_result != -1)
                {
                    *sep = '/';
                }

                if (chdir_result != 0)
                {
                    if (make_dir_errno != 0 && errno == ENOENT)
                    {
                        errno = make_dir_errno;
                    }
                    return chdir_result;
                }
            }

            component = p;
        }

    }
    return (component - file);
}


int make_dir_parents(char * dir)
{
    int mkdir_errno = 0;

    if (mkdir_errno == 0)
    {
        int prefix_len = 0;

        prefix_len = mkancesdirs(dir);
        if (prefix_len < 0)
        {
            if (prefix_len < -1)
            {
                return 0;
            }
            mkdir_errno = errno;
        }

        if (0 <= prefix_len)
        {
            if (0 != mkdir (dir + prefix_len, 0755))
            {
                mkdir_errno = errno;
            }

            struct stat st;
            if (mkdir_errno == 0 || (mkdir_errno != ENOENT && stat (dir + prefix_len, &st) == 0 && S_ISDIR (st.st_mode)))
            {
                return 0;
            }
        }
    }

    return -1;
}


int makedir(const char * dir)
{
    char dir_buf[PATH_MAX] = {0};
    if (NULL == getcwd(dir_buf, PATH_MAX))
    {
        return -1;
    }
    char temp[PATH_MAX] = {0};
    sprintf(temp, "%s", dir);
    if (0 != make_dir_parents(temp))
    {
        // 使用系统命令mkdir -p建目录
        chdir(dir_buf);
        sprintf(dir_buf, "mkdir -p %s", dir);
        return (system(dir_buf));
    }
    return 0;
}


int mkdir_p(const char * dir)
{
    char dir_buf[PATH_MAX] = {0};
    if (NULL == getcwd(dir_buf, PATH_MAX))
    {
        return -1;
    }
    int ret = makedir(dir);

    if (0 != chdir(dir_buf))
    {
        ret = -1;
    }
    return ret;
}

int mkdir_p(const char * prefix, const char * dir)
{
    char dir_buf[PATH_MAX] = {0};
    if (NULL == getcwd(dir_buf, PATH_MAX))
    {
        return -1;
    }

    if (0 != chdir(prefix))
    {
        chdir(dir_buf);
        sprintf(dir_buf, "%s/%s", prefix, dir);
        return (mkdir_p(dir_buf));
    }

    int ret = makedir(dir);

    if (0 != chdir(dir_buf))
    {
        ret = -1;
    }
    return ret;
}

int mkdir_p(const char * prefix, uint32_t time)
{
    char time_str[15] = {0};
    sprintf(time_str, "%u", time);
    return mkdir_p(prefix, time_str);
}
