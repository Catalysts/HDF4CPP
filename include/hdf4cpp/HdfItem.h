//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFITEM_H
#define HDF4CPP_HDFITEM_H

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfAttribute.h>
#include <hdf4cpp/HdfException.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <hdf/hdf.h>

namespace hdf4cpp {

struct Range {
    int32 begin;
    int32 quantity;
    int32 stride;

    Range(int32 begin = 0, int32 quantity = 0, int32 stride = 1) : begin(begin), quantity(quantity), stride(stride) {}

    intn size() const {
        if(!stride) {
            return FAIL;
        }
        return quantity / stride;
    }

    bool check(const int32& dim) const {
        return begin >= 0 ||
               begin < dim ||
               quantity >= 0 ||
               begin + quantity <= dim ||
               stride > 0;
    }

    static void fill(std::vector<Range>& ranges, const std::vector<int32>& dims) {
        for(size_t i = ranges.size(); i < dims.size(); ++i) {
            ranges.push_back(Range(0, dims[i]));
        }
    }
};

class HdfItemBase : public HdfObject {
public:

    HdfItemBase(int32 id, const Type& type) : HdfObject(type, ITEM), id(id) {
        if(id == FAIL) {
            raiseException(INVALID_ID);
        }
    }
    virtual ~HdfItemBase() {}

    virtual int32 getId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<int32> getDims() = 0;
    virtual intn size() const = 0;

    virtual HdfAttribute getAttribute(const std::string& name) const = 0;

protected:
    int32 id;

    virtual int32 getDataType() const = 0;
};

class HdfDatasetItem : public HdfItemBase {
public:
    HdfDatasetItem(int32 id);
    ~HdfDatasetItem();

    int32 getId() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;
    HdfAttribute getAttribute(const std::string& name) const;

    template <class T> void read(std::vector<T>& dest, std::vector<Range>& ranges) {
        std::vector<int32> dims = getDims();
        Range::fill(ranges, dims);
        intn length = 1;
        for(size_t i = 0; i < ranges.size(); ++i) {
            if(!ranges[i].check(dims[i])) {
                raiseException(INVALID_RANGES);
            }
            length *= ranges[i].size();
        }
        auto it = typeSizeMap.find(getDataType());
        if(it != typeSizeMap.end()) {
            if((size_t) it->second != sizeof(T)) {
                raiseException(BUFFER_SIZE_NOT_ENOUGH);
            }
        } else {
            raiseException(INVALID_DATA_TYPE);
        }
        dest.resize(length);
        std::vector<int32> start, quantity, stride;
        for(const auto& range : ranges) {
            start.push_back(range.begin);
            quantity.push_back(range.quantity);
            stride.push_back(range.stride);
        }

        if(SDreaddata(id, start.data(), stride.data(), quantity.data(), dest.data()) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }
    }

    template <class T> void read(std::vector<T>& dest) {
        std::vector<int32> dims = getDims();
        std::vector<Range> ranges;
        for(const auto& dim : dims) {
            ranges.push_back(Range(0, dim));
        }
        read(dest, ranges);
    }

private:
    intn _size;
    int32 dataType;
    std::string name;
    std::vector<int32> dims;

    int32 getDataType() const;
};

class HdfGroupItem : public HdfItemBase {
public:
    HdfGroupItem(int32 id);
    ~HdfGroupItem();

    int32 getId() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;

    HdfAttribute getAttribute(const std::string& name) const;

private:
    std::string name;

    int32 getDataType() const;
};

class HdfDataItem : public HdfItemBase {
public:
    HdfDataItem(int32 id);

    ~HdfDataItem();

    int32 getId() const;

    std::string getName() const;

    std::vector<int32> getDims();

    intn size() const;

    HdfAttribute getAttribute(const std::string &name) const;

    template<class T>
    void read(std::vector<T> &dest, const std::string &field, int32 records) {
        if (!records) {
            records = nrRecords;
        }

        if (VSsetfields(id, field.c_str()) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }

        int32 fieldSize = VSsizeof(id, (char *) field.c_str());
        if (sizeof(T) < fieldSize) {
            raiseException(BUFFER_SIZE_NOT_ENOUGH);
        }

        size_t size = records * fieldSize;
        std::vector<uint8> buff(size);
        if (VSread(id, buff.data(), records, interlace) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }

        dest.resize(records);
        VOIDP buffptrs[1];
        buffptrs[0] = dest.data();
        if (VSfpack(id, _HDF_VSUNPACK, field.c_str(), buff.data(), size, records, field.c_str(), buffptrs) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }
    }


