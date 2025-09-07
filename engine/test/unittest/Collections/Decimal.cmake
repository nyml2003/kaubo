set(test_name "TEST_DECIMAL")

add_executable(
        ${test_name}
        ${kaubo_dir}/test/unittest/Collections/Decimal.cpp
)

# gtest
set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "")
target_link_libraries(${test_name} gtest gtest_main kaubo_common)
add_test(NAME ${test_name} COMMAND ${test_name})
