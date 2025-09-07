# include(FetchContent)
# FetchContent_Declare(
#         googletest
#         GIT_REPOSITORY https://github.com/google/googletest.git
#         GIT_TAG release-1.12.1  # 指定版本
# )
# FetchContent_MakeAvailable(googletest)
# enable_testing()
# include_directories(${kaubo_src_dir})
# include(${kaubo_dir}/test/unittest/Collections/Collections.cmake)
# include(${kaubo_dir}/test/unittest/Object/Object.cmake)