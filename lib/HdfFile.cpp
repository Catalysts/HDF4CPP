//
// Created by patrik on 10.08.17.
//

#include <hdf/mfhdf.h>
#include <stdexcept>

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfFile.h>



HdfFile::HdfFile(const std::string& path) {
    sId = SDstart(path.c_str(), DFACC_READ);
    vId = Hopen(path.c_str(), DFACC_READ, 0);
    Vstart(vId);

    loneSize = Vlone(vId, nullptr, 0);
    loneRefs.resize((size_t) loneSize);
    Vlone(vId, loneRefs.data(), loneSize);
}
HdfFile::~HdfFile() {
    SDend(sId);
    Vend(vId);
    Hclose(vId);
}
int32 HdfFile::getDatasetId(const std::string &name) {
    int32 index = SDnametoindex(sId, name.c_str());
    return (index == FAIL) ? (FAIL) : (SDselect(sId, index));
}
int32 HdfFile::getGroupId(const std::string &name) {
    int32 ref = Vfind(vId, name.c_str());
    return (!ref) ? (FAIL) : (Vattach(vId, ref, "r"));
}
HdfItem HdfFile::get(const std::string& name) {
    int32 id = getDatasetId(name);
    if(id == FAIL) {
        id = getGroupId(name);
        if(id == FAIL) {
            return HdfItem(nullptr, sId, vId);
        }
        return HdfItem(new HdfGroupItem(id), sId, vId);
    }
    return HdfItem(new HdfDatasetItem(id), sId, vId);
}
std::vector<int32> HdfFile::getDatasetIds(const std::string &name) {
    std::vector<int32> ids;
    char nameDataset[MAX_NAME_LENGTH];
    int32 datasets, attrs;
    SDfileinfo(sId, &datasets, &attrs);
    for(int32 i = 0; i < datasets; ++i) {
        int32 id = SDselect(sId, i);
        int32 rank, dimSize, nt, nAttr;
        SDgetinfo(id, nameDataset, &rank, &dimSize, &nt, &nAttr);
        if(name == std::string(nameDataset)) {
            ids.push_back(id);
        } else {
            SDendaccess(id);
        }
    }
    return ids;
}
std::vector<int32> HdfFile::getGroupIds(const std::string &name) {
    std::vector<int32> ids;
    char nameGroup[MAX_NAME_LENGTH];
    int32 ref = Vgetid(vId, -1);
    while(ref != FAIL) {
        int32 id = Vattach(vId, ref, "r");
        Vgetname(id, nameGroup);
        if(Visvg(id, ref) && name == std::string(nameGroup)) {
            ids.push_back(id);
        } else {
            Vdetach(id);
        }
        ref = Vgetid(vId, ref);
    }
    return ids;
}
std::vector<HdfItem> HdfFile::getAll(const std::string& name) {
    std::vector<HdfItem> items;
    std::vector<int32> ids;
    ids = getDatasetIds(name);
    for(const auto& id : ids) {
        items.push_back(HdfItem(new HdfDatasetItem(id), sId, vId));
    }
    ids = getGroupIds(name);
    for(const auto& id : ids) {
        items.push_back(HdfItem(new HdfGroupItem(id), sId, vId));
    }
    return std::move(items);
}
HdfAttribute HdfFile::getAttribute(const std::string &name) {
    return HdfAttribute(new HdfDatasetAttribute(sId, name));
}
bool HdfFile::isValid() {
    return sId != FAIL || vId != FAIL;
}
HdfFile::Iterator HdfFile::begin() const {
    return Iterator(this, 0);
}
HdfFile::Iterator HdfFile::end() const {
    int32 size;
    size = Vlone(vId, nullptr, 0);
    if(size == FAIL) {
        size = 0;
    }
    return Iterator(this, size);
}
