set(test_name "TEST_STRING")

add_executable(
        ${test_name}
        ${kaubo_dir}/test/unittest/Collections/String.cpp
)
set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "")
# gtest
target_link_libraries(${test_name} kaubo_common gtest gtest_main)
add_test(NAME ${test_name} COMMAND ${test_name})
