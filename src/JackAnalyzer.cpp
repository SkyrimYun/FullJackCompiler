#include "JackAnalyzer.h"

void JackAnalyzer::beginAnalyzing()
{
    JackTokenizer tokenizer(filepath);
    string outputPath_xml = filepath.substr(0, filepath.size() - 4) + "xml";
    string outputPath_vm = filepath.substr(0, filepath.size() - 4) + "vm";
    ofstream of(outputPath_vm);
    VMWriter writer(of);
    CompilationEngine engine(tokenizer, writer, outputPath_xml);
}
