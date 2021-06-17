cmake_minimum_required(VERSION 3.19)
project(STL)

set(CMAKE_CXX_STANDARD 20)

add_executable(STL main.cpp rb_tree.cpp rb_tree.h Alloc.cpp Alloc.h)

# stl_uninitialzed.cpp stl_uninitialzed.h