/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH

#ifndef HDF4CPP_HDFFILE_H
#define HDF4CPP_HDFFILE_H

#include <string>
#include <vector>

#include <hdf4cpp/HdfObject.h>

namespace hdf4cpp {

class HdfItem;
class HdfAttribute;

/// Opens an hdf file and provides operations with it
class HdfFile : public HdfObject {
  public:
    HdfFile(const std::string& path);
    HdfFile(const HdfFile& file) = delete;
    HdfFile(HdfFile&& file);
    HdfFile& operator=(const HdfFile& file) = delete;
    HdfFile& operator=(HdfFile&& file);
    ~HdfFile();

    int32 getSId() const;
    int32 getVId() const;

    /// \returns an item from the file with the given name
    /// \param name the name of the item
    /// \note: If there are multiple items with the same name then the first will be returned
    HdfItem get(const std::string& name) const;

    /// \returns all the items from the file with the given name
    /// \param name the name of the item(s)
    std::vector<HdfItem> getAll(const std::string& name) const;

    /// \returns the attribute with the given name
    /// \param name the name of the attribute
    HdfAttribute getAttribute(const std::string& name) const;

    class Iterator;

    Iterator begin() const;
    Iterator end() const;

  private:

    int32 getDatasetId(const std::string& name) const;
    int32 getGroupId(const std::string& name) const;
    int32 getDataId(const std::string& name) const;

    std::vector<int32> getDatasetIds(const std::string& name) const;
    std::vector<int32> getGroupDataIds(const std::string& name) const;

    int32 sId;
    int32 vId;

    std::vector<std::pair<int32, Type> > loneRefs;
};

/// HdfFile iterator, gives the possibility to iterate over the items in the file
class HdfFile::Iterator : public HdfObject, public std::iterator<std::bidirectional_iterator_tag, HdfItem> {
public:
    Iterator(const HdfFile* file, int32 index, const HdfDestroyerChain& chain) : HdfObject(HFILE, ITERATOR, chain),
                                                                                 file(file),
                                                                                 index(index) {}

    bool operator!=(const Iterator& it) { return index != it.index; }
    bool operator==(const Iterator& it) { return index == it.index; }

    Iterator& operator++() {
        ++index;
        return *this;
    }
    Iterator operator++(int) {
        Iterator it(*this);
        ++index;
        return it;
    }
    Iterator& operator--() {
        --index;
        return *this;
    }
    Iterator operator--(int) {
        Iterator it(*this);
        --index;
        return it;
    }

    HdfItem operator*();
private:
    const HdfFile *file;
    int32 index;
};

}

#endif //HDF4CPP_HDFFILE_H
