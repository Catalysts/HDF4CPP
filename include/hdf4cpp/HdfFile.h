//
// Created by patrik on 10.08.17.
//

#ifndef GRASP_SEGMENTER_HDFFILE_H
#define GRASP_SEGMENTER_HDFFILE_H

#include <string>
#include <vector>

#include <hdf4cpp/HdfItem.h>

class HdfFile {
  public:
    HdfFile(const std::string& path);
    ~HdfFile();
    bool isValid();

    HdfItem get(const std::string& name);
    std::vector<HdfItem> getAll(const std::string& name);

    HdfAttribute getAttribute(const std::string& name);

    class Iterator;

    HdfItem::Iterator begin() const;
    HdfItem::Iterator end() const;

  private:
    int32 getDatasetId(const std::string& name);
    int32 getGroupId(const std::string& name);

    std::vector<int32> getDatasetIds(const std::string& name);
    std::vector<int32> getGroupIds(const std::string& name);

    int32 sId;
    int32 vId;
};

#endif //GRASP_SEGMENTER_HDFFILE_H
