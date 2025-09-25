# # 针对当前 CPU 的指令集优化
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

# # 向量化和数学优化
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -ftree-vectorize")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -ftree-vectorize")

# 链接时间优化
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)

if(NOT ipo_result)
    message(WARNING "IPO / LTO is not supported: ${ipo_output}")
else()
    message(STATUS "IPO / LTO is supported. Enabling for targets.")
endif()

# 定义为DLL目标设置编译选项的函数
# 定义优先减小体积的DLL配置函数
function(kaubo_setup_dll_target target_name)
    # 基础警告与安全选项（保持不变）
    set(base_cxx_flags
        -Wall -Wextra -Werror -pedantic -pedantic-errors
        -Wconversion -Wsign-conversion -Wshadow
        -Wdouble-promotion -Wformat=2 -Wnull-dereference
        -Wold-style-cast -Woverloaded-virtual -Wunused -Wunreachable-code
        -Wdeprecated -Winvalid-pch -Wstrict-aliasing -Wstrict-overflow=5 -Wcast-align
        -Wmissing-declarations -fstack-protector-strong -D_FORTIFY_SOURCE=2
        -fcolor-diagnostics
    )

    # Release模式：优先减小体积的优化选项
    set(release_cxx_flags
        -Os # 替换-O3为-Os（专门优化体积，牺牲少量速度）
        -flto # 保留LTO，但禁用可能增加体积的扩展选项
        -fomit-frame-pointer
        -fvisibility=hidden
        -fvisibility-inlines-hidden
        -ffunction-sections
        -fdata-sections
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables

        # 精简后端优化，避免代码膨胀
        -mllvm -inline-threshold=300 # 降低内联阈值（减少过度内联）
    )

    # 链接器选项：强化体积优化
    set(link_flags
        -Wl,--gc-sections # 强制移除未使用的节
        -Wl,--strip-all # 剥离所有符号表（Release模式安全）
        -Wl,--icf=all # 合并相同的函数/数据（Clang支持）
        -fstack-protector-strong
    )

    # 应用基础选项
    target_compile_options(${target_name}
        PRIVATE
        ${base_cxx_flags}
    )

    # 应用链接选项
    target_link_options(${target_name}
        PRIVATE
        ${link_flags}
    )

    # 仅Release模式应用体积优化
    target_compile_options(${target_name}
        PRIVATE
        $<$<CONFIG:Release>:${release_cxx_flags}>
    )

    # Windows DLL特定配置
    if(WIN32)
        set_target_properties(${target_name}
            PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS OFF # 禁止自动导出符号
            COMPILE_DEFINITIONS "$<$<CONFIG:Release>:NDEBUG>"

        )
    endif()

    message(STATUS "已为DLL目标 '${target_name}' 配置体积优先的优化选项")
endfunction()



    