#include "log.hpp"

#ifndef NDEBUG
  #include <cstdio>
  using namespace std;

  namespace {
    time_t now = time(0);
    tm* t = localtime(&now);
    const int logname_len = 16;
    char logname[logname_len];
    int ret = snprintf( logname, logname_len, "%s%4d%02d%02d.log",
                        "log", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday );
  }
  std::ofstream fout(logname);
#endif // NDEBUG
