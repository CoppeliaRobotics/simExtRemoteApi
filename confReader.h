#pragma once

#include <vector>
#include <string>

class CConfReader
{
public:
    CConfReader();
    virtual ~CConfReader();

    bool readConfiguration(const char* filename);

    bool getString(const char* variableName,std::string& variable);
    bool getInteger(const char* variableName,int& variable);
    bool getFloat(const char* variableName,float& variable);
    bool getBoolean(const char* variableName,bool& variable);

private:
    void _removeComments(std::string& line);
    bool _removeFrontAndBackSpacesAndTabs(std::string& line);
    int _getVariableIndex(const char* variableName);
    std::vector<std::string> _variables;
    std::vector<std::string> _values;
};
