#include "porting.h"

#ifdef _WIN32
DWORD getTimeInMs(void)
{
    return(timeGetTime()&0x03ffffff);
}
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)
DWORD getTimeInMs(void)
{
    struct timeval tv;
    DWORD result=0;
    if (gettimeofday(&tv,NULL)==0)
        result=(tv.tv_sec*1000+tv.tv_usec/1000)&0x03ffffff;
    return(result);
}
#endif /* __linux || __APPLE__ */

DWORD getTimeDiffInMs(DWORD lastTime)
{
    DWORD currentTime=getTimeInMs();
    if (currentTime<lastTime)
        return(currentTime+0x03ffffff-lastTime);
    return(currentTime-lastTime);
}
