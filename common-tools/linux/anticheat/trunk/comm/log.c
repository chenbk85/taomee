#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <glib.h>

#include "log.h"

#ifdef  likely
#undef  likely
#endif
#define likely(x) __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x) __builtin_expect(!!(x), 0)

#define MAX_LOG_CNT 10000000

static log_lvl_t  log_level	 = log_lvl_debug;	  // default log level
static log_dest_t g_log_dest = log_dest_terminal; // write log to terminal by default
static int g_multi_thread;
static int has_init;
static int max_log_files;
static uint32_t log_size;
static char log_pre[32];
static char log_dir[256];

static const char* log_color[] = {
	"\e[1m\e[35m", "\e[1m\e[33m", "\e[1m\e[34m", "\e[1m\e[31m",
	"\e[1m\e[32m", "\e[1m\e[36m", "\e[1m\e[1m", "\e[1m\e[37m",	"\e[1m\e[37m"
};
static const char* color_end = "\e[m";

static struct fds_t {
	int		seq;
	int		opfd;
	int		day;
	char	base_filename[64];
	int		base_filename_len;
} fds_info[log_lvl_max];

enum {
	log_buf_sz	= 8192
};

static const char* lognames[] = { "crit", "error", "warn", "info", "debug", "trace" };

static pthread_mutex_t g_shift_fd_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SHIFT_FD_LOCK() \
		if (g_multi_thread) \
			pthread_mutex_lock(&g_shift_fd_mutex)

#define SHIFT_FD_UNLOCK() \
		if (g_multi_thread) \
			pthread_mutex_unlock(&g_shift_fd_mutex)

static inline int
get_logfile_seqno(const char* filename, int loglvl)
{
	return atoi(&filename[fds_info[loglvl].base_filename_len + 10]);
}

static inline void
log_file_name(int lvl, int seq, char* file_name, const struct tm* tm)
{
	assert((lvl >= log_lvl_crit) && (lvl <= log_lvl_trace));

	sprintf(file_name, "%s_%04d%02d%02d_%03d", fds_info[lvl].base_filename,
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
}

static inline void
log_file_path(int lvl, int seq, char* file_name, const struct tm* tm)
{
	assert((lvl >= log_lvl_crit) && (lvl <= log_lvl_trace));

	sprintf(file_name, "%s/%s_%04d%02d%02d_%03d", log_dir, fds_info[lvl].base_filename,
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
}

static void
rm_files_by_seqno(int loglvl, int seqno, const struct tm* tm)
{
	char filename[128];

	log_file_name(loglvl, seqno, filename, tm);

	DIR* dir = opendir(log_dir);
	if (!dir) {
		return;
	}

	struct dirent* dentry;
	while ((dentry = readdir(dir))) {
		if ( (seqno == get_logfile_seqno(dentry->d_name, loglvl))
				&& (strncmp(dentry->d_name, fds_info[loglvl].base_filename, fds_info[loglvl].base_filename_len) == 0)
				&& (strcmp(filename, dentry->d_name) != 0) ) {
			char filepath[FILENAME_MAX];
			snprintf(filepath, sizeof(filepath), "%s/%s", log_dir, dentry->d_name);
			remove(filepath);
			// if there are duplicated seqno caused by the '!dir' above and thus leads to some bugs, we should remove the 'break;' below
			break;
		}
	}

	closedir(dir);
}

static int
get_log_seq_nonrecycle(int lvl)
{
	char file_name[FILENAME_MAX];

	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);	

	int seq;
	for (seq = 0; seq != MAX_LOG_CNT; ++seq) {
		log_file_path(lvl, seq, file_name, &tm);
		if (access(file_name, F_OK) == -1) {
			break;
		}
	}

	return (seq ? (seq - 1) : 0);
}

static int
get_log_seq_recycle(int lvl)
{
	char file_name[FILENAME_MAX] = { 0 };

	DIR* dir = opendir(log_dir);
	if (!dir) {
		return -1;
	}

	struct dirent* dentry;
	while ((dentry = readdir(dir))) {
		if ( (strncmp(dentry->d_name, fds_info[lvl].base_filename, fds_info[lvl].base_filename_len) == 0)
				&& (strcmp(dentry->d_name, file_name) > 0) ) {
			snprintf(file_name, sizeof(file_name), "%s", dentry->d_name);
		}
	}

	closedir(dir);

	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);

	if (file_name[0] == '\0') {
		log_file_name(lvl, 0, file_name, &tm);
	}

	char* date  = &file_name[fds_info[lvl].base_filename_len];
	char  today[9];
	int   seqno = get_logfile_seqno(file_name, lvl);

	snprintf(today, sizeof(today), "%4d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	if (strncmp(today, date, 8)) {
		++seqno;
	}

	return seqno % max_log_files;
}

