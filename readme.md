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
std::unordered_map copy ctor                                  1.078713s        1.000000x
std::map copy ctor                                            0.549252s        1.963968x
fea::unsigned_map copy ctor                                   0.072672s        14.843664x

#############################
Copy ctor 5000000 big objects
#############################
std::unordered_map copy ctor                                  4.891465s        1.000000x
std::map copy ctor                                            4.467373s        1.094931x
fea::unsigned_map copy ctor                                   2.837853s        1.723650x

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      0.412637s        1.000000x
std::map clear                                                0.252846s        1.631971x
fea::unsigned_map clear                                       0.000000s        4126369.000000x

#########################
Clear 5000000 big objects
#########################
std::map clear                                                1.470950s        1.000000x
std::unordered_map clear                                      1.450096s        1.014381x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 5000000 small objects
############################
std::unordered_map insert                                     0.891492s        1.000000x
std::map insert                                               0.874460s        1.019477x
fea::unsigned_map insert                                      0.081441s        10.946434x

##########################
Insert 5000000 big objects
##########################
std::unordered_map insert                                     3.587467s        1.000000x
std::map insert                                               3.193123s        1.123498x
fea::unsigned_map insert                                      0.800107s        4.483735x

###########################################
Erase 5000000 (all) small objects at random
###########################################
std::map erase                                                5.051563s        1.000000x
std::unordered_map erase                                      1.370136s        3.686907x
fea::unsigned_map erase                                       0.711962s        7.095271x

#########################################
Erase 5000000 (all) big objects at random
#########################################
std::map erase                                               11.221583s        1.000000x
std::unordered_map erase                                      5.874402s        1.910251x
fea::unsigned_map erase                                       2.200090s        5.100511x


##########################################
Insert 5000000 small objects after reserve
##########################################
std::unordered_map insert                                     0.962487s        1.000000x
std::map insert                                               0.837309s        1.149500x
fea::unsigned_map insert                                      0.088120s        10.922504x

########################################
Insert 5000000 big objects after reserve
########################################
std::unordered_map insert                                     6.384061s        1.000000x
std::map insert                                               4.738685s        1.347222x
fea::unsigned_map insert                                      0.721486s        8.848490x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.124778s        1.000000x
std::unordered_map iterate & assign                           0.121880s        1.023779x
fea::unsigned_map iterate & assign                            0.056032s        2.226912x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.585910s        1.000000x
std::unordered_map iterate & assign                           0.555040s        1.055616x
fea::unsigned_map iterate & assign                            0.525356s        1.115263x



########################################################
Benchmark using linear keys, 5000000 to 0, no duplicates
########################################################
Num unique keys : 5000000

###############################
Copy ctor 5000000 small objects
###############################
std::unordered_map copy ctor                                  2.540170s        1.000000x
std::map copy ctor                                            2.007614s        1.265268x
fea::unsigned_map copy ctor                                   0.072129s        35.216940x

#############################
Copy ctor 5000000 big objects
#############################
std::map copy ctor                                            9.541756s        1.000000x
std::unordered_map copy ctor                                  6.806677s        1.401823x
fea::unsigned_map copy ctor                                   2.753311s        3.465556x

###########################
Clear 5000000 small objects
###########################
std::unordered_map clear                                      1.080756s        1.000000x
std::map clear                                                0.819796s        1.318323x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 5000000 big objects
#########################
std::map clear                                                7.036801s        1.000000x
std::unordered_map clear                                      3.803246s        1.850209x
fea::unsigned_map clear                                       0.000000s        70368011.000000x

############################
Insert 5000000 small objects
############################
std::unordered_map insert                                     0.901515s        1.000000x
std::map insert                                               0.849191s        1.061616x
fea::unsigned_map insert                                      0.050741s        17.767133x

##########################
Insert 5000000 big objects
##########################
std::unordered_map insert                                     6.528842s        1.000000x
std::map insert                                               6.367369s        1.025359x
fea::unsigned_map insert                                      0.697547s        9.359721x

###########################################
Erase 5000000 (all) small objects at random
###########################################
std::map erase                                                5.142022s        1.000000x
std::unordered_map erase                                      1.491811s        3.446832x
fea::unsigned_map erase                                       0.703518s        7.309017x

#########################################
Erase 5000000 (all) big objects at random
#########################################
std::map erase                                               13.314276s        1.000000x
std::unordered_map erase                                     12.217892s        1.089736x
fea::unsigned_map erase                                       2.213079s        6.016177x


##########################################
Insert 5000000 small objects after reserve
##########################################
std::unordered_map insert                                     0.991927s        1.000000x
std::map insert                                               0.833040s        1.190732x
fea::unsigned_map insert                                      0.049761s        19.933659x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               8.813306s        1.000000x
std::unordered_map insert                                     8.373798s        1.052486x
fea::unsigned_map insert                                      0.716682s        12.297380x

##############################################
Iterate 5000000 small objects and assign value
##############################################
std::map iterate & assign                                     0.136340s        1.000000x
std::unordered_map iterate & assign                           0.120990s        1.126870x
fea::unsigned_map iterate & assign                            0.054616s        2.496362x

############################################
Iterate 5000000 big objects and assign value
############################################
std::map iterate & assign                                     0.591329s        1.000000x
std::unordered_map iterate & assign                           0.537788s        1.099557x
fea::unsigned_map iterate & assign                            0.484514s        1.220458x



############################################################
Benchmark using 10000000 random uniform distribution keys, with duplicates
############################################################
Num unique keys : 2454425

################################
Copy ctor 10000000 small objects
################################
std::unordered_map copy ctor                                  1.509087s        1.000000x
std::map copy ctor                                            1.150092s        1.312144x
fea::unsigned_map copy ctor                                   0.034461s        43.791282x

