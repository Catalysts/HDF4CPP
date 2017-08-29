/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH


#include <hdf4cpp/HdfAttribute_priv.h>
#include <hdf4cpp/HdfDefines.h>
#include <stdexcept>

using namespace hdf4cpp;

hdf4cpp::HdfAttribute::HdfDatasetAttribute::HdfDatasetAttribute(int32 id,
                                                                const std::string &name,
                                                                const HdfDestroyerChain &chain)
    : HdfAttributeBase(id, SDfindattr(id, name.c_str()), SDATA, chain) {
    char waste[MAX_NAME_LENGTH];
    if (SDattrinfo(id, index, waste, &dataType, &_size) == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }
}
intn hdf4cpp::HdfAttribute::HdfDatasetAttribute::size() const {
    return _size;
}
int32 hdf4cpp::HdfAttribute::HdfDatasetAttribute::getDataType() const {
    return dataType;
}
void hdf4cpp::HdfAttribute::HdfDatasetAttribute::get(void *dest) {
    int32 nrValues;
    char nameRet[MAX_NAME_LENGTH];
    int32 status = SDattrinfo(id, index, nameRet, &dataType, &nrValues);
    if (status == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }

    if (SDreadattr(id, index, dest) == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }
}
hdf4cpp::HdfAttribute::HdfGroupAttribute::HdfGroupAttribute(int32 id,
                                                            const std::string &name,
                                                            const HdfDestroyerChain &chain)
    : HdfAttributeBase(id, 0, VGROUP, chain) {
    int32 nrAtts = Vnattrs2(id);
    for (intn i = 0; i < nrAtts; ++i) {
        char names[MAX_NAME_LENGTH];
        int32 type, count, size, nFields;
        uint16 refNum;
        Vattrinfo2(id, i, names, &type, &count, &size, &nFields, &refNum);
        if (name == std::string(names)) {
            index = i;
            _size = count;
            dataType = type;
            return;
        }
    }
    raiseException(INVALID_NAME);
}
intn hdf4cpp::HdfAttribute::HdfGroupAttribute::size() const {
    return _size;
}
int32 hdf4cpp::HdfAttribute::HdfGroupAttribute::getDataType() const {
    return dataType;
}
void hdf4cpp::HdfAttribute::HdfGroupAttribute::get(void *dest) {
    if (Vgetattr2(id, index, dest) == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }
}
hdf4cpp::HdfAttribute::HdfDataAttribute::HdfDataAttribute(int32 id,
                                                          const std::string &name,
                                                          const HdfDestroyerChain &chain)
    : HdfAttributeBase(id, VSfindattr(id, _HDF_VDATA, name.c_str()), VDATA, chain) {
    if (VSattrinfo(id, _HDF_VDATA, index, nullptr, &dataType, &_size, nullptr) == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }
}
intn hdf4cpp::HdfAttribute::HdfDataAttribute::size() const {
    return _size;
}
void hdf4cpp::HdfAttribute::HdfDataAttribute::get(void *dest) {
    if (VSgetattr(id, _HDF_VDATA, index, dest) == FAIL) {
        raiseException(STATUS_RETURN_FAIL);
    }
}
int32 hdf4cpp::HdfAttribute::HdfDataAttribute::getDataType() const {
    return dataType;
}
hdf4cpp::HdfAttribute::HdfAttribute(HdfAttribute &&other)
    : HdfObject(other.getType(), other.getClassType(), std::move(other.chain))
    , attribute(std::move(other.attribute)) {
}
hdf4cpp::HdfAttribute &hdf4cpp::HdfAttribute::operator=(HdfAttribute &&attr) {
    attribute = std::move(attr.attribute);
    setType(attribute->getType());
    setClassType(attribute->getClassType());
    return *this;
}
intn hdf4cpp::HdfAttribute::size() const {
    return attribute->size();
}
HdfAttribute::HdfAttribute(HdfAttributeBase *attribute)
    : HdfObject(attribute)
    , attribute(attribute) {
}
int32 HdfAttribute::getDataType() const {
    return attribute->getDataType();
}
void HdfAttribute::getInternal(void *dest) {
    attribute->get(dest);
}
