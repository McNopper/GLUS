# AGENTS.md - GLUS working contract

GLUS is a cross-platform C utility library for OpenGL, OpenGL ES and OpenVG:
windowing and context setup, shader/program construction, procedural shapes,
Wavefront OBJ and glTF 2.0 loading, TGA/HDR/PKM image (de)code, and a math core
(column-major 4x4 matrices, quaternions, planes, boxes, intersections, ray
tracing, spherical harmonics, FFT, Perlin noise).

**Stability mandate.** This library is long established, widely depended on and
frequently vendored by downstream projects. Treat every change to `src/` as a
change to somebody else's build. Concretely:

- No API or ABI changes without an explicit decision. Renaming a `glus*`
  symbol, changing a struct layout (`GLUSshape`, `GLUSimage`, `GLUStgaimage`,
  `GLUSmaterial`, ...) or widening/narrowing a `GLUS*` typedef is a breaking
  change to every consumer, including prebuilt ones.
- No behaviour changes outside the specific defect being fixed. If a fix would
  alter results that are currently *correct*, stop and discuss it.
- Never reformat. Formatting and style are authored, not derived - there is no
  formatter config here on purpose, and tooling is configured `FormatStyle:
  none` so nothing rewrites a line you did not intend to touch.
- Prefer the smallest diff that removes the defect. No drive-by refactors, no
  renames, no "while I was here".

## Build (standalone)

```
cmake -S . -B build/ninja -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/ninja
```

Also buildable through MSVC or any other generator. This project is consumed
via `add_subdirectory()`/FetchContent by the example projects; the build policy
in `cmake/` is registered **only when GLUS is the top-level project**, so being
vendored never disturbs the host build.

Build variants (see the header comment in `CMakeLists.txt`): `OpenGL=ES2`,
`OpenGL=ES3`, `OpenGL=ES31` select the OpenGL ES flavour; `SoC=iMX6` selects
the i.MX6 platform. `Android/jni/Android.mk` covers the NDK build.

## Finding and eliminating bugs

The goal of the tooling below is bug detection. All of it is optional at build
time - a clean checkout with none of these tools installed builds exactly as
before.

| Layer | Command | Gate |
|---|---|---|
| Compiler warnings | part of every build | `ENABLE_WERROR=ON` makes them fatal |
| cppcheck | `cmake --build build/ninja --target cppcheck` | exits non-zero on findings |
| cppcheck (exhaustive) | `... --target cppcheck-strict` | opt-in, adds style + inconclusive |
| clang-tidy | `python tools/check_tidy.py --build-dir build/ninja` | `WarningsAsErrors` in `.clang-tidy` |
| Sanitizers | `-DENABLE_SANITIZER=address,undefined` then run the examples | runtime faults |

`python tools/check_tidy.py --list` shows which translation units would be
analysed; `--help` for the rest. It is read-only and never rewrites source.

Two switches in `cmake/warnings.cmake`:

- `ENABLE_WERROR` (default **OFF**) - promotes warnings to errors. Leave it off
  while a baseline is being cleared; turn it on once the tree is clean so the
  gate holds.
- `ENABLE_SANITIZER` (default off; GCC/Clang only) - `address`,
  `undefined`, `address,undefined` or `thread`. This is the highest-yield lane
  for this library, because the file parsers and mesh generators take
  dimensions and counts straight from untrusted input.

**Suggested bug-elimination cycle.** Build with `ENABLE_SANITIZER=address,undefined`,
run `cppcheck`, run `check_tidy.py`, then drive the decoders with malformed
input. Fix defects in that order of confidence (crash/overflow first, then
undefined behaviour, then wrong results), re-running the full set after each
change. When anything broken is found at any point, drop back to bugs
immediately.

Suppression policy: `cppcheck.supp` stays short and every entry carries a
justification comment. Prefer an inline `// cppcheck-suppress <id>` for a
one-off local finding over a global entry. Never suppress to make a gate go
green - fix the finding or justify it in writing.

## Layout

