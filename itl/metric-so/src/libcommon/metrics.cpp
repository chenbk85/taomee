/**
 * =====================================================================================
 *       @file  metrics.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/24/2010 05:20:43 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdlib.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include "so_proto.h"
extern "C"
{
#include <libtaomee/log.h>
}
#include "metrics.h"
#include "dmidecode.h"
#include "itl_util.h"

#define BUFFSIZE 16384
#define NHASH 101
#define MULTIPLIER 31
#define SYNAPSE_FAILURE -1
#define SYNAPSE_SUCCESS 0

#define SCALING_MAX_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
//df -h -T -x tmpfs
#define DF_COMMAND "/bin/df -h -T -xtmpfs > .df_tmp"

//ntp时间从年开始，本地时间从年开始，这是两者之间的差值
#define JAN_1970 0x83aa7e80 //3600s*24h*(365days*70years+17days)
//x*10^(-6)*2^32 微妙数转 NtpTime 结构的 fraction 部分
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11)) 
//NTPFRAC的逆运算
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

//#define DEF_NTP_SERVER "210.72.145.44" //国家授时中心 ip
//#define DEF_NTP_SERVER "stdtime.gov.hk" //香港标准时间
//#define DEF_NTP_SERVER "10.1.1.2" //内网ntp服务器
//#define DEF_NTP_SERVER "pool.ntp.org"  //ntp官方服务器
//#define DEF_NTP_SERVER "132.163.135.132"
#define DEF_NTP_SERVER "ntp.shidc.taomee.com" //外网ntp服务器
#define DEF_NTP_PORT 123
#define DEF_TIMEOUT 10

//ntp时间戳结构
typedef struct {
	unsigned int integer;
	unsigned int fraction;
} NtpTime;

//校准信息结构
typedef struct {
	struct timeval dlytime;
	struct timeval offtime;
	struct timeval newtime;
} NtpServResp;

char sys_devices_system_cpu[32];
int cpufreq;

typedef struct {
  struct timeval last_read;
  float thresh;
  const char *name;
  char buffer[BUFFSIZE];
} timely_file_t;

typedef struct net_dev_stats {
  const char *name;
  unsigned long rpi;
  unsigned long rpo;
  unsigned long rbi;
  unsigned long rbo;
  net_dev_stats *next;
} net_dev_stats_t;

static net_dev_stats_t *netstats[NHASH];

char proc_cpuinfo[BUFFSIZE];
char proc_sys_kernel_osrelease[BUFFSIZE];
char file_system_list[BUFFSIZE];

timely_file_t proc_stat    = {{0,0}, 1., "/proc/stat"};
timely_file_t proc_net_dev = {{0}, 1., "/proc/net/dev"};
timely_file_t proc_loadavg   = {{0,0}, 5., "/proc/loadavg"};
timely_file_t proc_meminfo = {{0,0}, 5., "/proc/meminfo"};

/* Linux Specific, but we are in the Linux machine file. */
#define MOUNTS "/proc/mounts"

struct nlist {
    struct nlist *next;
    char *name;
};

#define DFHASHSIZE 101
static struct nlist *DFhashvector[DFHASHSIZE];
static char inside_ip[INET_ADDRSTRLEN] = {0};
static char outside_ip[INET_ADDRSTRLEN] = {0};
static char cpu_model[128] = {0};
static int cpu_num = 0; 

