//
// Created by patrik on 10.08.17.
//

#ifndef GRASP_SEGMENTER_HDFFILE_H
#define GRASP_SEGMENTER_HDFFILE_H

#include <string>
#include <vector>

#include <hdf4cpp/HdfItem.h>
#include <hdf4cpp/HdfAttribute.h>

namespace hdf4cpp {

class HdfFile {
  public:
    HdfFile(const std::string& path);
    HdfFile(const HdfFile& file) = delete;
    HdfFile(HdfFile&& file);
    HdfFile& operator=(const HdfFile& file) = delete;
    HdfFile& operator=(HdfFile&& file);
    ~HdfFile();
    bool isValid() const;

    explicit operator bool() const { return isValid(); }

    HdfItem get(const std::string& name) const;
    std::vector<HdfItem> getAll(const std::string& name) const;

    HdfAttribute getAttribute(const std::string& name);

    class Iterator;

    Iterator begin() const;
    Iterator end() const;

  private:
    void destroy();

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

class HdfFile::Iterator : public std::iterator<std::bidirectional_iterator_tag, HdfItem> {
public:
    Iterator(const HdfFile* file, int32 index) : file(file), index(index) {}

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
            throw std::runtime_error("HDF4CPP: cannot access invalid item");
        }
        int32 ref = file->loneRefs[index].first;
        switch(file->loneRefs[index].second) {
            case VGROUP: {
                int32 id = Vattach(file->vId, ref, "r");
                return HdfItem(new HdfGroupItem(id), file->sId, file->vId);
            }
            case VDATA: {
                int32 id = VSattach(file->vId, ref, "r");
                return HdfItem(new HdfDataItem(id), file->sId, file->vId);
            }
            default: {
                return HdfItem(nullptr, file->sId, file->vId);
            }
        }
    }
private:
    const HdfFile *file;
    int32 index;
};

}

#endif //GRASP_SEGMENTER_HDFFILE_H
