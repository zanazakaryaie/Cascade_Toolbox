cmake_minimum_required(VERSION 2.8)

project(CascadeToolbox)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -std=c++14")

option(BUILD_SIMD_INFERENCE "Builds the inference_simd.cpp using Simd library" OFF)
set(SIMD_LIBRARY_PATH "" CACHE PATH "Path to the cloned Simd library")

find_package(OpenCV 2 QUIET)
if(NOT OpenCV_FOUND)
    find_package(OpenCV 3 QUIET)
    if(NOT OpenCV_FOUND)
        message(FATAL_ERROR "OpenCV 2 or 3 not found.")
    endif()
endif()

include_directories(include)
file( GLOB LIB_SOURCES src/*.cpp )

add_library( CascadeToolbox ${LIB_SOURCES})

add_executable(collect_train_data apps/collect_train_data.cpp)
target_link_libraries(collect_train_data ${OpenCV_LIBS} CascadeToolbox)

add_executable(collect_test_data apps/collect_test_data.cpp)
target_link_libraries(collect_test_data ${OpenCV_LIBS} CascadeToolbox)


add_executable(train apps/train.cpp)
target_link_libraries(train ${OpenCV_LIBS} CascadeToolbox)

add_executable(test apps/test.cpp)
target_link_libraries(test ${OpenCV_LIBS} CascadeToolbox)

add_executable(hard_negative_mine apps/hard_negative_mine.cpp)
target_link_libraries(hard_negative_mine ${OpenCV_LIBS} CascadeToolbox)

add_executable(inference apps/inference.cpp)
target_link_libraries(inference ${OpenCV_LIBS} CascadeToolbox)

if (BUILD_SIMD_INFERENCE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    include_directories(${SIMD_LIBRARY_PATH}/src)
    add_executable(inference_simd apps/inference_simd.cpp)
    target_link_libraries(inference_simd ${OpenCV_LIBS} CascadeToolbox ${SIMD_LIBRARY_PATH}/prj/cmake/libSimd.a)
endif()