| Path | Contents |
|---|---|
| `src/` | all implementation, one module per file (`glus_*.c`) |
| `src/GLUS/` | the public API headers (`glus.h` umbrella lives in `src/GL/`, `src/GLES2/`, `src/GLES3/`, `src/VG/`) |
| `src/GL/`, `src/GLES2/`, `src/GLES3/`, `src/VG/` | per-API umbrella headers selecting the `GLUS*` typedef sets |
| `shader/` | glTF PBR and IBL shaders shipped with the library (path exposed as `GLUS_SHADER_DIR`) |
| `Android/` | NDK build (`jni/Android.mk`, `Application.mk`) |
| `cmake/` | build policy: `warnings.cmake`, `cppcheck.cmake` |
| `tools/` | `check_tidy.py` - the clang-tidy execution lane |

`src/` is on the public include path, so `#include <GLUS/glus_xxx.h>` is how
consumers reach the API.

## Defect classes this code is prone to

Look here first; these are the patterns that have historically produced bugs
in this kind of code, and the analyser configuration is tuned for them.

- **Integer overflow in size and offset arithmetic.** `GLUSuint`/`GLUSint` are
  32-bit. Expressions like `pixelsRead * bitsPerPixel / 8`,
  `width * height * sizeof(...)`, `(rows + 1) * (columns + 1)` wrap silently
  and then size a `malloc` far too small while the fill loop still runs the
  full iteration count. Compute sizes in `size_t` and check the product
  *before* it truncates.
- **Truncation into narrower fields.** `GLUSushort` image dimensions, `(GLUSushort)width`
  casts, `GLUSint` pixel counters. A cast that narrows is a bug unless the
  range is checked on the line above.
- **Unvalidated parser input.** `sscanf` return values ignored, `strncmp(prefix, 1)`
  matching longer identifiers, per-line record counts driving allocations.
  Malformed `.obj`/`.mtl`/`.tga`/`.hdr`/`.pkm`/`.gltf` must be survivable.
- **Leaks on error paths.** Every early `return GLUS_FALSE` / `return NULL` in
  a loader must release what it already allocated.
- **Uninitialized values on failure paths.** A struct handed to a caller must
  be fully written even when the call fails, or the caller reads garbage.
- **Ignored `glusVector3Normalizef` return.** It returns `GLUS_FALSE` and
  leaves the vector untouched for a zero-length input; ignoring that yields a
  degenerate matrix/plane or a non-unit quaternion rather than a diagnosable
  error. `asinf`/`acosf` arguments need a clamp to `[-1, 1]`.
- **`malloc(0)` divergence.** `glus_memory.c` and `glus_memory_nodm.c` disagree
  on it; do not let a zero size reach the allocator.

## Conventions

- **Production code discipline**: everything here is production code and the
  same bar applies everywhere - clean code and clean architecture, **no
  hacks**. When a shortcut tempts (content sniffing, magic numbers, hidden
  special cases, duplicated blocks, indirections that silently die), solve it
  properly instead: explicit structure and shared helpers.
- **Work priority is a strict cycle**: **bugs first**, then refactoring or new
  features. When anything broken is found at any point, drop back to bugs
  immediately and re-enter the cycle from the top.
- **Dependency licence gate**: before adding any third-party code, check the
  licence for closed-source commercial use. MIT / BSD / zlib / Apache-2.0 /
  Unlicense pass; copyleft (GPL/LGPL/SSPL) and non-commercial (CC-BY-NC,
  CC-BY-NC-ND) fail; unclear means ask. Record the result in
  `THIRD-PARTY.md` in the same change. The existing `stb` pin is a moving
  `master` ref - pin it to a commit hash before any release.
- **Comments explain *why*, not *what***. The valuable comments here cite the
  spec section or derive the formula; keep that. Do not narrate code, and do
  not remove an existing rationale comment.
- **Determinism matters** to consumers doing golden-image comparison: avoid
  introducing fast-math or FP-contraction assumptions into the math core.
- **Language**: English only, always - in code, comments, docs and commit
  messages.
- After any change, build clean and re-run the analysis set above before
  considering the work done.
