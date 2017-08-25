//
// Created by patrik on 10.08.17.
//

#ifndef HDF4CPP_HDFFILE_H
#define HDF4CPP_HDFFILE_H

#include <string>
#include <vector>

#include <hdf4cpp/HdfObject.h>
#include <hdf4cpp/HdfItem.h>
#include <hdf4cpp/HdfAttribute.h>
#include <hdf4cpp/HdfDestroyer.h>

namespace hdf4cpp {

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

    HdfItem get(const std::string& name) const;
    std::vector<HdfItem> getAll(const std::string& name) const;

    HdfAttribute getAttribute(const std::string& name);

    class Iterator;

    Iterator begin() const;
    Iterator end() const;

  private:

    int32 getDatasetId(const std::string& name) const;
    int32 getGroupId(const std::string& name) const;
    int32 getDataId(const std::string& name) const;

    std::vector<int32> getDatasetIds(const std::string& name) const;
    std::vector<int32> getGroupIds(const std::string& name) const;
    std::vector<int32> getDataIds(const std::string& name) const;

    int32 sId;
    int32 vId;

    std::vector<std::pair<int32, Type> > loneRefs;
};

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

    HdfItem operator*() {
        if(index < 0 || index >= (int) file->loneRefs.size()) {
            raiseException(OUT_OF_RANGE);
        }
        int32 ref = file->loneRefs[index].first;
        switch(file->loneRefs[index].second) {
            case VGROUP: {
                int32 id = Vattach(file->vId, ref, "r");
                return HdfItem(new HdfGroupItem(id, chain), file->sId, file->vId);
            }
            case VDATA: {
                int32 id = VSattach(file->vId, ref, "r");
                return HdfItem(new HdfDataItem(id, chain), file->sId, file->vId);
            }
            default: {
                raiseException(OUT_OF_RANGE);
            }
        }
    }
private:
    const HdfFile *file;
    int32 index;
};

}

#endif //HDF4CPP_HDFFILE_H
