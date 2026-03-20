# AGENTS.md

## Cursor Cloud specific instructions

This is an OpenGL fixed-function pipeline study project (based on Lazy Foo' Productions tutorials) containing two C++ desktop GUI apps. The original build system is Visual Studio 2022 `.vcxproj` (Windows only); a `Makefile` is provided for Linux/Cloud builds.

### Building

```
make all        # builds both executables into build/
make clean      # removes build directory
```

### System dependencies (pre-installed by VM snapshot)

- `g++`, `freeglut3-dev`, `libglu1-mesa-dev`, `mesa-utils`, `xvfb`, `scrot`, `xdotool`

### Running the apps

The apps require an X11 display. On headless VMs, start Xvfb first:

```
Xvfb :99 -screen 0 1024x768x24 &
export DISPLAY=:99
```

Then run either executable:

```
./build/fixedPipelineStudy                # white quad on red background
./build/matrices_and_coloring_polygons    # cyan quad; press 'q' to toggle color, 'e' to cycle zoom
```

### Gotchas

- There are no automated tests in this project; verification is visual (screenshot or GUI).
- There is no linter configured; compile warnings from `g++ -Wall` serve as the lint check.
- The `.vcxproj` files reference hardcoded Windows paths for freeglut (`C:\Users\23568\...`). On Linux, the system-installed freeglut is used via the Makefile instead.
- Mesa software rendering (`llvmpipe`) is used on the VM since there is no GPU. This is sufficient for OpenGL 2.1 fixed-function pipeline calls.
