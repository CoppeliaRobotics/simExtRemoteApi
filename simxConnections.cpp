#include "simxConnections.h"

CSimxConnections::CSimxConnections()
{
// 3/3/2014 _synchronousSimulationTriggerConnection=NULL;
}

CSimxConnections::~CSimxConnections()
{
    removeAllConnections();
}

void CSimxConnections::removeAllConnections()
{
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
        delete _allSocketConnections[i];
    _allSocketConnections.clear();
}

void CSimxConnections::addSocketConnection(CSimxSocket* conn)
{
    _allSocketConnections.push_back(conn);
}

// 3/3/2014
/*
CSimxSocket* CSimxConnections::getSynchronousSimulationTriggerConnection()
{
    return(_synchronousSimulationTriggerConnection);
}

void CSimxConnections::setSynchronousSimulationTriggerConnection(CSimxSocket* conn)
{
    _synchronousSimulationTriggerConnection=conn;
}
*/

void CSimxConnections::removeSocketConnection(CSimxSocket* conn)
{
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
    {
        if (_allSocketConnections[i]==conn)
        {
            delete conn;
            _allSocketConnections.erase(_allSocketConnections.begin()+i);
            break;
        }
    }
}

CSimxSocket* CSimxConnections::getConnectionAtIndex(int index)
{
    if ((index>=0)&&(index<int(_allSocketConnections.size())))
        return(_allSocketConnections[index]);
    return(NULL);
}

CSimxSocket* CSimxConnections::getConnectionFromPort(int portNb)
{
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
    {
        if (_allSocketConnections[i]->getPortNb()==portNb)
            return(_allSocketConnections[i]);
    }
    return(NULL);
}

// 3/3/2014
bool CSimxConnections::thereWasARequestToCallTheMainScript()
{   // return value true means: do not execute the main script!
    // Since 3/3/2014 we can have several clients pre-enabled to generate the trigger!
    bool retVal=false;
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
    {
        _allSocketConnections[i]->thereWasARequestToCallTheMainScript();

        // corrected on 16/5/2014 if (_allSocketConnections[i]->getWaitForTriggerAuthorized())
        if (_allSocketConnections[i]->getWaitForTriggerEnabled())
        { // this socket has to generate the trigger (if enabled)
            if (_allSocketConnections[i]->getWaitForTrigger())
            { // trigger not yet received
                if (_allSocketConnections[i]->getStatus()==3) 
                    retVal=true; // wait only if the comm thread is running and the client is still connected!
            }
            else
            {
                // we will reactivate the trigger for next simulation loop (in function mainScriptWillBeCalled), when we are sure the main script will be called (since other remote API server sevices might still be waiting for a trigger)
                // _allSocketConnections[i]->setWaitForTrigger(true); // trigger received. Reactivate it for next time!
            }
        }
    }
    return(retVal);
}

void CSimxConnections::mainScriptWillBeCalled()
{ // Here we simple re-enable all trigger waits for next simulation loop:
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
    {
        if (_allSocketConnections[i]->getWaitForTriggerEnabled())
            _allSocketConnections[i]->setWaitForTrigger(true);
    }
}


void CSimxConnections::simulationEnded()
{
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
    {
        if (_allSocketConnections[i]->getActiveOnlyDuringSimulation())
        {
            removeSocketConnection(_allSocketConnections[i]);
            i=-1; // make sure to start the loop from the beginning (ordering has changed)
        }
    }
}

void CSimxConnections::instancePass()
{
    for (unsigned int i=0;i<_allSocketConnections.size();i++)
        _allSocketConnections[i]->instancePass();
}
