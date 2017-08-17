//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfItem.h>
#include <hdf/mfhdf.h>
#include <algorithm>
#include <iostream>

HdfDatasetItem::HdfDatasetItem(int32 id) : HdfItemBase(id) {
    std::vector<int32> dims = getDims();
    _size = 1;
    std::for_each(dims.begin(), dims.end(), [this](const int32& t) {
        _size *= t;
    });
    int32 dim[MAX_DIMENSION];
    int32 size, nrAtt;
    char _name[MAX_NAME_LENGTH];
    SDgetinfo(id, _name, &size, dim, &dataType, &nrAtt);
    name = std::string(_name);
}
std::vector<int32> HdfDatasetItem::getDims() {
    int32 dims[MAX_DIMENSION];
    int32 size, dataType, nrAtt;
    char name[MAX_NAME_LENGTH];
    SDgetinfo(id, name, &size, dims, &dataType, &nrAtt);
    return std::vector<int32>(dims, dims + size);
}
bool HdfDatasetItem::read(void *dest) {
    std::vector<int32> end = getDims();
    std::vector<int32> start(end.size(), 0);
    return SDreaddata(id, start.data(), nullptr, end.data(), dest) != FAIL;
}
HdfAttribute HdfDatasetItem::getAttribute(const std::string &name) {
    return HdfAttribute(new HdfDatasetAttribute(id, name));
}
Type HdfDatasetItem::getType() const {
    return DATASET;
}
std::string HdfDatasetItem::getName() const {
    return name;
}
int32 HdfDatasetItem::getId() const {
    return id;
}
int32 HdfDatasetItem::getDataType() const {
    return dataType;
}
HdfDatasetItem::~HdfDatasetItem() {
    if(isValid()) {
        SDendaccess(id);
    }
}
intn HdfDatasetItem::size() const {
    return _size;
}
HdfGroupItem::HdfGroupItem(int32 id) : HdfItemBase(id) {
    char _name[MAX_NAME_LENGTH];
    Vgetname(id, _name);
    name = std::string(_name);
}
std::vector<int32> HdfGroupItem::getDims() {
    throw std::runtime_error("HDF4CPP: getDims not defined for HdfGroupItem");
}
bool HdfGroupItem::read(void *) {
    throw std::runtime_error("HDF4CPP: read not defined for HdfGroupItem");
}
HdfAttribute HdfGroupItem::getAttribute(const std::string &name) {
    return HdfAttribute(new HdfGroupAttribute(id, name));
}
Type HdfGroupItem::getType() const {
    return GROUP;
}
std::string HdfGroupItem::getName() const {
    return name;
}
int32 HdfGroupItem::getId() const {
    return id;
}
HdfGroupItem::~HdfGroupItem() {
    if(isValid()) {
        Vdetach(id);
    }
}
intn HdfGroupItem::size() const {
    throw std::runtime_error("HdfFile: read not defined for HdfGroupItem");
}
int32 HdfGroupItem::getDataType() const {
    throw std::runtime_error("HDF4CPP: no data type for HdfGroup");
}
HdfItem::HdfItem(HdfItem&& other) : item(std::move(other.item)), sId(other.sId), vId(other.vId) {
}
std::vector<int32> HdfItem::getDims() {
    if(isValid()) {
        return item->getDims();
    } else {
        return std::vector<int32>();
    }
}
HdfAttribute HdfItem::getAttribute(const std::string &name) {
    return item->getAttribute(name);
}
Type HdfItem::getType() const {
    if(isValid()) {
        return item->getType();
    } else {
        return NONE;
    }
}
std::string HdfItem::getName() const {
    if(isValid()) {
        return item->getName();
    } else {
        return std::string();
    }
}
bool HdfItem::isValid() const {
    return item && item->isValid();
}
intn HdfItem::size() const {
    if(isValid()) {
        return item->size();
    } else {
        return FAIL;
    }
}
HdfItem::Iterator HdfItem::begin() const {
    return Iterator(sId, vId, item->getId(), 0);
}
HdfItem::Iterator HdfItem::end() const {
    int32 size = Vntagrefs(item->getId());
    if(size == FAIL) {
        return Iterator(sId, vId, item->getId(), 0);
    } else {
        return Iterator(sId, vId, item->getId(), size);
    }
}