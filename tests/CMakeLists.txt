enable_testing()

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(hdf4cpp-tests
        HdfFileTest.cpp)

target_include_directories(hdf4cpp-tests
        PRIVATE
        ${GTEST_INCLUDE_DIRS}
        )
target_link_libraries(hdf4cpp-tests
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        hdf4cpp
        )
target_compile_definitions(hdf4cpp-tests PRIVATE
        GTEST_DONT_DEFINE_FAIL
        GTEST_DONT_DEFINE_SUCCEED)


if (NOT DEFINED TEST_DATA_PATH)
    set(TEST_DATA_PATH "${PROJECT_SOURCE_DIR}/tests/test_data/")
    message(STATUS "No path to test data defined, using ${TEST_DATA_PATH} as default")
endif ()
target_compile_definitions(hdf4cpp-tests PRIVATE
        "TEST_DATA_PATH=\"${TEST_DATA_PATH}\"")

add_test(
        NAME hdf4cpp
        COMMAND hdf4cpp-tests --gtest_output=xml:${PROJECT_BINARY_DIR}/test_details.xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
