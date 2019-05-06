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
    HdfItem item = file.get("DataWithAttributes");
    // Check the type
    if (item.getType() == SDATA) {
        std::cout << "Yes, the type is SData\n";
    } else {
        std::cerr << "Type missmatch\n";
        return 1;
    }

    // Read the whole data
    std::vector<float32> vec;
    item.read(vec);
    std::cout << "The whole data: ";
    for (const auto &value : vec) {
        std::cout << value << ' ';
    }
    std::cout << std::endl;

    // Read in a range
    std::vector<Range> ranges;
    std::vector<int32> dims = item.getDims();
    for (const auto &dim : dims) {
        if (dim >= 1) {
            ranges.emplace_back(0, dim - 1);
        } else {
            ranges.emplace_back(0, dim);
        }
    }
    item.read(vec, ranges);
    std::cout << "Data in the range [0, dim-1) if it's possible, else [0, dim): ";
    for (const auto &value : vec) {
        std::cout << value << ' ';
    }
    std::cout << std::endl;

    // Read an attribute
    HdfAttribute attribute = item.getAttribute("Integers");
    std::vector<int32> integers;
    attribute.get(integers);
    std::cout << "The 'Integers' attribute data: ";
    for (const auto &integer : integers) {
        std::cout << integer << ' ';
    }
    std::cout << std::endl;
}
