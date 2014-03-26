cmake_minimum_required (VERSION 2.6)
project (generic_object_pool)
 
SET(CXX_FLAGS
 -g
 -rdynamic
 -fPIC
 -Wall
 -Wextra
 -Werror
 -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -Wno-unused-parameter
 -Wno-reorder
 -Wno-unused-variable
 -DVALGRIND
 -D_FILE_OFFSET_BITS=64
 -DHAVE_STDINT_H
 -DHAVE_INTTYPES_H
 )

STRING(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

include_directories("../include")

file(GLOB srcs1 "./*1.cc")
file(GLOB srcs2 "./*2.cc")

add_executable(example1 ${srcs1})
target_link_libraries(example1 glog boost_system)
add_executable(example2 ${srcs2})
target_link_libraries(example2 glog boost_system)


 
