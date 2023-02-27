cmake_minimum_required(VERSION 3.24)
project(DynDNS)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt)

include_directories(/usr/include/libxml2)
add_executable(DynDNS main.cpp libs/Domain.cpp libs/Domain.h)
target_link_libraries(DynDNS PRIVATE fmt::fmt curl curlcpp xml2 jsoncpp)
