# Vixen Programming Language

**NOTICE**: THIS PROJECT DOES NOT PRODUCE A COMPLETE LANGUAGE.

This project is meant to be an experimental exercise, to better
strengthen the author's understanding of programming and computer
science as a whole.

## Development
For the convenience of the user wishing to contribute to this project,
a wrapper script has been defined as `vixenw.sh` to perform certain
tasks the recommended way.

```bash
$ ./vixenw.sh
usage: vixenw.sh [task(s)...]

Vixen Build Wrapper
A convenience script for managing this project.

Available Tasks:
build         Build this project executable, libs & tests.
build_release Build a release version of executable, libs & tests
clean         Remove files generated from build system.
test          Run all tests.
```

### Requirements

#### C++ Standard 20
This project requires the **C++** 20 standard or
higher using **GCC** (13.2.1).

```bash
$ gcc --version
gcc (GCC) 13.2.1 20230728 (Red Hat 13.2.1-1)
```

#### Meson Build System
Development of this project requires the use of the **Meson** (v1.2.1)
build system. Please see their [quickstart](https://mesonbuild.com/Quick-guide.html)
guide for details on how install and basic usage.

```bash
$ meson --version
1.2.1
```

### Building Vixen
Using `vixenw.sh` you will have two tasks, as options, available to you:

- build; sets up the project for debugging, and compiles binaries.
- build_release; sets up the project with release optimizations and compiles
  binaries.
