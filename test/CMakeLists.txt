find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
include_directories(. ../src/)

add_executable(test_copy_hello test_copy_hello.c)
target_link_libraries(test_copy_hello ${CHECK_LIBRARIES} echocounter_lib)
add_test(test_copy_hello ${CMAKE_CURRENT_BINARY_DIR}/test_copy_hello)