/* --------------------------------------------------------------------------- */
unsigned int DFhash(const char *s)
{
    unsigned int hashval;
    for (hashval=0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % DFHASHSIZE;
}

void str_trim(char *str);
void str_num(char *str);
/* --------------------------------------------------------------------------- */
/* From K&R C book, pp. 144-145 */
struct nlist *seen_before(const char *name)
{
    struct nlist *found = 0, *np;
    unsigned int hashval;

    /* lookup */
    hashval = DFhash(name);
    for (np = DFhashvector[hashval]; np; np = np->next) {
        if (!strcmp(name,np->name)) {
            found = np;
            break;
        }
    }
    if (!found) {    /* not found */
        np = (struct nlist *)malloc(sizeof(*np));
        if (!np || !(np->name = (char *)strdup(name))) {
            return NULL;
        }
        np->next = DFhashvector[hashval];
        DFhashvector[hashval] = np;
        return NULL;
    } else { /* found name */
        return found;
    }
}

void DFcleanup()
{
    struct nlist *np, *next;
    int i;
    for (i=0; i<DFHASHSIZE; i++) {
        /* Non-standard for loop. Note the last clause happens at the end of the loop. */
        for (np = DFhashvector[i]; np; np=next) {
            next=np->next;
            free(np->name);
            free(np);
        }
        DFhashvector[i] = 0;
    }
}

char *skip_token(const char *p)
{
    while (isspace((unsigned char)*p)) {
        p++;
    }
    while (*p && !isspace((unsigned char)*p)) {
        p++;
    }

    return (char *)p;
}

char *skip_whitespace(const char *p)
{
    while (isspace((unsigned char)*p)) {
        p++;
    }
    return (char *)p;
}

int remote_mount(const char *device, const char *type)
{
    /* From ME_REMOTE macro in mountlist.h:
       A file system is `remote' if its Fs_name contains a `:'
       or if (it is of type smbfs and its Fs_name starts with `//'). */
    return ((strchr(device,':') != 0)
            || (!strcmp(type, "smbfs") && device[0]=='/' && device[1]=='/')
            || (!strncmp(type, "nfs", 3)) || (!strcmp(type, "autofs"))
            || (!strcmp(type,"gfs")) || (!strcmp(type,"none")) );
}

float device_space(char *mount, char *device, double *total_size, double *total_free)
{
    struct statvfs svfs;
    uint32_t blocksize;
    uint32_t free;
    uint32_t size;
    /* The percent used: used/total * 100 */
    float pct = 0.0;

    /* Avoid multiply-mounted disks - not done in df. */
    if (seen_before(device)) {
        return pct;
    }

    if (statvfs(mount, &svfs)) {
        /* Ignore funky devices... */
        return pct;
    }

    free = svfs.f_bavail;
    size = svfs.f_blocks;
    blocksize = svfs.f_bsize;
    /* Keep running sum of total used, free local disk space. */
    *total_size += size * (double) blocksize;
    *total_free += free * (double) blocksize;
    /* The percentage of space used on this partition. */
    pct = size ? ((size - free) / (float) size) * 100 : 0.0;
    return pct;
}

float find_disk_space(double *total_size, double *total_free)
{
    FILE *mounts;
    char procline[256];
    char mount[128], device[128], type[32], mode[128];
    /* We report in GB = 1e9 bytes. */
    double reported_units = 1e9;
    /* Track the most full disk partition, report with a percentage. */
    float thispct, max = 0.0;
    int rc;

    /* Read all currently mounted filesystems. */
    mounts = fopen(MOUNTS,"r");
    if (!mounts) {
        ERROR_LOG("Df Error: could not open mounts file %s. Are we on the right OS?\n", MOUNTS);
        return max;
    }
    while (fgets(procline, sizeof(procline), mounts)) {
        rc = sscanf(procline, "%s %s %s %s ", device, mount, type, mode);
        if (!rc) {
            continue;
        }
        if (!strncmp(mode, "ro", 2)) {
            continue;
        }
        if (remote_mount(device, type)) {
            continue;
        }
        if (strncmp(device, "/dev/", 5) != 0 && strncmp(device, "/dev2/", 6) != 0) {
            continue;
        }

        thispct = device_space(mount, device, total_size, total_free);
        // DEBUG_LOG("Counting device %s (%.2f %%)", device, thispct);
        if (!max || max<thispct) {
            max = thispct;
        }
    }

    fclose(mounts);
    *total_size = *total_size / reported_units;
    *total_free = *total_free / reported_units;
    // DEBUG_LOG("For all disks: %.3f GB total, %.3f GB free for users.", *total_size, *total_free);

    DFcleanup();
    return max;
}

/*
** Helper functions to hash /proc/net/dev stats (Kernighan & Pike)
*/
static unsigned int hashval(const char *s)
{
  unsigned int hval;
  unsigned char *p;

  hval = 0;
  for (p = (unsigned char *)s; *p != '\0'; p++)
    hval = MULTIPLIER * hval + *p;
  return hval % NHASH;
}

static net_dev_stats *hash_lookup(char *devname, size_t nlen)
{
  int hval;
  net_dev_stats *stats;
  char *name=strndup(devname,nlen);

  hval = hashval(name);
  for (stats = netstats[hval]; stats != NULL; stats = stats->next)
  {
    if (strcmp(name, stats->name) == 0) {
      free(name);
      return stats;
    }
  }

  stats = (net_dev_stats *)malloc(sizeof(net_dev_stats));
  if ( stats == NULL )
  {
    ERROR_LOG("unable to allocate memory for /proc/net/dev/stats in hash_lookup(%s,%u)",name,(unsigned)nlen);
    free(name);
    return NULL;
  }
  stats->name = strndup(devname,nlen);
  stats->rpi = 0;
  stats->rpo = 0;
  stats->rbi = 0;
  stats->rbo = 0;
  stats->next = netstats[hval];
  netstats[hval] = stats;

  free(name);
  return stats;
}

float timediff(const struct timeval *this_time, const struct timeval *last_time)
{
  float diff;

  diff = ((double) this_time->tv_sec * 1.0e6 +
          (double) this_time->tv_usec -
          (double) last_time->tv_sec * 1.0e6 -
          (double) last_time->tv_usec) / 1.0e6;

  return diff;
}

int read_proc(const char *filename, char *buffer, int buf_len)
{
    int fd, read_len;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
         ERROR_LOG("slurpfile() open() error on file %s", filename);
         return -1;
    }

read:
    read_len = read(fd, buffer, buf_len);
    if (read_len <= 0) {
        if (errno == EINTR) {
            goto read;
        }

        ERROR_LOG("slurpfile() read() error on file %s", filename);
        close(fd);
        return -1;
    }
    if (read_len == buf_len) {
        --read_len;
        ERROR_LOG("slurpfile() read() buffer overflow on file %s", filename);
    }
    buffer[read_len] = '\0';
    close(fd);

    return read_len;
}

char *update_file(timely_file_t *tf)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    if (timediff(&now, &tf->last_read) > tf->thresh) {
        if(read_proc(tf->name, tf->buffer, BUFFSIZE) == -1) {
            ERROR_LOG("update_file() got an error from slurpfile() reading %s", tf->name);
            return (char *)-1;
        } else {
            tf->last_read = now;
        }
    }

    return tf->buffer;
}


int get_ifdata(
        net_dev_stats * ns,
        char * p, 
        unsigned long * p_rbi_rise, 
        unsigned long * p_rpi_rise, 
        unsigned long * p_rbo_rise, 
        unsigned long * p_rpo_rise)
{


    /* receive */
    unsigned long rbi = strtoul(p, &p ,10);
    unsigned long rpi = strtoul(p, &p ,10);

    /* skip unneeded metrics */
    for (int i = 0; i < 6; i++) 
    { 
        strtoul(p, &p, 10);
    }

    /* transmit */
    unsigned long rbo = strtoul(p, &p ,10);
    unsigned long rpo = strtoul(p, &p ,10);

    if (rbi < ns->rbi
     || rpi < ns->rpi
     || rbo < ns->rbo
     || rpo < ns->rpo)
    {
        return -1;
    }

    *p_rbi_rise = rbi - ns->rbi;
    *p_rpi_rise = rpi - ns->rpi;
    *p_rbo_rise = rbo - ns->rbo;
    *p_rpo_rise = rpo - ns->rpo;

    ns->rbi = rbi;
    ns->rpi = rpi;
    ns->rbo = rbo;
    ns->rpo = rpo;

    return 0;
}


