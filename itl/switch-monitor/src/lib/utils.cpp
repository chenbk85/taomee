/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file utils.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-03-09
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "utils.h"

#define LOCKFILE "./daemon.pid"

int mysignal(int sig, void(*signal_handler)(int))
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = signal_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	return sigaction(sig, &act, 0);
}

static int lockfile(int fd)
{
	struct flock fl = {0};

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;

	return fcntl(fd, F_SETLK, &fl);
}

int already_running()
{
	int     fd = -1;
	char    buf[16] = {0};

	fd = open(LOCKFILE, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
	if (fd < 0) {
		return -1;
	}
	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return 1;
		}
		return -1;
	}
	if (ftruncate(fd, 0) != 0) {
		return -1;
	}
	sprintf(buf, "%d", (int)getpid());
	if (write(fd, buf, strlen(buf) + 1) == -1) {
		return -1;
	}
	
	return 0;
}

extern char **environ;
static int prog_argc = -1;
static char **pp_prog_argv = NULL;
static char *p_prog_last_argv = NULL;

void init_proc_title(int argc, char *argv[]) 
{
	char **old_environ = environ;

	int environ_count = 0;
	while (old_environ[environ_count] != NULL) {
		++environ_count;
	}

	int i = 0;
	char **p = NULL;
	if ((p = (char **) malloc((environ_count + 1) * sizeof(char *))) != NULL) {
		for (i = 0; old_environ[i] != NULL; i++) {
			size_t envp_len = strlen(old_environ[i]);

			p[i] = (char *)malloc(envp_len + 1);
			if (p[i] != NULL) {
				strncpy(p[i], old_environ[i], envp_len + 1);
			}
		}

		p[i] = NULL;
		environ = p;
	}

	pp_prog_argv = argv;
	prog_argc = argc;

	for (i = 0; i < prog_argc; ++i) {
		if (!i || (p_prog_last_argv + 1 == argv[i])) {
			p_prog_last_argv = argv[i] + strlen(argv[i]);
		}
	}

	for (i = 0; old_environ[i] != NULL; ++i) {
		if ((p_prog_last_argv + 1) == old_environ[i]) {
			p_prog_last_argv = old_environ[i] + strlen(old_environ[i]);
		}
	}
}

void uninit_proc_title() 
{
	if (environ) {
		unsigned int i;

		for (i = 0; environ[i] != NULL; ++i) {
			free(environ[i]);
		}
		free(environ);
		environ = NULL;
	}
}

void set_proc_title(const char *fmt, ...) 
{
	int maxlen = (p_prog_last_argv - pp_prog_argv[0]) - 2;

	if (fmt == NULL) {
		return;
	}

	va_list msg;
	va_start(msg, fmt);
	memset(pp_prog_argv[0], 0, maxlen);
	vsnprintf(pp_prog_argv[0], maxlen, fmt, msg);
	va_end(msg);

	int i = 0;
	for (i = 1; i < prog_argc; ++i) {
		pp_prog_argv[i] = NULL;
	}
}

int get_proc_title(char *buf, size_t bufsz) 
{
	if (buf == NULL || bufsz == 0) {
		return strlen(pp_prog_argv[0]);
	}

	strncpy(buf, pp_prog_argv[0], bufsz);
	
	return strlen(buf);
}

/**
 * @brief   类型判断函数
 * @param   const char*  要传入的数字符串
 * @return  false = no, true = yes 
 */
bool is_numeric(const char *number)
{
    char   tmp[1];
    double x;

    if(!number)
        return false;
    else if(sscanf(number, "%lf%c", &x, tmp) == 1)
        return true;
    else
        return false;
}

bool is_positive(const char *number)
{
    if (is_numeric(number) && atof(number) > 0.0)
        return true;
    else
        return false;
}

