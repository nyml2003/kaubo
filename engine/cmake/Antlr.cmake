set(kaubo_antlr4_grammar_dir ${kaubo_dir}/grammar)
set(ANTLR_SOURCE_DIR ${kaubo_dir}/third_party/antlr-runtime/src)
set(kaubo_antlr4_generated_dir ${kaubo_dir}/third_party/antlr-runtime/generated)
add_subdirectory(${kaubo_dir}/third_party/antlr-runtime ${CMAKE_BINARY_DIR}/antlr4_build)
include_directories(${kaubo_antlr4_generated_dir})
include_directories(${kaubo_dir}/third_party/antlr-runtime/src)

set(ANTLR_GENERATION_COMPLETE_FLAG ${kaubo_antlr4_generated_dir}/.antlr4_generation_complete)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows 使用 PowerShell 执行
    add_custom_command(
            OUTPUT ${ANTLR_GENERATION_COMPLETE_FLAG}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${kaubo_antlr4_generated_dir}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${kaubo_antlr4_generated_dir}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${kaubo_antlr4_grammar_dir} ${kaubo_antlr4_generated_dir}
            COMMAND powershell.exe -Command "java -jar '${kaubo_dir}/third_party/antlr-4.13.2-complete.jar' -Dlanguage=Cpp -visitor -o ${kaubo_antlr4_generated_dir} '${kaubo_antlr4_grammar_dir}/Python3Lexer.g4' '${kaubo_antlr4_grammar_dir}/Python3Parser.g4'"
            COMMAND ${CMAKE_COMMAND} -E touch ${ANTLR_GENERATION_COMPLETE_FLAG}
            WORKING_DIRECTORY ${kaubo_dir}
            COMMENT "Generating ANTLR files using local JAR (only once)"
            VERBATIM
    )
else ()
    # Linux/Mac 使用直接命令
    add_custom_command(
            OUTPUT ${ANTLR_GENERATION_COMPLETE_FLAG}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${kaubo_antlr4_generated_dir}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${kaubo_antlr4_generated_dir}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${kaubo_antlr4_grammar_dir} ${kaubo_antlr4_generated_dir}
            COMMAND java -jar ${kaubo_dir}/thiry_party/antlr-4.13.2-complete.jar
            -Dlanguage=Cpp
            -visitor
            -o ${kaubo_antlr4_generated_dir}
            ${kaubo_antlr4_grammar_dir}/Python3Lexer.g4
            ${kaubo_antlr4_grammar_dir}/Python3Parser.g4
            COMMAND ${CMAKE_COMMAND} -E touch ${ANTLR_GENERATION_COMPLETE_FLAG}
            WORKING_DIRECTORY ${kaubo_dir}
            COMMENT "Generating ANTLR files using local JAR (only once)"
            VERBATIM
    )
endif ()

add_custom_target(antlr4_generate ALL DEPENDS ${ANTLR_GENERATION_COMPLETE_FLAG})
file(GLOB_RECURSE kaubo_antlr4_generated_files ${kaubo_antlr4_generated_dir}/*.cpp ${kaubo_antlr4_generated_dir}/*.h)
foreach(file ${kaubo_antlr4_generated_files})
    # 匹配 .cpp 或 .h 文件
    if (${file} MATCHES ".*\\.(cpp|h)$")
        set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS
            "-w"  # 完全关闭所有警告（GCC/Clang）
        )
    endif()
endforeach()