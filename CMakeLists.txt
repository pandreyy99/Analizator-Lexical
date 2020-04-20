cmake_minimum_required(VERSION 3.16)
project(Lexical_Analyzer)

set(CMAKE_CXX_STANDARD 98)

add_executable(Lexical_Analyzer main.cpp Scanner.cpp Scanner.h)