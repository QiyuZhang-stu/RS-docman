cmake_minimum_required(VERSION 3.14)
project(docman)

# 强制使用C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 关键路径配置（适配图片中的目录结构）
include_directories(
    ${PROJECT_SOURCE_DIR}/third_parties  # 覆盖 cpp-httplib 和 nlohmann
)

# 查找OpenSSL库
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

# 打印验证信息
message(STATUS "-------------------------------------")
message(STATUS "最终包含路径：")
get_target_property(inc_dirs docman INCLUDE_DIRECTORIES)
foreach(dir ${inc_dirs})
    file(TO_NATIVE_PATH "${dir}" native_dir)
    message(STATUS "  ${native_dir}")
endforeach()