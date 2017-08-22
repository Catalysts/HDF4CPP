//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfAttribute.h>
#include <stdexcept>

hdf4cpp::HdfDatasetAttribute::HdfDatasetAttribute(int32 id, const std::string& name) : HdfAttributeBase(id, SDfindattr(id, name.c_str())) {
    if(index != FAIL) {
        int32 nrValues;
        char nameRet[MAX_NAME_LENGTH];
        SDattrinfo(id, index, nameRet, &dataType, &_size);
    } else {
        dataType = 0;
        _size = FAIL;
    }
}
hdf4cpp::Type hdf4cpp::HdfDatasetAttribute::getType() const {
    return VDATA;
}
intn hdf4cpp::HdfDatasetAttribute::size() const {
    return _size;
}
int32 hdf4cpp::HdfDatasetAttribute::getDataType() const {
    return dataType;
}
bool hdf4cpp::HdfDatasetAttribute::get(void *dest) {
    if(!isValid()) {
        return false;
    }

    int32 nrValues;
    char nameRet[MAX_NAME_LENGTH];
    int32 status = SDattrinfo(id, index, nameRet, &dataType, &nrValues);
    if(status == FAIL) {
        return false;
    }

    return SDreadattr(id, index, dest) != FAIL;
}
hdf4cpp::HdfGroupAttribute::HdfGroupAttribute(int32 id, const std::string& name) : HdfAttributeBase(id, -1), _size(FAIL) {
    int32 nrAtts = Vnattrs2(id);
    for(intn i = 0; i < nrAtts; ++i) {
        char names[MAX_NAME_LENGTH];
        int32 type, count, size, nFields;
        uint16 refNum;
        Vattrinfo2(id, i, names, &type, &count, &size, &nFields, &refNum);
        if(name == std::string(names)) {
            index = i;
            _size = count;
            dataType = type;
            break;
        }
    }
}
hdf4cpp::Type hdf4cpp::HdfGroupAttribute::getType() const {
    return VGROUP;
}
intn hdf4cpp::HdfGroupAttribute::size() const {
    return _size;
}
int32 hdf4cpp::HdfGroupAttribute::getDataType() const {
    return dataType;
}
bool hdf4cpp::HdfGroupAttribute::get(void *dest) {
    return Vgetattr2(id, index, dest) != FAIL;
}
hdf4cpp::HdfDataAttribute::HdfDataAttribute(int32 id, const std::string &name) : HdfAttributeBase(id, VSfindattr(id, _HDF_VDATA, name.c_str())) {
    if(index != FAIL) {
        int32 nrBytes;
        char _name[MAX_NAME_LENGTH];
        VSattrinfo(id, _HDF_VDATA, index, _name, &dataType, &_size, &nrBytes);
    } else {
        dataType = 0;
        _size = FAIL;
    }
}
hdf4cpp::Type hdf4cpp::HdfDataAttribute::getType() const {
    return VDATA;
}
intn hdf4cpp::HdfDataAttribute::size() const {
    return _size;
}
bool hdf4cpp::HdfDataAttribute::get(void *dest) {
    return VSgetattr(id, _HDF_VDATA, index, dest) != FAIL;
}
int32 hdf4cpp::HdfDataAttribute::getDataType() const {
    return dataType;
}
hdf4cpp::HdfAttribute::HdfAttribute(HdfAttribute &&other) : attribute(std::move(other.attribute)) {
}
hdf4cpp::HdfAttribute& hdf4cpp::HdfAttribute::operator=(HdfAttribute&& attr) {
    attribute = std::move(attr.attribute);\
    return *this;
}
bool hdf4cpp::HdfAttribute::isValid() const {
    return attribute && attribute->isValid();
}
hdf4cpp::Type hdf4cpp::HdfAttribute::getType() const {
    if(isValid()) {
        return attribute->getType();
    } else {
        return NONE;
    }
}
intn hdf4cpp::HdfAttribute::size() const {
    if(isValid()) {
        return attribute->size();
    } else {
        return FAIL;
    }
}