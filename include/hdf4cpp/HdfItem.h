//
// Created by patrik on 11.08.17.
//

#ifndef HDF4CPP_HDFITEM_H
#define HDF4CPP_HDFITEM_H

#include <hdf4cpp/HdfDefines.h>
#include <hdf4cpp/HdfFile.h>
#include <hdf4cpp/HdfException.h>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <hdf/hdf.h>

namespace hdf4cpp {

/// Structure to define in which range we want to read the data from the SData
/// It must be defined for every dimension
struct Range {
    /// The index in which begins the reading of the data
    int32 begin;
    /// The number of data to be read
    int32 quantity;
    /// The increasing number (stride = 1 : every data, stride = 2 : every second data, ...)
    int32 stride;

    Range(int32 begin = 0, int32 quantity = 0, int32 stride = 1) : begin(begin), quantity(quantity), stride(stride) {}

    /// What would be the number of data to read, if we read in this range
    intn size() const {
        if (!stride) {
            return 0;
        }
        return quantity / stride;
    }

    /// Checks if the range is correct for a specific dimension
    bool check(const int32& dim) const {
        return begin >= 0 ||
            begin < dim ||
            quantity >= 0 ||
            begin + quantity <= dim ||
            stride > 0;
    }

    /// Fills a range vector with a dimension array
    static void fill(std::vector<Range>& ranges, const std::vector<int32>& dims) {
        for (size_t i = ranges.size(); i < dims.size(); ++i) {
            ranges.push_back(Range(0, dims[i]));
        }
    }
};

class HdfAttribute;

/// Represents an hdf item
class HdfItem : public HdfObject {
public:
    HdfItem(const HdfItem& item) = delete;
    HdfItem(HdfItem&& item);
    HdfItem& operator=(const HdfItem& item) = delete;
    HdfItem& operator=(HdfItem&& it);

    /// \returns The name of the item
    std::string getName() const;

    /// \returns The dimensions of the item
    /// \note This operation is not supported for every item type
    std::vector<int32> getDims();

    /// \returns The number of data being in the item
    intn size() const;

    /// \returns the attribute of the item with the given name
    /// \param name the name of the attribute
    /// \note If there are multiple attributes with the same name then the first will be returned
    HdfAttribute getAttribute(const std::string& name) const;

    /// Reads the entire data from the item
    /// \param dest the destination vector in which the data will be stored
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

    /// Reads the data from the item in a specified range
    /// \param dest the destination vector in which the data will be stored
    /// \param ranges specifies the range in which the data will be read
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

    /// Reads the given field from the item
    /// \param dest the destination vector in which the data will be stored
    /// \param field the name of the field
    /// \param records the number of records to be read
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

    friend HdfItem HdfFile::get(const std::string& name) const;
    friend std::vector<HdfItem> HdfFile::getAll(const std::string& name) const;
    friend HdfItem HdfFile::Iterator::operator*();
    friend class HdfAttribute;

private:
    /// The base class of the item classes
    class HdfItemBase : public HdfObject {
    public:

        HdfItemBase(int32 id, const Type& type, const HdfDestroyerChain& chain) : HdfObject(type, ITEM, chain), id(id) {
            if (id == FAIL) {
                raiseException(INVALID_ID);
            }
        }
        virtual ~HdfItemBase() {}

        /// Get the id which is held by this object
        virtual int32 getId() const = 0;
        /// Get the name of the item
        virtual std::string getName() const = 0;
        /// Get the dimensions of the item
        virtual std::vector<int32> getDims() = 0;
        /// Get the number of data from the item
        virtual intn size() const = 0;
        /// Get the attribute from the item given by its name
        virtual HdfAttribute getAttribute(const std::string& name) const = 0;

    protected:
        int32 id;

        virtual int32 getDataType() const = 0;
    };
    /// Item class for the SData items
    class HdfDatasetItem : public HdfItemBase {
    public:
        HdfDatasetItem(int32 id, const HdfDestroyerChain& chain);
        ~HdfDatasetItem();

        int32 getId() const;
        std::string getName() const;
        std::vector<int32> getDims();
        intn size() const;
        HdfAttribute getAttribute(const std::string& name) const;

