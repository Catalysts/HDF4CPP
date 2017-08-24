//
// Created by patrik on 22.08.17.
//

#include <hdf4cpp/HdfException.h>

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