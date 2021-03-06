/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH


#ifndef HDF4CPP_HDFOBJECT_H
#define HDF4CPP_HDFOBJECT_H

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfException.h>

#include <functional>
#include <memory>
#include <vector>

namespace hdf4cpp {

/// The HdfObject class is the base class of all the HdfObjects.
class HdfObject {
  protected:
    /// This class is responsible to call the end access functions
    class HdfDestroyer {
      public:
        HdfDestroyer(const std::function<int32(int32)> &endFunction, int32 id)
            : endFunction(endFunction)
            , id(id) {
        }
        HdfDestroyer(const HdfDestroyer& other)
            : endFunction(other.endFunction)
            , id(other.id) {
        }
        ~HdfDestroyer() {
            endFunction(id);
        }

      private:
        std::function<int32(int32)> endFunction;
        int32 id;
    };
    /// A chain of destroyers.
    /// If an HdfObject creates a new one, then gives forward their chain.
    /// If all the HdfObjects would be destroyed, which holds a reference of the destroyer,
    /// then the destructor of the destroyer will call the end access function.
    class HdfDestroyerChain {
      public:
        HdfDestroyerChain() = default;
        HdfDestroyerChain(const HdfDestroyerChain &other) noexcept
            : chain(other.chain) {
        }
        HdfDestroyerChain(HdfDestroyerChain &&other) noexcept
            : chain(std::move(other.chain)) {
        }
        HdfDestroyerChain &operator=(const HdfDestroyerChain &other) noexcept {
            chain = other.chain; // copy
            return *this;
        }
        HdfDestroyerChain &operator=(HdfDestroyerChain &&other) noexcept {
            chain = std::move(other.chain);
            return *this;
        }

        void emplaceBack(const std::function<int32(int32)> &endFunction, int32 id) {
            chain.emplace_back(new HdfDestroyer(endFunction, id));
        }

      private:
        std::vector<std::shared_ptr<HdfDestroyer>> chain;
    };

  public:
    /// \returns the type of the object
    virtual Type getType() const noexcept {
        return type;
    }

    /// \returns the class type of the object
    virtual ClassType getClassType() const noexcept {
        return classType;
    }

  protected:
    HdfObject(const Type &type, const ClassType &classType, const HdfDestroyerChain &chain = HdfDestroyerChain())
        : type(type)
        , classType(classType)
        , chain(chain) {
    }
    HdfObject(const Type &type, const ClassType &classType, HdfDestroyerChain &&chain)
        : type(type)
        , classType(classType)
        , chain(std::move(chain)) {
    }
    HdfObject(const HdfObject *object)
        : type(object->getType())
        , classType(object->getClassType())
        , chain(object->chain) {
    }

    virtual void setType(const Type &type) noexcept {
        this->type = type;
    }
    virtual void setClassType(const ClassType &classType) noexcept {
        this->classType = classType;
    }

    /// Throws an HdfException by its type
    NORETURN void raiseException(const ExceptionType &exceptionType) const {
        throw HdfException(type, classType, exceptionType);
    }

    /// Thorws an HdfException by its message, the type will be OTHER.
    NORETURN void raiseException(const std::string &message) const {
        throw HdfException(type, classType, message);
    }

  private:
    /// The type of the object.
    Type type;
    /// The class type of the object.
    ClassType classType;

  protected:
    /// The destroyer chain of the object
    HdfDestroyerChain chain;
};
}

#endif // HDF4CPP_HDFOBJECT_H
