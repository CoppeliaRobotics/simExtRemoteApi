#include "simExtRemoteApi.h"
#include "scriptFunctionData.h"
#include "simLib.h"
#include <iostream>
#include "simxConnections.h"
#include "confReader.h"
#include <sstream>
#include <stdlib.h>

#ifdef _WIN32
    #ifdef QT_COMPIL
        #include <direct.h>
    #else
        #include <shlwapi.h> // required for PathRemoveFileSpec function
        #pragma comment(lib, "Shlwapi.lib")
    #endif
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #include <unistd.h>
    #define WIN_AFX_MANAGE_STATE
#endif /* __linux || __APPLE__ */

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)    CONCAT(x,y,z)

static LIBRARY simLib;
static CSimxConnections allConnections;

// --------------------------------------------------------------------------------------
// simExtRemoteApiStart
// --------------------------------------------------------------------------------------
#define LUA_START_COMMAND "simRemoteApi.start"
#define LUA_START_COMMANDOLD "simExtRemoteApiStart" // for backward compatibility

const int inArgs_START[]={
    4,
    sim_script_arg_int32,0,
    sim_script_arg_int32,0, // optional arg
    sim_script_arg_bool,0, // optional arg
    sim_script_arg_bool,0, // optional arg
};

void LUA_START_CALLBACK(SScriptCallBack* p)
{
    CScriptFunctionData D;
    int result=-1;
    if (D.readDataFromStack(p->stackID,inArgs_START,inArgs_START[0]-3,LUA_START_COMMAND)) // -3 because the last 3 args are optional
    {
        std::vector<CScriptFunctionDataItem>* inData=D.getInDataPtr();
        int port=inData->at(0).int32Data[0];

        int maxPacketSize=1300;
        if (port<0)
            maxPacketSize=3200000; // when using shared memory
        bool debug=false;
        bool triggerPreEnabled=false; // 3/3/2014
        if (inData->size()>1)
            maxPacketSize=inData->at(1).int32Data[0];
        if (inData->size()>2)
            debug=inData->at(2).boolData[0];
        if (inData->size()>3)
            triggerPreEnabled=inData->at(3).boolData[0];
        if (port<0)
        { // when using shared memory
            if (maxPacketSize<1000)
                maxPacketSize=1000;
            if (maxPacketSize>32000000)
                maxPacketSize=32000000;
        }
        else
        { // when using sockets
            if (maxPacketSize<200)
                maxPacketSize=200;
            if (maxPacketSize>30000)
                maxPacketSize=30000;
        }

        CSimxSocket* s=allConnections.getConnectionFromPort(port);
        if (s==NULL)
        {
            int prop,obj;
            if (-1!=simGetScriptProperty(p->scriptID,&prop,&obj))
            {
                int scriptType=((prop|sim_scripttype_threaded_old)-sim_scripttype_threaded_old);
                bool destroyAtSimulationEnd=( (scriptType==sim_scripttype_mainscript)||(scriptType==sim_scripttype_childscript) );
                CSimxSocket* oneSocketConnection=new CSimxSocket(port,false,destroyAtSimulationEnd,debug,maxPacketSize,triggerPreEnabled); // 3/3/2014
                oneSocketConnection->start();
                allConnections.addSocketConnection(oneSocketConnection);
                result=1;
            }
            else
                simSetLastError(LUA_START_COMMAND,"Unknown error."); // output an error
        }
        else
            simSetLastError(LUA_START_COMMAND,"Invalid port number."); // output an error
    }
    D.pushOutData(CScriptFunctionDataItem(result));
    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// simExtRemoteApiStop
// --------------------------------------------------------------------------------------
#define LUA_STOP_COMMAND "simRemoteApi.stop"
#define LUA_STOP_COMMANDOLD "simExtRemoteApiStop" // for backward compatibility

const int inArgs_STOP[]={
    1,
    sim_script_arg_int32,0,
};

void LUA_STOP_CALLBACK(SScriptCallBack* p)
{
    CScriptFunctionData D;
    int result=-1;
    if (D.readDataFromStack(p->stackID,inArgs_STOP,inArgs_STOP[0],LUA_STOP_COMMAND))
    {
        std::vector<CScriptFunctionDataItem>* inData=D.getInDataPtr();
        int port=inData->at(0).int32Data[0];
        CSimxSocket* s=allConnections.getConnectionFromPort(port);
        if (s!=NULL)
        {
            if (!s->getContinuousService())
            {
                allConnections.removeSocketConnection(s);
                result=1;
            }
            else
                simSetLastError(LUA_STOP_COMMAND,"Can't stop a continuous remote API server service."); // output an error
        }
        else
            simSetLastError(LUA_STOP_COMMAND,"Invalid port number."); // output an error
    }
    D.pushOutData(CScriptFunctionDataItem(result));
    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// simExtRemoteApiReset
// --------------------------------------------------------------------------------------
#define LUA_RESET_COMMAND "simRemoteApi.reset"
#define LUA_RESET_COMMANDOLD "simExtRemoteApiReset" // for backward compatibility

const int inArgs_RESET[]={
    1,
    sim_script_arg_int32,0,
};

void LUA_RESET_CALLBACK(SScriptCallBack* p)
{
    CScriptFunctionData D;
    int result=-1;
    if (D.readDataFromStack(p->stackID,inArgs_RESET,inArgs_RESET[0],LUA_RESET_COMMAND))
    {
        std::vector<CScriptFunctionDataItem>* inData=D.getInDataPtr();
        int port=inData->at(0).int32Data[0];
        CSimxSocket* s=allConnections.getConnectionFromPort(port);
        if (s!=NULL)
        {
            bool simulOnly=s->getActiveOnlyDuringSimulation();
            bool continuous=s->getContinuousService();
            bool debug=s->getDebug();
            int maxPacketS=s->getMaxPacketSize();
            bool triggerPreEnabled=s->getWaitForTriggerAuthorized();

            // Kill the thread/connection:
            allConnections.removeSocketConnection(s);
                
            // Now create a similar thread/connection:
            CSimxSocket* oneSocketConnection=new CSimxSocket(port,continuous,simulOnly,debug,maxPacketS,triggerPreEnabled);
            oneSocketConnection->start();
            allConnections.addSocketConnection(oneSocketConnection);
            
            result=1;
        }
        else
            simSetLastError(LUA_RESET_COMMAND,"Invalid port number."); // output an error
    }
    D.pushOutData(CScriptFunctionDataItem(result));
    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// simExtRemoteApiStatus
// --------------------------------------------------------------------------------------
#define LUA_STATUS_COMMAND "simRemoteApi.status"
#define LUA_STATUS_COMMANDOLD "simExtRemoteApiStatus" // for backward compatibility

const int inArgs_STATUS[]={
    1,
    sim_script_arg_int32,0,
};

void LUA_STATUS_CALLBACK(SScriptCallBack* p)
{
    CScriptFunctionData D;
    int result=-1;
    std::vector<int> info(5,0);
    int clientVersion=-1;
    char connectedMachineIP[200]="";
    if (D.readDataFromStack(p->stackID,inArgs_STATUS,inArgs_STATUS[0],LUA_STATUS_COMMAND))
    {
        std::vector<CScriptFunctionDataItem>* inData=D.getInDataPtr();
        int port=inData->at(0).int32Data[0];
        CSimxSocket* s=allConnections.getConnectionFromPort(port);
        if (s!=NULL)
        {
            result=s->getStatus();
            s->getInfo(&info[0]);
            clientVersion=s->getClientVersion();
            strcpy(connectedMachineIP,s->getConnectedMachineIP().c_str());
        }
    }
    D.pushOutData(CScriptFunctionDataItem(result));
    if (result>-1)
        D.pushOutData(CScriptFunctionDataItem(info));
    else
        D.pushOutData(CScriptFunctionDataItem());
    D.pushOutData(CScriptFunctionDataItem(SIMX_VERSION));
    D.pushOutData(CScriptFunctionDataItem(clientVersion));
    if (result>-1)
        D.pushOutData(CScriptFunctionDataItem(std::string(connectedMachineIP)));

    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------


// This is the plugin start routine:
SIM_DLLEXPORT unsigned char simStart(void* reservedPointer,int reservedInt)
{ // This is called just once, at the start of CoppeliaSim
    // Dynamically load and bind CoppeliaSim functions:
    char curDirAndFile[1024];
#ifdef _WIN32
    #ifdef QT_COMPIL
        _getcwd(curDirAndFile, sizeof(curDirAndFile));
    #else
        GetModuleFileName(NULL,curDirAndFile,1023);
        PathRemoveFileSpec(curDirAndFile);
    #endif
#elif defined (__linux) || defined (__APPLE__)
    getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

    std::string currentDirAndPath(curDirAndFile);
    std::string temp(currentDirAndPath);

#ifdef _WIN32
    temp+="\\coppeliaSim.dll";
#elif defined (__linux)
    temp+="/libcoppeliaSim.so";
#elif defined (__APPLE__)
    temp+="/libcoppeliaSim.dylib";
#endif /* __linux || __APPLE__ */

    simLib=loadSimLibrary(temp.c_str());
    if (simLib==NULL)
    {
        printf("simExtRemoteApi: error: could not find or correctly load the CoppeliaSim library. Cannot start the plugin.\n"); // cannot use simAddLog here.
        return(0); // Means error, CoppeliaSim will unload this plugin
    }
    if (getSimProcAddresses(simLib)==0)
    {
        printf("simExtRemoteApi: error: could not find all required functions in the CoppeliaSim library. Cannot start the plugin.\n"); // cannot use simAddLog here.
        unloadSimLibrary(simLib);
        return(0); // Means error, CoppeliaSim will unload this plugin
    }

    simRegisterScriptVariable("simRemoteApi","require('simExtRemoteApi')",0);

    // Register the new Lua commands:
    simRegisterScriptCallbackFunction(strConCat(LUA_START_COMMAND,"@","RemoteApi"),strConCat("number result=",LUA_START_COMMAND,"(number socketPort,number maxPacketSize=1300,boolean debug=false,boolean preEnableTrigger=false)"),LUA_START_CALLBACK);
    simRegisterScriptCallbackFunction(strConCat(LUA_STOP_COMMAND,"@","RemoteApi"),strConCat("number result=",LUA_STOP_COMMAND,"(number socketPort)"),LUA_STOP_CALLBACK);
    simRegisterScriptCallbackFunction(strConCat(LUA_RESET_COMMAND,"@","RemoteApi"),strConCat("number result=",LUA_RESET_COMMAND,"(number socketPort)"),LUA_RESET_CALLBACK);
    simRegisterScriptCallbackFunction(strConCat(LUA_STATUS_COMMAND,"@","RemoteApi"),strConCat("number status,table_5 info,number version,number clientVersion,string connectedIp=",LUA_STATUS_COMMAND,"(number socketPort)"),LUA_STATUS_CALLBACK);

    // Following for backward compatibility:
    simRegisterScriptVariable(LUA_START_COMMANDOLD,LUA_START_COMMAND,-1);
    simRegisterScriptCallbackFunction(strConCat(LUA_START_COMMANDOLD,"@","RemoteApi"),strConCat("Please use the ",LUA_START_COMMAND," notation instead"),0);
    simRegisterScriptVariable(LUA_STOP_COMMANDOLD,LUA_STOP_COMMAND,-1);
    simRegisterScriptCallbackFunction(strConCat(LUA_STOP_COMMANDOLD,"@","RemoteApi"),strConCat("Please use the ",LUA_STOP_COMMAND," notation instead"),0);
    simRegisterScriptVariable(LUA_RESET_COMMANDOLD,LUA_RESET_COMMAND,-1);
    simRegisterScriptCallbackFunction(strConCat(LUA_RESET_COMMANDOLD,"@","RemoteApi"),strConCat("Please use the ",LUA_RESET_COMMAND," notation instead"),0);
    simRegisterScriptVariable(LUA_STATUS_COMMANDOLD,LUA_STATUS_COMMAND,-1);
    simRegisterScriptCallbackFunction(strConCat(LUA_STATUS_COMMANDOLD,"@","RemoteApi"),strConCat("Please use the ",LUA_STATUS_COMMAND," notation instead"),0);

    // Read the configuration file to prepare socket connections:
    CConfReader conf;
    temp=currentDirAndPath;

#ifdef _WIN32
    temp+="\\remoteApiConnections.txt";
#endif
#ifdef __linux
    temp+="/remoteApiConnections.txt";
#endif
#ifdef __APPLE__
    temp+="../Resources/remoteApiConnections.txt";
#endif

    // Read the configuration file and start remote API server services accordingly:
    conf.readConfiguration(temp.c_str());
    conf.getBoolean("useAlternateSocketRoutines",CSimxSocket::useAlternateSocketRoutines);

    int index=1;
    while (true)
    {
        std::stringstream strStream;
        strStream << index;
        std::string variableNameBase("portIndex");
        variableNameBase+=strStream.str();
        std::string variableName=variableNameBase+"_port";
        int portNb;
        if (conf.getInteger(variableName.c_str(),portNb))
        {
            bool debug=false;
            int maxPacketSize=1300;
            if (portNb<0)
                maxPacketSize=3200000;

            bool synchronousTrigger=false;
            variableName=variableNameBase+"_maxPacketSize";
            conf.getInteger(variableName.c_str(),maxPacketSize);
            variableName=variableNameBase+"_debug";
            conf.getBoolean(variableName.c_str(),debug);
            variableName=variableNameBase+"_syncSimTrigger";
            conf.getBoolean(variableName.c_str(),synchronousTrigger);

            if (portNb<0)
            { // when using shared memory
                if (maxPacketSize<1000)
                    maxPacketSize=1000;
                if (maxPacketSize>32000000)
                    maxPacketSize=32000000;
            }
            else
            { // when using sockets
                if (maxPacketSize<200)
                    maxPacketSize=200;
                if (maxPacketSize>30000)
                    maxPacketSize=30000;
            }

            if (allConnections.getConnectionFromPort(portNb)==NULL)
            {
                CSimxSocket* oneSocketConnection=new CSimxSocket(portNb,true,false,debug,maxPacketSize,synchronousTrigger);
                oneSocketConnection->start();
                allConnections.addSocketConnection(oneSocketConnection);
                std::string tmp("starting a remote API server on port ");
                tmp+=std::to_string(portNb);
                simAddLog("RemoteApi",sim_verbosity_loadinfos,tmp.c_str());
            }
            else
            {
                std::string tmp("failed starting a remote API server on port ");
                tmp+=std::to_string(portNb);
                simAddLog("RemoteApi",sim_verbosity_errors,tmp.c_str());
            }
            index++;
        }
        else
            break;
    }


    // Check if we need to start additional remote API server services:
    for (int iarg=0;iarg<9;iarg++)
    {
        char* str=simGetStringParameter(sim_stringparam_app_arg1+iarg);
        if (str!=NULL)
        {
            std::string arg(str);
            simReleaseBuffer(str);
            if ( (arg.compare(0,23,"REMOTEAPISERVERSERVICE_")==0)&&(arg.length()>23) )
            {
                size_t pos1=arg.find('_',24);
                size_t pos2=std::string::npos;
                size_t pos3=std::string::npos;
                if (pos1!=std::string::npos)
                    pos2=arg.find('_',pos1+1);
                if (pos2!=std::string::npos)
                    pos3=arg.find('_',pos2+1);
                int portNb=-1;
                bool debug=false;
                bool syncTrigger=true;
                int maxPacketSize=1300;
                std::string portStr;
                if (pos1!=std::string::npos)
                {
                    portStr=arg.substr(23,pos1-23);
                    if (pos2!=std::string::npos)
                    {
                        debug=(arg.compare(pos1+1,pos2-pos1-1,"TRUE")==0);
                        if (pos3!=std::string::npos)
                            syncTrigger=(arg.compare(pos2+1,pos3-pos2-1,"TRUE")==0);
                        else
                            syncTrigger=(arg.compare(pos2+1,std::string::npos,"TRUE")==0);
                    }
                    else
                        debug=(arg.compare(pos1+1,std::string::npos,"TRUE")==0);
                }
                else
                    portStr=arg.substr(23,std::string::npos);
                if (portStr.length()!=0)
                {
                    portNb=atoi(portStr.c_str());

                    if (allConnections.getConnectionFromPort(portNb)==NULL)
                    {
                        CSimxSocket* oneSocketConnection=new CSimxSocket(portNb,true,false,debug,maxPacketSize,syncTrigger);
                        oneSocketConnection->start();
                        allConnections.addSocketConnection(oneSocketConnection);
                        std::string tmp("starting a remote API server on port ");
                        tmp+=std::to_string(portNb);
                        simAddLog("RemoteApi",sim_verbosity_loadinfos,tmp.c_str());
                    }
                    else
                    {
                        std::string tmp("failed starting a remote API server on port ");
                        tmp+=std::to_string(portNb);
                        simAddLog("RemoteApi",sim_verbosity_errors,tmp.c_str());
                    }
                }
            }
        }
    }

    return(SIMX_VERSION); // initialization went fine, we return the version number of this extension module (can be queried with simGetModuleName)
}

// This is the plugin end routine:
SIM_DLLEXPORT void simEnd()
{ // This is called just once, at the end of CoppeliaSim
    allConnections.removeAllConnections();

    unloadSimLibrary(simLib); // release the library
}

// This is the plugin messaging routine (i.e. CoppeliaSim calls this function very often, with various messages):
SIM_DLLEXPORT void* simMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle

    // This function should not generate any error messages:
    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);

    void* retVal=NULL;

    if (message==sim_message_eventcallback_instancepass)
    { // This message is sent each time the scene was rendered (well, shortly after) (very often)
        allConnections.instancePass();
    }

    if (message==sim_message_eventcallback_mainscriptabouttobecalled)
    { // Main script is about to be run
        if (allConnections.thereWasARequestToCallTheMainScript())
        {
            simSetBooleanParameter(sim_boolparam_waiting_for_trigger,1); // the remote API client can query that value
            replyData[0]=0; // this tells CoppeliaSim that we don't wanna execute the main script
        }
        else
        {
            simSetBooleanParameter(sim_boolparam_waiting_for_trigger,0); // the remote API client can query that value
            allConnections.mainScriptWillBeCalled(); // this simply tells all remote API server services to reactivate their triggers (if that function is enabled)
        }
    }

    if (message==sim_message_eventcallback_simulationended)
    { // Simulation just ended
        simSetBooleanParameter(sim_boolparam_waiting_for_trigger,0);
        allConnections.simulationEnded();
    }

    simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
    return(retVal);
}

