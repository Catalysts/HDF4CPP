//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFATTRIBUTE_PRIV_H
#define HDF4CPP_HDFATTRIBUTE_PRIV_H
#include <hdf4cpp/HdfAttribute.h>
#include <hdf4cpp/HdfException.h>
#include <hdf4cpp/HdfObject.h>

#include <hdf/hdf.h>
#include <hdf/mfhdf.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace hdf4cpp {

/// The base class of the attribute classes
class HdfAttribute::HdfAttributeBase : public HdfObject {
  public:
    HdfAttributeBase(int32 id, int32 index, Type type, const HdfDestroyerChain& chain) : HdfObject(type,
                                                                                                   ATTRIBUTE,
                                                                                                   chain),
                                                                                         id(id),
                                                                                         index(index) {
        if (id == FAIL || index == FAIL) {
            raiseException(INVALID_ID);
        }
    }
    virtual ~HdfAttributeBase() {}

    /// \returns The number of existing data in the attribute
    virtual intn size() const = 0;

    /// \returns The data type number of the data held by the attribute
    virtual int32 getDataType() const = 0;

    /// Reads the data from the attribute
    /// \param dest The destination vector
    virtual void get(void* dest) = 0;
  protected:
    int32 id;

    int32 index;
};

/// Attribute class for the SData attributes
class HdfAttribute::HdfDatasetAttribute : public HdfAttributeBase {
public:
    HdfDatasetAttribute(int32 id, const std::string& name, const HdfDestroyerChain& chain);

    intn size() const;

private:
    intn _size;
    int32 dataType;

    void get(void *dest);
    int32 getDataType() const;
};

/// Attribute class for the VGroup attributes
class HdfAttribute::HdfGroupAttribute : public HdfAttributeBase {
public:
    HdfGroupAttribute(int32 id, const std::string& name, const HdfDestroyerChain& chain);

    intn size() const;

private:
    intn _size;
    int32 dataType;

    void get(void *dest);
    int32 getDataType() const;
};

/// Attribute class for the VData attributes
class HdfAttribute::HdfDataAttribute : public HdfAttributeBase {
public:
    HdfDataAttribute(int32 id, const std::string& name, const HdfDestroyerChain& chain);

    intn size() const;

private:
    intn _size;
    int32 dataType;
    void get(void *dest);
    int32 getDataType() const;
};



}

#endif //HDF4CPP_HDFATTRIBUTE_PRIV_H
