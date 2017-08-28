//
// Created by patrik on 24.08.17.
//
#ifndef HDF4CPP_HDFATTRIBUTE_H
#define HDF4CPP_HDFATTRIBUTE_H

#include <hdf/hdfi.h>
#include <hdf4cpp/HdfObject.h>
#include <hdf4cpp/HdfItem.h>

namespace hdf4cpp {

/// Represents an hdf attribute
class HdfAttribute : public HdfObject {
  public:
    HdfAttribute(const HdfAttribute&) = delete;
    HdfAttribute(HdfAttribute&& attr);
    HdfAttribute& operator=(const HdfAttribute& attribute) = delete;
    HdfAttribute& operator=(HdfAttribute&& attribute);
    /// \returns the number of elements of the attribute data
    intn size() const;

    /// Reads the data from the attribute
    /// \param dest the vector in which the data will be stored
    template <class T> void get(std::vector<T> &dest) {
        intn length = size();
        auto it = typeSizeMap.find(getDataType());
        if (it != typeSizeMap.end()) {
            if ((size_t)it->second != sizeof(T)) {
                raiseException(BUFFER_SIZE_NOT_ENOUGH);
            }
            dest.resize(length);
            getInternal(dest.data());
        } else {
            raiseException(INVALID_DATA_TYPE);
        }
    }

    friend HdfAttribute HdfFile::getAttribute(const std::string &name) const;
    friend HdfAttribute HdfItem::HdfDatasetItem::getAttribute(const std::string &name) const;
    friend HdfAttribute HdfItem::HdfGroupItem::getAttribute(const std::string &name) const;
    friend HdfAttribute HdfItem::HdfDataItem::getAttribute(const std::string &name) const;

  private:
    class HdfAttributeBase;
    class HdfDatasetAttribute;
    class HdfGroupAttribute;
    class HdfDataAttribute;

    HdfAttribute(HdfAttributeBase *attribute);

    /// An internal get function
    /// \param dest The destination buffer
    void getInternal(void *dest);
    int32 getDataType() const;
    /// Holds an attribute object address
    std::unique_ptr<HdfAttributeBase> attribute;
};
}

#endif //HDF4CPP_HDFATTRIBUTE_H
