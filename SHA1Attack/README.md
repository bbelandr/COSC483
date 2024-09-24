# Compilation Steps
Before running any of these makefile scripts, make sure that an exe/ folder exists in the current working directory.
- The command `make run` simply should print everything to the screen. 
- The command `make output` places all of the output into output.txt. 

These commands will also show graphs of all iterations for PreImage attacks and Collision attacks.

Everything should already be compiled for a Linux machine but if things still are not working these next steps might be worth looking into.

**Ensure that openssl is compiled for your system:**
- run these commands if nothing is working:
```
./openssl/Config
cd openssl && make
```

**Ensure that matplotplusplus is compiled for your system:**
- Make sure these steps from the matplotplusplus README are completed (have cmake installed)
## MatPlotPlusPlus compilation steps

Include FetchContent in your CMake build script:

```cmake
include(FetchContent)
```

Declare the source for the contents:

```cmake
FetchContent_Declare(matplotplusplus
        GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
        GIT_TAG origin/master) # or whatever tag you want
```

Let CMake download the repository and include it as a subdirectory.

```cmake
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
```

When creating your executable, link the library to the targets you want:

```
add_executable(my_target main.cpp)
target_link_libraries(my_target PUBLIC matplot)
```
