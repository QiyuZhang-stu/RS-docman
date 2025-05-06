cmake_minimum_required(VERSION 3.14)
project(docman)

include_directories(
    ${PROJECT_SOURCE_DIR}/third_parties/cpp-httplib
    ${PROJECT_SOURCE_DIR}/third_parties/nlohmann/include
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

set_target_properties(docman PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)