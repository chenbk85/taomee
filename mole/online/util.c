#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "util.h"

int pipe_create(int* pipe_handles)
{
	if (pipe (pipe_handles) == -1)
		return -1;

	fcntl (pipe_handles[0], F_SETFL, O_NONBLOCK | O_RDONLY);
	fcntl (pipe_handles[1], F_SETFL, O_NONBLOCK | O_WRONLY);

	fcntl (pipe_handles[0], F_SETFD, FD_CLOEXEC);
	fcntl (pipe_handles[1], F_SETFD, FD_CLOEXEC);

	return 0;
}

//-----------------------------------------------------------
int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("Failed to Load %s", file), -1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load File %s", file);
}

int log_init_ex(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime)
{
    if (logtime == 0) {
        return log_init(dir,lvl,size,maxfiles,pre_name);
    } else {
        return log_init_t(dir,lvl,pre_name,logtime);
    }
}

