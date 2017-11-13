#ifndef __PORTING_H__
#define __PORTING_H__

#ifdef _WIN32
    #include <Windows.h>
    #include <process.h>
    #ifndef QT_COMPIL
        #pragma message("-----------------------> Adding library: Winmm.lib")
        #pragma comment(lib,"Winmm.lib")
        #pragma message("Adding library: Ws2_32.lib")
        #pragma comment(lib,"Ws2_32.lib")
    #endif
#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#endif /* __linux || __APPLE__ */

#include <string>


#ifdef _WIN32

typedef SOCKET              _SOCKET;
typedef timeval             _timeval;
typedef int                 _socklen;

#endif /* _WIN32 */

#if defined (__linux) || defined (__APPLE__)

#define LINUX_NAMED_PIPES_DIR "/tmp/"
#define INVALID_SOCKET        -1
#define Sleep(x) (usleep(x*1000))

typedef int                 _SOCKET;
typedef struct timeval      _timeval;
typedef socklen_t           _socklen;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned short      u_short;

#endif /* __linux || __APPLE__ */

DWORD getTimeInMs();
DWORD getTimeDiffInMs(DWORD lastTime);

#endif /* __PORTING_H__ */
