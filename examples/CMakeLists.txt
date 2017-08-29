project(examples)

add_executable(example_sdata
        ReadingSData.cpp
        )

target_sources(example_sdata PRIVATE
        ReadingSData.cpp
        )

target_link_libraries(example_sdata PRIVATE
        hdf4cpp)

target_compile_definitions(example_sdata PRIVATE
        "TEST_DATA_PATH=\"${TEST_DATA_PATH}\"")

add_executable(example_vdata
        ReadingVData.cpp
        )

target_sources(example_vdata PRIVATE
        ReadingVData.cpp)

target_link_libraries(example_vdata PRIVATE
        hdf4cpp
        )

target_compile_definitions(example_vdata PRIVATE
        "TEST_DATA_PATH=\"${TEST_DATA_PATH}\"")