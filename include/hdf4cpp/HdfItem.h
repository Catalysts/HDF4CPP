//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFITEM_H
#define HDF4CPP_HDFITEM_H

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfAttribute.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <hdf/hdf.h>


struct Range {
    int32 begin;
    int32 quantity;
    int32 stride;

    Range(int32 begin = 0, int32 quantity = 0, int32 stride = 1) : begin(begin), quantity(quantity), stride(stride) {}

    intn size() const {
        return quantity / stride;
    }
};

class HdfItemBase {
public:

    HdfItemBase(int32 id) : id(id) {}
    virtual ~HdfItemBase() {}
    virtual bool isValid() const { return id != FAIL; }

    virtual Type getType() const = 0;
    virtual int32 getId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<int32> getDims() = 0;
    virtual intn size() const = 0;

    template <class T> bool read(std::vector<T> &dest, const std::vector<Range>& ranges) {
        if(size() == FAIL) {
            return false;
        }
        std::vector<int32> dims = getDims();
        if(ranges.size() != dims.size()) {
            throw std::runtime_error("HDF4CPP: incorrect number of ranges");
        }
        intn length = 1;
        for(int i = 0; i < dims.size(); ++i) {
            if(ranges[i].begin < 0 || ranges[i].begin >= dims[i] || ranges[i].quantity < 0 || ranges[i].begin + ranges[i].quantity > dims[i] || ranges[i].stride <= 0) {
                throw std::runtime_error("HDF4CPP: incorrect range");
            }
            length *= ranges[i].size();
        }
        if(length > 0) {
            auto it = typeSizeMap.find(getDataType());
            if(it != typeSizeMap.end()) {
                if(it->second != sizeof(T)) {
                    throw std::runtime_error("HDF4CPP: type size missmatch");
                }
            } else {
                throw std::runtime_error("HDF4CPP: hdf data set type not supported");
            }
            dest.resize(length);
            return read(dest.data(), ranges);
        } else {
            return false;
        }
    }

    template <class T> bool read(std::vector<T> &dest) {
        std::vector<int32> dims = getDims();
        std::vector<Range> ranges(dims.size());
        std::transform(dims.begin(), dims.end(), ranges.begin(), [](const int32& t) {
            return Range(0, t);
        });
        return read(dest, ranges);
    }

    virtual HdfAttribute getAttribute(const std::string& name) = 0;

protected:
    int32 id;

    virtual bool read(void *dest, const std::vector<Range>& ranges) = 0;
    virtual int32 getDataType() const = 0;
};

class HdfDatasetItem : public HdfItemBase {
public:
    HdfDatasetItem(int32 id);
    ~HdfDatasetItem();

    Type getType() const;
    int32 getId() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;
    HdfAttribute getAttribute(const std::string& name);

private:
    intn _size;
    int32 dataType;
    std::string name;

    bool read(void *dest, const std::vector<Range>& ranges);
    int32 getDataType() const;
};

class HdfGroupItem : public HdfItemBase {
public:
    HdfGroupItem(int32 id);
    ~HdfGroupItem();

    Type getType() const;
    int32 getId() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;

    HdfAttribute getAttribute(const std::string& name);

private:
    std::string name;

    bool read(void *dest, const std::vector<Range>& ranges);
    int32 getDataType() const;
};

class HdfItem {
public:
    HdfItem(HdfItemBase *item, int32 sId, int32 vId) : item(item), sId(sId), vId(vId) {}
    HdfItem(const HdfItem& item) = delete;
    HdfItem(HdfItem&& item);
    bool isValid() const;

    Type getType() const;
    std::string getName() const;
    std::vector<int32> getDims();
    intn size() const;
    HdfAttribute getAttribute(const std::string& name);

    template <class T> bool read(std::vector<T> &dest, const std::vector<Range>& ranges) {
        if(isValid()) {
            return item->read(dest, ranges);
        } else {
            return false;
        }
    }

    template <class T> bool read(std::vector<T> &dest) {
        if(isValid()) {
            return item->read(dest);
        } else {
            return false;
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

class HdfItem::Iterator : public std::iterator<std::bidirectional_iterator_tag, HdfItem> {
public:
    Iterator(int32 sId, int32 vId, int32 key, int32 index) : sId(sId), vId(vId), key(key), index(index) {}

    bool operator!=(const Iterator& it) { return index != it.index; }
    bool operator==(const Iterator& it) { return index == it.index; }
//    bool operator<(const Iterator& it) { return index < it.index; }
//    bool operator>(const Iterator& it) { return index > it.index; }
//    bool operator>=(const Iterator& it) { return index >= it.index; }
//    bool operator<=(const Iterator& it) { return index <= it.index; }

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
            throw std::runtime_error("HDF4CPP: cannot access invalid item");
        }
        if(Visvs(key, ref)) {
            throw std::runtime_error("HDF4CPP: vdata not supported yet");
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

#endif //HDF4CPP_HDFITEM_H
