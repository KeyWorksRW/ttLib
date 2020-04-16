# Contributing

We welcome your contributions to the code base. Following the guidelines listed below will make it easier for your code to be merged into the code base.

## Pull Requests

Your pull requests are welcome. Please note that all pull requests, except for translations and user documentation, need to be attached to an issue. Opening an issue beforehand allows the administrators and the community to discuss bugs and enhancements before work begins, preventing wasted effort.

Pull requests regarding enhancement issues must first be approved by one of project's administrators before being merged into the project. An approved enhancement issue will have the label **Accepted**.

## Coding Style

It's important to avoid creating a difference when the only thing that has changed is formatting. Using **clang-format** ensures that coding style is consistent throughout the project.

There is a **.clang-format** file in the root of the project. If your editor supports clang formatting, then your editor can format your code to match KeyWorks' coding style. If your editor doesn't support clang-foramt directly, then you should run `clang-format -i file`, replacing _file_ with the name of every file you have modified.

Before checking in changes, please run `git clang-format` to confirm that your files have been formatted with clang-format.

If you don't have **clang-format.exe** you can download it as part of the [LLVM](http://releases.llvm.org/download.html) (CLANG) project.
