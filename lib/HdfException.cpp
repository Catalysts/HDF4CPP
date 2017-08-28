/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH

#include <hdf4cpp/HdfException.h>


const std::string hdf4cpp::HdfException::exceptionMessagePrefix = "HDF4CPP: ";

const std::map<hdf4cpp::ExceptionType, std::string> hdf4cpp::HdfException::exceptionTypeMap = {
        {INVALID_ID, "cannot construct object, the id is invalid"},
        {INVALID_OPERATION, "operation not supported for this type"},
        {INVALID_NAME, "invalid item name"},
        {OUT_OF_RANGE, "out of range, cannot access element (usually thrown when an iterator points to an inaccessible address)"},
        {BUFFER_SIZE_NOT_ENOUGH, "not enough buffer size (usually thrown when the type of the vector in which we read the data is too small)"},
        {BUFFER_SIZE_NOT_DIVISIBLE, "buffer cannot be splited up (usually thrown when the size of the readable data is not divisible by the type size of the vector"},
        {INVALID_RANGES, "the given ranges are invalids (usually thrown when the ranges is negative or it is bigger than the dimension of tha data)"},
        {STATUS_RETURN_FAIL, "hdf routine failed"},
        {INVALID_DATA_TYPE, "the type of the data in the hdf item is not supported"},
        {OTHER, "exception thrown"},
};

hdf4cpp::Type hdf4cpp::HdfException::getType() const noexcept {
    return type;
}

hdf4cpp::ClassType hdf4cpp::HdfException::getClassType() const noexcept {
    return classType;
}

hdf4cpp::ExceptionType hdf4cpp::HdfException::getExceptionType() const noexcept {
    return exceptionType;
}

std::string hdf4cpp::HdfException::getMessage() const noexcept {
    return message;
}