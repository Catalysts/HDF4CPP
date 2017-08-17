# - Find the HDF4 library together with dependencies...
# This module defines the following variables:
#  HDF4_INCLUDE_DIRS - include directories for HDF4
#  HDF4_LIBRARIES - libraries to link against HDF4
#  HDF4_FOUND - true if HDF4 has been found and can be used

find_path(HDF4_INCLUDE_DIR hdf.h PATH_SUFFIXES hdf)
find_library(MFHDF_LIB NAMES mfhdf mfhdfalt)
find_library(DF_LIB NAMES df dfalt)

# dependencies for the static library version of hdf4
find_package(JPEG)
find_package(ZLIB)

set(HDF4_INCLUDE_DIRS ${HDF4_INCLUDE_DIR})
set(HDF4_LIBRARIES
  ${MFHDF_LIB}
  ${DF_LIB}
  ${JPEG_LIBRARIES}
  ${ZLIB_LIBRARIES}
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HDF4
	FOUND_VAR HDF4_FOUND
	REQUIRED_VARS MFHDF_LIB DF_LIB HDF4_INCLUDE_DIR JPEG_LIBRARIES ZLIB_LIBRARIES) 

if(HDF4_FOUND)
	message(-- " HDF4 include path: ${HDF4_INCLUDE_DIRS}")
	message(-- " HDF4 libs:  ${HDF4_LIBRARIES}")
else()
	message(WARNING "HDF4 not found")	
endif()

mark_as_advanced(HDF4_INCLUDE_DIR MFHDF_LIB DF_LIB)
