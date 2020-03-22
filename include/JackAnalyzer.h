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
#define ARGUMENT 27
#define LOCAL 28
#define NONE 29
#define CONSTANT 30
#define TEMP 31
#define POINTER 32
#define THAT 33

class SymbolTable
{
private:
    struct Variable
    {
        string type;
        int kind;
        int index;

        Variable(string type, int kind, int index) : type(type), kind(kind), index(index){};
        Variable(){};
    };

    map<string, Variable> classST;
    map<string, Variable> subST;

    int fieldCount;
    int staticCount;
    int argCount;
    int localCount;

public:
    SymbolTable() : fieldCount(0), staticCount(0), argCount(0), localCount(0){};

    //starts a new subroutine scope (resets the subroutine's symbol table)
    void startSubroutine();

    //defines a new identifier of the given name, type, and kind, and assigns it a runing index.
    //STATIC and FIELD identifiers have a class scope, while ARG and VAR identifier have a subroutine scop
    void define(string name, string type, int kind);

    //returns the number of variables of the given kind already defined in the current scope
    int VarCount(int kind);

    //returns the kind of the named identifier in the current scope.
    //if the identifier is unknown in the current scope, returns NONE
    int KindOf(string name);

    //returns the type of the named identifier in the current scope
    string TypeOf(string name);

    //returns the index assigned to the named identifier
    int IndexOf(string name);
};

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

class JackAnalyzer
{
private:
    string filepath;

public:
    JackAnalyzer(string &path) : filepath(path){};

    void beginAnalyzing();
};

class VMWriter
{
private:
    ofstream ofs;
    string className;

public:
    //create a new output .vm file and prepares it for writing
    VMWriter(ofstream &ofs);

    VMWriter(){};

    void setClassName(string &);

    VMWriter &operator=(VMWriter &);

    void writeLine(string);

    //writes a VM push command
    void writePush(int segment, int index);

    //write a VM pop command
    void writePop(int segment, int index);

    //writes a VM arithmetic-logical command
    void writeArithmetic(char);

    void writeLabel(string label);

    void writeGoto(string label);

    void writeIf(string label);

    void writeCall(string name, int nArgs);

    void writeFuncation(string name, int nLocals);

    void writeReturn();

    void close();
};

class CompilationEngine
{
private:
    JackTokenizer tokenizer;
    VMWriter writer;
    ofstream ost;
    SymbolTable ST;

    int labelC = 0;

    string className;
    bool isClassNameStore = false;

    void writeLine(string);
    void writeXML();

public:
    CompilationEngine(JackTokenizer &, VMWriter &, string &);

    //compiles a complete class
    void CompileClass();

    //comppiles a static variable declaration, or a field declaration
    void CompileClassVarDec();

    //compiles a complete method, function, or constructor
    void CompileSubroutineDec();

    //compiles a (possibly empty) parameter list. does not handle the enclosing "()"
    void CompileParameterList();

    //compiles a subroutine's body
    void CompileSubroutineBody(string &, int);

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
    int CompileExpressionList();
};
