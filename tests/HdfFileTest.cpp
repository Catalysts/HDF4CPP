//
// Created by patrik on 10.08.17.
//

#include <gtest/gtest.h>
#include <hdf4cpp/HdfFile.h>

using namespace hdf4cpp;

class HdfFileTest : public ::testing::Test {
  protected:
    HdfFileTest() : file(std::string(TEST_DATA_PATH) + "small_test.hdf") {}

    HdfFile file;
};

TEST_F(HdfFileTest, DatasetValidity) {
    ASSERT_NO_THROW(file.get("Data"));
}

TEST_F(HdfFileTest, GroupValidity) {
    ASSERT_NO_THROW(file.get("Group"));
}

TEST_F(HdfFileTest, InvalidItem) {
    ASSERT_THROW(file.get("InvalidKey"), HdfException);
}

TEST_F(HdfFileTest, ReadData1) {
    HdfItem item = file.get("Data");
    std::vector<int32> vec;
    item.read(vec);
    ASSERT_EQ(vec, std::vector<int32>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST_F(HdfFileTest, ReadData2) {
    HdfItem item = file.get("DataWithAttributes");
    std::vector<float32> vec;
    item.read(vec);
    ASSERT_EQ(vec, std::vector<float32>({0.0, 0.1, 0.2, 1.0, 1.1, 1.2, 2.0, 2.1, 2.2}));
}

TEST_F(HdfFileTest, ReadDatasetAttributes) {
    HdfItem item = file.get("DataWithAttributes");

    HdfAttribute attribute1 = item.getAttribute("Integer");
    std::vector<int32> integer;
    attribute1.get(integer);
    ASSERT_EQ(integer, std::vector<int32>({12345}));

    HdfAttribute attribute2 = item.getAttribute("Integers");
    std::vector<int32> integers;
    attribute2.get(integers);
    ASSERT_EQ(integers, std::vector<int32>({1, 12, 123, 1234, 12345}));
}

TEST_F(HdfFileTest, ReadGroupAttributes) {
    HdfItem item = file.get("GroupWithOnlyAttribute");

    {
        HdfAttribute attribute = item.getAttribute("Egy");
        std::vector<int8> egy;
        attribute.get(egy);
        ASSERT_EQ(egy, std::vector<int8>({1}));
    }

    {
        HdfAttribute attribute = item.getAttribute("One");
        std::vector<int16> one;
        attribute.get(one);
        ASSERT_EQ(one, std::vector<int16>({1}));
    }

    {
        HdfAttribute attribute = item.getAttribute("Ein");
        std::vector<int32> ein;
        attribute.get(ein);
        ASSERT_EQ(ein, std::vector<int32>({1}));
    }
}

TEST_F(HdfFileTest, ReadInvalidData) {
    ASSERT_THROW(file.get("InvalidKey"), HdfException);
}

TEST_F(HdfFileTest, ReadDataInRange) {
    {
        HdfItem item = file.get("Data");
        std::vector<int32> vec;
        item.read(vec, std::vector<Range>({Range(0, 2), Range(1, 2)}));
        ASSERT_EQ(vec, std::vector<int32>({2, 3, 5, 6}));
    }
    {
        HdfItem item = file.get("DataWithAttributes");
        std::vector<float32> vec;
        item.read(vec, std::vector<Range>({Range(2, 1), Range(0, 2)}));
        ASSERT_EQ(vec, std::vector<float32>({2.0, 2.1}));
    }
}

TEST_F(HdfFileTest, ReadInvalidDatasetAttribute) {
    HdfItem item = file.get("Data");
    ASSERT_THROW(HdfAttribute attribute = item.getAttribute("Attribute"), HdfException);
}

TEST_F(HdfFileTest, ReadInvalidGroupAttribute) {
    HdfItem item = file.get("Group");
    ASSERT_THROW(HdfAttribute attribute = item.getAttribute("Attribute"), HdfException);
}

TEST_F(HdfFileTest, GetAllDatsetsWithTheSameName) {
    std::vector<HdfItem> items = file.getAll("DoubleDataset");
    ASSERT_EQ(items.size(), 2);
    std::vector<int32> vec;
    items[0].read(vec);
    ASSERT_EQ(vec, std::vector<int32>({0}));
    items[1].read(vec);
    ASSERT_EQ(vec, std::vector<int32>({0, 1}));
}

TEST_F(HdfFileTest, DatasetTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("Data");
    ASSERT_THROW(item.read(vec), HdfException);
}

TEST_F(HdfFileTest, DatasetAttributeTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("DataWithAttributes");
    HdfAttribute attribute = item.getAttribute("Integer");
    ASSERT_THROW(attribute.get(vec), HdfException);
}

TEST_F(HdfFileTest, GroupAttributeTypeIncompatibility) {
    std::vector<std::string> vec;
    HdfItem item = file.get("GroupWithOnlyAttribute");
    HdfAttribute attribute = item.getAttribute("Egy");
    ASSERT_THROW(attribute.get(vec), HdfException);
}

TEST_F(HdfFileTest, GlobalAttribute) {
    std::vector<int8> vec;
    HdfAttribute attribute = file.getAttribute("GlobalAttribute");
    attribute.get(vec);
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
    ASSERT_NO_THROW(file.get("RIG0.0"));
}

TEST_F(HdfFileTest, VDataRead1) {
    HdfItem item = file.get("Vdata");
    ASSERT_EQ(item.getName(), "Vdata");
    std::vector<int32> vec;
    item.read(vec, "age");
    ASSERT_EQ(vec, std::vector<int32>({39, 19, 55}));
}

TEST_F(HdfFileTest, VDataRead2) {
    HdfItem item = file.get("Vdata");
    ASSERT_EQ(item.getName(), "Vdata");
    std::vector<std::vector<char> > vec;
    item.read(vec, "name");
    std::vector<std::string> exp = {"Patrick Jane", "Barry Allen", "Angus MacGyver"};
    ASSERT_EQ(vec.size(), 3);
    for(int i = 0; i < 3; ++i) {
        ASSERT_EQ(std::string(vec[i].data()), exp[i]);
    }
}

TEST_F(HdfFileTest, VDataAttributes) {
    HdfItem item = file.get("Vdata");
    ASSERT_EQ(item.getName(), "Vdata");
    HdfAttribute attribute = item.getAttribute("attribute");
    std::vector<int32> vec;
    attribute.get(vec);
    ASSERT_EQ(vec, std::vector<int32>({1, 2, 3, 3, 2, 1}));
}