#include <JackAnalyzer.h>

void SymbolTable::startSubroutine()
{
    subST.clear();
    argCount = 0;
    localCount = 0;
}

void SymbolTable::define(string name, string type, int kind)
{
    switch (kind)
    {
    case FIELD:
    {
        Variable classVar(type, kind, fieldCount);
        classST[name] = classVar;
        fieldCount++;
        break;
    }
    case STATIC:
    {
        Variable classVar(type, kind, staticCount);
        classST[name] = classVar;
        staticCount++;
        break;
    }
    case ARGUMENT:
    {
        Variable subVar(type, kind, argCount);
        classST[name] = subVar;
        argCount++;
        break;
    }
    case LOCAL:
    {
        Variable subVar(type, kind, localCount);
        classST[name] = subVar;
        localCount++;
        break;
    }
    }
}

int SymbolTable::VarCount(int kind)
{
    switch (kind)
    {
    case FIELD:
    {
        return fieldCount;
        break;
    }
    case STATIC:
    {
        return staticCount;
        break;
    }
    case ARGUMENT:
    {
        return argCount;
        break;
    }
    case LOCAL:
    {
        return localCount;
        break;
    }
    }

    return -1;
}

int SymbolTable::KindOf(string name)
{
    map<string, Variable>::iterator STI;
    STI = classST.find(name);
    if (STI == classST.end())
    {
        STI = subST.find(name);
        if (STI == subST.end())
            return NONE;
        else
            return STI->second.kind;
    }
    else
    {
        return STI->second.kind;
    }
}

string SymbolTable::TypeOf(string name)
{
    map<string, Variable>::iterator STI;
    STI = classST.find(name);
    if (STI == classST.end())
    {
        STI = subST.find(name);
        if (STI == subST.end())
            throw runtime_error("cannot find this variable in the symbol table");
        else
            return STI->second.type;
    }
    else
    {
        return STI->second.type;
    }
}

int SymbolTable::IndexOf(string name)
{
    map<string, Variable>::iterator STI;
    STI = classST.find(name);
    if (STI == classST.end())
    {
        STI = subST.find(name);
        if (STI == subST.end())
            return NONE;
        else
            return STI->second.index;
    }
    else
    {
        return STI->second.index;
    }
}