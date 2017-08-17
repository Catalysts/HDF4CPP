cmake_minimum_required(VERSION 2.8)

project(HDF4CPP)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(HDF4)
add_library(hdf4cpp
        lib/HdfFile.cpp
        lib/HdfItem.cpp
        lib/HdfAttribute.cpp)

target_include_directories(hdf4cpp
        PUBLIC
        include/
        ${HDF4_INCLUDE_DIRS}
        )

target_link_libraries(hdf4cpp
        ${HDF4_LIBRARIES}
        )

enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(hdf4cpp-tests
        tests/HdfFileTest.cpp)

target_include_directories(hdf4cpp-tests
        PRIVATE
        ${GTEST_INCLUDE_DIRS}
        ${hdf4cpp_INCLUDE_DIRS}
        )
target_link_libraries(hdf4cpp-tests
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        hdf4cpp
        )

if (NOT DEFINED TEST_DATA_PATH)
    set(TEST_DATA_PATH "${PROJECT_SOURCE_DIR}/tests/test_data/")
    message(STATUS "No path to test data defined, using ${TEST_DATA_PATH} as default")
endif ()
target_compile_definitions(hdf4cpp-tests PRIVATE
        "TEST_DATA_PATH=\"${TEST_DATA_PATH}\"")