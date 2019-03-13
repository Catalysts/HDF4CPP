/// \copyright Copyright (c) Catalysts GmbH
/// \author Patrik Kovacs, Catalysts GmbH

#include <hdf4cpp/hdf.h>
#include <iostream>

using namespace hdf4cpp;

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    // Open the file
    HdfFile file(TEST_DATA_PATH "small_test.hdf");
    // Get the item
    HdfItem item = file.get("Vdata");
    // Reading scalar field
    {
        std::vector<int32> vec;
        item.read(vec, "age");
        std::cout << "Age field values: ";
        for (const auto &value : vec) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
    }
    // Reading array field (string)
    {
        std::vector<std::vector<char> > vec;
        item.read(vec, "name");
        std::cout << "Name field values: ";
        for(const auto& char_vec : vec) {
            std::string string(char_vec.data());
            std::cout << '\'' << string << "' ";
        }
        std::cout << std::endl;
    }
    // Reading attribute
    {
        HdfAttribute attribute = item.getAttribute("attribute");
        std::vector<int32> vec;
        attribute.get(vec);
        std::cout << "Attribute values: ";
        for(const auto& value : vec) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
    }
}
