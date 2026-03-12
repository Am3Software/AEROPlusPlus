# AERO++

C++ header-only API for aircraft preliminary design — aerodynamics, weight & balance, inertia, performance, statistical analysis, 3D visualization, and data export.

## Features

- Header-only library (easy integration)
- Eigen for linear algebra
- Gnuplot integration for 2D data visualization
- **VTK-based 3D aircraft visualization** — interactive rendering and multi-view PNG export
- **Per-component color mapping** — assign colors to each aircraft component by name
- OpenVSP DegenGeom CSV parser
- OpenCASCADE integration for STEP/IGES geometry import
- Cross-platform support (Linux, macOS, Windows)

## Dependencies

| Dependency | Required | Purpose |
|------------|----------|---------|
| **Boost** | Optional | gnuplot-iostream plotting |
| **gnuplot** | Optional | 2D data visualization |
| **Eigen** | Included | Linear algebra |
| **gnuplot-iostream** | Included | gnuplot C++ wrapper |
| **OpenXLSX** | Auto-downloaded | Excel file I/O |
| **tinyxml2** | Auto-downloaded | XML parsing |
| **VTK** | Optional | 3D aircraft visualization |
| **OpenCASCADE** | Optional | STEP/IGES geometry import |
| **Python3** | Optional | Scripting / bindings |

> OpenXLSX and tinyxml2 are automatically downloaded via CMake FetchContent if not found.

---

## Installation

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libboost-all-dev gnuplot libvtk9-dev
```

### macOS

```bash
brew install cmake boost gnuplot vtk
```

### Windows (MSYS2 — Recommended)

1. **Download and install MSYS2** from [msys2.org](https://www.msys2.org/)

2. **Open MSYS2 MinGW 64-bit terminal** and run:

```bash
pacman -Syu                                    # Update package database
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-boost
pacman -S mingw-w64-x86_64-gnuplot
pacman -S mingw-w64-x86_64-vtk                 # 3D visualization
pacman -S mingw-w64-x86_64-opencascade         # STEP/IGES import (optional)
pacman -S mingw-w64-x86_64-python              # Python bindings (optional)
```

> **Important:** Always use the **MSYS2 MinGW 64-bit** terminal, not the default MSYS2 terminal.

---

## Quick Start

### Build with CMake (Recommended)

```bash
git clone https://github.com/Am3Software/AEROPlusPlus.git
cd AEROPlusPlus
mkdir build && cd build
cmake ..
cmake --build .
```

CMake will automatically:
- Download OpenXLSX and tinyxml2 if not found
- Auto-detect VTK in common installation paths
- Print a configuration summary with all detected dependencies
- Skip unavailable optional features with a warning (no hard failures)

### Run the tests

```bash
./TestVSPCreator     # Linux/macOS/MSYS2
TestVSPCreator.exe   # Windows CMD
```

### Custom VTK path

If VTK is installed in a non-standard location:
```bash
cmake -DVTK_DIR=/path/to/vtk/cmake ..
```

---

## 3D Visualization — AircraftPlotter

The `AircraftPlotter` class (in `include/Plotter3D.h`) translates MATLAB's `plotDegenSurf` and `PlotAircraft` into C++ using VTK.

### Basic usage

```cpp
#include "Plotter3D.h"
#include "DegenGeomParser.h"
#include <filesystem>

// Parse OpenVSP DegenGeom CSV
DegenGeomReader reader("P2012_DegenGeom.csv");
auto components = reader.read();

// Create plotter
AircraftPlotter plotter("P2012", "logo/AeroPlusPLus_logo.png");
plotter.setResolution(2560, 1440);              // 2K output
plotter.setBackground(38, 38, 38);              // dark grey background (RGB 0-255)

// Assign colors per component (RGB 0-255, matched by name prefix)
std::map<std::string, std::array<double, 3>> colorMap = {
    {"wing",       {  0,   0, 255}},
    {"horizontal", {  0, 255,   0}},
    {"vertical",   {255,   0, 255}},
    {"Fuselage",   {180, 180, 180}},
    {"nacelle",    {100, 200, 255}},
    {"disk",       { 80,  80,  80}},
};

for (const auto& surf : components)
    plotter.addComponentWithColorMap(surf, colorMap);

