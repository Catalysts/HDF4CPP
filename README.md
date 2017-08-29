# HDF4CPP

## Introduction

The HDF4CPP library is a wrapper library of the hdf4 C interfaces.
The target was to create a common c++ api to read and write hdf scientific data.
The only supports the reading of the data at the moment.

## Api overview

### The idea

The idea is to use objects and their methods to do the operations.
These classes are representing different kind of data,
so the operation functions are separated. 
It is easy to decide which object's method I have to call.

### Classes and objects

**HdfObject**       - the base class of all the classes

**HdfFile**         - represents an hdf file

**HdfItem**         - represents an item (SData, VGroup, VData)
                    - you can get from a file, calling its get function
**HdfAttribute**    - represents an attribute of an item or of a file
                    - you can get from a file or from an item, calling its getAttribute function

## How does it work?

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

Check example codes for data specific read instructions.

Note: The library does a type size check, and throws exception 
in case of missmatch.

### Reading attribute data

Reading attribute data is much more easier than reading the item's data.
That's because all the attribute data has an array structure.
The **HdfAttribute** objects have a read function which receives a vector reference.
Here will be the data stored.

Note: The library does a type size check, and throws exception 
in case of missmatch.

## Exception mechanism

The library throws **HdfException**.
This specific exception has **getType** and **getClassType** methods, 
just to get information about the thrower object.
Has a **getExceptionType** which returns information about the type
of the exception, and a **getMessage** method which returns a message
as string.