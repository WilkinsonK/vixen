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
This project, was first ported from the prototype into **C++** when
C++20 was available. Love it or hate it, the author of Vixen likes
to make use of the latest tools when they are available.

#### Meson Build System
Development of this project requires the use of the **Meson** build
system. Please see their [quickstart](https://mesonbuild.com/Quick-guide.html)
guide for details on how install and basic usage.

```bash
$ which meson
/path/to/meson
```

### Building Vixen
Using `vixenw.sh` you will have two tasks, as options, available to you:

- build; sets up the project for debugging, and compiles binaries.
- build_release; sets up the project with release optimizations and compiles
  binaries.