static double bytes_in = 0, eth0_recv = 0, eth1_recv = 0, bytes_out = 0,
              eth0_send = 0, eth1_send = 0, pkts_in = 0, pkts_out = 0;
int update_ifdata(const char *caller)
{
    char * p;
    static struct timeval stamp = {0, 0};
    unsigned long l_bytes_in = 0, l_bytes_out = 0, l_pkts_in = 0, l_pkts_out = 0;
    unsigned long l_eth0_send = 0, l_eth1_send = 0, l_eth0_recv = 0, l_eth1_recv = 0;
    double l_bin, l_bout, l_pin, l_pout, l_e0in, l_e1in, l_e0out, l_e1out;
    bool m_eth0_fail = false;
    bool m_eth1_fail = false;

    p = update_file(&proc_net_dev);
    if (p == (char *)-1) {
        return -1;
    }
    if ((proc_net_dev.last_read.tv_sec != stamp.tv_sec) &&
            (proc_net_dev.last_read.tv_usec != stamp.tv_usec)) {
        // 跳过前面2行说明文字
        p = index (p, '\n')+1;
        p = index (p, '\n')+1;


        while (*p != 0x00)
        {
            char *src;
            size_t n = 0;

            while (p != 0x00 && isblank(*p)) {
                p++;
            }
            src = p;                //src指向网络接口名称的第一个字符
            while (p != 0x00 && *p != ':')   {
                n++;                //n为网络接口名称的字符个数
                p++;
            }

            p = index(p, ':');       //p指向网络接口名称的下一个字符

            // 只监控eth0和eth1

            if (p && 0 == strncmp(src, "eth0", n))
            {
                p++;
                net_dev_stats * ns = hash_lookup(src, n); 
                if (!ns) 
                {
                    ERROR_LOG("ERROR: hash_lookup().");
                    return -1;
                }

                unsigned long rbi_rise = 0;
                unsigned long rpi_rise = 0;
                unsigned long rbo_rise = 0;
                unsigned long rpo_rise = 0;
                if (0 == get_ifdata(ns, p, &rbi_rise, &rpi_rise, &rbo_rise, &rpo_rise))
                {
                    l_eth0_recv = rbi_rise;
                    l_eth0_send = rbo_rise;
                    l_bytes_in += rbi_rise;
                    l_bytes_out += rbo_rise;
                    l_pkts_in += rpi_rise;
                    l_pkts_out += rpo_rise;
                }
                else
                {
                    m_eth0_fail = true;
                }
            }


            if (p && 0 == strncmp(src, "eth1", n))
            {
                p++;
                net_dev_stats * ns = hash_lookup(src, n); 
                if (!ns) 
                {
                    ERROR_LOG("ERROR: hash_lookup().");
                    return -1;
                }

                unsigned long rbi_rise = 0;
                unsigned long rpi_rise = 0;
                unsigned long rbo_rise = 0;
                unsigned long rpo_rise = 0;
                if (0 == get_ifdata(ns, p, &rbi_rise, &rpi_rise, &rbo_rise, &rpo_rise))
                {
                    l_eth1_recv = rbi_rise;
                    l_eth1_send = rbo_rise;
                    l_bytes_in += rbi_rise;
                    l_bytes_out += rbo_rise;
                    l_pkts_in += rpi_rise;
                    l_pkts_out += rpo_rise;
                }
                else
                {
                    m_eth1_fail = true;
                }
            }
            p = index (p, '\n') + 1;    // skips a line
        }

        /*
         ** Compute timediff. Check for bogus delta-t
         */
        float t = timediff(&proc_net_dev.last_read, &stamp);
        if (t <  proc_net_dev.thresh) {
            ERROR_LOG("update_ifdata(%s) - Dubious delta-t: %f",caller,t);
            return -1;
        }
        stamp = proc_net_dev.last_read;

        /*
         ** Compute rates in local variables
         */
        // 转换成bit/s
        l_bin = 8 * l_bytes_in / t;
        l_bout = 8 * l_bytes_out / t;
        l_pin = l_pkts_in / t;
        l_pout = l_pkts_out / t;

        l_e0in = 8 * l_eth0_recv / t;
        l_e1in = 8 * l_eth1_recv / t;
        l_e0out = 8 * l_eth0_send / t;
        l_e1out = 8 * l_eth1_send / t;

#ifdef REMOVE_BOGUS_SPIKES
        /*
         ** Check for "invalid" data, caused by HW error. Throw away dubious data points
         ** FIXME: This should be done per-interface, with threshholds depending on actual link speed
         */	
        if (l_bin > 1e9 || l_bout > 1e9
         || l_pin > 1e8 || l_pout > 1e8
         || l_e0in > 1e9 || l_e0out > 1e9
         || l_e1in > 1e9 || l_e0out > 1e9)
         {
            ERROR_LOG("update_ifdata(%s): %g %g %g %g %g %g %g %g / %g",
                    caller,
                    l_bin, l_bout,
                    l_pin, l_pout,
                    l_e0in, l_e0out,
                    l_e1in, l_e1out,
                    t);
            return -1;
        }
#endif

        /*
         ** Finally return Values
         */
        if (!m_eth0_fail && !m_eth1_fail)
        {
            bytes_in  = l_bin;
            bytes_out = l_bout;
            pkts_in   = l_pin;
            pkts_out  = l_pout;
        }

        if (!m_eth0_fail)
        {
            eth0_send = l_e0out;
            eth0_recv = l_e0in;
        }

        if (!m_eth1_fail)
        {
            eth1_send = l_e1out;
            eth1_recv = l_e1in;
        }
    }

    return 0;
}

