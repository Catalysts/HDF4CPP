//
// Created by patrik on 11.08.17.
//

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfAttribute.h>
#include <stdexcept>

HdfDatasetAttribute::HdfDatasetAttribute(int32 id, const std::string& name) : HdfAttributeBase(id, SDfindattr(id, name.c_str())) {
    if(index != FAIL) {
        int32 nrValues;
        char nameRet[MAX_NAME_LENGTH];
        SDattrinfo(id, index, nameRet, &dataType, &_size);
    } else {
        dataType = 0;
        _size = FAIL;
    }
}
Type HdfDatasetAttribute::getType() const {
    return DATASET;
}
intn HdfDatasetAttribute::size() const {
    return _size;
}
int32 HdfDatasetAttribute::getDataType() const {
    return dataType;
}
bool HdfDatasetAttribute::get(void *dest) {
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
HdfGroupAttribute::HdfGroupAttribute(int32 id, const std::string& name) : HdfAttributeBase(id, -1), _size(FAIL) {
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
Type HdfGroupAttribute::getType() const {
    return GROUP;
}
intn HdfGroupAttribute::size() const {
    return _size;
}
int32 HdfGroupAttribute::getDataType() const {
    return dataType;
}
bool HdfGroupAttribute::get(void *dest) {
    return Vgetattr2(id, index, dest) != FAIL;
}
HdfAttribute::HdfAttribute(HdfAttribute &&other) : attribute(std::move(other.attribute)) {
}
bool HdfAttribute::isValid() const {
    return attribute && attribute->isValid();
}
Type HdfAttribute::getType() const {
    if(isValid()) {
        return attribute->getType();
    } else {
        return NONE;
    }
}
intn HdfAttribute::size() const {
    if(isValid()) {
        return attribute->size();
    } else {
        return FAIL;
    }
}