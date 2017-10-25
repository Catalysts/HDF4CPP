cmake_minimum_required(VERSION 2.8)

project(HDF4CPP LANGUAGES CXX)

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

option(HDF4CPP_BUILD_TESTS "Enable building tests" ON)
option(HDF4CPP_BUILD_EXAMPLES "Enable building examples" ON)

if (HDF4CPP_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if (HDF4CPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(TARGETS hdf4cpp DESTINATION lib)
install(FILES ${HEADERS}
        DESTINATION "include/hdf4cpp"
        )
