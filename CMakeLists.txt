cmake_minimum_required(VERSION 2.8)

project(homeworks)
include_directories(vendor/catch2/single_include/catch2)
set(CMAKE_CXX_STANDARD 17)


add_library(catch_main STATIC EXCLUDE_FROM_ALL main.cpp)


file(GLOB SRC_HEADERS *.h)

function(add_unittest target)
    add_executable(${target} EXCLUDE_FROM_ALL ${ARGN} ${SRC_HEADERS})
    target_link_libraries(${target} catch_main)
    set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)
    add_test(NAME ${target} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()


add_definitions(-DNODEBUG)
add_unittest(palindrome_test palindrome.cpp)
add_unittest(feature_trait_test feature_trait.cpp)