/*
 ** A helper function to determine the number of cpustates in /proc/stat (MKN)
 */
#define NUM_CPUSTATES_24X 4
#define NUM_CPUSTATES_26X 7
static unsigned int num_cpustates;

unsigned int num_cpustates_func()
{
    char *p;
    unsigned int i = 0;

    proc_stat.last_read.tv_sec = 0;
    proc_stat.last_read.tv_usec = 0;
    p = update_file(&proc_stat);
    if (p == (char *)-1) {
        return 0;
    }
    proc_stat.last_read.tv_sec = 0;
    proc_stat.last_read.tv_usec = 0;

    /*
     ** Skip initial "cpu" token
     */
    p = skip_token(p);
    p = skip_whitespace(p);
    /*
     ** Loop over file until next "cpu" token is found.
     ** i=4 : Linux 2.4.x
     ** i=7 : Linux 2.6.x
     */
    while (strncmp(p,"cpu",3)) {
        p = skip_token(p);
        p = skip_whitespace(p);
        i++;
    }

    return i;
}

unsigned long total_jiffies_func()
{
    char *p;
    unsigned long user_jiffies, nice_jiffies, system_jiffies, idle_jiffies,
                  wio_jiffies, irq_jiffies, sirq_jiffies;

    p = update_file(&proc_stat);
    if (p == (char *)-1) {
        return -1; 
    }
    p = skip_token(p);
    p = skip_whitespace(p);
    user_jiffies = (unsigned long)strtod(p, &p);
    p = skip_whitespace(p);
    nice_jiffies = (unsigned long)strtod(p, &p); 
    p = skip_whitespace(p);
    system_jiffies = (unsigned long)strtod(p , &p); 
    p = skip_whitespace(p);
    idle_jiffies = (unsigned long)strtod(p , &p);

    if (num_cpustates == NUM_CPUSTATES_24X) {
        return user_jiffies + nice_jiffies + system_jiffies + idle_jiffies;
    }

    p = skip_whitespace(p);
    wio_jiffies = (unsigned long)strtod(p , &p);
    p = skip_whitespace(p);
    irq_jiffies = (unsigned long)strtod(p , &p);
    p = skip_whitespace(p);
    sirq_jiffies = (unsigned long)strtod(p , &p);

    return user_jiffies + nice_jiffies + system_jiffies + idle_jiffies +
        wio_jiffies + irq_jiffies + sirq_jiffies; 
}

int pkts_in_func(c_value * val)
{

    update_ifdata("PI");
    *val = pkts_in;
    // DEBUG_LOG(" ********** pkts_in:  %f", pkts_in);
    return 0;
}

int pkts_out_func(c_value * val)
{

    update_ifdata("PO");
    *val = pkts_out;
    // DEBUG_LOG(" ********** pkts_out:  %f", pkts_out);
    return 0;
}

int bytes_out_func(c_value * val)
{

    update_ifdata("BO");
    *val = bytes_out;
    // DEBUG_LOG(" ********** bytes_out:  %f", bytes_out);
    return 0;
}

int bytes_in_func(c_value * val)
{

    update_ifdata("BI");
    *val = bytes_in;
    // DEBUG_LOG(" ********** bytes_in:  %f", bytes_in);
    return 0;
}

int eth0_send_func(c_value * val)
{

    update_ifdata("eth0 send");
    *val = eth0_send;
    // DEBUG_LOG(" ********** bytes_in:  %f", bytes_in);
    return 0;
}

int eth1_send_func(c_value * val)
{

    update_ifdata("eth1 send");
    *val = eth1_send;
    // DEBUG_LOG(" ********** bytes_in:  %f", bytes_in);
    return 0;
}

int eth0_recv_func(c_value * val)
{

    update_ifdata("eth0 recv");
    *val = eth0_recv;
    // DEBUG_LOG(" ********** bytes_in:  %f", bytes_in);
    return 0;
}

int eth1_recv_func(c_value * val)
{

    update_ifdata("eth1 recv");
    *val = eth1_recv;
    // DEBUG_LOG(" ********** bytes_in:  %f", bytes_in);
    return 0;
}

int mem_total_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "MemTotal:");
    if (p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    }
    else 
    {
        return -1;
    }

}

int mem_free_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "MemFree:");
    if(p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    } 
    else
    {
        return -1;
    }

}

int mem_buffers_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "Buffers:");
    if(p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    }
    else 
    {
        return -1;
    }

}

int mem_cached_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "Cached:");
    if(p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    }
    else 
    {
        return -1;
    }

}

int swap_free_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "SwapFree:");
    if(p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    } 
    else 
    {
        return -1;
    }

}

int swap_total_func(c_value * val)
{
    char *p;

    p = update_file(&proc_meminfo);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = strstr(p, "SwapTotal:");
    if(p) 
    {
        p = skip_token(p);
        *val = atof(p); 
        return 0;
    }
    else
    {
        return -1;
    }

}

int disk_total_func(c_value * val)
{
    double total_free = 0.0;
    double total_size = 0.0;

    find_disk_space(&total_size, &total_free);

    *val = total_size;
    return 0;
}

//report file system disk space usage
int disk_space_usage_func(c_value * val)
{
    system(DF_COMMAND);
    if(access("./.df_tmp", F_OK | R_OK))
    {
        ERROR_LOG("ERROR: execute system call failed.");
        return -1;
    }
    FILE  *fp = fopen("./.df_tmp", "r");
    if(fp == NULL)
    {
        ERROR_LOG("ERROR: fopen df_tmp file failed.");
        return -1;
    }
    memset(file_system_list, 0, sizeof(file_system_list));
    char    line[BUFFSIZE] = {0};
    char    *p, *write_pos = file_system_list;
    int     len = 0, remain_len = sizeof(file_system_list) - 1;
    int     flag = 0;
    if(fgets(line, sizeof(line) - 1, fp) != NULL)
    {
        memset(line, 0, sizeof(line));
        while(fgets(line, sizeof(line) - 1, fp) != NULL)
        {
            p = line;
            while(*p != '\0' && *p != '\n')
            {
                if(isspace(*p))
                {
                    if(flag == 0)
                    {
                        *p = ',';
                        flag = 1;
                    }
                }
                else
                {
                    flag = 0;
                }
                p++;
            }
            *p = '\0';
            str_trim(line);
            len = snprintf(write_pos, remain_len, "%s;", line);
            write_pos += len;
            remain_len -= len;
            memset(line, 0, sizeof(line));
        }
        if(remain_len >= 0)
            *(write_pos - 1) = 0;
    }
    fclose(fp);
    *val = file_system_list;
    return 0; 
}

