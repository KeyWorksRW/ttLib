# ttLib

This repository contains several classes and functions that have been used in most of the KeyWorks Software projects since the company was formed in 1998. As such, it will appear as a sub-module in most repositories owned by **KeyWorks Software**.

The source code for the library is split into two primary directories. The **src/** directory contains code that should work on any platform. The **src/winsrc/** directory contains code that can only be used when compiling for Windows.

The easiest way to build the libraries is to run the following commands:

```
    cmake -G "Ninja Multi-Config" . -B build
    cmake.exe --build build --config Release
```

If you wish to contribute to the project, please refer to the [Contributing](CONTRIBUTING.md) documentation.

## License

There are multiple licenses used by this project.

All KeyWorks Software contributions use a Apache License 2.0 [LICENSE](LICENSE).

The **ttdib** and **ttshadebtn** header and source files use a CPOL [LICENSE](license_code_project.htm)

[![CodeQL](https://github.com/KeyWorksRW/ttLib/workflows/CodeQL/badge.svg)](https://github.com/KeyWorksRW/ttLib/actions?query=workflow:"CodeQL")
