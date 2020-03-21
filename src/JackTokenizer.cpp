#include "JackAnalyzer.h"

string JackTokenizer::removeLineBlank(string &line)
{
    string re_line;
    stringstream lst(line);
    while (!lst.eof())
    {
        string s;
        lst >> s;
        re_line += (s + " ");
    }
    return re_line;
}

string JackTokenizer::removeComments(string &file)
{
    string s;
    for (int i = 0; i < file.size(); i++)
    {
        if (file[i] == '/' && file[i + 1] == '/')
        {
            while (file[i] != '\n')
            {
                i++;
            }
        }
        else if (file[i] == '/' && file[i + 1] == '*')
        {
            while (!(file[i] == '/' && file[i - 1] == '*'))
            {
                i++;
            }
            i += 2; //jump over '\n'
        }
        else if (file[i] == '/' && file[i + 1] == '*' && file[i + 2] == '*')
        {
            while (!(file[i] == '/' && file[i - 1] == '*'))
            {
                i++;
            }
            i += 2; //jump over '\n'
        }
        else
        {
            s += file[i];
        }
    }
    return s;
}

char JackTokenizer::getNextCharacter()
{
    char c = fileBuffer[index];
    index++;
    return c;
}

void JackTokenizer::unget()
{
    index--;
}

bool JackTokenizer::isSymbol(char c)
{
    for (char s : symbolList)
    {
        if (c == s)
            return true;
    }
    return false;
}

bool JackTokenizer::isKeyword(string &s)
{
    for (string ss : keywords)
    {
        if (s == ss)
            return true;
    }
    return false;
}

JackTokenizer::JackTokenizer(string &path)
{
    ifstream ist = ifstream(path.c_str());

    if (!ist)
        throw runtime_error("cannot open input file");

    while (!ist.eof())
    {
        string line;
        getline(ist, line);
        line = removeLineBlank(line);
        if (line[0] != ' ')
        {
            line += '\n';
            fileBuffer += line;
        }
    }

    fileBuffer = removeComments(fileBuffer);
}

bool JackTokenizer::hasMoreTokens()
{
    return index < fileBuffer.size();
}

void JackTokenizer::advance()
{
    if (!curToken.val.empty())
        prevToken = curToken;

    if (index != 0)
        previndex = index;

    if (hasMoreTokens())
    {
        char c = getNextCharacter();
        string curValue;

        //handle string constant
        if (c == '"')
        {
            c = getNextCharacter();
            do
            {
                curValue += c;
                c = getNextCharacter();
            } while (c != '"');
            curToken.set(curValue, STRING_CONST);
        }
        //handle keyword or indentifier
        else if (isalpha(c))
        {
            while (c != ' ' && (!isSymbol(c) || c == '_'))
            {
                curValue += c;
                c = getNextCharacter();
            }
            if (isKeyword(curValue))
                curToken.set(curValue, KEYWORD);
            else
                curToken.set(curValue, IDENTIFIER);

            unget();
        }
        //handle integer constant
        else if (isdigit(c))
        {
            while (isdigit(c))
            {
                curValue += c;
                c = getNextCharacter();
            }
            curToken.set(curValue, INT_CONST);

            unget();
        }
        //handle symbol
        else if (isSymbol(c))
        {
            curValue = c;
            curToken.set(curValue, SYMBOL);
        }
        else if (isspace(c))
        {
            if (index == fileBuffer.size())
                curToken.set(curValue, NULL);
            if (hasMoreTokens())
                advance();
        }
        else
        {
            throw runtime_error("invalid input Token!");
        }
    }
}

int JackTokenizer::tokenType()
{
    return curToken.type;
}

int JackTokenizer::keyWord()
{
    if (curToken.type != KEYWORD)
        throw runtime_error("current token is not a keyword!");
    for (int i = 0; i < keywords.size(); i++)
    {
        if (curToken.val == keywords[i])
            return i + 5;
    }
}

char JackTokenizer::symbol()
{
    if (curToken.type != SYMBOL)
        throw runtime_error("current token is not a symbol!");
    return curToken.val[0];
}

string JackTokenizer::identifier()
{
    if (curToken.type != IDENTIFIER)
        throw runtime_error("current token is not a identifier!");
    return curToken.val;
}

int JackTokenizer::intVal()
{
    if (curToken.type != INT_CONST)
        throw runtime_error("current token is not a integer constant!");
    stringstream sToInt(curToken.val);
    int ret;
    sToInt >> ret;
    return ret;
}

string JackTokenizer::stringVal()
{
    if (curToken.type != STRING_CONST)
        throw runtime_error("current token is not a string constant!");
    return curToken.val;
}

void JackTokenizer::rollBack()
{
    curToken = prevToken;
    index = previndex;
}

bool JackTokenizer::isOperator()
{
    if (tokenType() == SYMBOL)
    {
        char c = curToken.val[0];
        switch (c)
        {
        case '+':
            return true;
        case '-':
            return true;
        case '*':
            return true;
        case '/':
            return true;
        case '&':
            return true;
        case '|':
            return true;
        case '<':
            return true;
        case '>':
            return true;
        case '=':
            return true;
        }
    }
    return false;
}