int disk_free_func(c_value * val)
{
    double total_free = 0.0;
    double total_size = 0.0;

    find_disk_space(&total_size, &total_free);

    *val = total_free;
    return 0;
}

int part_max_used_func(c_value * val)
{
    double total_free = 0.0;
    double total_size = 0.0;

    float most_full = find_disk_space(&total_size, &total_free);

    *val = most_full;
    return 0;
}

int boottime_func(c_value * val)
{
    char *p;

    p = update_file(&proc_stat); 
    if (p == (char *)-1) 
    {
        return -1;
    }

    p = strstr( p, "btime");
    if (p) 
    {
        p = skip_token(p); 
        *val = atoi(p); 
        return 0;
    }
    else 
    {
        return -1;
    }

}

int sys_clock_func(c_value * val)
{

    *val = time(NULL);
    return 0;
}

int machine_type_func(c_value * val)
{

#ifdef __i386__
    *val = "x86";
#endif
#ifdef __x86_64__
    *val = "x86_64";
#endif
#ifdef __ia64__
    *val = "ia64";
#endif
#ifdef __sparc__
    *val = "sparc";
#endif
#ifdef __alpha__
    *val = "alpha";
#endif
#ifdef __powerpc__
    *val = "powerpc";
#endif
#ifdef __m68k__
    *val = "m68k";
#endif
#ifdef __mips__
    *val = "mips";
#endif
#ifdef __arm__
    *val = "arm";
#endif
#ifdef __hppa__
    *val = "hppa";
#endif
#ifdef __s390__
    *val = "s390";
#endif

    return 0;
}

int os_name_func(c_value * val)
{
    *val = "Linux";
    return 0;
}

int os_release_func(c_value * val)
{

    *val = proc_sys_kernel_osrelease;
    return 0;
}

int file_system_func(c_value * val)
{

    *val = file_system_list;
    return 0;
}

int proc_run_func(c_value * val)
{
    char *p;

    p = update_file(&proc_loadavg);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = skip_token(p);
    p = skip_token(p);
    p = skip_token(p);
    *val = strtol(p, (char **)NULL, 10) - 1;

    return 0; 
}

int proc_total_func(c_value * val)
{
    char *p;

    p = update_file(&proc_loadavg);
    if (p == (char *)-1)
    {
        return -1;
    }
    p = skip_token(p);
    p = skip_token(p);
    p = skip_token(p); 
    p = skip_whitespace(p);
    while (isdigit(*p)) 
    {
        p++; 
    }
    p++;  /* skip the slash-/ */ 
    *val = strtol(p, (char **)NULL, 10); 

    return 0; 
}

int load_one_func(c_value * val)
{

    char *p = update_file(&proc_loadavg);
    if (p == (char *)-1) 
    {
        return -1;
    }
    *val = strtod(p, (char **)NULL);

    return 0; 
}

int load_five_func(c_value * val)
{
    char *p;

    p = update_file(&proc_loadavg);
    if (p == (char *)-1) 
    {
        return -1;
    }
    p = skip_token(p);
    *val = strtod(p, (char **)NULL);

    return 0; 
}

int load_fifteen_func(c_value * val)
{
    char *p;

    p = update_file(&proc_loadavg);
    if (p == (char *)-1) 
    {
        return -1;
    }

    p = skip_token(p);
    p = skip_token(p);
    *val = strtod(p, (char **)NULL);

    return 0;
}

int cpu_num_func(c_value * val)
{
    *val = cpu_num;

    return 0;
}

int update_cpu_info()
{
    FILE *fp = NULL;
    char buf[1024] = {0};

    fp = fopen("/proc/cpuinfo", "r");
    if(fp == NULL)
    {
        EMERG_LOG("fopen /proc/cpuinfo failed, sys error:%s.", strerror(errno));
        return -1;
    }

    while(fgets(buf, sizeof(buf) - 1, fp) != NULL)
    {
        const char *key_name = "model name";
        unsigned int len = strlen(key_name);
        char *endline = NULL;
        if((endline = index(buf, '\n')) != NULL)
        {
            *endline = '\0';
        }
        if(!strncasecmp(buf, key_name, len))//行首是key的名字
        {
            char *start = NULL;
            if((start = index(buf, ':')) != NULL)
            {
                str_trim(start + 1);//去掉空格和换行符等
                strncpy(cpu_model, start + 1, sizeof(cpu_model));
            }
            else
            {
                str_trim(buf + len);//去掉空格和换行符等
                strncpy(cpu_model, buf + len, sizeof(cpu_model));
            }
            break;
        }
        memset(buf, 0, sizeof(buf));
    }
    fclose(fp);
    return 0;
}

int cpu_info_func(c_value * val)
{
    update_cpu_info();//不要判断错误
    val->sprintf("%s*%d", cpu_model, cpu_num);
    return 0;
}

