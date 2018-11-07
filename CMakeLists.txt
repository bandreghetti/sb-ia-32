cmake_minimum_required(VERSION 3.5.1)
project(SB2018-2)

add_definitions(-std=c++14)
add_definitions(-g)

include_directories(
  tradutor/include/
)

link_directories(
)

add_executable(tradutor.out
  tradutor/src/main.cpp
  tradutor/src/preprocessor.cpp
  tradutor/src/translator.cpp
  tradutor/src/utils.cpp
)
