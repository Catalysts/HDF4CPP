/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH


#include <hdf4cpp/HdfAttribute.h>
#include <hdf4cpp/HdfFile.h>
#include <hdf4cpp/HdfItem.h>
#include <mfhdf.h>
#include <numeric>
#include <sstream>

hdf4cpp::HdfItem::HdfDatasetItem::HdfDatasetItem(int32 id, const HdfDestroyerChain &chain)
    : HdfItemBase(id, SDATA, chain) {
    int32 dim[MAX_DIMENSION];
    int32 size;
    char _name[MAX_NAME_LENGTH];
    SDgetinfo(id, _name, &size, dim, &dataType, nullptr);
    dims = std::vector<int32>(dim, dim + size);
    _size = std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<int32>());
    name = std::string(_name);
    this->chain.pushBack(new HdfDestroyer(&SDendaccess, id));
}
std::vector<int32> hdf4cpp::HdfItem::HdfDatasetItem::getDims() {
    return dims;
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::HdfDatasetItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfAttribute::HdfDatasetAttribute(id, name, chain));
}
std::string hdf4cpp::HdfItem::HdfDatasetItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfItem::HdfDatasetItem::getId() const {
    return id;
}
hdf4cpp::HdfItem::HdfDatasetItem::~HdfDatasetItem() {
}
hdf4cpp::HdfItem::HdfGroupItem::HdfGroupItem(int32 id, const HdfDestroyerChain &chain)
    : HdfItemBase(id, VGROUP, chain) {
    char _name[MAX_NAME_LENGTH];
    Vgetname(id, _name);
    name = std::string(_name);
    this->chain.pushBack(new HdfDestroyer(&Vdetach, id));
}
std::vector<int32> hdf4cpp::HdfItem::HdfGroupItem::getDims() {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::HdfGroupItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfAttribute::HdfGroupAttribute(id, name, chain));
}
std::string hdf4cpp::HdfItem::HdfGroupItem::getName() const {
    return name;
}
int32 hdf4cpp::HdfItem::HdfGroupItem::getId() const {
    return id;
}
hdf4cpp::HdfItem::HdfGroupItem::~HdfGroupItem() {
}
hdf4cpp::HdfItem::HdfDataItem::HdfDataItem(int32 id, const HdfDestroyerChain &chain)
    : HdfItemBase(id, VDATA, chain) {
    this->chain.pushBack(new HdfDestroyer(&VSdetach, id));
    char _name[MAX_NAME_LENGTH];
    VSinquire(id, &nrRecords, &interlace, nullptr, &recordSize, _name);
    name = std::string(_name);
}
hdf4cpp::HdfItem::HdfDataItem::~HdfDataItem() {
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::HdfDataItem::getAttribute(const std::string &name) const {
    return HdfAttribute(new HdfAttribute::HdfDataAttribute(id, name, chain));
}
int32 hdf4cpp::HdfItem::HdfDataItem::getId() const {
    return id;
}
std::string hdf4cpp::HdfItem::HdfDataItem::getName() const {
    return name;
}
std::vector<int32> hdf4cpp::HdfItem::HdfDataItem::getDims() {
    raiseException(INVALID_OPERATION);
}
hdf4cpp::HdfItem::HdfItem(HdfItemBase *item, int32 sId, int32 vId)
    : HdfObject(item)
    , item(item)
    , sId(sId)
    , vId(vId) {
}
hdf4cpp::HdfItem::HdfItem(HdfItem &&other)
    : HdfObject(other.getType(), other.getClassType(), std::move(other.chain))
    , item(std::move(other.item))
    , sId(other.sId)
    , vId(other.vId) {
}
hdf4cpp::HdfItem &hdf4cpp::HdfItem::operator=(HdfItem &&it) {
    setType(it.getType());
    setClassType(it.getClassType());
    chain = std::move(it.chain);
    item = std::move(it.item);
    return *this;
}
std::vector<int32> hdf4cpp::HdfItem::getDims() {
    return item->getDims();
}
hdf4cpp::HdfAttribute hdf4cpp::HdfItem::getAttribute(const std::string &name) const {
    return item->getAttribute(name);
}
std::string hdf4cpp::HdfItem::getName() const {
    return item->getName();
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::begin() const {
    return Iterator(sId, vId, item->getId(), 0, getType(), chain);
}
hdf4cpp::HdfItem::Iterator hdf4cpp::HdfItem::end() const {
    switch (item->getType()) {
    case VGROUP: {
        int32 size = Vntagrefs(item->getId());
        return Iterator(sId, vId, item->getId(), size, getType(), chain);
    }
    default: { return Iterator(sId, vId, item->getId(), 0, getType(), chain); }
    }
}
