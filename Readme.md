# README

## Introduction

Welcome to our project! This README serves as a guide to understanding the project's purpose and how to get started.

## Project Overview

Our project simplifies the compilation process using g++ and provides a specific directory structure for organizing your source files.

## Features

- Simple compilation
- Debug compilation with gdb support
- Linker integration
- Support for static libraries (.a files)
- Multithreading
- Files not intended for compilation are not modified

## Installation

To install and run this project locally, follow these steps:

1. Clone the repository: `git clone https://github.com/Lancelot65/make-custom.git`
2. Add the `custom_make` script to your system's PATH.
3. Organize your project files as follows:




```plaintext
project_root/
├── src/             # Source files
│   ├── main.cpp     # Main file
│   ├── foo.hpp      # Header file
│   └── foo.cpp      # Secondary file
├── bin/             # Executable binaries
│   ├── release/
│   │   └── foo.exe  # Release executable
│   └── debug/
│       └── foo.exe  # Debug executable (use with gdb)
└── obj/             # Object files
    ├── release/
    │   ├── main.o   # Main object file (release)
    │   └── foo.o    # Secondary object file (release)
    └── debug/
        ├── main.o   # Main object file (debug)
        └── foo.o    # Secondary object file (debug)
```


Note: If the `bin` and `obj` directories do not exist, they will be created automatically.

### sublime text : 

create a new build system

```
{
    "cmd": ["custom_make.exe", "-lcurl", "-lsqlite3", "&&","bin\\release\\a.exe<inputf.in>outputf.out"],
    "shell":true,
    "working_dir":"$folder",
}
```

Create file inputf.in and outputf.out and open it: Ctrl + 3 and view/Groups/Max columns: 2. Open these two new files in the two new windows.

## Usage

Once the project is installed, you can use the following commands:

- `make_custom`: Compile the project.
- `make_custom -d`: Compile the project with debugging symbols for gdb.
- `make_custom -d -lsqlite3 -lcurl`: Compile the project with additional linker libraries (e.g., sqlite3, curl).

## Contact

If you have any questions or concerns regarding the project, please create an issue on GitHub.

Thank you for using our project!