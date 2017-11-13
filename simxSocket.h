#pragma once

#include <vector>
#include "inConnection.h"
#include "simxContainer.h"
#include "porting.h"

#if defined (__linux) || defined (__APPLE__)
    #include <pthread.h>
#endif /* __linux || __APPLE__ */

class CSimxSocket
{
public:
    CSimxSocket(int portNb,bool continuousService,bool simulationOnly,bool debug,int maxPacketSize,bool waitForTriggerFunctionAuthorized);
    virtual ~CSimxSocket();

    void start();

    void thereWasARequestToCallTheMainScript();
    void instancePass();

    void getInfo(int info[5]);
    int getClientVersion();
    int getStatus();
    int getPortNb();
    bool getActiveOnlyDuringSimulation();
    bool getContinuousService();
    bool getDebug();
    int getMaxPacketSize();


    void setWaitForTrigger(bool w);
    bool getWaitForTrigger();
    void setWaitForTriggerEnabled(bool e);
    bool getWaitForTriggerEnabled();
    bool getWaitForTriggerAuthorized();

    void addErrorString(const char* err);
    char* getAndClearFullErrorStringBuffer(int& size);
    std::string getConnectedMachineIP();

    static bool useAlternateSocketRoutines;

protected:
    void* _run();
    void _lock();
    void _unlock();

    void _stop();
    void _executeCommands();

    volatile bool _commThreadLaunched;
    volatile bool _commThreadEnded;
    volatile bool clientIsConnected;
    bool otherSideIsBigEndian;
    int _portNb;
    bool _simulationOnly;
    bool _continuousService;
    bool _debug;
    int _maxPacketSize;
    int _auxConsoleHandle;
    bool _waitForTrigger;
    bool _waitForTriggerFunctionEnabled;
    bool _waitForTriggerFunctionAuthorized;
    int _stopSimulationRequestCounter;

    int _lastReceivedMessage_time;
    int _lastReceivedMessage_clientVersion;
    int _lastSentMessage_time;
    int _successiveReception_time;
    int _lastReceivedMessage_cmdCnt;
    int _lastSentMessage_cmdCnt;

    std::vector<std::string> _textToPrintToConsole;
    std::vector<std::string> _last50Errors;
    
    CSimxContainer* _receivedCommands;
    CSimxContainer* _dataToSend;

    CInConnection* connection;

    int _lockLevel;
#ifdef _WIN32
    static DWORD WINAPI _staticThreadProc(LPVOID arg);
    #define MUTEX_HANDLE HANDLE
    #define MUTEX_HANDLE_X MUTEX_HANDLE
    #define THREAD_ID DWORD
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    static void* _staticThreadProc(void *arg);
    #define MUTEX_HANDLE pthread_mutex_t
    #define MUTEX_HANDLE_X MUTEX_HANDLE*
    #define THREAD_ID pthread_t
    pthread_t _theThread;
#endif /* __linux || __APPLE__ */

    void _simpleLock(MUTEX_HANDLE_X mutex);
    void _simpleUnlock(MUTEX_HANDLE_X mutex);

    MUTEX_HANDLE _mutex;
    MUTEX_HANDLE _mutexAux;
    THREAD_ID _lockThreadId;
};