int cpu_speed_func(c_value * v)
{
    char *p;
    static uint32_t val = 0;

    /* we'll use scaling_max_freq before we fallback on proc_cpuinfo */
    if (cpufreq && ! val) 
    {
        p = sys_devices_system_cpu;
        val = (uint32_t)(strtol(p, (char **)NULL , 10) / 1000);
    }

    /* i386, ia64, x86_64 and hppa all report MHz in the same format */
#if defined (__i386__) || defined(__ia64__) || defined(__hppa__) || defined(__x86_64__)
    if (!val) {
        p = proc_cpuinfo;  
        p = strstr(p, "cpu MHz");
        if (p) {
            p = strchr(p, ':');
            p++;
            p = skip_whitespace(p);
            val = (uint32_t)strtol(p, (char **)NULL , 10);
        } else {
            val = 0;
        }
    }
#endif
#if defined (__alpha__)
    if (!val) {
        int num;
        p = proc_cpuinfo;
        p = strstr(p, "cycle frequency [Hz]");
        if (p) {
            p = strchr(p, ':');
            p++;
            p = skip_whitespace(p);
            sscanf(p, "%d", &num);
            num = num / 1000000;  /* Convert to Mhz */
            val = (uint32_t)num;
        } else {
            val = 0;
        }
    }
#endif
#if defined (__powerpc__)
    if (!val) {
        p = proc_cpuinfo;
        p = strstr(p, "clock");
        if (p) { 
            p = strchr(p, ':');
            p++;
            p = skip_whitespace(p);
            val = (uint32_t)strtol(p, (char **)NULL , 10);
        } else {
            val = 0;
        }
    }
#endif

    *v = val;
    return 0;
}

int cpu_user_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_user_jiffies,  user_jiffies, 
                  last_total_jiffies, total_jiffies, diff;

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        val = 0.0;
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        user_jiffies = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = user_jiffies - last_user_jiffies; 

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_user_jiffies  = user_jiffies;
        last_total_jiffies = total_jiffies;
    }

    *v = val;
    return 0;
}

int cpu_nice_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_nice_jiffies, nice_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        val = 0.0;
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        nice_jiffies  = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = (nice_jiffies  - last_nice_jiffies);

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_nice_jiffies  = nice_jiffies;
        last_total_jiffies = total_jiffies;

    }    
    *v = val;
    return 0;
}

int cpu_system_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp={0,0};
    static double last_system_jiffies,  system_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        val = 0.0;
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        system_jiffies = strtod(p , (char **)NULL);
        if (num_cpustates > NUM_CPUSTATES_24X) {
            p = skip_token(p);
            p = skip_token(p);
            p = skip_token(p);
            system_jiffies += strtod(p , (char **)NULL); /* "intr" counted in system */
            p = skip_token(p);
            system_jiffies += strtod(p , (char **)NULL); /* "sintr" counted in system */
        }
        total_jiffies  = total_jiffies_func();

        diff = system_jiffies  - last_system_jiffies;

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_system_jiffies  = system_jiffies;
        last_total_jiffies = total_jiffies;   

    }
    *v = val;
    return 0;
}

int cpu_idle_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_idle_jiffies,  idle_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        val = 0.0;
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        idle_jiffies = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = idle_jiffies - last_idle_jiffies;

        if (diff) { 
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_idle_jiffies  = idle_jiffies;
        last_total_jiffies = total_jiffies;
    }

    *v = val;
    return 0;
}

int cpu_aidle_func(c_value * v)
{
    char *p;
    static float val;
    double idle_jiffies, total_jiffies;

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        val = 0.0;
        return -1;
    }

    p = skip_token(p);
    p = skip_token(p);
    p = skip_token(p);
    p = skip_token(p);
    idle_jiffies = strtod(p , (char **)NULL);
    total_jiffies = total_jiffies_func();

    val = (idle_jiffies / total_jiffies) * 100;

    *v = val;
    return 0;
}

int cpu_wio_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_wio_jiffies,  wio_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    if (num_cpustates == NUM_CPUSTATES_24X) 
    {
        val = 0.;
        *v = val;
        return 0;
    }

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        wio_jiffies  = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = wio_jiffies - last_wio_jiffies;

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_wio_jiffies  = wio_jiffies;
        last_total_jiffies = total_jiffies;
    }

    *v = val;

    return 0;
}

int cpu_intr_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_intr_jiffies, intr_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    if (num_cpustates == NUM_CPUSTATES_24X) 
    {
        val = 0.;
        *v = val;
        return 0;
    }

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        intr_jiffies  = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = intr_jiffies - last_intr_jiffies;

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_intr_jiffies  = intr_jiffies;
        last_total_jiffies = total_jiffies;
    }

    *v = val;
    return 0;
}

int cpu_sintr_func(c_value * v)
{
    char *p;
    static float val;
    static struct timeval stamp = {0, 0};
    static double last_sintr_jiffies,  sintr_jiffies,
                  last_total_jiffies, total_jiffies, diff;

    if (num_cpustates == NUM_CPUSTATES_24X) 
    {
        val = 0.;
        *v = val;
        return 0;
    }

    p = update_file(&proc_stat);
    if (p == (char *)-1) 
    {
        return -1;
    }
    if ((proc_stat.last_read.tv_sec != stamp.tv_sec) &&
            (proc_stat.last_read.tv_usec != stamp.tv_usec)) {
        stamp = proc_stat.last_read;

        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        p = skip_token(p);
        sintr_jiffies  = strtod(p , (char **)NULL);
        total_jiffies = total_jiffies_func();

        diff = sintr_jiffies - last_sintr_jiffies;

        if (diff) {
            val = (diff / (total_jiffies - last_total_jiffies)) * 100;
        } else {
            val = 0.0;
        }

        last_sintr_jiffies  = sintr_jiffies;
        last_total_jiffies = total_jiffies;
    }

    *v = val;
    return 0;
}


int inside_ip_func(c_value * val)
{
    get_ip(0x01, inside_ip);
    *val = inside_ip;
    return 0;
}

int outside_ip_func(c_value * val)
{
    get_ip(0x02, outside_ip);
    *val = outside_ip;
    return 0;
}


