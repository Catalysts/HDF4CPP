//
// Created by patrik on 10.08.17.
//

#include <gtest/gtest.h>
#include <hdf4cpp/HdfFile.h>

using namespace hdf4cpp;

class HdfFileTest : public ::testing::Test {
protected:
    HdfFileTest() : file(std::string(TEST_DATA_PATH) + "small_test.hdf") {}

    void writeOut(const HdfItem& item, std::ostream& out, const std::string& tab = std::string()) {
        if(!item.isValid()) return;
        out << tab << item.getName() << '\n';
        for(const auto& it : item) {
            writeOut(it, out, tab + "\t");
        }
    }

    void writeOut(const HdfFile& file, std::ostream& out) {
        for(const auto& it : file) {
            writeOut(it, out);
        }
    }

    HdfFile file;
};

TEST_F(HdfFileTest, FileValidity) {
    ASSERT_TRUE(file.isValid());
}

TEST_F(HdfFileTest, DatasetValidity) {
    ASSERT_TRUE(file.get("Data").isValid());
}

TEST_F(HdfFileTest, GroupValidity) {
    ASSERT_TRUE(file.get("Group").isValid());
}

TEST_F(HdfFileTest, InvalidItem) {
    ASSERT_FALSE(file.get("InvalidKey").isValid());
}

