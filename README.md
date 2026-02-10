# AERO++

C++ header-only API for aircraft preliminary design — aerodynamics, weight & balance, inertia, performance,  statistical analysis, and data visualization

## Features

- Header-only library (easy integration)
- Includes Eigen for linear algebra
- Gnuplot integration for data visualization
- Cross-platform support (Linux, macOS, Windows)

## Dependencies

This library requires:
- **Boost** (iostreams, system, filesystem)
- **gnuplot** (for plotting functionality)
- **Eigen** (included in this repository)
- **gnuplot-iostream** (included in this repository)

---

## Installation

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libboost-all-dev gnuplot
```

### macOS

Install dependencies using Homebrew:

```bash
brew install cmake boost gnuplot
```

### Windows (MSYS2 - Recommended)

1. **Download and install MSYS2** from [msys2.org](https://www.msys2.org/)

2. **Open MSYS2 MinGW 64-bit terminal** and run:

```bash
pacman -Syu  # Update package database
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-boost
pacman -S mingw-w64-x86_64-gnuplot
pacman -S mingw-w64-x86_64-cmake
pacman -S make
```

**Important:** Always use the **MSYS2 MinGW 64-bit** terminal for compilation, not the default MSYS2 terminal.

---

## Quick Start

### Option 1: Using CMake (Recommended)

1. **Clone the repository:**
```bash
git clone https://github.com/Am3Software/AEROPlusPlus.git
cd AEROPlusPlus
```

2. **Build the test:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

3. **Run the test:**
```bash
./RegressionTest        # Linux/macOS/MSYS2
RegressionTest.exe      # Windows CMD (if not using MSYS2)
```

### Option 2: Direct compilation with g++

```bash
g++ -I./include test/RegressionTest.cpp -lboost_iostreams -lboost_system -o RegressionTest
./RegressionTest
```

**Note for MSYS2 users:** Use `-lboost_iostreams-mt -lboost_system-mt` flags instead.

---

## Usage in Your Project

### Method 1: Include directly

1. Clone or download this repository
2. Add the include path to your compiler:

```cpp
#include "AEROPlusPlus/include/your_header.h"
#include <Eigen/Dense>
#include "gnuplot-iostream.h"
```

Compile with:
```bash
g++ -I/path/to/AEROPlusPlus/include your_code.cpp -lboost_iostreams -lboost_system -o your_program
```

### Method 2: CMake integration

Add to your `CMakeLists.txt`:

```cmake
# Add include directory
include_directories(/path/to/AEROPlusPlus/include)

# Find Boost
find_package(Boost COMPONENTS iostreams system filesystem REQUIRED)

# Link your executable
add_executable(your_program your_code.cpp)
target_link_libraries(your_program ${Boost_LIBRARIES})
```

---

## Project Structure

```
AEROPlusPlus/
├── include/                # Header files
│   ├── Eigen/              # Eigen library (included)
│   ├── gnuplot-iostream.h  # Gnuplot wrapper (included)
│   └── *.h                # AERO++ API headers
├── test/                  # Test files
│   └── RegressionTest.cpp
|   └── AircraftData.cpp
|   └── TestLaunchVSP.cpp
|   └── TestVSPCreator.cpp
├── CMakeLists.txt       # CMake configuration
├── README.md
└── LICENSE
```

---

## Examples

## Quick Example — Wing Definition + Weight & Balance (P2012 case study)
```cpp
// Define wing geometry and generate OpenVSP script
VSP::Wing wing;
wing.id    = "wing";
wing.span  = {2.882, 2.059, 2.059};
wing.croot = {2.0038, 2.0038, 1.7269};
wing.ctip  = {2.0038, 1.7269, 1.45};
wing.xloc  = 4.568;

script.makeWing(wing, 2);

// Weight & Balance — center of gravity computation
COG::COGCalculator cog("P2012", commonData, wingData,
                        fuselageData, engineData, ...);
cog.calculateCOGAircraft();

std::cout << "Xcg: " << cog.getCOGData().xCG << " m\n";
```

> Full workflow from geometry to W&B on a real aircraft: [`test/TestVSPCreator.cpp`](test/TestVSPCreator.cpp)

## Troubleshooting

### Windows: "Boost not found"
Make sure you're using the MSYS2 MinGW 64-bit terminal and have installed boost with:
```bash
pacman -S mingw-w64-x86_64-boost
```

### "gnuplot not found"
Ensure gnuplot is installed and in your system PATH:
- **Linux/macOS:** `which gnuplot`
- **Windows (MSYS2):** `where gnuplot`

### Compilation errors with Boost
Try specifying the Boost libraries explicitly:
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

- [Eigen](https://eigen.tuxfamily.org/) - C++ template library for linear algebra
- [gnuplot-iostream](https://github.com/dstahlke/gnuplot-iostream) - C++ interface to gnuplot
- [Boost](https://www.boost.org/) - C++ libraries