int os_version_func(c_value * val)
{

    FILE *fp = NULL;
    char buf[1024] = {0};
    char vsn_pre[64] = "Debian";                /**<版本信息前缀*/
    char vsn_path[128] = "/etc/debian_version"; /**<存放版本信息文件路径*/

#ifdef CENTOS
    sprintf(vsn_path, "%s", "/etc/redhat-release");
    sprintf(vsn_pre, "%s", "CentOS");
#endif

    //只适用于debian系统
    if(access(vsn_path, F_OK | R_OK))//文件不存在
        return -1;

    fp = fopen(vsn_path, "r");
    if(fp == NULL)
    {
        EMERG_LOG("fopen [%s] failed, sys error: %s.", vsn_path, strerror(errno));
        return -1;
    }

    if(fgets(buf, sizeof(buf) - 1, fp) != NULL)//只有一行
    {
        //str_trim(buf);//去掉空格和换行符
        str_num(buf);//去掉非数字字符
        fclose(fp);
    }
    else
    {
        fclose(fp);
        return -1;
    }

    struct utsname uname_buf;
    if(uname(&uname_buf) != 0)//获取机器类型
    {
        val->sprintf("%s%s", vsn_pre, buf);
        return 0;
    }
    else
    {
        val->sprintf("%s%s_%s", vsn_pre, buf, uname_buf.machine);
        return 0;
    }
}


int metric_init()
{
    int ret = 0;
    struct stat struct_stat;

    num_cpustates = num_cpustates_func();

    /* scaling_max_freq will contain the max CPU speed if available */
    cpufreq = 0;
    if (stat(SCALING_MAX_FREQ, &struct_stat) == 0) {
        cpufreq = 1;
        read_proc(SCALING_MAX_FREQ, sys_devices_system_cpu, 32);
    }

    ret = read_proc("/proc/cpuinfo", proc_cpuinfo, BUFFSIZE);
    if (ret == SYNAPSE_FAILURE) {
        ERROR_LOG("metric_init() got an error from slurpfile() /proc/cpuinfo");
        return -1;
    }

    // os_release
    ret = read_proc( "/proc/sys/kernel/osrelease", proc_sys_kernel_osrelease, BUFFSIZE);
    if (ret == SYNAPSE_FAILURE) {
        ERROR_LOG("metric_init() got an error from slurpfile()");
        return -1;
    }

    /* Get rid of pesky \n in osrelease */
    proc_sys_kernel_osrelease[ret - 1] = '\0';

    // 网络初始化
    if ((char *)-1 == update_file(&proc_net_dev)) {
        ERROR_LOG("metric_init() got an error from update_file()");
        return -1;
    }

    if (-1 == update_ifdata("metric_init")) {
        ERROR_LOG("metric_init() got an error from update_ifdata()");
        return -1;
    }


    /* Only need to do this once */
    cpu_num = get_nprocs();
    return 0;
}

/**
 * @brief 过滤一个字符串中的非数字字符
 * @param   str  要过滤的字符串头指针
 * @return  void
 */
void str_num(char *str)
{
    if(NULL == str)
    {
        return ;
    }

    char *out = str;
    char *in = str;
    while(*in != '\0')
    {
        if((*in >= '0' && *in <= '9') || *in == '.')
        {
            *out++ = *in;
        }
        in++;
    }
    *out = '\0';
    return;
}

/**
 * @brief 过滤一个字符串中的space字符
 * @param   str  要过滤的字符串头指针
 * @return  void
 */
void str_trim(char *str)
{
    if(NULL == str)
    {
        return ;
    }

    char *out = str;
    char *in = str;
    while(*in != '\0')
    {
        if(*in == ' ' || *in == '\t' || *in == '\r' || *in == '\n')
        {
            in++;
        }
        else
        {
            *out++ = *in++;
        }
    }
    *out = '\0';
    return;
}


/*
 * 构造并发送 ntp 协议包
 * */
