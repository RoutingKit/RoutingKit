# RoutingKit Setup

## Dependencies

RoutingKit was written for and tested on Linux with GCC 4.8. The code base uses some GCC extensions that should also be available in most other compilers such as Clang, IBM, or Intel. A notable exception to this list is VC. We further use POSIX functions. However, we also provide slower fallback functions. These are enabled by defining `ROUTING_KIT_NO_GCC_EXTENSIONS` and `ROUTING_KIT_NO_POSIX`. If you have an operating system not supporting `aligned_alloc` (such as macOS) then define `ROUTING_KIT_NO_ALIGNED_ALLOC`. RoutingKit makes use of some networking functions to translate integers in network byte order into system byte order. You can avoid this dependency by defining `ROUTING_KIT_ASSUME_LITTLE_ENDIAN`.

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

# Building under MSYS2/MinGW

We do not recommend using Windows or MinGW as a development platform. However, if for some reason you must do this, then these instructions might be helpful.  

The first step consists of installing MSYS2. Next, we need to install the right build tools. These are:

```bash
pacman -S mingw-w64-x86_64-toolchain git
```

We then clone RoutingKit as follows:

```bash
git clone https://github.com/RoutingKit/RoutingKit.git

```

We need to set two compilation options to make RoutingKit build under MSYS2. For this, open the file `Makefile` and add `-DROUTING_KIT_ASSUME_LITTLE_ENDIAN`, `-DROUTING_KIT_NO_ALIGNED_ALLOC`, and `-DROUTING_KIT_NO_POSIX` to the `CFLAGS` variable. Next, we build only the static library version of RoutingKit. You can do this as follows:

```bash
make lib/libroutingkit.a
```

It should also be possible to get the DLL version to work with some file renaming and tweaking of the make file. Further, you must make sure that all paths during the execution of your executable are correct. Our recommendation is to avoid all this and go the simpler route of just statically linking the library into every executable. 

Strictly speaking, `ROUTING_KIT_NO_POSIX` should not be necessary and RoutingKit will build without it. However, without it, we have witnessed problems in the PBF reader. As these do not occur under Linux, we suspect that there is a problem with the MinGW POSIX compatibility layer.

Getting the Windows vs Linux paths right in MSYS2 is tricky. We therefore recommend to install RoutingKit MSYS2 system-wide. You do this by copying files into the appropriate places as follows:

```bash
cp -r include/routingkit /mingw64/include
cp lib/libroutingkit.a /mingw64/lib
```

Now, RoutingKit should be setup.


Copy the example code from the landing page into a file `main.cpp`. We can now build it as follows:

```bash
g++ main.cpp -static -pthread -fopenmp -lroutingkit -lz -o prog.exe 
```

The produced executable `prog.exe` is a self-contained Windows executable that only depends on basic Windows infrastructure. It has no dependencies on MSYS2 or any MinGW libraries. As everything is statically linked the executable is quite large. We can reduce its size by striping everything from it that was linked but is not necessary as follows:

```bash
strip prog.exe
```

Finally, let us test whether it works.

```bash
curl http://download.geofabrik.de/europe/luxembourg-latest.osm.pbf -o file.pbf
./prog.exe
```

After some startup time, you can enter a pair of geographic positions and the program responds with the distance.
