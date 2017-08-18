//
// Created by patrik on 10.08.17.
//

#ifndef GRASP_SEGMENTER_HDFFILE_H
#define GRASP_SEGMENTER_HDFFILE_H

#include <string>
#include <vector>

#include <hdf4cpp/HdfItem.h>
#include <hdf4cpp/HdfAttribute.h>



class HdfFile {
  public:
    HdfFile(const std::string& path);
    ~HdfFile();
    bool isValid();

    HdfItem get(const std::string& name);
    std::vector<HdfItem> getAll(const std::string& name);

    HdfAttribute getAttribute(const std::string& name);

    class Iterator;

    Iterator begin() const;
    Iterator end() const;

  private:
    int32 getDatasetId(const std::string& name);
    int32 getGroupId(const std::string& name);

    std::vector<int32> getDatasetIds(const std::string& name);
    std::vector<int32> getGroupIds(const std::string& name);

    int32 sId;
    int32 vId;

    int32 loneSize;
    std::vector<int32> loneRefs;
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
        if(index < 0 || index >= file->loneSize) {
            throw std::runtime_error("HDF4CPP: cannot access invalid item");
        }
        int32 ref = file->loneRefs[index];
        int32 id = Vattach(file->vId, file->loneRefs[index], "r");
        return HdfItem(new HdfGroupItem(id), file->sId, file->vId);
    }
private:
    const HdfFile *file;
    int32 index;
};

#endif //GRASP_SEGMENTER_HDFFILE_H
