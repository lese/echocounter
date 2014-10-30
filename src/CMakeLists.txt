find_package(Libevent2 REQUIRED)

include_directories(${LIBEVENT2_INCLUDE_DIR})
include_directories(.)

add_definitions("-Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(lib_srcs "test.c")

add_library(echocounter_lib ${lib_srcs})

add_executable(echocounter "main.c")
target_link_libraries(echocounter ${LIBEVENT2_LIBRARIES} echocounter_lib)
