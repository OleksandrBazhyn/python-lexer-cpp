cmake_minimum_required(VERSION 3.10)

project(PythonLexerCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(PythonLexer main.cpp)
target_link_libraries(PythonLexer PRIVATE pthread)

add_executable(PythonLexerTests lexer_tests.cpp)
target_link_libraries(PythonLexerTests PRIVATE gtest gtest_main pthread)

add_test(NAME LexerTests COMMAND PythonLexerTests)