cmake_minimum_required(VERSION 3.20)
project(kaubo)
cmake_policy(SET CMP0177 NEW)
include(cmake/Settings.cmake)
include(cmake/Test.cmake)
include(src/source.cmake)

message(STATUS "kaubo_core_files: ${kaubo_core_files}")

add_library(kaubo_common SHARED ${kaubo_common_files})
add_dependencies(kaubo_common copy_compile_commands)
target_compile_options(kaubo_common PRIVATE ${kaubo_cxx_flags})
target_include_directories(kaubo_common PUBLIC
    ${kaubo_include_directories}
)

# 可执行文件定义，并链接动态库
add_executable(Demo ${kaubo_src_dir}/Demo/main.cpp)

# 链接kaubo_common动态库
target_link_libraries(Demo PRIVATE kaubo_common)