# fea_unsigned_map

Simple `unordered_map` like container, optimized for unsigned keys.

Has better copy/clear performance since it doesn't need rehashing. Has better value iteration performance since it doesn't use buckets and values are tightly packed. Similar to "slot_map", but keys are user provided and there is no concept of key generation. This conforms more closely to stl maps apis and provides more flexibility. If key generations are desirable for your use-case, use a `slot_map` instead. Internal container and allocator are *not* customizable at this time.

**Warning : Key container will grow as large as the biggest key!**

## Examples

```c++

```

## Build
`fea_unsigned_map` is a single header with no dependencies other than the stl.

The unit tests depend on gtest. They are not built by default. Use conan to install the dependencies when running the test suite.

### Windows
```
mkdir build && cd build
..\conan.bat
cmake .. -A x64 -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_unsigned_map_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
..\conan.sh
cmake .. -DBUILD_TESTING=On && cmake --build . --config debug
bin\fea_unsigned_map_tests.exe

// Optionally
cmake --build . --target install
```
