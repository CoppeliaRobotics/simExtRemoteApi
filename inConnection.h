#pragma once

#include <vector>
#include "porting.h"
#include "shared_memory.h"

class CInConnection
{
public:
    CInConnection(int theConnectionPort,int maxPacketSize,bool newVersion);
    virtual ~CInConnection();

    bool connectToClient();
    char* receiveMessage(int& messageSize);
    bool replyToReceivedMessage(char* message,int messageSize);

    std::string getConnectedMachineIP();
    bool isOtherSideBigEndian();
    void stopWaitingForConnection();

protected:
    bool _sendSimplePacket(char* packet,int packetLength,WORD packetsLeft);
    int _receiveSimplePacket(std::vector<char>& packet);

    bool _send_sharedMem(const char* data,int dataLength);
    char* _receive_sharedMem(int& dataLength);

    _timeval        _socketTimeOut;
    int             _socketConnectionPort;
    std::string     _socketConnectedMachineIP;
    #ifdef _WIN32
        WSADATA         _socketWsaData;
    #endif /* _WIN32 */

    shared_memory_info_t _shared_memory_info;
    int             _maxPacketSize;
    bool            _otherSideIsBigEndian;
    bool            _connected;
    bool            _newVersion;
    bool            _usingSharedMem;
    bool            _leaveConnectionWait;


    // old version:
    _SOCKET             _socketServer;
    _SOCKET             _socketClient;
    struct sockaddr_in  _socketLocal;
    fd_set              _socketTheSet;

    // new version:
    _SOCKET             _local_socket;
    _SOCKET             _accepted_socket;
    struct sockaddr_in  _address;
    fd_set              _read_fd;
    bool                _listening;
};