        /// Reads the data in a specific range. See Range
        /// \param dest The destination vector
        /// \param ranges The vector of ranges
        template <class T>
        void read(std::vector<T>& dest, std::vector<Range>& ranges) {
            std::vector<int32> dims = getDims();
            Range::fill(ranges, dims);
            intn length = 1;
            for (size_t i = 0; i < ranges.size(); ++i) {
                if (!ranges[i].check(dims[i])) {
                    raiseException(INVALID_RANGES);
                }
                length *= ranges[i].size();
            }
            auto it = typeSizeMap.find(getDataType());
            if (it != typeSizeMap.end()) {
                if ((size_t)it->second != sizeof(T)) {
                    raiseException(BUFFER_SIZE_NOT_ENOUGH);
                }
            } else {
                raiseException(INVALID_DATA_TYPE);
            }
            dest.resize(length);
            std::vector<int32> start, quantity, stride;
            for (const auto& range : ranges) {
                start.push_back(range.begin);
                quantity.push_back(range.quantity);
                stride.push_back(range.stride);
            }

            if (SDreaddata(id, start.data(), stride.data(), quantity.data(), dest.data()) == FAIL) {
                raiseException(STATUS_RETURN_FAIL);
            }
        }

        /// Reads the whole data
        /// \param dest The destination vector
        template <class T>
        void read(std::vector<T>& dest) {
            std::vector<int32> dims = getDims();
            std::vector<Range> ranges;
            for (const auto& dim : dims) {
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
    /// Item class for VGroup items
    class HdfGroupItem : public HdfItemBase {
    public:
        HdfGroupItem(int32 id, const HdfDestroyerChain& chain);
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
    /// Item class for VData items
    class HdfDataItem : public HdfItemBase {
    public:
        HdfDataItem(int32 id, const HdfDestroyerChain& chain);

        ~HdfDataItem();

        int32 getId() const;

        std::string getName() const;

        std::vector<int32> getDims();

        intn size() const;

        HdfAttribute getAttribute(const std::string& name) const;

        /// Reads a specific number of the data of a specific field
        /// The records are simple values
        /// \param dest The destination vector
        /// \param field The specific field name
        /// \param records The number of records to be read
        template <class T>
        void read(std::vector<T>& dest, const std::string& field, int32 records) {
            if (!records) {
                records = nrRecords;
            }

            if (VSsetfields(id, field.c_str()) == FAIL) {
                raiseException(STATUS_RETURN_FAIL);
            }

            int32 fieldSize = VSsizeof(id, (char*)field.c_str());
            if (sizeof(T) < (size_t) fieldSize) {
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

        /// Reads a specific number of the data of a specific field
        /// The records are arrays itself
        /// \param dest The destination matrix (every record is a vector)
        /// \param field The specific field name
        /// \param records The number of records to be read
        template <class T>
        void read(std::vector<std::vector<T> >& dest, const std::string& field, int32 records) {
            if (!records) {
                records = nrRecords;
            }

            if (VSsetfields(id, field.c_str()) == FAIL) {
                raiseException(STATUS_RETURN_FAIL);
            }

            int32 fieldSize = VSsizeof(id, (char*)field.c_str());
            if (fieldSize % sizeof(T) != 0) {
                raiseException(BUFFER_SIZE_NOT_DIVISIBLE);
            }

            size_t size = records * fieldSize;
            std::vector<uint8> buff(size);
            if (VSread(id, buff.data(), records, interlace) == FAIL) {
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
            while (i < records) {
                j = 0;
                while (j < divided) {
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

        int32 getDataType() const;
    };

    HdfItem(HdfItemBase *item, int32 sId, int32 vId);

    /// Holds an item object address
    std::unique_ptr<HdfItemBase> item;

    /// The file handle ids (needed by the iterator)
    int32 sId;
    int32 vId;
};

/// HdfItem iterator, gives the possibility to iterate over the items from the item
class HdfItem::Iterator : public HdfObject, public std::iterator<std::bidirectional_iterator_tag, HdfItem> {
public:
    Iterator(int32 sId, int32 vId, int32 key, int32 index, Type type, const HdfDestroyerChain& chain) :
            HdfObject(type, ITERATOR, chain),
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
            return HdfItem(new HdfDataItem(id, chain), sId, vId);
        } else if(Visvg(key, ref)) {
            int32 id = Vattach(vId, ref, "r");
            return HdfItem(new HdfGroupItem(id, chain), sId, vId);
        } else {
            int32 id = SDselect(sId, SDreftoindex(sId, ref));
            return HdfItem(new HdfDatasetItem(id, chain), sId, vId);
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
