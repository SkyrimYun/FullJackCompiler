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

CompilationEngine::CompilationEngine(JackTokenizer &jt, VMWriter &wt, string &xml)
{
    tokenizer = jt;
    writer = wt;
    ost = ofstream(xml);
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
                if (!isClassNameStore)
                {
                    className = tokenizer.tokenVal();
                    isClassNameStore = true;
                }
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
        ST.define("this", className, ARGUMENT); // write 'this' to argument 0

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
            writeXML(); // write operator
            writer.writeArithmetic(tokenizer.symbol());
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
        string Name = tokenizer.identifier();

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
                string methodName = tokenizer.identifier();
                tokenizer.advance();
                writeXML(); //write "("

                // push this object to stack as argument 0
                writer.writePush(ST.KindOf(Name), ST.IndexOf(Name));

                int numArg = CompileExpressionList() + 1;
                writeXML(); //write ")"

                string funcationCall = Name + '.' + methodName;
                writer.writeCall(funcationCall, numArg);
                break;
            }
            //varName()
            case '(':
            {
                writeXML();
                writer.writePush(POINTER, 0);
                int numArg = CompileExpressionList() + 1;
                writeXML(); //write ")"
                writer.writeCall(Name, numArg);
                break;
            }
            //varName[]
            case '[':
            {
                writeXML();
                CompiileExpression();

                writer.writePush(ST.KindOf(Name), ST.IndexOf(Name));
                writer.writeArithmetic('+');
                writer.writePop(POINTER, 1);
                writer.writePush(THAT, 0);

                tokenizer.advance();
                writeXML(); // write "]"
                break;
            }
            // other operands
            default:
            {
                tokenizer.rollBack();
                writer.writePush(ST.KindOf(tokenizer.tokenVal()), ST.IndexOf(tokenizer.tokenVal()));
            }
            }
        }
        else
        {
            tokenizer.rollBack();
            writer.writePush(ST.KindOf(tokenizer.tokenVal()), ST.IndexOf(tokenizer.tokenVal()));
        }
    }
    else if (tokenizer.tokenType() == SYMBOL && (tokenizer.symbol() == '-' || tokenizer.symbol() == '~'))
    {
        tokenizer.advance();
        CompileTerm();
        writer.writeArithmetic(tokenizer.symbol());
    }
    else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '(')
    {
        CompiileExpression();
        tokenizer.advance();
        writeXML(); // write ")"
    }
    else if (tokenizer.tokenType() == STRING_CONST)
    {
        string val = tokenizer.stringVal();
        writer.writePush(CONSTANT, val.length());
        writer.writeCall("String.new", 1);
        for (int i = 0; i < val.length(); i++)
        {
            writer.writePush(CONSTANT, val[i]);
            writer.writeCall("String.appendChar", 2);
        }
    }
    else if (tokenizer.tokenType() == INT_CONST)
    {
        writer.writePush(CONSTANT, tokenizer.intVal());
    }
    else if (tokenizer.tokenType() == KEYWORD)
    {
        string curKey = tokenizer.tokenVal();
        if (curKey == "true" || curKey == "flase" || curKey = "null" || curKey == "this")
        {
            switch (tokenizer.keyWord())
            {
            case TRUE:
            {
                writer.writePush(CONSTANT, 1);
                writer.writeArithmetic('U');
                break;
            }

            case FALSE:
            {
                writer.writePush(CONSTANT, 0);
                break;
            }

            case THIS:
            {
                writer.writePush(POINTER, 0);
                break;
            }
            case NULL:
            {
                writer.writePush(CONSTANT, 0);
                break;
            }
            }
        }
    }

    writeLine("</term>");
}

//get into the functin as current token = '('
//return ')'
int CompilationEngine::CompileExpressionList()
{
    writeLine("<expressionList>");
    int count = 0;

    tokenizer.advance();

    while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')'))
    {
        count++;

        tokenizer.rollBack();
        CompiileExpression();

        tokenizer.advance();

        if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',')
        {
            writeXML();
            tokenizer.advance();
        };
    }
    writeLine("</expressionList>");
    return count;
}