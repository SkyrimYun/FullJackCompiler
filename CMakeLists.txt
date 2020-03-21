cmake_minimum_required(VERSION 3.5)

project (JackCompiler)

set(SOURCES
    src/CompilationEngine.cpp
    src/main.cpp
    src/JackAnalyzer.cpp
    src/JackTokenizer.cpp
    src/SymbolTable.cpp
)

add_executable(JackCompiler ${SOURCES})

target_include_directories(JackCompiler
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)