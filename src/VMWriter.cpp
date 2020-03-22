#include <JackAnalyzer.h>

void VMWriter::writeLine(string s)
{
    ofs << s << endl;
}

VMWriter::VMWriter(ofstream &of)
{
    ofs.swap(of);
}

VMWriter &VMWriter::operator=(VMWriter &wt)
{
    ofs.swap(wt.ofs);
}

void VMWriter::writePush(int segment, int index)
{
    string segmentN;
    switch (segment)
    {
    case LOCAL:
        segmentN = "local";
        break;
    case ARGUMENT:
        segmentN = "argument";
        break;
    case FIELD:
        segmentN = "this";
        break;
    case THAT:
        segmentN = "that";
        break;
    case CONSTANT:
        segmentN = "constant";
        break;
    case STATIC:
        segmentN = "static";
        break;
    case POINTER:
        segmentN = "pointer";
        break;
    case TEMP:
        segmentN = "temp";
        break;
    default:
        segmentN = to_string(segment);
    }
    writeLine("push " + segmentN + " " + to_string(index));
}

void VMWriter::writePop(int segment, int index)
{
    string segmentN;
    switch (segment)
    {
    case LOCAL:
        segmentN = "local";
        break;
    case ARGUMENT:
        segmentN = "argument";
        break;
    case THIS:
        segmentN = "this";
        break;
    case THAT:
        segmentN = "that";
        break;
    case CONSTANT:
        segmentN = "constant";
        break;
    case STATIC:
        segmentN = "static";
        break;
    case POINTER:
        segmentN = "pointer";
        break;
    case TEMP:
        segmentN = "temp";
        break;
    default:
        segmentN = "nmsl";
    }
    writeLine("pop " + segmentN + " " + to_string(index));
}

void VMWriter::writeArithmetic(char c)
{
    string ari;
    switch (c)
    {
    case '+':
        ari = "add";
        break;
    case '-':
        ari = 'sub';
        break;
    case '/':
        ari = "call Math.divide 2";
        break;
    case '*':
        ari = "call Math.multiply 2";
        break;
    case '~':
        ari = "not";
        break;
    case 'U':
        ari = "neg";
        break;
    case '<':
        ari = "lt";
        break;
    case '>':
        ari = "gt";
        break;
    case '=':
        ari = "eq";
        break;
    case '&':
        ari = "and";
        break;
    case '|':
        ari = "or";
        break;
    }
    writeLine(ari);
}

void VMWriter::writeCall(string name, int nArgs)
{
    writeLine("call " + name + " " + to_string(nArgs));
}