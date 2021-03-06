# Building ttLib.lib

At an absolute minimum, you will need to have the following in your PATH:

- [Ninja.exe](https://github.com/ninja-build/ninja)

- Either of the following compilers:
  - [CLANG](https://clang.llvm.org/) (version 7 or higher)
  - MSVC

Before building, you will need to start one of the Visual Studio command shells, preferably one that builds 64-bit targets. From within that shell, you can use the following commands to build the library:

	cd src
	nmake

By default, this will build a 64-bit release version of the library (lib/ttLib.lib).

If you have MINGW installed and both MINGW and the **clang-cl** compiler are in your PATH, you can use the following commands to build the library:

	cd src
	mingw32-make

## Building ttLib.lib

While ttLib.lib is designed to be cross-platform, currently it can only be built under Windows. Cross platform building is planned, but not ETA as of yet.
