cmake_minimum_required(VERSION 3.14)
project(docman)

# 强制使用C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 精确包含第三方库路径（关键修复）
include_directories(
    ${PROJECT_SOURCE_DIR}/third_parties/cpp-httplib
    ${PROJECT_SOURCE_DIR}/third_parties/nlohmann/include  # 指向 include 目录
)

# 查找OpenSSL依赖
find_package(OpenSSL REQUIRED)

# 构建可执行文件
add_executable(docman
    main.cpp
    citation.cpp
    citation.h
    utils.hpp
)

# 链接加密库
target_link_libraries(docman PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Windows专用配置
if(WIN32)
    target_link_libraries(docman PRIVATE ws2_32)
endif()