int send_packet(int fd)
{
    unsigned int data[12];
    int ret;
    struct timeval now;

#define LI 0 //协议头中的元素
#define VN 3 //版本
#define MODE 3 //模式 : 客户端请求
#define STRATUM 0
#define POLL 4 //连续信息间的最大间隔
#define PREC -6 //本地时钟精度
    memset((char *)data, 0, sizeof (data));
    data[0] = htonl((LI << 30) | (VN << 27) | (MODE << 24) 
            | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
    data[1] = htonl(1 << 16);
    data[2] = htonl(1 << 16);
    //获得本地时间
    gettimeofday(&now, NULL);

    data[10] = htonl(now.tv_sec + JAN_1970);
    data[11] = htonl(NTPFRAC(now.tv_usec));
    DEBUG_LOG("system time[%X.%X]" , data[10], data[11]);
    ret = send(fd, data, 48, 0);
    DEBUG_LOG("send %u byte[s] packet to ntp server", ret);
    return ret;
}

/*
 * 获得并解析 ntp 协议包
 * @sock -- 与时间服务器通讯的套接字
 * @resp -- 从服务器应答中提取的有用信息
 * */
int get_server_time(int sock, NtpServResp *resp)
{
    int ret;
    unsigned int data[12];
    NtpTime oritime, rectime, tratime;
    struct timeval offtime;
    struct timeval now;

    bzero(data, sizeof (data));
    ret = recvfrom (sock, data, sizeof (data), 0, NULL, 0);
    gettimeofday(&now, NULL);
    if (ret == -1) { 
        ERROR_LOG("recvfrom failed!" );
        //return -1;
        return 0;
    } else if (ret == 0) {
        ERROR_LOG("recvfrom receive 0!" );
        //return -1 ;
        return 0 ;
    }

#define DATA(i) ntohl(((unsigned int *)data)[i])
    oritime.integer = DATA(6);
    oritime.fraction = DATA(7);
    rectime.integer = DATA(8);
    rectime.fraction = DATA(9);
    tratime.integer = DATA(10);
    tratime.fraction = DATA(11);
#undef DATA

    //Originate Timestamp T1 客户端发送请求的时间
    //Receive Timestamp T2 服务器接收请求的时间
    //Transmit Timestamp T3 服务器答复时间
    //Destination Timestamp T4 客户端接收答复的时间
    //网络延时 d 和服务器与客户端的时差 t
    //d = (T2 - T1) + (T4 - T3);t = [(T2 - T1) + (T3 - T4)] / 2;
#define MKSEC(ntpt) ((ntpt).integer - JAN_1970)
#define MKUSEC(ntpt) (USEC((ntpt).fraction))
#define TTLUSEC(sec,usec) ((long long )(sec)*1000000 + (usec))
#define GETSEC(us) ((us)/1000000) 
#define GETUSEC(us) ((us)%1000000) 
    long long orius, recus, traus, desus, offus;

    orius = TTLUSEC(MKSEC(oritime), MKUSEC(oritime));
    recus = TTLUSEC(MKSEC(rectime), MKUSEC(rectime));
    traus = TTLUSEC(MKSEC(tratime), MKUSEC(tratime));
    desus = TTLUSEC(now.tv_sec, now.tv_usec);

    offus = ((recus - orius) + (traus - desus))/2;

    offtime.tv_sec = GETSEC(offus);
    offtime.tv_usec = GETUSEC(offus);

    DEBUG_LOG("system time offset[%lX.%lX]", offtime.tv_sec, offtime.tv_usec);
#undef MKSEC
#undef MKUSEC
#undef TTLUSEC
#undef GETSEC
#undef GETUSEC

    //获得本地时间与标准时间的差值，总是返回正数
    return offtime.tv_sec > 0 ? offtime.tv_sec : -offtime.tv_sec;
}

/*
 * 连接时间服务器
 * */
int ntp_conn_server(const char *servname, int port)
{
    int sock;

    int addr_len = sizeof (struct sockaddr_in);
    struct sockaddr_in addr_src;//本地 socket <netinet/in.h>
    struct sockaddr_in addr_dst;//服务器 socket

    //UDP数据报套接字
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        ERROR_LOG("create socket error! " );
        return -1;
    }
    memset(&addr_src, 0, addr_len);
    addr_src.sin_family = AF_INET;
    addr_src.sin_port = htons(0);
    addr_src.sin_addr.s_addr = htonl(INADDR_ANY);//<arpa/inet.h>
    //绑定本地地址
    if (-1 == bind(sock, (struct sockaddr *) &addr_src, addr_len)) {
        ERROR_LOG("bind sock error !" );
        close(sock);
        return -1;
    }
    memset(&addr_dst, 0, addr_len);
    addr_dst.sin_family = AF_INET;
    addr_dst.sin_port = htons(port);

    struct hostent *host = gethostbyname(servname);//<netdb.h>
    if (host == NULL) {
        ERROR_LOG("gethostbyname[%s] error!", servname);
        close(sock);
        return -1;
    }
    memcpy (&(addr_dst.sin_addr.s_addr), host->h_addr_list[0], 4);
    if (-1 == connect(sock, (struct sockaddr *) &addr_dst, addr_len)) {
        ERROR_LOG("connect to ntp server error[-1]!" );
        close(sock);
        return -1;
    }
    char buf[32];
    DEBUG_LOG("ctreat udp to %s[%s]", servname,
            inet_ntop(host->h_addrtype, host->h_addr_list[0], buf, sizeof(buf)));
    return sock;
}

int system_time_offset_func(c_value * val)
{
    int sock;
    int ret;
    NtpServResp response;
    struct timeval timeout;//<sys/time.h>
    //连接 ntp 服务器
    sock = ntp_conn_server(DEF_NTP_SERVER, DEF_NTP_PORT);
    if(sock == -1) 
    {
        *val = 0;
        return 0;
    }

    //发送 ntp 包
    send_packet(sock);

    fd_set fds_read;
    FD_ZERO(&fds_read);
    FD_SET(sock, &fds_read);

    timeout.tv_sec = DEF_TIMEOUT;
    timeout.tv_usec = 0;

    ret = select(sock + 1, &fds_read, NULL, NULL, &timeout);
    if (ret == -1) {
        ERROR_LOG("select sock+1 error!" );
        goto error;
    }
    if (ret == 0 || !FD_ISSET (sock, &fds_read)) {
        ERROR_LOG("timeout[%u]!", DEF_TIMEOUT);
        goto error;
    }
    *val = get_server_time(sock, &response);
    close(sock);
    return 0;

error:
    close(sock);
    return 0;
}

uint32_t get_network_card_speed(const char *device)
{
    uint32_t speed = 0;
    /* Setup our control structures. */
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name) - 1);

    /* Open control socket. */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        ERROR_LOG("Cannot get control[%s] socket: %s", device, strerror(errno));
        return speed;
    }

    int err;
    struct ethtool_cmd ecmd;
    ecmd.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (caddr_t)&ecmd;
    err = ioctl(fd, SIOCETHTOOL, &ifr);
    if (err == 0) {
        speed = (ecmd.speed_hi << 16) | ecmd.speed;
        if (speed == 0 || speed == (uint16_t)(-1) || speed == (uint32_t)(-1)) {
            speed = 0;//unknow
        } else {
            speed *= 1024 * 1024;
        }
    } else if (errno != EOPNOTSUPP) {
        ERROR_LOG("Cannot get device[%s] settings: %s", device, strerror(errno));
    }

    close(fd);
    return speed;
}

int eth0_speed_func(c_value * val)
{
    *val = get_network_card_speed("eth0");

    return 0;
}

int eth1_speed_func(c_value * val)
{
    *val = get_network_card_speed("eth1");

    return 0;
}
