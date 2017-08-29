/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH

#ifndef HDF4CPP_HDFATTRIBUTE_H
#define HDF4CPP_HDFATTRIBUTE_H

#include <hdf/hdfi.h>
#include <hdf4cpp/HdfItem.h>
#include <hdf4cpp/HdfObject.h>

namespace hdf4cpp {

/// Represents an hdf attribute
class HdfAttribute : public HdfObject {
  private:
    /// The base class of the attribute classes
    class HdfAttributeBase : public HdfObject {
      public:
        HdfAttributeBase(int32 id, int32 index, Type type, const HdfDestroyerChain &chain)
            : HdfObject(type, ATTRIBUTE, chain)
            , id(id)
            , index(index) {
            if (id == FAIL || index == FAIL) {
                raiseException(INVALID_ID);
            }
        }
        virtual ~HdfAttributeBase() {
        }

        /// \returns The number of existing data in the attribute
        virtual intn size() const = 0;

        /// \returns The data type number of the data held by the attribute
        virtual int32 getDataType() const = 0;

        /// Reads the data from the attribute
        /// \param dest The destination vector
        virtual void get(void *dest) = 0;

      protected:
        int32 id;

        int32 index;
    };

    /// Attribute class for the SData attributes
    class HdfDatasetAttribute : public HdfAttributeBase {
      public:
        HdfDatasetAttribute(int32 id, const std::string &name, const HdfDestroyerChain &chain);

        intn size() const;

      private:
        intn _size;
        int32 dataType;

        void get(void *dest);
        int32 getDataType() const;
    };

    /// Attribute class for the VGroup attributes
    class HdfGroupAttribute : public HdfAttributeBase {
      public:
        HdfGroupAttribute(int32 id, const std::string &name, const HdfDestroyerChain &chain);

        intn size() const;

      private:
        intn _size;
        int32 dataType;

        void get(void *dest);
        int32 getDataType() const;
    };

    /// Attribute class for the VData attributes
    class HdfDataAttribute : public HdfAttributeBase {
      public:
        HdfDataAttribute(int32 id, const std::string &name, const HdfDestroyerChain &chain);

        intn size() const;

      private:
        intn _size;
        int32 dataType;
        void get(void *dest);
        int32 getDataType() const;
    };

  public:
    HdfAttribute(const HdfAttribute &) = delete;
    HdfAttribute(HdfAttribute &&attr);
    HdfAttribute &operator=(const HdfAttribute &attribute) = delete;
    HdfAttribute &operator=(HdfAttribute &&attribute);
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
    HdfAttribute(HdfAttributeBase *attribute);

    /// An internal get function
    /// \param dest The destination buffer
    void getInternal(void *dest);
    int32 getDataType() const;
    /// Holds an attribute object address
    std::unique_ptr<HdfAttributeBase> attribute;
};
}

#endif // HDF4CPP_HDFATTRIBUTE_H