    template <class T> void read(std::vector<std::vector<T> >& dest, const std::string& field, int32 records) {
        if(!records) {
            records = nrRecords;
        }

        if(VSsetfields(id, field.c_str()) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }

        int32 fieldSize = VSsizeof(id, (char *) field.c_str());
        if(fieldSize % sizeof(T) != 0) {
            raiseException(BUFFER_SIZE_NOT_DIVISIBLE);
        }

        size_t size = records * fieldSize;
        std::vector<uint8> buff(size);
        if(VSread(id, buff.data(), records, interlace) == FAIL) {
            raiseException(STATUS_RETURN_FAIL);
        }

        int32 divided = fieldSize / sizeof(T);
        dest.resize(records, std::vector<T>(divided));
        std::vector<T> linearDest(records * divided);
        VOIDP buffptrs[1];
        buffptrs[0] = linearDest.data();
        VSfpack(id, _HDF_VSUNPACK, field.c_str(), buff.data(), size, records, field.c_str(), buffptrs);
        int i = 0;
        int j;
        while(i < records) {
            j = 0;
            while(j < divided) {
                dest[i][j] = linearDest[i * divided + j];
                ++j;
            }
            ++i;
        }
    }

private:
    intn _size;
    std::string name;

    int32 nrRecords;
    int32 interlace;
    int32 recordSize;
    std::vector<std::string> fieldNames;

    int32 getDataType() const;
};


class HdfItem : public HdfObject {
public:
    HdfItem(HdfItemBase *item, int32 sId, int32 vId);
    HdfItem(const HdfItem& item) = delete;
    HdfItem(HdfItem&& item);
    HdfItem& operator=(const HdfItem& item) = delete;
    HdfItem& operator=(HdfItem&& it);

    Type getType() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;
    HdfAttribute getAttribute(const std::string& name) const;

    template <class T> void read(std::vector<T>& dest) {
        switch(item->getType()) {
            case SDATA: {
                HdfDatasetItem *dItem = dynamic_cast<HdfDatasetItem *>(item.get());
                dItem->read(dest);
                break;
            }
            default:
                raiseException(INVALID_OPERATION);
        }
    }

    template <class T> void read(std::vector<T>& dest, std::vector<Range> ranges) {
        switch(item->getType()) {
            case SDATA: {
                HdfDatasetItem *dItem = dynamic_cast<HdfDatasetItem *>(item.get());
                dItem->read(dest, ranges);
                break;
            }
            default:
                raiseException(INVALID_OPERATION);
        }
    }

    template <class T> void read(std::vector<T>& dest, const std::string& field, int32 records = 0) {
        switch(item->getType()) {
            case VDATA: {
                HdfDataItem *vItem = dynamic_cast<HdfDataItem*>(item.get());
                vItem->read(dest, field, records);
                break;
            }
            default:
                raiseException(INVALID_OPERATION);
        }
    }

    class Iterator;

    Iterator begin() const;
    Iterator end() const;

private:
    std::unique_ptr<HdfItemBase> item;
    int32 sId;
    int32 vId;
};

class HdfItem::Iterator : public HdfObject, public std::iterator<std::bidirectional_iterator_tag, HdfItem> {
public:
    Iterator(int32 sId, int32 vId, int32 key, int32 index, Type type) :
                                                                        HdfObject(type, ITERATOR),
                                                                        sId(sId),
                                                                        vId(vId),
                                                                        key(key),
                                                                        index(index) {}

    bool operator!=(const Iterator& it) { return index != it.index; }
    bool operator==(const Iterator& it) { return index == it.index; }

    Iterator& operator++() {
        ++index;
        return *this;
    }
    Iterator operator++(int) {
        Iterator it(*this);
        ++index;
        return it;
    }
    Iterator& operator--() {
        --index;
        return *this;
    }
    Iterator operator--(int) {
        Iterator it(*this);
        --index;
        return it;
    }

    HdfItem operator*() {
        int32 tag, ref;
        if(Vgettagref(key, index, &tag, &ref) == FAIL) {
            raiseException(OUT_OF_RANGE);
        }
        if(Visvs(key, ref)) {
            int32 id = VSattach(vId, ref, "r");
            return HdfItem(new HdfDataItem(id), sId, vId);
        } else if(Visvg(key, ref)) {
            int32 id = Vattach(vId, ref, "r");
            return HdfItem(new HdfGroupItem(id), sId, vId);
        } else {
            int32 id = SDselect(sId, SDreftoindex(sId, ref));
            return HdfItem(new HdfDatasetItem(id), sId, vId);
        }
    }

private:
    int32 sId;
    int32 vId;
    int32 key;

    int32 index;
};

}

#endif //HDF4CPP_HDFITEM_H
