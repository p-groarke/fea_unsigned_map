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
########################################################
Benchmark using linear keys, 0 to 5000000, no duplicates
########################################################
Num unique keys : 5000000

###############################
Copy ctor 5000000 small objects
###############################
std::unordered_map copy ctor                                  1.030680s        1.000000x
std::map copy ctor                                            0.518667s        1.987170x
fea::unsigned_map copy ctor                                   0.063243s        16.297014x

#############################
Copy ctor 5000000 big objects
#############################
std::unordered_map copy ctor                                  4.561138s        1.000000x
std::map copy ctor                                            3.930026s        1.160587x
fea::unsigned_map copy ctor                                   2.438289s        1.870631x

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      0.390338s        1.000000x
std::map clear                                                0.232739s        1.677154x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 5000000 big objects
#########################
std::unordered_map clear                                      1.427185s        1.000000x
std::map clear                                                1.295675s        1.101499x
fea::unsigned_map clear                                       0.000000s        14271850.000000x

############################
Insert 5000000 small objects
############################
std::map insert                                               0.867482s        1.000000x
std::unordered_map insert                                     0.865539s        1.002244x
fea::unsigned_map insert                                      0.072803s        11.915449x

##########################
Insert 5000000 big objects
##########################
std::unordered_map insert                                     3.392330s        1.000000x
std::map insert                                               3.120629s        1.087066x
fea::unsigned_map insert                                      0.703591s        4.821452x

###########################################
Erase 5000000 (all) small objects at random
###########################################
std::map erase                                                4.580800s        1.000000x
std::unordered_map erase                                      1.305914s        3.507734x
fea::unsigned_map erase                                       0.689747s        6.641277x

#########################################
Erase 5000000 (all) big objects at random
#########################################
std::map erase                                               10.229258s        1.000000x
std::unordered_map erase                                      5.282570s        1.936417x
fea::unsigned_map erase                                       2.162146s        4.731068x

##########################################
Insert 5000000 small objects after reserve
##########################################
std::unordered_map insert                                     0.946062s        1.000000x
std::map insert                                               0.831772s        1.137405x
fea::unsigned_map insert                                      0.075824s        12.477007x

########################################
Insert 5000000 big objects after reserve
########################################
std::unordered_map insert                                     5.829804s        1.000000x
std::map insert                                               4.466473s        1.305237x
fea::unsigned_map insert                                      0.717545s        8.124652x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.123994s        1.000000x
std::unordered_map iterate & assign                           0.115709s        1.071602x
fea::unsigned_map iterate & assign                            0.055469s        2.235379x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.564463s        1.000000x
std::unordered_map iterate & assign                           0.522961s        1.079361x
fea::unsigned_map iterate & assign                            0.484842s        1.164221x



########################################################
Benchmark using linear keys, 5000000 to 0, no duplicates
########################################################
Num unique keys : 5000000

###############################
Copy ctor 5000000 small objects
###############################
std::unordered_map copy ctor                                  2.622488s        1.000000x
std::map copy ctor                                            1.977551s        1.326129x
fea::unsigned_map copy ctor                                   0.072999s        35.924881x

#############################
Copy ctor 5000000 big objects
#############################
std::map copy ctor                                            9.872361s        1.000000x
std::unordered_map copy ctor                                  7.298691s        1.352621x
fea::unsigned_map copy ctor                                   5.983556s        1.649915x

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      1.220236s        1.000000x
std::map clear                                                0.883373s        1.381338x
fea::unsigned_map clear                                       0.000000s        12202363.000000x

#########################
Clear 5000000 big objects
#########################
std::map clear                                                7.522813s        1.000000x
std::unordered_map clear                                      3.931792s        1.913329x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 5000000 small objects
############################
std::unordered_map insert                                     0.914929s        1.000000x
std::map insert                                               0.876767s        1.043526x
fea::unsigned_map insert                                      0.090503s        10.109379x

##########################
Insert 5000000 big objects
##########################
std::unordered_map insert                                     6.089507s        1.000000x
std::map insert                                               5.649382s        1.077907x
fea::unsigned_map insert                                      0.878746s        6.929771x

###########################################
Erase 5000000 (all) small objects at random
###########################################
std::map erase                                                5.144582s        1.000000x
std::unordered_map erase                                      1.478963s        3.478506x
fea::unsigned_map erase                                       0.713138s        7.214002x

#########################################
Erase 5000000 (all) big objects at random
#########################################
std::map erase                                               13.530908s        1.000000x
std::unordered_map erase                                     12.252496s        1.104339x
fea::unsigned_map erase                                       2.219848s        6.095421x

##########################################
Insert 5000000 small objects after reserve
##########################################
std::unordered_map insert                                     1.015718s        1.000000x
std::map insert                                               0.797840s        1.273085x
fea::unsigned_map insert                                      0.050668s        20.046657x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               8.838233s        1.000000x
std::unordered_map insert                                     8.816223s        1.002497x
fea::unsigned_map insert                                      0.855444s        10.331756x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.136948s        1.000000x
std::unordered_map iterate & assign                           0.121636s        1.125881x
fea::unsigned_map iterate & assign                            0.055707s        2.458355x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.678697s        1.000000x
std::unordered_map iterate & assign                           0.586725s        1.156754x
fea::unsigned_map iterate & assign                            0.498260s        1.362135x



############################################################
Benchmark using 10000000 random uniform distribution keys, with duplicates
############################################################
Num unique keys : 2454236

###############################
Copy ctor 2454236 small objects
###############################
std::unordered_map copy ctor                                  1.462446s        1.000000x
std::map copy ctor                                            1.135592s        1.287826x
fea::unsigned_map copy ctor                                   0.031856s        45.908594x

