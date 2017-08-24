//
// Created by patrik on 22.08.17.
//

#ifndef HDF4CPP_HDFEXCEPTION_H
#define HDF4CPP_HDFEXCEPTION_H

#include <hdf4cpp/HdfDefines.h>

namespace hdf4cpp {


const std::string exceptionMessagePrefix = "HDF4CPP: ";

const std::map<ExceptionType, std::string> exceptionTypeMap = {
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

class HdfException : public std::exception {
  public:
    HdfException(const Type& type, const ClassType& classType, const std::string& message) : type(type),
                                                                                             classType(classType),
                                                                                             exceptionType(OTHER),
                                                                                             message(exceptionMessagePrefix + message) {}
    HdfException(const Type& type, const ClassType& classType, const ExceptionType& exceptionType) : type(type),
                                                                                                     classType(classType),
                                                                                                     exceptionType(exceptionType),
                                                                                                     message(exceptionMessagePrefix + exceptionTypeMap.at(exceptionType)) {}
    Type getType() const noexcept;
    ClassType getClassType() const noexcept;
    ExceptionType getExceptionType() const noexcept;
    std::string getMessage() const noexcept;

    const char *what() const noexcept {
        return message.data();
    }

  private:
    Type type;
    ClassType classType;
    ExceptionType exceptionType;
    std::string message;
};


}


#endif //HDF4CPP_HDFEXCEPTION_H
