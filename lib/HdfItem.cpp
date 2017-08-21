//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfItem.h>
#include <hdf/mfhdf.h>
#include <sstream>

hdf4cpp::HdfDatasetItem::HdfDatasetItem(int32 id) : HdfItemBase(id) {
    if(id == FAIL) {
        _size = FAIL;
    } else {
        std::vector<int32> dims = getDims();
        _size = 1;
        std::for_each(dims.begin(), dims.end(), [this](const int32 &t) {
            _size *= t;
        });
        int32 dim[MAX_DIMENSION];
        int32 size, nrAtt;
        char _name[MAX_NAME_LENGTH];
        SDgetinfo(id, _name, &size, dim, &dataType, &nrAtt);
        name = std::string(_name);
    }
}
std::vector<int32> hdf4cpp::HdfDatasetItem::getDims() {
    int32 dims[MAX_DIMENSION];
    int32 size, dataType, nrAtt;
    char name[MAX_NAME_LENGTH];
    SDgetinfo(id, name, &size, dims, &dataType, &nrAtt);
    return std::vector<int32>(dims, dims + size);
}
hdf4cpp::HdfAttribute hdf4cpp::HdfDatasetItem::getAttribute(const std::string &name) {
    return HdfAttribute(new HdfDatasetAttribute(id, name));
}
hdf4cpp::Type hdf4cpp::HdfDatasetItem::getType() const {
    return SDATA;
}
std::string hdf4cpp::HdfDatasetItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfDatasetItem::getId() const {
    return id;
}
int32 hdf4cpp::HdfDatasetItem::getDataType() const {
    return dataType;
}
hdf4cpp::HdfDatasetItem::~HdfDatasetItem() {
    if(isValid()) {
        SDendaccess(id);
    }
}
intn hdf4cpp::HdfDatasetItem::size() const {
    return _size;
}
hdf4cpp::HdfGroupItem::HdfGroupItem(int32 id) : HdfItemBase(id) {
    char _name[MAX_NAME_LENGTH];
    Vgetname(id, _name);
    name = std::string(_name);
}
std::vector<int32> hdf4cpp::HdfGroupItem::getDims() {
    throw std::runtime_error("HDF4CPP: getDims not defined for HdfGroupItem");
}
hdf4cpp::HdfAttribute hdf4cpp::HdfGroupItem::getAttribute(const std::string &name) {
    return HdfAttribute(new HdfGroupAttribute(id, name));
}
hdf4cpp::Type hdf4cpp::HdfGroupItem::getType() const {
    return VGROUP;
}
std::string hdf4cpp::HdfGroupItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfGroupItem::getId() const {
    return id;
}
hdf4cpp::HdfGroupItem::~HdfGroupItem() {
    if(isValid()) {
        Vdetach(id);
    }
}
intn hdf4cpp::HdfGroupItem::size() const {
    throw std::runtime_error("HdfFile: read not defined for HdfGroupItem");
}
int32 hdf4cpp::HdfGroupItem::getDataType() const {
    throw std::runtime_error("HDF4CPP: no data type for HdfGroup");
}
hdf4cpp::HdfDataItem::HdfDataItem(int32 id) : HdfItemBase(id) {
    char fieldNameList[MAX_NAME_LENGTH];
    char _name[MAX_NAME_LENGTH];
    VSinquire(id, &nrRecords, &interlace, fieldNameList, &recordSize, _name);
    name = std::string(_name);
    std::istringstream in(fieldNameList);
    std::string token;
    while(getline(in, token, ',')) {
        fieldNames.push_back(token);
    }
}
hdf4cpp::HdfDataItem::~HdfDataItem() {
    if(isValid()) {
        Vdetach(id);
    }
}
hdf4cpp::Type hdf4cpp::HdfDataItem::getType() const {
    return VDATA;
}
int32 hdf4cpp::HdfDataItem::getId() const {
    return id;
}
std::string hdf4cpp::HdfDataItem::getName() const {
    return name;
}
std::vector<int32> hdf4cpp::HdfDataItem::getDims() {
    std::vector<int32> dims;
    dims.push_back(nrRecords);
    dims.push_back((int32) fieldNames.size());
    return dims;
}
intn hdf4cpp::HdfDataItem::size() const {
    return nrRecords * (int32) fieldNames.size();
}
int32 hdf4cpp::HdfDataItem::getDataType() const {
    return 0;
}
hdf4cpp::HdfItem::HdfItem(HdfItem&& other) : item(std::move(other.item)), sId(other.sId), vId(other.vId) {
}
std::vector<int32> hdf4cpp::HdfItem::getDims() {
    if(isValid()) {
        return item->getDims();
    } else {
        return std::vector<int32>();
    }
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::getAttribute(const std::string &name) {
    return item->getAttribute(name);
}
hdf4cpp::Type hdf4cpp::HdfItem::getType() const {
    if(isValid()) {
        return item->getType();
    } else {
        return NONE;
    }
}
std::string hdf4cpp::HdfItem::getName() const {
    if(isValid()) {
        return item->getName();
    } else {
        return std::string();
    }
}
bool hdf4cpp::HdfItem::isValid() const {
    return item && item->isValid();
}
intn hdf4cpp::HdfItem::size() const {
    if(isValid()) {
        return item->size();
    } else {
        return FAIL;
    }
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::begin() const {
    return Iterator(sId, vId, item->getId(), 0);
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::end() const {
    int32 size = Vntagrefs(item->getId());
    if(size == FAIL) {
        return Iterator(sId, vId, item->getId(), 0);
    } else {
        return Iterator(sId, vId, item->getId(), size);
    }
}