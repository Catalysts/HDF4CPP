//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFATTRIBUTE_H
#define HDF4CPP_HDFATTRIBUTE_H
#include <hdf4cpp/HdfException.h>
#include <hdf4cpp/HdfObject.h>
#include <hdf4cpp/HdfDestroyer.h>
#include <iostream>
#include <hdf/hdf.h>
#include <hdf/mfhdf.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace hdf4cpp {

class HdfAttributeBase : public HdfObject {
public:
    HdfAttributeBase(int32 id, int32 index, Type type, const HdfDestroyerChain& chain) : HdfObject(type, ATTRIBUTE, chain), id(id), index(index) {
        if(id == FAIL || index == FAIL) {
            raiseException(INVALID_ID);
        }
    }
    virtual ~HdfAttributeBase() {}

    virtual intn size() const = 0;

    template <class T> void get(std::vector<T> &dest) {
        intn length = size();
        auto it = typeSizeMap.find(getDataType());
        if (it != typeSizeMap.end()) {
            if ((size_t) it->second != sizeof(T)) {
                raiseException(BUFFER_SIZE_NOT_ENOUGH);
            }
            dest.resize(length);
            get(dest.data());
        } else {
            raiseException(INVALID_DATA_TYPE);
        }
    }

protected:
    int32 id;
    int32 index;

    virtual void get(void *dest) = 0;
    virtual int32 getDataType() const = 0;
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

class HdfAttribute : public HdfObject {
public:
    HdfAttribute(HdfAttributeBase *attribute) : HdfObject(attribute), attribute(attribute) {}
    HdfAttribute(const HdfAttribute&) = delete;
    HdfAttribute(HdfAttribute&& attr);
    HdfAttribute& operator=(const HdfAttribute& attribute) = delete;
    HdfAttribute& operator=(HdfAttribute&& attribute);

    Type getType() const;

    intn size() const;
    template <class T> void get(std::vector<T> &dest) {
        attribute->get(dest);
    }

private:
    std::unique_ptr<HdfAttributeBase> attribute;
};

}

#endif //HDF4CPP_HDFATTRIBUTE_H
