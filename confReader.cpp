#include "confReader.h"
#include <fstream>
#include <stdlib.h>

CConfReader::CConfReader()
{
}

CConfReader::~CConfReader()
{
}

bool CConfReader::readConfiguration(const char* filename)
{
    bool retVal=false;
    std::ifstream file;
    file.open(filename);
    if (file.is_open())
    {
        while (!file.eof())
        {
            char l[400];
            file.getline(l,400);
            std::string line(l);
            if ((line.length()!=0)&&(line[line.length()-1]==13)) // Needed on Linux. Thanks to Patrick Bouffard
                line.erase(line.begin()+line.length()-1);
            _removeComments(line);
            size_t ePos=line.find('=');
            if ( (ePos!=std::string::npos)&&(ePos>0)&&(ePos+1<line.length()) )
            {
                std::string var,val;
                var.assign(line.begin(),line.begin()+ePos);
                val.assign(line.begin()+ePos+1,line.end());
                if ( _removeFrontAndBackSpacesAndTabs(var)&&_removeFrontAndBackSpacesAndTabs(val)&&(var.find(' ')==std::string::npos) )
                {
                    int ind=_getVariableIndex((const char*)var.c_str());
                    if (ind!=-1)
                    { // Variable multiple defined. We keep the last one!
                        _variables.erase(_variables.begin()+ind);
                        _values.erase(_values.begin()+ind);
                    }
                    _variables.push_back(var);
                    _values.push_back(val);
                }
            }
        }
        file.close();
        retVal=true;
    }
    return(retVal);
}

bool CConfReader::getString(const char* variableName,std::string& variable)
{
    int ind=_getVariableIndex(variableName);
    if (ind==-1)
        return(false);
    variable=_values[ind];
    return(true);
}

bool CConfReader::getBoolean(const char* variableName,bool& variable)
{
    int ind=_getVariableIndex(variableName);
    if (ind==-1)
        return(false);
    std::string val=_values[ind];
    for (int i=0;i<int(val.length());i++)
    {
        if (val[i]>='a')
            val[i]-=('a'-'A');
    }
    if (val.compare("TRUE")==0)
    {
        variable=true;
        return(true);
    }
    if (val.compare("FALSE")==0)
    {
        variable=false;
        return(true);
    }
    return(false);
}


bool CConfReader::getInteger(const char* variableName,int& variable)
{
    int ind=_getVariableIndex(variableName);
    if (ind==-1)
        return(false);
    variable=atoi(_values[ind].c_str());
    return(true);
}

bool CConfReader::getFloat(const char* variableName,float& variable)
{
    int ind=_getVariableIndex(variableName);
    if (ind==-1)
        return(false);
    variable=float(atof(_values[ind].c_str()));
    return(true);
}

int CConfReader::_getVariableIndex(const char* variableName)
{
    for (int i=0;i<int(_variables.size());i++)
    {
        if (_variables[i].compare(variableName)==0)
            return(i);
    }
    return(-1);
}

void CConfReader::_removeComments(std::string& line)
{
    for (int i=0;i<int(line.length())-1;i++)
    {
        if ( (line[i]=='/')&&(line[i+1]=='/') )
        {
            line.erase(line.begin()+i,line.end());
            return;
        }
    }
}

bool CConfReader::_removeFrontAndBackSpacesAndTabs(std::string& line)
{
    while ((line.length()!=0)&&( (line[0]==' ')||(line[0]==9) ) )
        line.erase(line.begin());
    while ((line.length()!=0)&&( (line[line.length()-1]==' ')||(line[line.length()-1]==9)  ) )
        line.erase(line.begin()+line.length()-1);
    return(line.length()!=0);
}