static inline int
request_log_seq(int lvl)
{
	return (max_log_files ? get_log_seq_recycle(lvl) : get_log_seq_nonrecycle(lvl));
}

static int
open_fd(int lvl, const struct tm* tm)
{
	int flag = O_WRONLY | O_CREAT | O_APPEND/* | O_LARGEFILE*/;

	char file_name[FILENAME_MAX];
	log_file_path(lvl, fds_info[lvl].seq, file_name, tm);

	fds_info[lvl].opfd = open(file_name, flag, 0644);
	if (fds_info[lvl].opfd != -1) {
		fds_info[lvl].day = tm->tm_yday;

		flag  = fcntl(fds_info[lvl].opfd, F_GETFD, 0);
		flag |= FD_CLOEXEC;
		fcntl(fds_info[lvl].opfd, F_SETFD, flag);
		// remove files only if logfile recyle is used
		if (max_log_files) {
			rm_files_by_seqno(lvl, fds_info[lvl].seq, tm);
		}
	}

	return fds_info[lvl].opfd;
}

static int shift_fd(int lvl, const struct tm* tm)
{
	SHIFT_FD_LOCK();

	if ( unlikely((fds_info[lvl].opfd < 0) && (open_fd(lvl, tm) < 0)) ) {
		SHIFT_FD_UNLOCK();
		return -1;
	}

	off_t length = lseek(fds_info[lvl].opfd, 0, SEEK_END);
	if (likely((length < log_size) && (fds_info[lvl].day == tm->tm_yday))) {
		SHIFT_FD_UNLOCK();
		return 0;
	}

	close(fds_info[lvl].opfd);

	if (max_log_files) {
		// if logfile recycle is used
		fds_info[lvl].seq = (fds_info[lvl].seq + 1) % max_log_files;
	} else {
		// if logfile recycle is not used
		if (fds_info[lvl].day != tm->tm_yday) {
			fds_info[lvl].seq = 0;
		} else {
			fds_info[lvl].seq = (fds_info[lvl].seq + 1) % MAX_LOG_CNT;
		}
	}

	int ret = open_fd(lvl, tm);
	SHIFT_FD_UNLOCK();
	return ret;
}

void boot_log(int ok, int dummy, const char *fmt, ...)
{
#if 0
#define SCREEN_COLS	80
#define BOOT_OK		"\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL	"\e[1m\e[31m[ failed ]\e[m"
#endif
#define SCREEN_COLS	80
#define BOOT_OK		"OK"
#define BOOT_FAIL	"\e[1m\e[31m FAILED \e[m"

	int end, i, pos;
	va_list ap;

	char log_buffer[log_buf_sz];

	va_start(ap, fmt);
	end = vsprintf(log_buffer, fmt, ap);
	va_end(ap);

	pos = SCREEN_COLS - 10 - (end - dummy) % SCREEN_COLS;
	for (i = 0; i < pos; i++)
		log_buffer[end + i] = ' ';
	log_buffer[end + i] = '\0';

	strcat(log_buffer, ok == 0 ? BOOT_OK : BOOT_FAIL);
	printf("\r%s\n", log_buffer);

	if (ok != 0)
		exit(ok);
}