##############################
Copy ctor 10000000 big objects
##############################
std::map copy ctor                                            5.067774s        1.000000x
std::unordered_map copy ctor                                  3.021655s        1.677152x
fea::unsigned_map copy ctor                                   1.250526s        4.052513x

############################
Clear 10000000 small objects
############################
std::unordered_map clear                                      1.048098s        1.000000x
std::map clear                                                0.844290s        1.241396x
fea::unsigned_map clear                                       0.000000s        infx

##########################
Clear 10000000 big objects
##########################
std::map clear                                                8.310714s        1.000000x
std::unordered_map clear                                      4.380595s        1.897166x
fea::unsigned_map clear                                       0.000000s        infx

#############################
Insert 10000000 small objects
#############################
std::map insert                                               7.864493s        1.000000x
std::unordered_map insert                                     1.021655s        7.697798x
fea::unsigned_map insert                                      0.214692s        36.631598x

###########################
Insert 10000000 big objects
###########################
std::map insert                                              16.612631s        1.000000x
std::unordered_map insert                                     9.159119s        1.813780x
fea::unsigned_map insert                                      1.371795s        12.110140x

###########################################
Erase 2454425 (all) small objects at random
###########################################
std::map erase                                                7.514489s        1.000000x
std::unordered_map erase                                      0.891329s        8.430654x
fea::unsigned_map erase                                       0.281938s        26.652966x

#########################################
Erase 2454425 (all) big objects at random
#########################################
std::map erase                                               13.355291s        1.000000x
std::unordered_map erase                                      6.455035s        2.068973x
fea::unsigned_map erase                                       1.072845s        12.448483x


###########################################
Insert 10000000 small objects after reserve
###########################################
std::map insert                                               7.837540s        1.000000x
std::unordered_map insert                                     1.093446s        7.167746x
fea::unsigned_map insert                                      0.232442s        33.718318x

#########################################
Insert 10000000 big objects after reserve
#########################################
std::map insert                                              17.372435s        1.000000x
std::unordered_map insert                                     9.153488s        1.897903x
fea::unsigned_map insert                                      1.993549s        8.714326x

##############################################
Iterate 2454425 small objects and assign value
##############################################
std::map iterate & assign                                     0.221212s        1.000000x
std::unordered_map iterate & assign                           0.046939s        4.712731x
fea::unsigned_map iterate & assign                            0.026746s        8.270837x

############################################
Iterate 2454425 big objects and assign value
############################################
std::map iterate & assign                                     0.548973s        1.000000x
std::unordered_map iterate & assign                           0.260803s        2.104934x
fea::unsigned_map iterate & assign                            0.236233s        2.323863x



#####################################################
Benchmark using 10000000 rand() keys, many duplicates
#####################################################
Num unique keys : 32768

################################
Copy ctor 10000000 small objects
################################
std::unordered_map copy ctor                                  0.003960s        1.000000x
std::map copy ctor                                            0.003531s        1.121534x
fea::unsigned_map copy ctor                                   0.001193s        3.320030x

##############################
Copy ctor 10000000 big objects
##############################
fea::unsigned_map copy ctor                                   0.016647s        1.000000x
std::map copy ctor                                            0.012358s        1.347093x
std::unordered_map copy ctor                                  0.011738s        1.418223x

############################
Clear 10000000 small objects
############################
std::map clear                                                0.002324s        1.000000x
std::unordered_map clear                                      0.001093s        2.125984x
fea::unsigned_map clear                                       0.000000s        infx

##########################
Clear 10000000 big objects
##########################
std::map clear                                                0.005462s        1.000000x
std::unordered_map clear                                      0.002938s        1.859248x
fea::unsigned_map clear                                       0.000000s        54621.000000x

#############################
Insert 10000000 small objects
#############################
std::map insert                                               1.448252s        1.000000x
std::unordered_map insert                                     0.190856s        7.588212x
fea::unsigned_map insert                                      0.040489s        35.769200x

###########################
Insert 10000000 big objects
###########################
std::map insert                                               3.219489s        1.000000x
std::unordered_map insert                                     1.472666s        2.186164x
fea::unsigned_map insert                                      0.568267s        5.665452x

#########################################
Erase 32768 (all) small objects at random
#########################################
std::unordered_map erase                                     78.574991s        1.000000x
std::map erase                                                0.070487s        1114.741274x
fea::unsigned_map erase                                       0.019312s        4068.692224x

#######################################
Erase 32768 (all) big objects at random
#######################################
std::unordered_map erase                                     78.965765s        1.000000x
std::map erase                                                0.076899s        1026.875018x
fea::unsigned_map erase                                       0.027366s        2885.490096x


###########################################
Insert 10000000 small objects after reserve
###########################################
std::map insert                                               1.463999s        1.000000x
std::unordered_map insert                                     0.280501s        5.219226x
fea::unsigned_map insert                                      0.039720s        36.857507x

#########################################
Insert 10000000 big objects after reserve
#########################################
std::map insert                                               3.323736s        1.000000x
std::unordered_map insert                                     2.033208s        1.634725x
fea::unsigned_map insert                                      0.575729s        5.773095x

############################################
Iterate 32768 small objects and assign value
############################################
std::map iterate & assign                                     0.001294s        1.000000x
std::unordered_map iterate & assign                           0.000618s        2.092173x
fea::unsigned_map iterate & assign                            0.000356s        3.637335x

##########################################
Iterate 32768 big objects and assign value
##########################################
std::map iterate & assign                                     0.005063s        1.000000x
std::unordered_map iterate & assign                           0.003239s        1.563237x
fea::unsigned_map iterate & assign                            0.003181s        1.591744x
```