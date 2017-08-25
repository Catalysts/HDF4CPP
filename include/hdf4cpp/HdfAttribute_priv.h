//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFATTRIBUTE_PRIV_H
#define HDF4CPP_HDFATTRIBUTE_PRIV_H
#include "HdfAttribute.h"
#include <hdf4cpp/HdfException.h>
#include <hdf4cpp/HdfObject.h>
#include <hdf4cpp/HdfDestroyer.h>

#include <hdf/hdf.h>
#include <hdf/mfhdf.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace hdf4cpp {

class HdfAttributeBase : public HdfObject {
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

    virtual intn size() const = 0;

    virtual int32 getDataType() const = 0;


    virtual void get(void* dest) = 0;
  protected:
    int32 id;

    int32 index;
};

class HdfDatasetAttribute : public HdfAttributeBase {
public:
    HdfDatasetAttribute(int32 id, const std::string& name, const HdfDestroyerChain& chain);

    intn size() const;

private:
    intn _size;
    int32 dataType;

    void get(void *dest);
    int32 getDataType() const;
};

class HdfGroupAttribute : public HdfAttributeBase {
public:
    HdfGroupAttribute(int32 id, const std::string& name, const HdfDestroyerChain& chain);

    intn size() const;

private:
    intn _size;
    int32 dataType;

    void get(void *dest);
    int32 getDataType() const;
};

class HdfDataAttribute : public HdfAttributeBase {
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
