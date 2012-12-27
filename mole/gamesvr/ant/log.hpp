#ifndef ANT_LOG_HPP
#define ANT_LOG_HPP

#ifdef    TRACE
  #undef  TRACE
#endif  // TRACE
#ifndef NDEBUG
  #include <ctime>
  #include <fstream>
  extern std::ofstream fout;
  #define TRACE(x) do { \
                      time_t now = std::time(0); \
                      char buf[26]; \
                      ctime_r(&now, buf); \
                      fout << x << ": " << buf << std::endl; \
                   } while (0)
#else
  #define TRACE(x)
#endif // NDEBUG

#endif // ANT_LOG_HPP
