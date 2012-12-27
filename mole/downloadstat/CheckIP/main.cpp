#include <string>
#include "log.h"
#include "CEpoll.h"

#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

const char *log_path = "./log";
const int log_size = (1<<30);

int daemon_init(void);

int main ()
{
    // daemon_init();

	string ip = "0.0.0.0";
	uint16_t	port = 3210;

	boot_log (0, 0, "Check IP server log...");
	log_init (log_path, 7, log_size, NULL);

	//some signal will be ignored here
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGCLD, SIG_IGN);

	daemon (1, 1);

    /* allow core dump */
    struct rlimit rlim;
    rlim.rlim_cur = 1<<30;
    rlim.rlim_max = 1<<30;
    setrlimit(RLIMIT_CORE, &rlim);

	C_Epoll ep(ip, port);
	if (ep.epoll_init() == -1)
    {
		ERROR_LOG ("Init error ! ");
		return -1;
	}

	ep.epoll_proc ();


	return 0;
}

/*
int daemon_init(void)
{ 
    pid_t pid;
    if((pid = fork()) < 0) 
        return -1;
    else if(pid != 0) 
        exit(0);
    setsid();
    chdir("/");
    umask(0);
    close(0);
    close(1);
    close(2);
    return 0; 
} 
*/
