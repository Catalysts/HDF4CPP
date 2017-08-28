//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFDEFINES_H
#define HDF4CPP_HDFDEFINES_H

#include <map>
#include <hdf/mfhdf.h>

#define MAX_DIMENSION 32
#define MAX_NAME_LENGTH 1000

namespace hdf4cpp {
/// \enum Type
/// The type of the HdfObject.
/// \var HFILE
/// file type
/// \var SDATA
/// SData type
/// \var VGROUP
/// VGroup type
/// \var VDATA
/// VData type
enum Type {HFILE, SDATA, VGROUP, VDATA};

/// \enum ClassType
/// What kind of object it represents:
/// \var FILE
/// file
/// \var ITEM
/// item
/// \var ATTRIBUTE
/// attribute
/// \var ITERATOR
/// iterator
enum ClassType {FILE, ITEM, ATTRIBUTE, ITERATOR};

/// \enum ExceptionType The type of the HdfException
enum ExceptionType {
    INVALID_ID,
    INVALID_OPERATION,
    INVALID_NAME,
    OUT_OF_RANGE,
    BUFFER_SIZE_NOT_ENOUGH,
    BUFFER_SIZE_NOT_DIVISIBLE,
    INVALID_RANGES,
    STATUS_RETURN_FAIL,
    INVALID_DATA_TYPE,
    OTHER
};

const std::multimap<int32, int32> typeSizeMap = {
        {DFNT_CHAR, SIZE_CHAR},
        {DFNT_CHAR8, SIZE_CHAR8},
        {DFNT_CHAR16, SIZE_CHAR16},
        {DFNT_FLOAT32, SIZE_FLOAT32},
        {DFNT_FLOAT64, SIZE_FLOAT64},
        {DFNT_FLOAT128, SIZE_FLOAT128},
        {DFNT_INT8, SIZE_INT8},
        {DFNT_INT16, SIZE_INT16},
        {DFNT_INT32, SIZE_INT32},
        {DFNT_INT64, SIZE_INT64},
        {DFNT_UINT8, SIZE_UINT8},
        {DFNT_UINT16, SIZE_UINT16},
        {DFNT_UINT32, SIZE_UINT32},
        {DFNT_UINT64, SIZE_UINT64},
        {DFNT_UCHAR, SIZE_UCHAR},
        {DFNT_UCHAR8, SIZE_UCHAR8},
        {DFNT_UCHAR16, SIZE_UCHAR16},
};

}

#endif //HDF4CPP_HDFDEFINES_H
