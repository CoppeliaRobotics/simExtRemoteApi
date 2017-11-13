#pragma once

#include <vector>
#include "simxCmd.h"

class CSimxContainer
{
public:
    CSimxContainer(bool isInputContainer);
    virtual ~CSimxContainer();

    void clearAll();

    void addCommand(CSimxCmd* cmd,bool doNotOverwriteSameCommand);
    char* addPartialCommand(const char* buffer,bool otherSideIsBigEndian);
    int _arePartialCommandsSame(const char* buff1,const char* buff2,bool otherSideIsBigEndian);
    void executeCommands(CSimxContainer* outputContainer,CSimxSocket* sock);
    void setCommandsAlreadyExecuted(bool e);
    bool getCommandsAlreadyExecuted();
    int getDataString(std::vector<char>& dataString,bool otherSideIsBigEndian);
    int getDataStringOfSplitOrGradualCommands(std::vector<char>& dataString,bool otherSideIsBigEndian);
    int getStreamCommandCount();
    void setMessageID(int id);
    int getMessageID();
    void setDataTimeStamp(int ts);
    void setDataServerTimeStamp(int st);
    void setSceneID(WORD id);
    void setServerState(BYTE serverState);

    int getCommandCount();
    void setOtherSideIsBigEndian(bool bigEndian);

protected:
    int _getIndexOfSimilarCommand(CSimxCmd* cmd);
    void _removeNonContinuousCommands();

    int _messageID;
    int _dataTimeStamp; // client time stamp
    int _dataServerTime; // server time stamp
    WORD _sceneID;
    BYTE _serverState;
    bool _isInputContainer;
    bool _otherSideIsBigEndian;
    std::vector<CSimxCmd*> _allCommands;
    std::vector<char*> _partialCommands;
};
