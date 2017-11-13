#pragma once

#include <vector>
#include "simxSocket.h"

class CSimxConnections
{
public:
    CSimxConnections();
    virtual ~CSimxConnections();

    void addSocketConnection(CSimxSocket* conn);
// 3/3/2014 CSimxSocket* getSynchronousSimulationTriggerConnection();
// 3/3/2014 void setSynchronousSimulationTriggerConnection(CSimxSocket* conn);
    void removeSocketConnection(CSimxSocket* conn);

    CSimxSocket* getConnectionAtIndex(int index);
    CSimxSocket* getConnectionFromPort(int portNb);


    bool thereWasARequestToCallTheMainScript();
    void mainScriptWillBeCalled();
    void simulationEnded();
    void instancePass();

    void removeAllConnections();

protected:
    std::vector<CSimxSocket*> _allSocketConnections;
// 3/3/2014 CSimxSocket* _synchronousSimulationTriggerConnection;
};
