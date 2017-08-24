//
// Created by patrik on 23.08.17.
//

#ifndef GRASP_SEGMENTER_HDFOBJECT_H
#define GRASP_SEGMENTER_HDFOBJECT_H

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfException.h>
#include <hdf4cpp/HdfDestroyer.h>

namespace hdf4cpp {

class HdfObject {
  public:
    virtual Type getType() const { return type; }
    virtual ClassType getClassType() const { return classType; }

  protected:
    HdfObject(const Type& type, const ClassType& classType, const HdfDestroyerChain& chain = HdfDestroyerChain()) : type(type),
                                                                                                                    classType(classType),
                                                                                                                    chain(chain) {}
    HdfObject(const HdfObject *object) : type(object->getType()), classType(object->getClassType()), chain(object->chain) {}

    virtual void setType(const Type& type) { this->type = type; }
    virtual void setClassType(const ClassType& classType) { this->classType = classType; }

    virtual void raiseException(const ExceptionType& exceptionType) const {
        throw HdfException(type, classType, exceptionType);
    }

    virtual void raiseException(const std::string& message) const {
        throw HdfException(type, classType, message);
    }

  private:
    Type type;
    ClassType classType;

  protected:
    HdfDestroyerChain chain;
};
}



#endif //GRASP_SEGMENTER_HDFOBJECT_H
