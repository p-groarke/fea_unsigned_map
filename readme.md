# fea_unsigned_map

Simple `unordered_map` like container, optimized for unsigned keys that grow from 0 to N.

Has better copy/clear performance since it doesn't need rehashing. Has better value iteration performance since it doesn't use buckets and values are tightly packed. Insert is darn fast. Optimized for speed, not memory usage.

Similar to a `slot_map`, but keys are user provided and there is no concept of key generation. This conforms more closely to stl maps apis and provides more flexibility. If key generations are desirable for your use-case, use a `slot_map` instead.

Internal container and allocator are *not* customizable at this time.

**Warning : Key container will grow as large as the biggest key!**


## Build
`fea_unsigned_map` is a single header with no dependencies other than the stl.

Currently only tested on Windows.

Unit tests and benchmarks are not built by default.
Use conan to install the dependencies when running the test suite.

### Windows
```
mkdir build && cd build
..\conan.bat
cmake .. -A x64 -DBUILD_TESTING=On -DBUILD_BENCHMARKS=On && cmake --build . --config debug
bin\fea_unsigned_map_tests.exe
bin\fea_unsigned_map_benchmarks.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
../conan.sh
cmake .. -DBUILD_TESTING=On -DBUILD_BENCHMARKS=On && cmake --build . --config debug
bin/fea_unsigned_map_tests
bin/fea_unsigned_map_benchmarks

// Optionally
cmake --build . --target install
```


## Examples

```c++

```


## Benchmarks

```
#####################################################
Benchmark using 10000000 random keys, with duplicates
#####################################################
Num unique keys : 32768

############################
Clear 32768 small objects
############################
std::map clear                                                0.001525s        1.000000x
std::unordered_map clear                                      0.000896s        1.702881x
fea::unsigned_map clear                                       0.000000s        infx

##########################
Clear 32768 big objects
##########################
std::map clear                                                0.007799s        1.000000x
std::unordered_map clear                                      0.005959s        1.308804x
fea::unsigned_map clear                                       0.000000s        77989.000000x

#############################
Insert 10000000 small objects
#############################
std::map insert                                               1.501415s        1.000000x
std::unordered_map insert                                     0.187247s        8.018374x
fea::unsigned_map insert                                      0.040118s        37.424873x

###########################
Insert 10000000 big objects
###########################
std::map insert                                               3.068673s        1.000000x
std::unordered_map insert                                     1.202107s        2.552745x
fea::unsigned_map insert                                      0.529949s        5.790504x


###########################################
Insert 10000000 small objects after reserve
###########################################
std::map insert                                               1.486769s        1.000000x
std::unordered_map insert                                     0.262409s        5.665849x
fea::unsigned_map insert                                      0.038387s        38.731451x

#########################################
Insert 10000000 big objects after reserve
#########################################
std::map insert                                               3.019885s        1.000000x
std::unordered_map insert                                     1.653764s        1.826067x
fea::unsigned_map insert                                      0.544592s        5.545230x

############################################
Iterate 32768 small objects and assign value
############################################
std::map iterate & assign                                     0.001379s        1.000000x
std::unordered_map iterate & assign                           0.000629s        2.190945x
fea::unsigned_map iterate & assign                            0.000360s        3.834306x

##########################################
Iterate 32768 big objects and assign value
##########################################
std::map iterate & assign                                     0.004998s        1.000000x
std::unordered_map iterate & assign                           0.003614s        1.383049x
fea::unsigned_map iterate & assign                            0.003215s        1.554505x



#########################################################
Benchmark using linear keys, 0 to 5000000, no duplicates
#########################################################
Num unique keys : 5000000

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      0.416772s        1.000000x
std::map clear                                                0.216490s        1.925130x
fea::unsigned_map clear                                       0.000000s        4167719.000000x

#########################
Clear 5000000 big objects
#########################
std::unordered_map clear                                      1.638965s        1.000000x
std::map clear                                                1.476568s        1.109983x
fea::unsigned_map clear                                       0.000000s        16389653.000000x

############################
Insert 5000000 small objects
############################
std::map insert                                               0.817681s        1.000000x
std::unordered_map insert                                     0.802688s        1.018679x
fea::unsigned_map insert                                      0.082890s        9.864622x

##########################
Insert 5000000 big objects
##########################
std::map insert                                               3.126524s        1.000000x
std::unordered_map insert                                     3.107309s        1.006184x
fea::unsigned_map insert                                      0.877206s        3.564185x


##########################################
Insert 5000000 small objects after reserve
##########################################
std::map insert                                               0.832597s        1.000000x
std::unordered_map insert                                     0.786038s        1.059232x
fea::unsigned_map insert                                      0.083400s        9.983188x

########################################
Insert 5000000 big objects after reserve
########################################
std::unordered_map insert                                     3.335792s        1.000000x
std::map insert                                               3.331514s        1.001284x
fea::unsigned_map insert                                      0.871499s        3.827648x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.123395s        1.000000x
std::unordered_map iterate & assign                           0.117555s        1.049675x
fea::unsigned_map iterate & assign                            0.055048s        2.241577x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.667376s        1.000000x
std::unordered_map iterate & assign                           0.590707s        1.129792x
fea::unsigned_map iterate & assign                            0.572655s        1.165408x



#########################################################
Benchmark using linear keys, 5000000 to 0, no duplicates
#########################################################
Num unique keys : 5000000

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      0.609331s        1.000000x
std::map clear                                                0.339284s        1.795930x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 5000000 big objects
#########################
std::unordered_map clear                                      2.614184s        1.000000x
std::map clear                                                1.916937s        1.363730x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 5000000 small objects
############################
std::map insert                                               0.844469s        1.000000x
std::unordered_map insert                                     0.803254s        1.051310x
fea::unsigned_map insert                                      0.055462s        15.226033x

##########################
Insert 5000000 big objects
##########################
std::map insert                                               3.294518s        1.000000x
std::unordered_map insert                                     3.204371s        1.028133x
fea::unsigned_map insert                                      0.661582s        4.979754x


##########################################
Insert 5000000 small objects after reserve
##########################################
std::map insert                                               0.828210s        1.000000x
std::unordered_map insert                                     0.798856s        1.036745x
fea::unsigned_map insert                                      0.048237s        17.169604x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               3.647501s        1.000000x
std::unordered_map insert                                     3.587541s        1.016713x
fea::unsigned_map insert                                      0.671358s        5.433023x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.136954s        1.000000x
std::unordered_map iterate & assign                           0.122586s        1.117208x
fea::unsigned_map iterate & assign                            0.055162s        2.482788x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.596296s        1.000000x
std::unordered_map iterate & assign                           0.530290s        1.124471x
fea::unsigned_map iterate & assign                            0.474918s        1.255575x
```