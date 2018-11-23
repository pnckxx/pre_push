cmake_minimum_required(VERSION 2.8)

enable_testing()
project(homeworks)
include_directories(vendor/catch2/single_include/catch2)

set(CMAKE_CXX_STANDARD 17)

set(SANITIZER_FLAGS -fsanitize=address,leak,undefined,bounds) # ubsan failed to work with travis' ubuntu16.04
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS} -Wall -Werror")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold ${CMAKE_EXE_LINKER_FLAGS}")


add_subdirectory(src)
