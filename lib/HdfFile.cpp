/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH


#include <mfhdf.h>
#include <stdexcept>

#include <hdf4cpp/HdfAttribute.h>
#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfException.h>
#include <hdf4cpp/HdfFile.h>
#include <hdf4cpp/HdfItem.h>


hdf4cpp::HdfFile::HdfFile(const std::string &path)
    : HdfObject(HFILE, FILE) {
    sId = SDstart(path.c_str(), DFACC_READ);
    vId = Hopen(path.c_str(), DFACC_READ, 0);

    if (sId == FAIL || vId == FAIL) {
        raiseException(INVALID_ID);
    }

    Vinitialize(vId);

    chain.pushBack(new HdfDestroyer(&SDend, sId));
    chain.pushBack(new HdfDestroyer(&Vfinish, vId));
    chain.pushBack(new HdfDestroyer(&Hclose, vId));

    int32 loneSize = Vlone(vId, nullptr, 0);
    std::vector<int32> refs((size_t)loneSize);
    Vlone(vId, refs.data(), loneSize);
    for (const auto &ref : refs) {
        loneRefs.emplace_back(ref, VGROUP);
    }

    int32 loneVdata = VSlone(vId, nullptr, 0);
    refs.resize((size_t)loneVdata);
    VSlone(vId, refs.data(), loneVdata);
    for (const auto &ref : refs) {
        loneRefs.emplace_back(ref, VDATA);
    }
}
hdf4cpp::HdfFile::HdfFile(HdfFile &&file)
    : HdfObject(file.getType(), file.getClassType(), std::move(file.chain)) {
    sId = file.sId;
    vId = file.vId;
    loneRefs = std::move(file.loneRefs);
    file.sId = file.vId = FAIL;
}
hdf4cpp::HdfFile &hdf4cpp::HdfFile::operator=(HdfFile &&file) {
    setType(file.getType());
    setClassType(file.getClassType());
    chain = std::move(file.chain);
    sId = file.sId;
    vId = file.vId;
    loneRefs = std::move(file.loneRefs);
    file.sId = file.vId = FAIL;
    return *this;
}
hdf4cpp::HdfFile::~HdfFile() = default;
int32 hdf4cpp::HdfFile::getSId() const {
    return sId;
}
int32 hdf4cpp::HdfFile::getVId() const {
    return vId;
}
int32 hdf4cpp::HdfFile::getDatasetId(const std::string &name) const {
    int32 index = SDnametoindex(sId, name.c_str());
    return (index == FAIL) ? (FAIL) : (SDselect(sId, index));
}
int32 hdf4cpp::HdfFile::getGroupId(const std::string &name) const {
    int32 ref = Vfind(vId, name.c_str());
    return (ref == 0) ? (FAIL) : (Vattach(vId, ref, "r"));
}
int32 hdf4cpp::HdfFile::getDataId(const std::string &name) const {
    int32 ref = VSfind(vId, name.c_str());
    return (ref == 0) ? (FAIL) : (VSattach(vId, ref, "r"));
}
hdf4cpp::HdfItem hdf4cpp::HdfFile::get(const std::string &name) const {
    int32 id = getDatasetId(name);
    if (id != FAIL) {
        return HdfItem(new HdfItem::HdfDatasetItem(id, chain), sId, vId);
    }
    id = getGroupId(name);
    if (id != FAIL) {
        return HdfItem(new HdfItem::HdfGroupItem(id, chain), sId, vId);
    }
    id = getDataId(name);
    if (id != FAIL) {
        return HdfItem(new HdfItem::HdfDataItem(id, chain), sId, vId);
    }
    raiseException(INVALID_ID);
}
std::vector<int32> hdf4cpp::HdfFile::getDatasetIds(const std::string &name) const {
    std::vector<int32> ids;
    char nameDataset[MAX_NAME_LENGTH];
    int32 datasets, waste;
    SDfileinfo(sId, &datasets, &waste);
    for (int32 i = 0; i < datasets; ++i) {
        int32 id = SDselect(sId, i);
        SDgetinfo(id, nameDataset, nullptr, nullptr, nullptr, nullptr);
        if (id != FAIL && name == std::string(nameDataset)) {
            ids.push_back(id);
        } else {
            SDendaccess(id);
        }
    }
    return ids;
}
std::vector<int32> hdf4cpp::HdfFile::getGroupDataIds(const std::string &name) const {
    std::vector<int32> ids;
    char nameGroup[MAX_NAME_LENGTH];
    int32 ref = Vgetid(vId, -1);
    while (ref != FAIL) {
        int32 id = Vattach(vId, ref, "r");
        Vgetname(id, nameGroup);
        if (name == std::string(nameGroup)) {
            ids.push_back(id);
        } else {
            Vdetach(id);
        }
        ref = Vgetid(vId, ref);
    }
    return ids;
}
std::vector<hdf4cpp::HdfItem> hdf4cpp::HdfFile::getAll(const std::string &name) const {
    std::vector<HdfItem> items;
    std::vector<int32> ids;
    ids = getDatasetIds(name);
    for (const auto &id : ids) {
        items.push_back(HdfItem(new HdfItem::HdfDatasetItem(id, chain), sId, vId));
    }
    ids = getGroupDataIds(name);
    for (const auto &id : ids) {
        items.push_back(HdfItem(new HdfItem::HdfGroupItem(id, chain), sId, vId));
    }
    return items;
}
hdf4cpp::HdfAttribute hdf4cpp::HdfFile::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfAttribute::HdfDatasetAttribute(sId, name, chain));
}
hdf4cpp::HdfFile::Iterator hdf4cpp::HdfFile::begin() const {
    return Iterator(this, 0, chain);
}
hdf4cpp::HdfFile::Iterator hdf4cpp::HdfFile::end() const {
    return Iterator(this, (int32)loneRefs.size(), chain);
}
hdf4cpp::HdfItem hdf4cpp::HdfFile::Iterator::operator*() {
    if (index < 0 || index >= (int)file->loneRefs.size()) {
        raiseException(OUT_OF_RANGE);
    }
    int32 ref = file->loneRefs[index].first;
    switch (file->loneRefs[index].second) {
    case VGROUP: {
        int32 id = Vattach(file->vId, ref, "r");
        return HdfItem(new HdfItem::HdfGroupItem(id, chain), file->sId, file->vId);
    }
    case VDATA: {
        int32 id = VSattach(file->vId, ref, "r");
        return HdfItem(new HdfItem::HdfDataItem(id, chain), file->sId, file->vId);
    }
    default: { raiseException(INVALID_OPERATION); }
    }
}