// Save all views + open interactive window
plotter.plotAndSave(std::filesystem::current_path().string());
```

### Available views

| Method | Description |
|--------|-------------|
| `saveAllViews(dir)` | Saves Top, Side, Front, Perspective PNG — non-blocking |
| `savePNG(file, view)` | Saves a single PNG with chosen view |
| `show(view)` | Opens interactive 3D window — blocking |
| `plotAndSave(dir)` | Saves all views then opens interactive window |

### Camera views

```cpp
CameraView::TOP         // Top-down view (X+ = nose up)
CameraView::SIDE        // Side view (nose left, tail right)
CameraView::FRONT       // Front view (nose toward observer)
CameraView::PERSPECTIVE // Perspective view (azimuth=-45, elevation=45)
```

### Setters

```cpp
plotter.setResolution(3840, 2160);          // 4K
plotter.setBackground(135, 206, 235);       // sky blue (RGB 0-255)
plotter.setOpacity(0.8);                    // semi-transparent surfaces
plotter.setLogo("logo/logo.png", 0.12, 20); // logo, size 12% of width, 20px margin
```

---

## Usage in Your Project

### Method 1: Include directly

```cpp
#include "AEROPlusPlus/include/Plotter3D.h"
#include "AEROPlusPlus/include/DegenGeomParser.h"
#include <Eigen/Dense>
```

### Method 2: CMake integration

```cmake
include_directories(/path/to/AEROPlusPlus/include)

find_package(VTK REQUIRED)
find_package(Boost COMPONENTS iostreams system filesystem REQUIRED)

add_executable(your_program your_code.cpp)
target_link_libraries(your_program ${Boost_LIBRARIES} ${VTK_LIBRARIES})
vtk_module_autoinit(TARGETS your_program MODULES ${VTK_LIBRARIES})
```

---

## Project Structure

```
AEROPlusPlus/
├── include/                    # All API headers (header-only)
│   ├── Eigen/                  # Eigen library (included)
│   ├── gnuplot-iostream.h      # Gnuplot C++ wrapper (included)
│   ├── Plotter3D.h             # VTK 3D aircraft visualization
│   ├── DegenGeomParser.h       # OpenVSP DegenGeom CSV parser
│   └── *.h                     # Other AERO++ API headers
├── test/                       # Test / example files
│   ├── RegressionTest.cpp
│   ├── AircraftData.cpp
│   ├── TestLaunchVSP.cpp
│   ├── TestVSPCreator.cpp
│   └── A320Neo_Test.cpp
├── logo/                       # Project logo (used in PNG exports)
│   └── AeroPlusPLus_logo.png
├── ExcelFiles/                 # Example Excel data files
├── CMakeLists.txt
├── README.md
└── LICENSE

---

## Examples

### Wing Definition + Weight & Balance (P2012 case study)

```cpp
// Define wing geometry and generate OpenVSP script
VSP::Wing wing;
wing.id    = "wing";
wing.span  = {2.882, 2.059, 2.059};
wing.croot = {2.0038, 2.0038, 1.7269};
wing.ctip  = {2.0038, 1.7269, 1.45};
wing.xloc  = 4.568;

script.makeWing(wing, 2);

// Weight & Balance — center of gravity
COG::COGCalculator cog("P2012", commonData, wingData,
                        fuselageData, engineData, ...);
cog.calculateCOGAircraft();
std::cout << "Xcg: " << cog.getCOGData().xCG << " m\n";
```

> Full workflow: [`test/TestVSPCreator.cpp`](test/TestVSPCreator.cpp)

---

## Troubleshooting

### "VTK not found"
```bash
# MSYS2
pacman -S mingw-w64-x86_64-vtk

# Or specify manually
cmake -DVTK_DIR=/path/to/vtk/cmake ..
```

### "Boost not found"
```bash
# MSYS2
pacman -S mingw-w64-x86_64-boost

# Linux
sudo apt install libboost-all-dev
```

### "gnuplot not found"
Ensure gnuplot is installed and in your PATH:
```bash
which gnuplot        # Linux/macOS
where gnuplot        # Windows CMD
```

### Compilation errors with Boost on MSYS2
```bash
g++ -I./include test.cpp -lboost_iostreams-mt -lboost_system-mt -lboost_filesystem-mt
```

---

## License

Apache 2.0

---

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

---

## Authors

Amedeo Falco

---

## Acknowledgments

- [Eigen](https://eigen.tuxfamily.org/) — C++ template library for linear algebra
- [gnuplot-iostream](https://github.com/dstahlke/gnuplot-iostream) — C++ interface to gnuplot
- [Boost](https://www.boost.org/) — C++ libraries
- [VTK](https://vtk.org/) — Visualization Toolkit for 3D rendering
- [OpenVSP](https://openvsp.org/) — Open-source parametric aircraft geometry tool