#ifndef LOG_H
#define LOG_H
#include <string>
using namespace std;
#define NOTICE	0
#define ERROR   1		/* non-fatal errors */
#define NORMAL  2
#define DEBUG   3		/* suppressed unless DEBUG defined;*/
#define TRACE	4		/* not shown by default */

class Log
{
public:
	Log (const char* dir,int lvl,int s,int num,const char* p = NULL);
	~Log() {};

	void hex_log (const char* file_name, const char* buf, int length);
	void write_log (int lvl, const char* fmt, ...);
protected:
	static const int LOG_SHIFT_COUNT  = 32;
	int shift_files (const char* file_name);
	string log_file_name (const char* pre_name, int cur_index);
	string log_file_name (int type);

	char	path[256];
	int		level;
	int 	max_log_size;
	int 	max_log_num;

	char	prefix[32];
};

extern Log* g_pLog;
#define LOG g_pLog->write_log
#define HEXLOG g_pLog->hex_log
#endif
