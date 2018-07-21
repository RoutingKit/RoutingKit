# RoutingKit Setup

## Dependencies

RoutingKit was written for and tested on Linux with GCC 4.8. The code base uses some GCC extensions that should also be available in most other compilers such as Clang, IBM, or Intel. A notable exception to this list is VC. We further use POSIX functions. However, we also provide slower fallback functions. These are enabled by defining `ROUTING_KIT_NO_GCC_EXTENSIONS` and `ROUTING_KIT_NO_POSIX`. If you have an operating system not supporting `aligned_alloc` (such as macOS) then define `ROUTING_KIT_NO_ALIGNED_ALLOC`.

RoutingKit has requires zlib to work. Under Debian and derived distributions (such as Ubuntu) you can install them using:

```bash
sudo apt-get install zlib1g-dev
```

## Compilation

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
g++ -I~/RoutingKit/include -L~/RoutingKit/lib -std=c++11 ~/main.cpp -o ~/main -lroutingkit -lz -fopenmp -pthread -lm
```
