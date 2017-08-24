//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfFile.h>
#include <hdf4cpp/HdfItem.h>
#include <hdf/mfhdf.h>
#include <sstream>

hdf4cpp::HdfDatasetItem::HdfDatasetItem(int32 id) : HdfItemBase(id, SDATA) {
    _size = 1;
    std::for_each(dims.begin(), dims.end(), [this](const int32 &t) {
        _size *= t;
    });
    int32 dim[MAX_DIMENSION];
    int32 size;
    char _name[MAX_NAME_LENGTH];
    SDgetinfo(id, _name, &size, dim, &dataType, nullptr);
    dims = std::vector<int32>(dim, dim + size);
    name = std::string(_name);
}
std::vector<int32> hdf4cpp::HdfDatasetItem::getDims() {
    return dims;
}
hdf4cpp::HdfAttribute hdf4cpp::HdfDatasetItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfDatasetAttribute(id, name));
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
    SDendaccess(id);
}
intn hdf4cpp::HdfDatasetItem::size() const {
    return _size;
}
hdf4cpp::HdfGroupItem::HdfGroupItem(int32 id) : HdfItemBase(id, VGROUP) {
    char _name[MAX_NAME_LENGTH];
    Vgetname(id, _name);
    name = std::string(_name);
}
std::vector<int32> hdf4cpp::HdfGroupItem::getDims() {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfAttribute hdf4cpp::HdfGroupItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfGroupAttribute(id, name));
}
std::string hdf4cpp::HdfGroupItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfGroupItem::getId() const {
    return id;
}
hdf4cpp::HdfGroupItem::~HdfGroupItem() {
    Vdetach(id);
}
intn hdf4cpp::HdfGroupItem::size() const {
    raiseException(INVALID_OPERATION);
}
int32 hdf4cpp::HdfGroupItem::getDataType() const {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfDataItem::HdfDataItem(int32 id) : HdfItemBase(id, VDATA) {
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
    VSdetach(id);
}
hdf4cpp::HdfAttribute hdf4cpp::HdfDataItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfDataAttribute(id, name));
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
hdf4cpp::HdfItem::HdfItem(HdfItemBase *item, int32 sId, int32 vId) : HdfObject(item),
                                                                    item(item),
                                                                    sId(sId),
                                                                    vId(vId) {}
hdf4cpp::HdfItem::HdfItem(HdfItem&& other) : HdfObject(other.getType(), other.getClassType()),
                                             item(std::move(other.item)),
                                             sId(other.sId),
                                             vId(other.vId) {}
hdf4cpp::HdfItem& hdf4cpp::HdfItem::operator=(HdfItem&& it) {
    item = std::move(it.item);
    return *this;
}
std::vector<int32> hdf4cpp::HdfItem::getDims() {
    return item->getDims();
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::getAttribute(const std::string &name) const {
    return item->getAttribute(name);
}
hdf4cpp::Type hdf4cpp::HdfItem::getType() const {
    return item->getType();
}
std::string hdf4cpp::HdfItem::getName() const {
    return item->getName();
}
intn hdf4cpp::HdfItem::size() const {
    return item->size();
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::begin() const {
    return Iterator(sId, vId, item->getId(), 0, getType());
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::end() const {
    switch(item->getType()) {
        case VGROUP: {
            int32 size = Vntagrefs(item->getId());
            return Iterator(sId, vId, item->getId(), size, getType());
        }
        default: {
            return Iterator(sId, vId, item->getId(), 0, getType());
        }
    }
}