bool is_negative(const char *number)
{
    if (is_numeric(number) && atof(number) < 0.0)
        return true;
    else
        return false;
}
bool is_nonnegative(const char *number)
{
    if (is_numeric(number) && atof(number) >= 0.0)
        return true;
    else
        return false;
}

bool is_percentage(const char *number)
{
    int x;
    if(is_numeric(number) && (x = atof(number)) >= 0 && x <= 100)
        return true;
    else
        return false;
}

bool is_integer(const char *number)
{
    long int n;

    if(!number || (strspn(number, "-0123456789 ") != strlen(number)))
        return false;

    n = strtol(number, NULL, 10);

    if(errno != ERANGE && n >= LONG_MIN && n <= LONG_MAX)
        return true;
    else
        return false;
}
bool is_intpos(const char *number)
{
    if(is_integer(number) && atoi(number) > 0)
        return true;
    else
        return false;
}

bool is_intneg(const char *number)
{
    if(is_integer(number) && atoi(number) < 0)
        return true;
    else
        return false;
}

bool is_intnonneg(const char *number)
{
    if(is_integer(number) && atoi(number) >= 0)
        return true;
    else
        return false;
}

bool is_intpercent(const char *number)
{
    int i;
    if(is_integer(number) && (i = atoi(number)) >= 0 && i <= 100)
        return true;
    else
        return false;
}
bool is_option(const char *str)
{
    if(!str)
        return false;
    else if(strspn(str, "-") == 1 || strspn(str, "-") == 2)
        return true;
    else
        return false;
}
/** 
 * @brief   解析一个host或网络地址
 * @param   address  地址
 * @param   af       地址族
 * @return  true or false 
 */
bool resolve_host_or_addr(const char *address, int family)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int    retval;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    retval = getaddrinfo(address, NULL, &hints, &res);

    if(retval != 0)
        return false;
    else 
    {   
        freeaddrinfo(res);
        return true;
    }   
}
/** 
 * @brief   获得当前机器的ip
 * @param   buf  缓存
 * @param   len  缓存长度
 * @return  0=success,-1=failed
 */

int get_local_ip_str(char *buf, unsigned int len)
{
    char   buffer[4096] = {0};
    char   host_name[MAX_STR_LEN] = {0};
    struct hostent host_ent, *hp;
    int    error_num = 0;

    if(0 != gethostname(host_name, sizeof(host_name)))
    {   
        //fprintf(stderr, "gethostname error.\n");
        return -1; 
    }   

    if(0 != gethostbyname_r(host_name, &host_ent, buffer, sizeof(buffer), &hp, &error_num))
    {   
        //fprintf(stderr, "gethostbyname_r failed,error:%s\n", hstrerror(error_num));
        return -1; 
    }   

    strncpy(buf, inet_ntoa(*(struct in_addr*)(host_ent.h_addr)), len);
    return 0;
}

/** 
 * @brief   根据hostname或者ip地址获取ip网络地址
 * @param   host_name  host name
 * @return  0=success,-1=failed
 */
int hostname_to_s_addr(const char *host_name)
{
    if(host_name == NULL)
    {
        return -1;
    }

    struct hostent host_ent, *hp;
    char   buf[4096] = {0};
    int    error_num = 0;

    if(0 != gethostbyname_r(host_name, &host_ent, buf, sizeof(buf), &hp, &error_num))
    {   
        //ERROR_LOG("gethostbyname_r failed,error:%s", hstrerror(error_num));
        return -1;
    }   
    return ((struct in_addr*)host_ent.h_addr)->s_addr;
}

/** 
 * @brief   sleep和usleep的封装
 * @param   usec  微妙数
 * @return  0=success,-1=failed
 */
int my_sleep(unsigned int usec)
{
    if(usec == 0)
        return 0;
    unsigned int sec = (unsigned int)(usec / 1000000);
    if(sec != 0)
        while((sec = sleep(sec)) != 0)
        {
            continue;
        }
    return usleep(usec % 1000000);
}