#############################
Copy ctor 2454236 big objects
#############################
std::map copy ctor                                            5.308363s        1.000000x
std::unordered_map copy ctor                                  3.184679s        1.666844x
fea::unsigned_map copy ctor                                   1.382318s        3.840190x

###########################
Clear 2454236 small objects
###########################
std::unordered_map clear                                      1.075505s        1.000000x
std::map clear                                                0.833266s        1.290711x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 2454236 big objects
#########################
std::map clear                                                8.188461s        1.000000x
std::unordered_map clear                                      4.399827s        1.861087x
fea::unsigned_map clear                                       0.000000s        81884610.000000x

#############################
Insert 10000000 small objects
#############################
std::map insert                                               7.956933s        1.000000x
std::unordered_map insert                                     1.051418s        7.567813x
fea::unsigned_map insert                                      0.217734s        36.544249x

###########################
Insert 10000000 big objects
###########################
std::map insert                                              16.800703s        1.000000x
std::unordered_map insert                                     9.272153s        1.811953x
fea::unsigned_map insert                                      1.370833s        12.255831x

###########################################
Erase 2454236 (all) small objects at random
###########################################
std::map erase                                                7.420850s        1.000000x
std::unordered_map erase                                      0.873164s        8.498806x
fea::unsigned_map erase                                       0.291501s        25.457410x

#########################################
Erase 2454236 (all) big objects at random
#########################################
std::map erase                                               12.740296s        1.000000x
std::unordered_map erase                                      6.665889s        1.911267x
fea::unsigned_map erase                                       1.086406s        11.727011x

###########################################
Insert 10000000 small objects after reserve
###########################################
std::map insert                                               7.991981s        1.000000x
std::unordered_map insert                                     1.318075s        6.063372x
fea::unsigned_map insert                                      0.232419s        34.386168x

#########################################
Insert 10000000 big objects after reserve
#########################################
std::map insert                                              17.531058s        1.000000x
std::unordered_map insert                                     9.132980s        1.919533x
fea::unsigned_map insert                                      2.013429s        8.707063x

##############################################
Iterate 2454236 small objects and assign value
##############################################
std::map iterate & assign                                     0.233792s        1.000000x
std::unordered_map iterate & assign                           0.048991s        4.772121x
fea::unsigned_map iterate & assign                            0.026915s        8.686227x

############################################
Iterate 2454236 big objects and assign value
############################################
std::map iterate & assign                                     0.555006s        1.000000x
std::unordered_map iterate & assign                           0.251389s        2.207757x
fea::unsigned_map iterate & assign                            0.244752s        2.267626x



#####################################################
Benchmark using 10000000 rand() keys, many duplicates
#####################################################
Num unique keys : 32768

#############################
Copy ctor 32768 small objects
#############################
std::unordered_map copy ctor                                  0.003674s        1.000000x
std::map copy ctor                                            0.003544s        1.036740x
fea::unsigned_map copy ctor                                   0.001120s        3.279771x

###########################
Copy ctor 32768 big objects
###########################
fea::unsigned_map copy ctor                                   0.016727s        1.000000x
std::map copy ctor                                            0.011900s        1.405625x
std::unordered_map copy ctor                                  0.011588s        1.443482x

#########################
Clear 32768 small objects
#########################
std::map clear                                                0.003375s        1.000000x
std::unordered_map clear                                      0.000961s        3.512281x
fea::unsigned_map clear                                       0.000000s        33746.000000x

#######################
Clear 32768 big objects
#######################
std::map clear                                                0.004916s        1.000000x
std::unordered_map clear                                      0.003221s        1.526500x
fea::unsigned_map clear                                       0.000000s        infx

#############################
Insert 10000000 small objects
#############################
std::map insert                                               1.465890s        1.000000x
std::unordered_map insert                                     0.196547s        7.458233x
fea::unsigned_map insert                                      0.039764s        36.865113x

###########################
Insert 10000000 big objects
###########################
std::map insert                                               3.261352s        1.000000x
std::unordered_map insert                                     1.480518s        2.202846x
fea::unsigned_map insert                                      0.569539s        5.726304x

#########################################
Erase 32768 (all) small objects at random
#########################################
std::unordered_map erase                                     79.619657s        1.000000x
std::map erase                                                0.070560s        1128.390097x
fea::unsigned_map erase                                       0.019664s        4048.944361x

#######################################
Erase 32768 (all) big objects at random
#######################################
std::unordered_map erase                                     79.621191s        1.000000x
std::map erase                                                0.078922s        1008.864386x
fea::unsigned_map erase                                       0.027639s        2880.723879x

###########################################
Insert 10000000 small objects after reserve
###########################################
std::map insert                                               1.494652s        1.000000x
std::unordered_map insert                                     0.293070s        5.099983x
fea::unsigned_map insert                                      0.040337s        37.053834x

#########################################
Insert 10000000 big objects after reserve
#########################################
std::map insert                                               3.315303s        1.000000x
std::unordered_map insert                                     2.006184s        1.652542x
fea::unsigned_map insert                                      0.578706s        5.728819x

############################################
Iterate 32768 small objects and assign value
############################################
std::map iterate & assign                                     0.001313s        1.000000x
std::unordered_map iterate & assign                           0.000608s        2.159566x
fea::unsigned_map iterate & assign                            0.000364s        3.609568x

##########################################
Iterate 32768 big objects and assign value
##########################################
std::map iterate & assign                                     0.005058s        1.000000x
std::unordered_map iterate & assign                           0.003380s        1.496553x
fea::unsigned_map iterate & assign                            0.003047s        1.660179x
```