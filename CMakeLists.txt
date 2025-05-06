cmake_minimum_required(VERSION 3.14)
project(docman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    third_parties/cpp-httplib          
    third_parties/nlohmann             
)

find_package(OpenSSL REQUIRED)

add_executable(docman
    main.cpp
    citation.cpp
    citation.h
    utils.hpp
)

target_link_libraries(docman PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(WIN32)
    target_link_libraries(docman PRIVATE ws2_32)
endif()