#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "notifier.h"
#include "benchapi.h"

static char buffer[4096];
static int pipe_handles[2];
int pipe_create()
{
	int ret_code = -1;
	if (pipe (pipe_handles) == -1)
		goto create_pipe_exit;

	fcntl(pipe_handles[0], F_SETFL, O_NONBLOCK | O_RDONLY);
	fcntl(pipe_handles[1], F_SETFL, O_NONBLOCK | O_WRONLY);

	fcntl(pipe_handles[0], F_SETFD, FD_CLOEXEC);
	fcntl(pipe_handles[1], F_SETFD, FD_CLOEXEC);
	return 0;
create_pipe_exit:	
	BOOT_LOG (ret_code, "create notify pipe");
}

void wr_pipe_close () {
	close (pipe_handles[1]);
}

void rd_pipe_close () {
	close (pipe_handles[0]);
}

int pipe_rd_fd () {
	return pipe_handles[0];
}

void read_pipe () {
	read (pipe_handles[0], buffer, sizeof (buffer));
}

void write_pipe () {
	char notify_chr;
	write (pipe_handles[1], &notify_chr, 1);
}
