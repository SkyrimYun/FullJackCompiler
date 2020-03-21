#include "JackAnalyzer.h"

void CompilationEngine::writeLine(string s)
{
    ost << s << endl;
}

void CompilationEngine::writeXML()
{
    switch (tokenizer.tokenType())
    {
    case KEYWORD:
    {
        writeLine("<keyword> " + tokenizer.tokenVal() + " </keyword>");
        break;
    }
    case IDENTIFIER:
    {
        string name = tokenizer.tokenVal();
        if (ST.KindOf(name) != NONE)
            writeLine("<identifier> " + tokenizer.identifier() + " </identifier> " + ST.TypeOf(name) + " " + to_string(ST.KindOf(name)) + " " + to_string(ST.IndexOf(name)));
        else
            writeLine("<identifier> " + tokenizer.identifier() + " </identifier> ");
        break;
    }
    case SYMBOL:
    {
        if (tokenizer.symbol() == '"')
        {
            writeLine("<symbol> &quot; </symbol>");
        }
        else if (tokenizer.symbol() == '<')
        {
            writeLine("<symbol> &lt; </symbol>");
        }
        else if (tokenizer.symbol() == '>')
        {
            writeLine("<symbol> &gt; </symbol>");
        }
        else if (tokenizer.symbol() == '&')
        {
            writeLine("<symbol> &amp; </symbol>");
        }
        else
        {
            writeLine("<symbol> " + tokenizer.tokenVal() + " </symbol>");
        }
        break;
    }
    case INT_CONST:
    {
        writeLine("<integerConstant> " + tokenizer.tokenVal() + " </integerConstant>");
        break;
    }
    case STRING_CONST:
    {
        writeLine("<stringConstant> " + tokenizer.stringVal() + " </stringConstant>");
        break;
    }
    }
}

CompilationEngine::CompilationEngine(JackTokenizer &jt, string &s)
{
    tokenizer = jt;
    ost = ofstream(s);
    CompileClass();
}

void CompilationEngine::CompileClass()
{
    writeLine("<class>");
    while (tokenizer.hasMoreTokens())
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD)
        {
            int keyword = tokenizer.keyWord();
            if (keyword == FUNCTION || keyword == CONSTRUCTOR || keyword == METHOD)
            {
                CompileSubroutineDec();
            }
            else if (keyword == STATIC || keyword == FIELD)
            {
                CompileClassVarDec();
            }
            else
            {
                writeXML();
            }
        }
        else
        {
            writeXML();
        }
    }
    writeLine("</class>");
}

void CompilationEngine::CompileClassVarDec()
{
    writeLine("<classVarDec>");

    writeXML();
    int kind = tokenizer.keyWord(); // kind of the variable (field or static)

    tokenizer.advance();
    writeXML();
    string type = tokenizer.tokenVal(); // type of the variable

    vector<string> varName;
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML();
            varName.push_back(tokenizer.tokenVal());
        }
        else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',')
        {
            writeXML();
        }
    }

    for (string name : varName)
    {
        ST.define(name, type, kind);
    }

    writeLine("</classVarDec>");
}

void CompilationEngine::CompileSubroutineDec()
{
    writeLine("<subroutineDec>");
    writeXML(); // write type of subroutine (constructor, method, function)

    ST.startSubroutine();
    if (tokenizer.tokenVal() == "method")
        ST.define("this", tokenizer.tokenVal(), ARGUMENT); // write 'this' to argument 0

    // write the return type and the name of this subroutine
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '('))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML();
        }
    }

    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {
        CompileParameterList();
    }

    tokenizer.advance();

    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '{')
    {
        CompileSubroutineBody();
    }
    writeLine("</subroutineDec>");
}

void CompilationEngine::CompileParameterList()
{
    writeLine("<parameterList>");
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML();                         // write type of the argument
            string type = tokenizer.tokenVal(); // type of the variable

            tokenizer.advance();
            if (tokenizer.tokenType() == KEYWORD || tokenizer.tokenType() == IDENTIFIER)
            {
                writeXML();                         // write name of the argument
                string name = tokenizer.tokenVal(); // name of the variable
                ST.define(name, type, ARGUMENT);
            }
        }

        if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() != ')')
        {
            writeXML(); // write ','
        }
    }
    writeLine("</parameterList>");
    writeXML();
}

void CompilationEngine::CompileSubroutineBody()
{
    writeLine("<subroutineBody>");
    writeXML(); // write '{'

    tokenizer.advance();
    while (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == VAR)
    {
        CompileVarDec();
        tokenizer.advance();
    }
    CompileStatements();
    writeXML();
    writeLine("</subroutineBody>");
}

void CompilationEngine::CompileVarDec()
{
    writeLine("<varDec>");
    writeXML();

    tokenizer.advance();
    writeXML();
    string type = tokenizer.tokenVal(); // type of the variable

    vector<string> varName;
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.advance();
        if (tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML();
            varName.push_back(tokenizer.tokenVal());
        }
        else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',')
        {
            writeXML();
        }
    }

    for (string name : varName)
    {
        ST.define(name, type, LOCAL);
    }
    writeLine("</varDec>");
}

