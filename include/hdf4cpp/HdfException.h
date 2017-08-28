/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH

#ifndef HDF4CPP_HDFEXCEPTION_H
#define HDF4CPP_HDFEXCEPTION_H

#include <hdf4cpp/HdfDefines.h>

namespace hdf4cpp {

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
    /// Get the Type of the object which threw the exception.
    Type getType() const noexcept;
    /// Get the ClassType of the object which threw the exception.
    ClassType getClassType() const noexcept;
    /// Get the ExceptionType of the object which threw the exception.
    ExceptionType getExceptionType() const noexcept;
    /// Get the message of the exception.
    std::string getMessage() const noexcept;
    /// Get the exception message.
    /// Returns the same string (but in c string type) which is returned by the getMessage() function.
    const char *what() const noexcept {
        return message.data();
    }

  private:

    /// Exception message prefix
    static const std::string exceptionMessagePrefix;
    /// Associates the exception type with a specific message
    /// \note All the exception types must have a message associated with
    static const std::map<ExceptionType, std::string> exceptionTypeMap;

    Type type;
    ClassType classType;
    ExceptionType exceptionType;
    std::string message;
};


}


#endif //HDF4CPP_HDFEXCEPTION_H
