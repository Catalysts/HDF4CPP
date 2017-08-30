# HDF4CPP

## Introduction

The HDF4CPP library is a wrapper library of the hdf4 C interfaces.
The target was to create a common c++ api to read and write hdf scientific data.
The library only supports reading of the data at the moment.

## API overview

### The idea

The idea is to use objects and their methods to do the operations.
These classes are representing different kind of data,
so the operation functions are separated. 
Only reading is supported right now.

### Classes and objects

**HdfObject**       - the base class of all the classes

**HdfFile**         - represents an hdf file

**HdfItem**         - represents an item (SData, VGroup, VData)
                    - you can get from a file, calling its get function
**HdfAttribute**    - represents an attribute of an item or of a file
                    - you can get from a file or from an item, calling its getAttribute function

## How does it work?

### Include the library

You have to include a single file.
```cpp
#include <hdf4cpp/hdf.h>
```

### Open an file

```cpp
hdf4cpp::HdfFile file("/path/to/the/file");
```

### Get an item, an attribute

```cpp
hdf4cpp::HdfAttribute fileAttr = file.getAttribute("file attribute name");
hdf4cpp::HdfItem item = file.get("item name");
hdf4cpp::HdfAttribute itemAttr = item.getAttribute("item attribute name");
```

### Get all the items with a specific name

```cpp
std::vector<hdf4cpp::HdfItem> items = file.getAll("items name");
```

### Object type

Every kind of hdf data (SData, Vgroup, Vdata) stores different structures.
These information must be read in a different way, we have to call different methods.

So we get the type of the object just to be sure that we want to read the data in a correct way.

```cpp
switch(item.getType()) {
    case hdf4cpp::SDATA:
        // reading the sdata
    case hdf4cpp::VDATA:
        // reading the vdata
    default:
        std::cerr << "data is not readable";
        // note: VGroup has no data to be read
}
```

### Reading the data

The **HdfItem** objects have a read function but overridden
for every reading procedure. 
The first parameter is a vector in every case, 
in which the data will be stored.

#### Reading SData

You have two possibilities.

1. You can read the entire data.

```cpp
std::vector<int> vec;
item.read(vec);
```
2. You can read the data in a specific range.

```cpp
std::vector<int> vec;
std::vector<hdf4cpp::Range> ranges;
// We have to specify the range for every dimension
for(size_t i = 0; i < number_of_dimensions; ++i) {
    ranges.push_back(hdf4cpp::Range(begin, quantity, stride));
    // begin - the first index which is included in the range
    // quantity - the number of value to be read
    // stride - reading pattern (1 : every element, 2 : every second element, ...)
    // stride has a default value (1 : every element)
}
item.read(vec, ranges);
```

#### Reading VData

You have to specify:
* which field do you want to read
* how many records do you want to read

1. Reading scalar data

The data can be scalar: a field has only one value for every record.

```cpp
std::vector<int> vec;
item.read(vec, "field_name", number_of_records);
```

2. Reading array data

The data can be array: a field has multiple values for every record.
For example: if a field's type is string, it is considered that 
every record has multiple character values (character array).

```cpp
// Reading string field
std::vector<std::vector<char> > vec;
item.read(vec, "field_name"); // the number_of_records it can be eliminated if I want to read all the records
// Getting the string vector
std::vector<std::string> string_vec;
for(const auto& char_vec : vec) {
    string_vec.push_back(char_vec.data());
}
```

Note: The library does a type size check, and throws an exception 
in case of mismatch.

### Reading attribute data

The **HdfAttribute** objects have a read function which receives a vector reference.
Here will be the data stored.

```cpp
std::vector<short> vec;
attribute.get(vec);
```

Note: The library does a type size check, and throws an exception 
in case of mismatch.

## Exception mechanism

The library throws an **HdfException** on any errors.
This specific exception has **getType** and **getClassType** methods, 
just to get information about the thrower object.
Has a **getExceptionType** which returns information about the type
of the exception, and a **getMessage** method which returns a message
as string.

## Supported compilers
```
g++
clang++
msvc
```

## Build instructions

The hdf4 C library is needed to build this project.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Note: on windows you need to point cmake to your hdf4-C installation. This
can be done using `-DCMAKE_PREFIX_PATH=<path/to/hdf4>`.

## Install
First build as above, then run:
```bash
cmake --build . --target install
```

Note: you can use `-DCMAKE_INSTALL_PREFIX=<path>` in the previous `cmake`
call to change the install location.

## Generating documentation

1. Generating the user documentation.
It includes documentation which is necessary for using this library.

```bash
cmake --build . --target docs_user
```

2. Generating the whole documentations.

```bash
cmake --build . --target docs_all
```
