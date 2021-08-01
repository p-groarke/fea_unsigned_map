<br/>

# This repository is inactive and kept for posterity's sake.
# Any further development will happen in [fea_libs](https://github.com/p-groarke/fea_libs). Cheers.
<br/>
<br/>

# fea_unsigned_map
[![Build Status](https://travis-ci.org/p-groarke/fea_unsigned_map.svg?branch=master)](https://travis-ci.org/p-groarke/fea_unsigned_map)
[![Build status](https://ci.appveyor.com/api/projects/status/3aljp09xanvka0qy/branch/master?svg=true)](https://ci.appveyor.com/project/p-groarke/fea-unsigned-map/branch/master)

`unordered_map` like containers, optimized for unsigned keys.

Internal container and allocator are *not* customizable at this time.

## unsigned_map
`unsigned_map` is a slot map that follows the `unordered_map` c++ standard apis as close as possible. Unlike most slot map implementations, you provide the unique key. This is useful when retro-fitting slot maps into legacy code. If key generations are desirable for your use-case, use a `slot_map` instead.

**Warning : Key container will grow as large as the biggest key!**

### When To Use
* Keys grow from 0 to N.
* Each key is unique, aka a perfect hash.
* Memory is not a concearn or you can guarantee linearity in the keys (aka memory consumption isn't wasted).

### Features
* Has better copy/clear performance since it doesn't need rehashing.
* Has better value iteration performance since it doesn't use buckets and values are tightly packed.
* Insert is darn fast.
* Optimized for speed, not memory usage.


## flat_unsigned_hashmap
The `flat_unsigned_hashmap` is a more traditional hashing map, that has similar constraints as a slot map. Here, the lookup container will **not** grow as big as the biggest key. It is still much more performant than `unordered_map`, all the while offering a `data()` api which returns a pointer to the underlying values (hence flat).

### When To Use
* Keys aren't guaranteed to be unique, though as with all hashmaps, performance will degrade if there are too many collisions.
* You cannot guarantee key linearity (ex. pointers).
* You need to iterate contained values often.
* You must cross an ABI boundary, and wish to return a pointer to the data (or a `span`).

### Features
* Has overall better performance than an equivalent `unordered_map`.
* Doesn't use as much memory as `unsigned_map`, though it uses more memory than `unordered_map`.
* Data is stored contiguously.
* Access to underlying value buffer.

## Benchmarks
Benchmarks are available [here](benchmarks.md)

## Build
`fea_unsigned_map` is a header library with no dependencies other than the stl.

Use conan to install the dependencies when running the test suite.

### Windows
```
mkdir build && cd build
cmake .. -A x64 -DFEA_UNSIGNED_MAP_TESTS=On -DFEA_UNSIGNED_MAP_BENCHMARKS=On && cmake --build .
bin\fea_unsigned_map_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
cmake .. -DFEA_UNSIGNED_MAP_TESTS=On -DFEA_UNSIGNED_MAP_BENCHMARKS=On && cmake --build .
bin/fea_unsigned_map_tests

// Optionally
cmake --build . --target install
```
