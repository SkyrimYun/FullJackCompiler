#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <io.h>

using namespace std;

#define KEYWORD 0
#define SYMBOL 1
#define IDENTIFIER 2
#define INT_CONST 3
#define STRING_CONST 4
#define CLASS 5
#define METHOD 6
#define FUNCTION 7
#define CONSTRUCTOR 8
#define INT 9
#define BOOLEAN 10
#define CHAR 11
#define VOID 12
#define VAR 13
#define STATIC 14
#define FIELD 15
#define LET 16
#define DO 17
#define IF 18
#define ELSE 19
#define WHILE 20
#define RETURN 21
#define TRUE 22
#define FALSE 23
#define NULL 24
#define THIS 25
#define INVALID 26

class JackTokenizer
{
private:
    struct Token
    {
        string val;
        int type;

        Token(){};
        void set(string &s, int t)
        {
            val = s;
            type = t;
        }
        void reset()
        {
            val = "";
            type = -1;
        }
    };

    string fileBuffer;
    Token curToken;
    Token prevToken;
    int index = 0;
    int previndex = 0;
    vector<string> keywords{
        "class",
        "method",
        "function",
        "constructor",
        "int",
        "boolean",
        "char",
        "void",
        "var",
        "static",
        "field",
        "let",
        "do",
        "if",
        "else",
        "while",
        "return",
        "true",
        "false",
        "null",
        "this",
    };
    vector<char> symbolList{'{', '}', '(', ')', '[', ']', '.', ',', '.', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '_', '~'};

    string removeLineBlank(string &);
    string removeComments(string &);
    bool isSymbol(char);
    bool isKeyword(string &);
    char getNextCharacter();
    void unget();

public:
    JackTokenizer(string &);
    JackTokenizer(){};

    //are there more tokens in the input
    bool hasMoreTokens();

    //gets the next token from the input, and makes it the current token.
    //this method should only be called if hasMoreTokens is true
    void advance();

    //return the type of the current token
    int tokenType();

    //returns the keyword which is the current token, as a constant
    //this method should be called only if tokenType is KEYWORD
    int keyWord();

    //returns the character which is the current token
    //should only be called if tokenType is SYMBOL
    char symbol();

    //returns the identifier which is the current token
    //should be called only if tokenType is IDENTIFIER
    string identifier();

    //returns the inter value of the current token.
    //should be called only if tokenType is INT_CONST.
    int intVal();

    //returns the string value of the current token, without the two enclosing double quotes.
    //should be called only if tokenType is STRING_CONST
    string stringVal();

    string tokenVal()
    {
        return curToken.val;
    }

    void rollBack();

    bool isOperator();
};

class CompilationEngine
{
private:
    JackTokenizer tokenizer;
    ofstream ost;

    void writeLine(string);
    void writeXML();

public:
    CompilationEngine(JackTokenizer &, string &);

    //compiles a complete class
    void CompileClass();

    //comppiles a static variable declaration, or a field declaration
    void CompileClassVarDec();

    //compiles a complete method, function, or constructor
    void CompileSubroutineDec();

    //compiles a (possibly empty) parameter list. does not handle the enclosing "()"
    void CompileParameterList();

    //compiles a subroutine's body
    void CompileSubroutineBody();

    //compiles a var declaration
    void CompileVarDec();

    //compiles a sequence of statements. does not handle the enclosing "{}"
    void CompileStatements();

    //compiles a let statement
    void CompiileLet();

    //compiles an if statement, possibly with a trailing else caluse
    void CompileIf();

    //compiles a while statement
    void CompiileWhile();

    //compiles a do statement
    void CompiileDo();

    //compiles a return statement
    void CompiileReturn();

    //compiles a expression statement
    void CompiileExpression();

    /*compiles a term. 
    if the current token is an identifier, the routine must distinguish between a variable, an array entry, or a subroutine call.
    a single look-ahead token, which may be one of '[', '(', or '.', suffices to distinguish between the possibilities.
    any other token is not part of this term and should not be advanced over */
    void CompileTerm();

    //compiles a (possibly empty) comma-separated list of expressions
    void CompileExpressionList();
};

class JackAnalyzer
{
private:
    string filepath;

public:
    JackAnalyzer(string &path) : filepath(path){};

    void beginAnalyzing();
};