void write_log(int lvl, const char *fmt, ...)
{
	if (lvl > log_level) {
		return;
	}

	va_list ap;
	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);	va_start(ap, fmt);

	if (unlikely(!has_init || (g_log_dest & log_dest_terminal))) {
		va_list aq;
		va_copy(aq, ap);
		switch (lvl) {
		case log_lvl_crit:		
		case log_lvl_error:
			fprintf(stderr, "%s %04d-%02d-%02d %02d:%02d:%02d\t",
					log_color[lvl], tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
					tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stderr, fmt, aq);
			fprintf(stderr, "%s", color_end);
			break;
		default:
			fprintf(stdout, "%s %04d-%02d-%02d %02d:%02d:%02d\t",
					log_color[lvl], tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
					tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stdout, fmt, aq);
			fprintf(stdout, "%s", color_end);
			break;
		}
		va_end(aq);
	}

	if (unlikely(!(g_log_dest & log_dest_file) || (shift_fd(lvl, &tm) < 0))) {
		va_end(ap);
		return;
	}

	char log_buffer[log_buf_sz];
	int pos = snprintf(log_buffer, log_buf_sz,
			"[%04d-%02d-%02d %02d:%02d:%02d]\t",
			tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
	int end = vsnprintf(log_buffer + pos, log_buf_sz - pos, fmt, ap);
	va_end(ap);

	if (unlikely(end >= log_buf_sz - pos)) {
		end = log_buf_sz - pos;
		log_buffer[log_buf_sz - 1] = '\n';
	}

	write(fds_info[lvl].opfd, log_buffer, end + pos);
}

void write_syslog(int lvl, const char* fmt, ...)
{
	if (lvl > log_level) {
		return;
	}

	va_list ap;
	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);
	va_start(ap, fmt);

	if (unlikely(!has_init || (g_log_dest & log_dest_terminal))) {
		switch (lvl) {
		case log_lvl_crit:		
		case log_lvl_error:
			fprintf(stderr, "%s%02d:%02d:%02d ", log_color[lvl], tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stderr, fmt, ap);
			fprintf(stderr, "%s", color_end);
			break;
		default:
			fprintf(stdout, "%s%02d:%02d:%02d ", log_color[lvl], tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stdout, fmt, ap);
			fprintf(stdout, "%s", color_end);
			break;
		}
	}

	if (g_log_dest & log_dest_file) {		
		char log_buffer[log_buf_sz];
		int pos = snprintf(log_buffer, log_buf_sz, "[%02d:%02d:%02d][%05d]", tm.tm_hour, tm.tm_min, tm.tm_sec, getpid());
		vsnprintf(log_buffer + pos, log_buf_sz - pos, fmt, ap);
		syslog(lvl, "%s", log_buffer);
	}

	va_end(ap);
}

int log_init(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name)
{
	assert((maxfiles >= 0) && (maxfiles <= MAX_LOG_CNT));

	log_level = lvl;

	if (!dir || (strlen(dir) == 0)) {
		return 0;
	}

	int i, ret_code = -1;

	log_size      = size;
	max_log_files = maxfiles;
	if ((lvl < log_lvl_crit) || (lvl > log_lvl_trace)) {
		fprintf(stderr, "init log error, invalid log level=%d\n", lvl);
		goto exit_entry;
	}

	/*
	 * log file is no larger than 2GB
	 */
	if ( size > (1 << 31) ) {
		fprintf(stderr, "init log error, invalid log size=%d\n", size);
		goto exit_entry;
	}

	if (g_mkdir_with_parents(dir, 0755) == -1) {
		fprintf(stderr, "invalid log dir: %s", dir);
		goto exit_entry;
	}

	if (access(dir, W_OK)) {
		fprintf(stderr, "access log dir %s error, %m\n", dir);
		goto exit_entry;
	}

	strncpy(log_dir, dir, sizeof(log_dir) - 1);
	if (pre_name != NULL) {
		strncpy(log_pre, pre_name, sizeof(log_pre) - 1);
	}

	for (i = log_lvl_crit; i <= log_lvl_trace; i++) {
		fds_info[i].base_filename_len = snprintf(fds_info[i].base_filename,
				sizeof(fds_info[i].base_filename), "%s%s", log_pre, lognames[i]);

		fds_info[i].opfd = -1;
		fds_info[i].seq  = request_log_seq(i);

		if (fds_info[i].seq < 0) {
			goto exit_entry;
		}
	}

	g_log_dest  = log_dest_file;
	has_init    = 1;
	ret_code    = 0;
exit_entry:
	BOOT_LOG(ret_code, "Set log dir %s, per file size %g MB", dir, size / 1024.0 / 1024.0);
}

void set_log_dest(log_dest_t dest)
{
	assert(has_init);

	g_log_dest = dest;
}

void enable_multi_thread()
{
	g_multi_thread = 1;
}