TEST_F(HdfFileTest, ReadData1) {
    HdfItem item = file.get("Data");
    std::vector<int32> vec;
    ASSERT_TRUE(item.read(vec));
    ASSERT_EQ(vec, std::vector<int32>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST_F(HdfFileTest, ReadData2) {
    HdfItem item = file.get("DataWithAttributes");
    std::vector<float32> vec;
    ASSERT_TRUE(item.read(vec));
    ASSERT_EQ(vec, std::vector<float32>({0.0, 0.1, 0.2, 1.0, 1.1, 1.2, 2.0, 2.1, 2.2}));
}

TEST_F(HdfFileTest, ReadDatasetAttributes) {
    HdfItem item = file.get("DataWithAttributes");

    HdfAttribute attribute1 = item.getAttribute("Integer");
    std::vector<int32> integer;
    ASSERT_TRUE(attribute1.get(integer));
    ASSERT_EQ(integer, std::vector<int32>({12345}));

    HdfAttribute attribute2 = item.getAttribute("Integers");
    std::vector<int32> integers;
    ASSERT_TRUE(attribute2.get(integers));
    ASSERT_EQ(integers, std::vector<int32>({1, 12, 123, 1234, 12345}));
}

TEST_F(HdfFileTest, ReadGroupAttributes) {
    HdfItem item = file.get("GroupWithOnlyAttribute");

    {
        HdfAttribute attribute = item.getAttribute("Egy");
        std::vector<int8> egy;
        ASSERT_TRUE(attribute.get(egy));
        ASSERT_EQ(egy, std::vector<int8>({1}));
    }

    {
        HdfAttribute attribute = item.getAttribute("One");
        std::vector<int16> one;
        ASSERT_TRUE(attribute.get(one));
        ASSERT_EQ(one, std::vector<int16>({1}));
    }

    {
        HdfAttribute attribute = item.getAttribute("Ein");
        std::vector<int32> ein;
        ASSERT_TRUE(attribute.get(ein));
        ASSERT_EQ(ein, std::vector<int32>({1}));
    }
}

TEST_F(HdfFileTest, ReadInvalidData) {
    HdfItem item = file.get("InvalidKey");
    std::vector<int32> vec;
    ASSERT_FALSE(item.read(vec));
}

TEST_F(HdfFileTest, ReadDataInRange) {
    {
        HdfItem item = file.get("Data");
        ASSERT_TRUE(item.isValid());
        std::vector<int32> vec;
        ASSERT_TRUE(item.read(vec, std::vector<Range>({Range(0, 2), Range(1, 2)})));
        ASSERT_EQ(vec, std::vector<int32>({2, 3, 5, 6}));
    }
    {
        HdfItem item = file.get("DataWithAttributes");
        ASSERT_TRUE(item.isValid());
        std::vector<float32> vec;
        ASSERT_TRUE(item.read(vec, std::vector<Range>({Range(2, 1), Range(0, 2)})));
        ASSERT_EQ(vec, std::vector<float32>({2.0, 2.1}));
    }
}

TEST_F(HdfFileTest, ReadInvalidDatasetAttribute) {
    HdfItem item = file.get("Data");
    ASSERT_TRUE(item.isValid());
    HdfAttribute attribute = item.getAttribute("Attribute");
    ASSERT_FALSE(attribute.isValid());
    std::vector<int32> vec;
    ASSERT_FALSE(attribute.get(vec));
}

TEST_F(HdfFileTest, ReadInvalidGroupAttribute) {
    HdfItem item = file.get("Group");
    ASSERT_TRUE(item.isValid());
    HdfAttribute attribute = item.getAttribute("Attribute");
    ASSERT_FALSE(attribute.isValid());
    std::vector<int32> vec;
    ASSERT_FALSE(attribute.get(vec));
}

TEST_F(HdfFileTest, MovingItems) {
    HdfItem item1 = file.get("Data");
    HdfItem item2 = std::move(item1);
    ASSERT_FALSE(item1.isValid());
    std::vector<int32> vec;
    ASSERT_TRUE(item2.read(vec));
    ASSERT_EQ(vec, std::vector<int32>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST_F(HdfFileTest, GetAllDatsetsWithTheSameName) {
    std::vector<HdfItem> items = file.getAll("DoubleDataset");
    ASSERT_EQ(items.size(), 2);
    std::vector<int32> vec;
    ASSERT_TRUE(items[0].read(vec));
    ASSERT_EQ(vec, std::vector<int32>({0}));
    ASSERT_TRUE(items[1].read(vec));
    ASSERT_EQ(vec, std::vector<int32>({0, 1}));
}

TEST_F(HdfFileTest, DatasetTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("Data");
    ASSERT_ANY_THROW(item.read(vec));
}

TEST_F(HdfFileTest, DatasetAttributeTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("DataWithAttributes");
    HdfAttribute attribute = item.getAttribute("Integer");
    ASSERT_ANY_THROW(attribute.get(vec));
}

TEST_F(HdfFileTest, GroupAttributeTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("GroupWithOnlyAttribute");
    HdfAttribute attribute = item.getAttribute("Egy");
    ASSERT_ANY_THROW(attribute.get(vec));
}

TEST_F(HdfFileTest, GlobalAttribute) {
    std::vector<int8> vec;
    HdfAttribute attribute = file.getAttribute("GlobalAttribute");
    ASSERT_TRUE(attribute.isValid());
    ASSERT_TRUE(attribute.get(vec));
    ASSERT_EQ(vec, std::vector<int8>({11, 22}));
}

TEST_F(HdfFileTest, FileIterator) {
    std::ostringstream out;
    for(auto it : file) {
        out << it.getName() << '*';
    }
    ASSERT_EQ(out.str(), std::string("Group*GroupWithOnlyAttribute*RIG0.0*/home/patrik/HDF4CPP/tests/test_data/small_test.hdf*Egy*One*Ein*Vdata*attribute*"));
}

TEST_F(HdfFileTest, GroupIterator) {
    HdfItem item = file.get("Group");
    ASSERT_TRUE(item.isValid());
    std::ostringstream out;
    for(auto it : item) {
        out << it.getName() << '*';
    }
    ASSERT_EQ(out.str(), "Data*DataWithAttributes*");
}

TEST_F(HdfFileTest, IteratingOverNonGroupItems) {
    std::ostringstream out;
    HdfItem sdata = file.get("DoubleDataset");
    for(const auto& item : sdata) {
        out << item.getName();
    }
    HdfItem vdata = file.get("Vdata");
    for(const auto& item : vdata) {
        out << item.getName();
    }
    ASSERT_TRUE(out.str().empty());
}

TEST_F(HdfFileTest, ItemIterator3) {
    auto it = file.begin();
    HdfItem item = *it;
    ASSERT_EQ(item.getName(), "Group");
}

TEST_F(HdfFileTest, HiddenGroup) {
    HdfItem item = file.get("RIG0.0");
    ASSERT_TRUE(item.isValid());
}

TEST_F(HdfFileTest, VDataRead1) {
    HdfItem item = file.get("Vdata");
    ASSERT_TRUE(item.isValid());
    ASSERT_EQ(item.getName(), "Vdata");
    std::vector<int32> vec;
    ASSERT_TRUE(item.read(vec, "age"));
    ASSERT_EQ(vec, std::vector<int32>({39, 19, 55}));
}

TEST_F(HdfFileTest, VDataRead2) {
    HdfItem item = file.get("Vdata");
    ASSERT_TRUE(item.isValid());
    ASSERT_EQ(item.getName(), "Vdata");
    std::vector<std::vector<char> > vec;
    ASSERT_TRUE(item.read(vec, "name"));
    std::vector<std::string> exp = {"Patrick Jane", "Barry Allen", "Angus MacGyver"};
    ASSERT_EQ(vec.size(), 3);
    for(int i = 0; i < 3; ++i) {
        ASSERT_EQ(std::string(vec[i].data()), exp[i]);
    }
}

TEST_F(HdfFileTest, VDataAttributes) {
    HdfItem item = file.get("Vdata");
    ASSERT_TRUE(item.isValid());
    ASSERT_EQ(item.getName(), "Vdata");
    HdfAttribute attribute = item.getAttribute("attribute");
    ASSERT_TRUE(attribute.isValid());
    std::vector<int32> vec;
    ASSERT_TRUE(attribute.get(vec));
    ASSERT_EQ(vec, std::vector<int32>({1, 2, 3, 3, 2, 1}));
}