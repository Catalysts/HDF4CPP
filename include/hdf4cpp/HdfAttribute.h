//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFATTRIBUTE_H
#define HDF4CPP_HDFATTRIBUTE_H

#include <hdf/hdf.h>
#include <hdf/mfhdf.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace hdf4cpp {

class HdfAttributeBase {
public:
    HdfAttributeBase(int32 id, int32 index) : id(id), index(index) {}
    virtual ~HdfAttributeBase() {}
    virtual bool isValid() const { return index != FAIL; }
    virtual Type getType() const = 0;

    virtual intn size() const = 0;

    template <class T> bool get(std::vector<T> &dest) {
        intn length = size();
        if(length != FAIL) {
            auto it = typeSizeMap.find(getDataType());
            if(it != typeSizeMap.end()) {
                if(it->second != sizeof(T)) {
                    throw std::runtime_error("HDF4CPP: type size missmatch");
                }
            } else {
                throw std::runtime_error("HDF4CPP: hdf data set type not supported");
            }
            dest.resize(length);
            return get(dest.data());
        } else {
            return false;
        }
    }

protected:
    int32 id;
    int32 index;

    virtual bool get(void *dest) = 0;
    virtual int32 getDataType() const = 0;
};

class HdfDatasetAttribute : public HdfAttributeBase {
public:
    HdfDatasetAttribute(int32 id, const std::string& name);
    Type getType() const;

    intn size() const;

private:
    intn _size;
    int32 dataType;

    bool get(void *dest);
    int32 getDataType() const;
};

class HdfGroupAttribute : public HdfAttributeBase {
public:
    HdfGroupAttribute(int32 id, const std::string& name);
    Type getType() const;

    intn size() const;

private:
    intn _size;
    int32 dataType;

    bool get(void *dest);
    int32 getDataType() const;
};

class HdfAttribute {
public:
    HdfAttribute(HdfAttributeBase *attribute) : attribute(attribute) {}
    HdfAttribute(const HdfAttribute&) = delete;
    HdfAttribute(HdfAttribute&& attribute);
    bool isValid() const;
    Type getType() const;

    intn size() const;
    template <class T> bool get(std::vector<T> &dest) {
        if(isValid()) {
            return attribute->get(dest);
        } else {
            return false;
        }
    }

private:
    std::unique_ptr<HdfAttributeBase> attribute;
};

}

#endif //HDF4CPP_HDFATTRIBUTE_H
