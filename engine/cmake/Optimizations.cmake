# # 针对当前 CPU 的指令集优化
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

# # 向量化和数学优化
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -ftree-vectorize")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -ftree-vectorize")


# 链接时间优化
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
if (NOT ipo_result)
    message(WARNING "IPO / LTO is not supported: ${ipo_output}")
else()
    message(STATUS "IPO / LTO is supported. Enabling for targets.")
endif()