# - Find the HDF4 library together with dependencies...
# This module defines the following variables:
#  HDF4_INCLUDE_DIRS - include directories for HDF4
#  HDF4_LIBRARIES - libraries to link against HDF4
#  HDF4_FOUND - true if HDF4 has been found and can be used


find_package(HDF4 CONFIG QUIET)

if (HDF4_FOUND)
    set(HDF4_LIBRARIES hdf4::mfhdf-static hdf4::hdf-static)
    set(HDF4_INCLUDE_DIRS
            $<TARGET_PROPERTY:hdf4::mfhdf-static,INTERFACE_INCLUDE_DIRECTORIES>
            $<TARGET_PROPERTY:hdf4::hdf-static,INTERFACE_INCLUDE_DIRECTORIES>)
else ()
    find_path(HDF4_INCLUDE_DIRS hdf.h PATH_SUFFIXES hdf)
    find_library(MFHDF_LIB NAMES mfhdf mfhdfalt)
    find_library(HDF_LIB NAMES df dfalt hdf)

    # dependencies for the static library version of hdf4
    find_package(JPEG)
    find_package(ZLIB)

    if (NOT (MFHDF_LIB AND HDF_LIB AND JPEG_LIBRARIES AND ZLIB_LIBRARIES))
        set (HDF4_LIBRARIES HDF4_LIBRARIES-NOTFOUND)
    else()
        set(HDF4_LIBRARIES
                ${MFHDF_LIB}
                ${HDF_LIB}
                ${JPEG_LIBRARIES}
                ${ZLIB_LIBRARIES}
                )
    endif ()
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HDF4
        FOUND_VAR HDF4_FOUND
        REQUIRED_VARS HDF4_INCLUDE_DIRS HDF4_LIBRARIES)

if (HDF4_FOUND)
    message(-- " HDF4 include path: ${HDF4_INCLUDE_DIRS}")
    message(-- " HDF4 libs:  ${HDF4_LIBRARIES}")
else ()
    message(WARNING "HDF4 not found")
endif ()

mark_as_advanced(HDF4_INCLUDE_DIR HDF4_LIBRARIES)
