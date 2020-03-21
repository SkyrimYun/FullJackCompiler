#include "JackAnalyzer.h"

bool checkjack(string &s)
{
    string ss;

    if (s.size() < 6)
        return false;

    for (int i = s.size() - 5; i < s.size(); i++)
    {
        ss.push_back(s[i]);
    }
    if (ss == ".jack")
        return true;
    else
        return false;
}

void getAllFiles(string path, vector<string> &files)
{

    if (checkjack(path))
    {
        files.push_back(path);
    }
    else
    {
        long hFile = 0;

        struct _finddata_t fileinfo;
        string p;

        if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                string check;
                check.append(fileinfo.name);
                if (checkjack(check))
                {
                    files.push_back(p.assign(path).append("/").append(fileinfo.name));
                }
            } while (_findnext(hFile, &fileinfo) == 0);
            _findclose(hFile);
        }
    }
}

int main()
{
    try
    {
        vector<string> files;

        string inputPath = "C:/Users/skyri/projects/FullJackCompiler/test/Ball.jack";

        getAllFiles(inputPath, files);

        if (files.size() == 0)
            throw runtime_error("no vaild input file!");

        for (int i = 0; i < files.size(); i++)
        {
            JackAnalyzer analyzer(files[i]);
            analyzer.beginAnalyzing();
        }
    }
    catch (const runtime_error &e)
    {
        cout << e.what() << "\n";
    }
}