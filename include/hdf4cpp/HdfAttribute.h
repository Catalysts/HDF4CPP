//
// Created by patrik on 24.08.17.
//
#ifndef HDF4CPP_HDFATTRIBUTE_H
#define HDF4CPP_HDFATTRIBUTE_H

#include "HdfObject.h"

namespace hdf4cpp {

class HdfAttributeBase;

class HdfAttribute : public HdfObject {
  public:
    HdfAttribute(HdfAttributeBase *attribute);
    HdfAttribute(const HdfAttribute&) = delete;
    HdfAttribute(HdfAttribute&& attr);
    HdfAttribute& operator=(const HdfAttribute& attribute) = delete;
    HdfAttribute& operator=(HdfAttribute&& attribute);

    Type getType() const;

    intn size() const;
    template <class T> void get(std::vector<T> &dest) {
        intn length = size();
        auto it = typeSizeMap.find(getDataType());
        if (it != typeSizeMap.end()) {
            if ((size_t)it->second != sizeof(T)) {
                raiseException(BUFFER_SIZE_NOT_ENOUGH);
            }
            dest.resize(length);
            get_internal(dest.data());
        } else {
            raiseException(INVALID_DATA_TYPE);
        }
    }

  private:
    void get_internal(void *dest);
    int32 getDataType() const;
    std::unique_ptr<HdfAttributeBase> attribute;
};
}

#endif //HDF4CPP_HDFATTRIBUTE_H
