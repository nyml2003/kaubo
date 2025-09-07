set(test_name "TEST_eventloop")

add_executable(
        ${test_name}
        ${kaubo_dir}/test/unittest/Object/eventloop.cpp
)
set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "")
# gtest
target_link_libraries(${test_name} gtest gtest_main kaubo_common)
add_test(NAME ${test_name} COMMAND ${test_name})
