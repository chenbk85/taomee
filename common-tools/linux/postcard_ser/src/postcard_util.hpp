#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static bool stringToTime(char *strDateStr, time_t &timeData)
{
    if (strDateStr == NULL)
    {
        return false;
    }

    char *pBeginPos = strDateStr;
    char *pPos = strstr(pBeginPos, "-");
    if(pPos == NULL)
    {
        return false;
    }
    
    int iYear = atoi(pBeginPos);
    int iMonth = atoi(pPos + 1);
    pPos = strstr(pPos + 1, "-");
    if(pPos == NULL)
    {
        return false;
    }
    
    int iDay = atoi(pPos + 1);
    struct tm sourcedate;
    bzero((void*)&sourcedate,sizeof(sourcedate));
    sourcedate.tm_mday = iDay;
    sourcedate.tm_mon = iMonth - 1;
    sourcedate.tm_year = iYear - 1900;
    timeData = mktime(&sourcedate);
    return true;
}


