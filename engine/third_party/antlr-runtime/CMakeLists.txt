option(ANTLR_BUILD_CPP_TESTS "Build C++ tests." OFF)
option(TRACE_ATN "Trace ATN simulation" OFF)
option(ANTLR_BUILD_SHARED "Build the shared library of the ANTLR runtime" OFF)
option(ANTLR_BUILD_STATIC "Build the static library of the ANTLR runtime" ON)

if (NOT ANTLR_BUILD_SHARED AND NOT ANTLR_BUILD_STATIC)
    message(FATAL_ERROR "Options ANTLR_BUILD_SHARED and ANTLR_BUILD_STATIC can't both be OFF")
endif ()
set(libantlrcpp_INCLUDE_INSTALL_DIR "include/antlr4-runtime")
set(libantlrcpp_INCLUDE_DIRS
        ${ANTLR_SOURCE_DIR}
        ${ANTLR_SOURCE_DIR}/atn
        ${ANTLR_SOURCE_DIR}/dfa
        ${ANTLR_SOURCE_DIR}/internal
        ${ANTLR_SOURCE_DIR}/misc
        ${ANTLR_SOURCE_DIR}/support
        ${ANTLR_SOURCE_DIR}/tree
        ${ANTLR_SOURCE_DIR}/tree/pattern
        ${ANTLR_SOURCE_DIR}/tree/xpath
)


file(GLOB libantlrcpp_SRC
        "${ANTLR_SOURCE_DIR}/*.cpp"
        "${ANTLR_SOURCE_DIR}/atn/*.cpp"
        "${ANTLR_SOURCE_DIR}/dfa/*.cpp"
        "${ANTLR_SOURCE_DIR}/internal/*.cpp"
        "${ANTLR_SOURCE_DIR}/misc/*.cpp"
        "${ANTLR_SOURCE_DIR}/support/*.cpp"
        "${ANTLR_SOURCE_DIR}/tree/*.cpp"
        "${ANTLR_SOURCE_DIR}/tree/pattern/*.cpp"
        "${ANTLR_SOURCE_DIR}/tree/xpath/*.cpp"
)

if (ANTLR_BUILD_SHARED)
    add_library(antlr4_shared SHARED ${libantlrcpp_SRC})
    target_include_directories(antlr4_shared PUBLIC
            "$<BUILD_INTERFACE:${libantlrcpp_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${libantlrcpp_INCLUDE_INSTALL_DIR}>")
endif ()
if (ANTLR_BUILD_STATIC)
    add_library(antlr4_static STATIC ${libantlrcpp_SRC})
    target_include_directories(antlr4_static PUBLIC
            "$<BUILD_INTERFACE:${libantlrcpp_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${libantlrcpp_INCLUDE_INSTALL_DIR}>")
endif ()

if (CMAKE_HOST_UNIX)
    # Make sure to link against threads (pthreads) library in order to be able to
    # make use of std::call_once in the code without producing runtime errors
    # (see also https://github.com/antlr/antlr4/issues/3708 and/or https://stackoverflow.com/q/51584960).
    find_package(Threads REQUIRED)

    if (TARGET antlr4_shared)
        target_link_libraries(antlr4_shared Threads::Threads)
    endif ()
    if (TARGET antlr4_static)
        target_link_libraries(antlr4_static Threads::Threads)
    endif ()
endif ()


if (APPLE)
    if (TARGET antlr4_shared)
        target_link_libraries(antlr4_shared ${COREFOUNDATION_LIBRARY})
    endif ()
    if (TARGET antlr4_static)
        target_link_libraries(antlr4_static ${COREFOUNDATION_LIBRARY})
    endif ()
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(disabled_compile_warnings "/wd4251")
else ()
    set(disabled_compile_warnings "-Wno-overloaded-virtual")
endif ()


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-dollar-in-identifier-extension -Wno-four-char-constants")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(disabled_compile_warnings "${disabled_compile_warnings} -Wno-multichar")
endif ()

set(extra_share_compile_flags "")
set(extra_static_compile_flags "")
set(static_lib_suffix "")

if (WIN32)
    set(static_lib_suffix "-static")
    if (TARGET antlr4_shared)
        target_compile_definitions(antlr4_shared PUBLIC ANTLR4CPP_EXPORTS)
    endif ()
    if (TARGET antlr4_static)
        target_compile_definitions(antlr4_static PUBLIC ANTLR4CPP_STATIC)
    endif ()
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(extra_share_compile_flags "-MP /wd4251")
        set(extra_static_compile_flags "-MP")
    endif ()
endif ()

if (TARGET antlr4_shared)
    set_target_properties(antlr4_shared
            PROPERTIES VERSION ${ANTLR_VERSION}
            SOVERSION ${ANTLR_VERSION}
            OUTPUT_NAME antlr4-runtime
            COMPILE_FLAGS "${disabled_compile_warnings} ${extra_share_compile_flags}")
endif ()

if (TARGET antlr4_static)
    set_target_properties(antlr4_static
            PROPERTIES VERSION ${ANTLR_VERSION}
            SOVERSION ${ANTLR_VERSION}
            OUTPUT_NAME "antlr4-runtime${static_lib_suffix}"
            COMPILE_PDB_NAME "antlr4-runtime${static_lib_suffix}"
            COMPILE_FLAGS "${disabled_compile_warnings} ${extra_static_compile_flags}")
endif ()

if (ANTLR_BUILD_CPP_TESTS)
    # Copy the generated binaries to dist folder (required by test suite)
    if (TARGET antlr4_shared)
        add_custom_command(
                TARGET antlr4_shared
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_HOME_DIRECTORY}/dist
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:antlr4_shared> ${CMAKE_HOME_DIRECTORY}/dist
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_LINKER_FILE:antlr4_shared> ${CMAKE_HOME_DIRECTORY}/dist)
    endif ()

    if (TARGET antlr4_static)
        add_custom_command(
                TARGET antlr4_static
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_HOME_DIRECTORY}/dist
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:antlr4_static> ${CMAKE_HOME_DIRECTORY}/dist)
    endif ()
endif ()

if (TARGET antlr4_shared)
    install(TARGETS antlr4_shared
            EXPORT antlr4-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (TARGET antlr4_static)
    install(TARGETS antlr4_static
            EXPORT antlr4-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

install(DIRECTORY "${ANTLR_SOURCE_DIR}/"
        DESTINATION "${libantlrcpp_INCLUDE_INSTALL_DIR}"
        COMPONENT dev
        FILES_MATCHING PATTERN "*.h"
)
