GLUS - Modern OpenGL Utilities
===============================

GLUS is a cross-platform utility library for OpenGL, OpenGL ES and OpenVG.
It provides window/context creation, math helpers, image and model loading,
shader utilities and more. GLUS lives in its own repository and is used by the
[McNopper/OpenGL](https://github.com/McNopper/OpenGL) examples.

## Changelog

### v1.1.1

Security and correctness fixes across parsers, math, and platform layers:
- **TGA loader**: bounded the RLE packet against the remaining output buffer (heap overflow); validated colour-map indices; skipped the Image ID field; validated `colorMapEntrySize`; endian-safe dimension decoding.
- **HDR (Radiance) loader**: bounded the resolution-line read (stack overflow); checked `sscanf` return and validated dimensions (uninitialised variable / integer overflow); fixed signed-char comparison breaking new-style RLE for common widths.
- **glTF loader**: call `cgltf_validate()` (arbitrary read/write primitives); clamp accessor component copy loop (heap overflow); validate morph-target and animation sampler sizes; add recursion depth guard; check all allocations; key texture cache on sRGB flag; reject path-traversal URIs.
- **Wavefront OBJ/MTL loader**: bounded all `sscanf` conversions (`%31s` / `%255s`); bounds-checked face indices with relative-index resolution; fixed unsigned underflow in fan triangulation; NULL-guarded material pointer; bounded struct-field string copies.
- **Math**: guard Perlin `% amplitude` against zero (SIGFPE); fix tangent-loop out-of-bounds write; add quaternion slerp shortest-path; fix vector reflect aliasing; fix disc UV formula; clamp `acosf`/`asinf` domains; fix oriented-box inverse rotation order.
- **Shape generators**: tighten `glusShapeCreateDomef` minimum; guard degenerate-UV tangent determinant; validate adjacency edge indices.
- **Program**: fix shader/program leaks on compile/link failure paths; handle zero-length info logs.
- **Window/OS**: fix `tolower` on GLFW key codes (UB); fix button-release XOR (phantom held button); update cached dimensions on resize; check `XOpenDisplay` result; fix X resource leaks; fix `glusExtensionIsSupported` on core-profile contexts.
- **Memory**: raise `glus_memory_nodm` alignment to `max_align_t`.
- **Files**: fix `fopen(NULL)` UB; fix `ftell` truncation into 32-bit `GLUSint`; treat short writes as errors.

### v1.1.0

- Added a **glTF 2.0 core loader** module (`glus_gltf.h` / `glus_gltf.c`):
  parses `.glb` / `.gltf` via cgltf and uploads render-ready primitives that
  match the GLUS glTF PBR shaders. Covers the full core spec — scenes / node
  hierarchy (TRS and matrix), meshes (all primitive modes), PBR
  metallic-roughness materials, textures / samplers / images (external, GLB
  embedded and `data:` URIs), sparse accessors, skins, TRS animation
  (STEP / LINEAR / CUBICSPLINE), morph targets and cameras. Khronos / vendor
  extensions are **not** interpreted by GLUS; the parsed cgltf tree stays
  reachable through `glusGltfGetCgltfData()` so each application can read the
  extension data it needs (e.g. `KHR_gaussian_splatting`) itself.
- GLUS now fetches **cgltf** and **stb** (stb_image) and exposes their include
  directories publicly, and defines **`GLUS_SHADER_DIR`** (the absolute path to
  GLUS' `shader/` directory) so applications load the glTF / IBL shaders
  straight from this source tree.

### v1.0.1

- Fixed two sign errors in the band-3 spherical-harmonics rotation
  (`glusSHBuildRotation3f`) that made the band-3 matrix non-orthogonal. Verified
  (orthogonality, group homomorphism, equivariance) against
  [andrewwillmott/sh-lib](https://github.com/andrewwillmott/sh-lib).
- Documented the SH rotation basis/convention in `glus_sh.h` (standard real SH;
  bridge to the Inria/3DGS/glTF sign convention via the Condon-Shortley phase).

### v1.0.0

- Initial public release.

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
- **cgltf v1.14** - https://github.com/jkuhlmann/cgltf - glTF 2.0 parser (used by the glTF loader)
- **stb** (stb_image) - https://github.com/nothings/stb - image decoding (used by the glTF loader)

When GLUS is added to a parent project that already provides these targets, the
existing ones are reused instead of being fetched again.

## Third-Party Licenses

GLUS is MIT licensed (see [LICENSE](LICENSE)). The desktop OpenGL build depends
on the following third-party projects, which are fetched automatically and are
**not** part of the GLUS source tree:

- **GLFW 3.4** - https://github.com/glfw/glfw - zlib/libpng license
- **GLEW 2.2.0** (via [glew-cmake](https://github.com/Perlmint/glew-cmake)) - MIT / BSD
- **cgltf v1.14** - https://github.com/jkuhlmann/cgltf - MIT (Copyright (c) 2018-2021 Johannes Kuhlmann). Compiled into the GLUS static library through `CGLTF_IMPLEMENTATION` by the glTF loader (`glus_gltf.c`).
- **stb / stb_image** - https://github.com/nothings/stb - MIT / Public Domain (Copyright (c) 2017 Sean Barrett). Compiled into the GLUS static library through `STB_IMAGE_IMPLEMENTATION` by the glTF loader (`glus_gltf.c`).

Because cgltf and stb_image are compiled into `GLUS`, their copyright and
permission notices are redistributed with any binary that links GLUS, as
required by their licenses.

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
