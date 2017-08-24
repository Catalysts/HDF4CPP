//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfFile.h>
#include <hdf4cpp/HdfItem.h>
#include <hdf/mfhdf.h>
#include <sstream>

hdf4cpp::HdfDatasetItem::HdfDatasetItem(int32 id, const HdfDestroyerChain& chain) : HdfItemBase(id, SDATA, chain) {
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
    this->chain.push_back(new HdfDatasetItemDestroyer(id));
}
std::vector<int32> hdf4cpp::HdfDatasetItem::getDims() {
    return dims;
}
hdf4cpp::HdfAttribute hdf4cpp::HdfDatasetItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfDatasetAttribute(id, name, chain));
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
}
intn hdf4cpp::HdfDatasetItem::size() const {
    return _size;
}
hdf4cpp::HdfGroupItem::HdfGroupItem(int32 id, const HdfDestroyerChain& chain) : HdfItemBase(id, VGROUP, chain) {
    char _name[MAX_NAME_LENGTH];
    Vgetname(id, _name);
    name = std::string(_name);
    this->chain.push_back(new HdfGroupItemDestroyer(id));
}
std::vector<int32> hdf4cpp::HdfGroupItem::getDims() {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfAttribute hdf4cpp::HdfGroupItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfGroupAttribute(id, name, chain));
}
std::string hdf4cpp::HdfGroupItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfGroupItem::getId() const {
    return id;
}
hdf4cpp::HdfGroupItem::~HdfGroupItem() {
}
intn hdf4cpp::HdfGroupItem::size() const {
    raiseException(INVALID_OPERATION);
}
int32 hdf4cpp::HdfGroupItem::getDataType() const {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfDataItem::HdfDataItem(int32 id, const HdfDestroyerChain& chain) : HdfItemBase(id, VDATA, chain) {
    this->chain.push_back(new HdfDataItemDestroyer(id));
    char _name[MAX_NAME_LENGTH];
    VSinquire(id, &nrRecords, &interlace, nullptr, &recordSize, _name);
    name = std::string(_name);
}
hdf4cpp::HdfDataItem::~HdfDataItem() {
}
hdf4cpp::HdfAttribute hdf4cpp::HdfDataItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfDataAttribute(id, name, chain));
}
int32 hdf4cpp::HdfDataItem::getId() const {
    return id;
}
std::string hdf4cpp::HdfDataItem::getName() const {
    return name;
}
std::vector<int32> hdf4cpp::HdfDataItem::getDims() {
    raiseException(INVALID_OPERATION);
}
intn hdf4cpp::HdfDataItem::size() const {
    raiseException(INVALID_OPERATION);
}
int32 hdf4cpp::HdfDataItem::getDataType() const {
    return 0;
}
hdf4cpp::HdfItem::HdfItem(HdfItemBase *item, int32 sId, int32 vId) : HdfObject(item),
                                                                     item(item),
                                                                     sId(sId),
                                                                     vId(vId) {}
hdf4cpp::HdfItem::HdfItem(HdfItem&& other) : HdfObject(other.getType(), other.getClassType(), std::move(other.chain)),
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
    return Iterator(sId, vId, item->getId(), 0, getType(), chain);
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::end() const {
    switch(item->getType()) {
        case VGROUP: {
            int32 size = Vntagrefs(item->getId());
            return Iterator(sId, vId, item->getId(), size, getType(), chain);
        }
        default: {
            return Iterator(sId, vId, item->getId(), 0, getType(), chain);
        }
    }
}