# RoutingKit Setup

# Dependencies

RoutingKit was written for and tested on Linux with GCC 4.8. The current version uses features that go beyond standard C++. We expect that any GCC compatible compiler (such as Clang) should work on any Unix compatible OS (such as Mac). Windows will most likely not work. Maybe MinGW works.

RoutingKit has two dependencies on external libraries: zlib and Google-Protocol-Buffers are needed.

# Compilation

Run the following Bash-Code to compile the library from source. Currently there are no binary distributions.

```bash
git clone https://github.com/RoutingKit/RoutingKit.git
cd RoutingKit
make
```

The `include` subdirectory contains the C++ header files. The `lib` subdirectory contains two library files: `libroutingkit.so` if you want to link dynamically and `libroutingkit.a` in case you want to link statically. We recommend to use the former, i.e., the shared library.

Suppose that you cloned the repository into your home directory, i.e., `~/RoutingKit` and have copied the code from the main page into a file called `~/main.cpp` then you can dynamically link against it as following:

```bash
g++ -I~/RoutingKit/include -L~/RoutingKit/lib -std=c++11 ~/main.cpp -o ~/main -lroutingkit
```

You can then run the executable as following:

```bash
export LD_LIBRARY_PATH=~/RoutingKit/lib
~/main
```

Instead of using `-I` and `-L` and `LD_LIBRARY_PATH` you can of course copy the include files and `libroutingkit.so` to some directory on your default search paths.

To use the static library you must first remove the `libroutingkit.so` file. Once that is done you can compile the file as following:

```bash
g++ -I~/RoutingKit/include -L~/RoutingKit/lib -std=c++11 ~/main.cpp -o ~/main -lroutingkit -lprotobuf -lz -fopenmp -pthread -lm
```
