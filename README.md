GLUS - Modern OpenGL Utilities
===============================

GLUS is a cross-platform utility library for OpenGL, OpenGL ES and OpenVG.
It provides window/context creation, math helpers, image and model loading,
shader utilities and more. GLUS lives in its own repository and is used by the
[McNopper/OpenGL](https://github.com/McNopper/OpenGL) examples.

## Prerequisites

- **CMake 3.14 or higher** - [Download CMake](https://cmake.org/download/)
- **C/C++ Compiler**:
  - Windows: Visual Studio 2013 or newer (MSVC)
  - Linux: GCC or Clang
  - macOS: Xcode Command Line Tools (Clang)
- **Git** - For fetching dependencies
- **OpenGL 3.2+** compatible graphics driver

## Building (standalone)

```bash
cmake -S . -B build
cmake --build build --config Release
```

This produces the static library `lib/GLUS.lib` (or `lib/libGLUS.a` on
Linux/macOS).

## Using GLUS in another project

GLUS can be consumed directly via CMake `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    glus
    GIT_REPOSITORY https://github.com/McNopper/GLUS.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(glus)

target_link_libraries(your_target PRIVATE GLUS)
```

## Dependencies

For the desktop OpenGL build, the following are automatically fetched and built
via CMake `FetchContent` - no manual installation required:

- **GLFW 3.4** - https://github.com/glfw/glfw - window and input management
- **GLEW 2.2.0** - https://github.com/Perlmint/glew-cmake - OpenGL extension loading

When GLUS is added to a parent project that already provides these targets, the
existing ones are reused instead of being fetched again.

## OpenGL ES / OpenVG

OpenGL ES 2.0/3.0/3.1 and OpenVG builds are selected via the `OpenGL` CMake
variable (e.g. `-DOpenGL=ES2`, `-DOpenGL=ES`, `-DOpenGL=ES31`). An Android NDK
build is available under `Android/jni`.

## Documentation

API documentation can be generated with [Doxygen](https://www.doxygen.nl/):

```bash
cd docs
doxygen Doxyfile
```

The HTML output is written to `docs/html`.

---

Yours Norbert Nopper
