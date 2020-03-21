#include "JackAnalyzer.h"

void JackAnalyzer::beginAnalyzing()
{
    JackTokenizer tokenizer(filepath);
    string outputPath = filepath.substr(0, filepath.size() - 4) + "xml";
    CompilationEngine engine(tokenizer, outputPath);
}