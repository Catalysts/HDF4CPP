cmake_minimum_required(VERSION 2.8)

project(HDF4CPP)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(HDF4 REQUIRED)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(Doxyfile_user.in ${PROJECT_BINARY_DIR}/Doxyfile_user @ONLY IMMEDIATE)

    add_custom_target(docs_user
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile_user
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile_user
            )

    configure_file(Doxyfile_all.in ${PROJECT_BINARY_DIR}/Doxyfile_all @ONLY IMMEDIATE)

    add_custom_target(docs_all
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile_all
            SOURCES ${PROJECT_BINARY_DIR}/Doxyfile_all
            )
else ()
    message(WARNING "Doxygen not found")
endif ()

set(HEADERS
        include/hdf4cpp/hdf.h
        include/hdf4cpp/HdfObject.h
        include/hdf4cpp/HdfAttribute.h
        include/hdf4cpp/HdfException.h
        include/hdf4cpp/HdfFile.h
        include/hdf4cpp/HdfItem.h
        include/hdf4cpp/HdfDefines.h)

add_library(hdf4cpp
        lib/HdfFile.cpp
        lib/HdfItem.cpp
        lib/HdfAttribute.cpp
        lib/HdfException.cpp
        ${HEADERS}
        )

target_include_directories(hdf4cpp
        PUBLIC
        include/
        ${HDF4_INCLUDE_DIRS}
        )

target_link_libraries(hdf4cpp
        ${HDF4_LIBRARIES}
        )

if (MSVC)
    target_compile_definitions(hdf4cpp PUBLIC
            NEEDS_NORETURN
            )
endif ()


enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(hdf4cpp-tests
        tests/HdfFileTest.cpp)

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
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} # we need the serialized test data for the integration test
)

add_subdirectory(examples)

install(TARGETS hdf4cpp DESTINATION lib)
install(FILES ${HEADERS}
        DESTINATION "include/hdf4cpp"
        )