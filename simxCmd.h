#pragma once

#include <string>
#include <vector>
#include "porting.h"

class CSimxSocket; // forward declaration

class CSimxCmd
{
public:
    CSimxCmd(int commandID,WORD delayOrSplit,int dataSize,const char* dataPointer);
    CSimxCmd();
    virtual ~CSimxCmd();

    int getRawCommand();
    int getOperationMode();
    void setLastTimeProcessed(DWORD t);
    DWORD getLastTimeProcessed();

    bool areCommandAndCommandDataSame(const CSimxCmd* otherCmd);
    void appendYourData(std::vector<char>& dataString,bool otherSideIsBigEndian);
    bool appendYourMemorizedSplitData(bool calledFromContainer,std::vector<char>& dataString,bool& removeCommand,bool otherSideIsBigEndian);
    CSimxCmd* copyYourself();
    void setDataReply_nothing(bool success);
    void setDataReply_custom_transferBuffer(char* customData,int customDataSize,bool success);
    void setDataReply_custom_copyBuffer(char* customData,int customDataSize,bool success);
    void setDataReply_1float(float floatVal,bool success,bool otherSideIsBigEndian);
    void setDataReply_1int(int intVal,bool success,bool otherSideIsBigEndian);
    void setDataReply_2int(int intVal1,int intVal2,bool success,bool otherSideIsBigEndian);
    void setDataReply_3int(int intVal1,int intVal2,int intVal3,bool success,bool otherSideIsBigEndian);
    void appendIntToString(std::string& str,int v,bool doConversion,bool otherSideIsBigEndian);
    void appendFloatToString(std::string& str,float v,bool doConversion,bool otherSideIsBigEndian);


    CSimxCmd* execute(CSimxSocket* sock,bool otherSideIsBigEndian);

protected:
    CSimxCmd* _executeCommand(CSimxSocket* sock,bool otherSideIsBigEndian);

    int _opMode;

    int _rawCmdID;

    // Following is command data:
    char _cmdData[8];
    std::string _cmdString;
    std::string _cmdString2;

    // Following is pure data:
    char* _pureData;
    int _pureDataSize;

    BYTE _status;
    WORD _processingDelayOrMaxDataSize;
    DWORD _lastTimeProcessed;
    int _dataSizeLeftToBeSent; // for simx_opmode_continuous_split
    int _executionTime; // in simulation time (in ms), or 0 if simulation is not running
    CSimxCmd* _memorizedSplitCmd;
};