void CompilationEngine::CompileStatements()
{
    writeLine("<statements>");

    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '}'))
    {
        switch (tokenizer.keyWord())
        {
        case IF:
            CompileIf();
            break;
        case WHILE:
            CompiileWhile();
            break;
        case DO:
            CompiileDo();
            break;
        case LET:
            CompiileLet();
            break;
        case RETURN:
            CompiileReturn();
            break;
        }
        tokenizer.advance();
    }

    writeLine("</statements>");
}

void CompilationEngine::CompiileLet()
{
    writeLine("<letStatement>");
    writeXML();
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {

        tokenizer.advance();
        if (tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == IDENTIFIER)
        {
            writeXML();
        }

        if (tokenizer.tokenType() == SYMBOL)
        {
            if (tokenizer.symbol() == '=' || tokenizer.symbol() == '[')
            {
                CompiileExpression();
            }
        }
    }

    writeLine("</letStatement>");
}

void CompilationEngine::CompileIf()
{
    writeLine("<ifStatement>");
    writeXML();

    tokenizer.advance();
    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {

        writeXML(); // write '('
        CompiileExpression();

        // write ')'
        tokenizer.advance();
        writeXML();

        // write '{'
        tokenizer.advance();
        writeXML();

        tokenizer.advance();

        CompileStatements();

        writeXML(); // write '}'
    }

    tokenizer.advance();
    if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == ELSE)
    {
        writeXML(); // write "else"
        // write '{'
        tokenizer.advance();
        writeXML();

        tokenizer.advance();
        CompileStatements();

        writeXML(); // write '}'
    }
    else
    {
        tokenizer.rollBack();
    }

    writeLine("</ifStatement>");
}

void CompilationEngine::CompiileWhile()
{
    writeLine("<whileStatement>");
    writeXML(); // write "while"

    tokenizer.advance();
    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {

        writeXML(); // write '('
        CompiileExpression();

        // write ')'
        tokenizer.advance();
        writeXML();

        // write '{'
        tokenizer.advance();
        writeXML();

        tokenizer.advance();

        CompileStatements();

        writeXML(); // write '}'
    }

    writeLine("</whileStatement>");
}

void CompilationEngine::CompiileDo()
{
    writeLine("<doStatement>");
    writeXML();
    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.advance();

        if (tokenizer.tokenType() == IDENTIFIER || tokenizer.tokenType() == SYMBOL || tokenizer.tokenType() == KEYWORD)
        {
            writeXML();
        }

        if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
        {
            CompileExpressionList();
            writeXML(); // write ')'
        }
    }
    writeLine("</doStatement>");
}

void CompilationEngine::CompiileReturn()
{
    writeLine("<returnStatement>");
    writeXML();

    tokenizer.advance();
    if (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';'))
    {
        tokenizer.rollBack();
        CompiileExpression();
        // write ';'
        tokenizer.advance();
        writeXML();
    }
    else
        writeXML();

    writeLine("</returnStatement>");
}

//get into the function one token before its real start token
//return token pointed to input token
void CompilationEngine::CompiileExpression()
{
    writeLine("<expression>");

    do
    {
        tokenizer.advance();
        CompileTerm();
        tokenizer.advance();

        if (tokenizer.tokenType() == SYMBOL && tokenizer.isOperator())
        {
            writeXML();
        }

    } while (tokenizer.tokenType() == SYMBOL && tokenizer.isOperator());

    tokenizer.rollBack();
    writeLine("</expression>");
}

//intput token is the exact right input token
//return token pointed to input token positon
void CompilationEngine::CompileTerm()
{
    writeLine("<term>");
    writeXML();

    //term is an identifier
    if (tokenizer.tokenType() == IDENTIFIER)
    {
        tokenizer.advance();

        //next token is a symbol
        if (tokenizer.tokenType() == SYMBOL)
        {
            switch (tokenizer.symbol())
            {
            //varName.method()
            case '.':
            {
                writeXML(); // write "."
                tokenizer.advance();
                writeXML(); // write "method"
                tokenizer.advance();
                writeXML(); //write "("
                CompileExpressionList();
                writeXML(); //write ")"
                break;
            }
            //varName()
            case '(':
            {
                writeXML();
                CompileExpressionList();
                writeXML(); //write ")"
                break;
            }
            //varName[]
            case '[':
            {
                writeXML();
                CompiileExpression();
                tokenizer.advance();
                writeXML(); // write "]"
                break;
            }
            default:
            {
                tokenizer.rollBack();
            }
            }
        }
        else
        {
            tokenizer.rollBack();
        }
    }
    else if (tokenizer.tokenType() == SYMBOL && (tokenizer.symbol() == '-' || tokenizer.symbol() == '~'))
    {
        tokenizer.advance();
        CompileTerm();
    }
    else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {
        CompiileExpression();
        tokenizer.advance();
        writeXML(); // write ")"
    }

    writeLine("</term>");
}

//get into the functin as current token = '('
//return ')'
void CompilationEngine::CompileExpressionList()
{
    writeLine("<expressionList>");

    tokenizer.advance();

    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')'))
    {

        tokenizer.rollBack();
        CompiileExpression();

        tokenizer.advance();

        if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',')
        {
            writeXML();
            tokenizer.advance();
        }
    };

    writeLine("</expressionList>");
}