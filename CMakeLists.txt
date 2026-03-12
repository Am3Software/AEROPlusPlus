cmake_minimum_required(VERSION 3.20)
project(AEROPlusPlus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==========================================
# Global compile definitions
# ==========================================
add_compile_definitions(_USE_MATH_DEFINES)

# ==========================================
# Header-only library - Include directories
# ==========================================
include_directories(${CMAKE_SOURCE_DIR}/include)

# ==========================================
# FetchContent (declared once)
# ==========================================
include(FetchContent)

# ==========================================
# Boost (optional - required for gnuplot-iostream)
#   MSYS2 : pacman -S mingw-w64-x86_64-boost
#   apt   : sudo apt install libboost-all-dev
#   brew  : brew install boost
#   vcpkg : vcpkg install boost
# ==========================================
find_package(Boost QUIET COMPONENTS iostreams system filesystem)
if(Boost_FOUND)
    message(STATUS "Found Boost: version ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(WARNING
        "\n"
        "  Boost not found — gnuplot-iostream plotting features will not be available.\n"
        "  Install Boost and re-run cmake:\n"
        "    MSYS2 : pacman -S mingw-w64-x86_64-boost\n"
        "    apt   : sudo apt install libboost-all-dev\n"
        "    brew  : brew install boost\n"
        "    vcpkg : vcpkg install boost\n"
    )
endif()

# ==========================================
# Check for gnuplot executable
# ==========================================
find_program(GNUPLOT_EXECUTABLE gnuplot)
if(NOT GNUPLOT_EXECUTABLE)
    message(WARNING "gnuplot executable not found. Install gnuplot to use plotting features.")
else()
    message(STATUS "Found gnuplot: ${GNUPLOT_EXECUTABLE}")
endif()

# ==========================================
# OpenXLSX (auto-download via FetchContent)
# ==========================================
find_package(OpenXLSX QUIET)
if(NOT OpenXLSX_FOUND)
    message(STATUS "OpenXLSX not found, downloading via FetchContent...")
    FetchContent_Declare(
        OpenXLSX
        GIT_REPOSITORY https://github.com/troldal/OpenXLSX.git
        GIT_TAG        v0.4.0
    )
    FetchContent_MakeAvailable(OpenXLSX)
endif()

# ==========================================
# tinyxml2 (auto-download via FetchContent)
# ==========================================
find_package(tinyxml2 QUIET)
if(NOT tinyxml2_FOUND)
    message(STATUS "tinyxml2 not found, downloading via FetchContent...")
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG        v9.0.0
    )
    FetchContent_MakeAvailable(tinyxml2)
endif()

# ==========================================
# Python3 (optional - for scripting/bindings)
#   MSYS2 : pacman -S mingw-w64-x86_64-python
#   apt   : sudo apt install python3-dev
#   brew  : brew install python
# ==========================================
find_package(Python3 QUIET COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Found Python3: ${Python3_VERSION}")
else()
    message(STATUS "Python3 not found — Python bindings will be skipped.")
endif()

# ==========================================
# OpenCASCADE (optional - for STEP/IGES import)
#   MSYS2 : pacman -S mingw-w64-x86_64-opencascade
#   apt   : sudo apt install libocct-foundation-dev
#   brew  : brew install opencascade
# ==========================================
find_package(OpenCASCADE QUIET)
if(OpenCASCADE_FOUND)
    message(STATUS "Found OpenCASCADE: ${OpenCASCADE_VERSION}")
    include_directories(${OpenCASCADE_INCLUDE_DIR})
    set(OCCT_LIBS
        TKernel TKMath TKG2d TKG3d TKGeomBase TKGeomAlgo
        TKBRep TKTopAlgo TKXSBase TKDESTEP TKDEIGES TKDESTL
        TKMesh TKShHealing TKBool TKBO
    )
else()
    # Fallback: cerca manualmente nei path comuni MSYS2/Linux
    find_path(OCCT_INCLUDE_DIR "Standard.hxx"
        PATHS
            "C:/msys64/mingw64/include/opencascade"
            "D:/msys64/mingw64/include/opencascade"
            "/usr/include/opencascade"
            "/usr/local/include/opencascade"
            "/opt/homebrew/include/opencascade"
        NO_DEFAULT_PATH
    )
    if(OCCT_INCLUDE_DIR)
        message(STATUS "Found OpenCASCADE headers: ${OCCT_INCLUDE_DIR}")
        include_directories(${OCCT_INCLUDE_DIR})
        set(OCCT_LIBS
            TKernel TKMath TKG2d TKG3d TKGeomBase TKGeomAlgo
            TKBRep TKTopAlgo TKXSBase TKDESTEP TKDEIGES TKDESTL
            TKMesh TKShHealing TKBool TKBO
        )
    else()
        message(STATUS "OpenCASCADE not found — STEP/IGES import features will not be available.")
        set(OCCT_LIBS "")
    endif()
endif()

# ==========================================
# VTK — 3D Aircraft Visualization
#
#   MSYS2 : pacman -S mingw-w64-x86_64-vtk
#   apt   : sudo apt install libvtk9-dev
#   brew  : brew install vtk
#   vcpkg : vcpkg install vtk
#
# Custom path: cmake -DVTK_DIR=/path/to/vtk/cmake ..
# ==========================================

# Suppress FindBoost policy warning from VTK cmake files (VTK 9.x + CMake 3.30+)
set(CMAKE_POLICY_DEFAULT_CMP0167 OLD)

# Auto-detect VTK cmake config across common installation paths
# (only if user has not already provided -DVTK_DIR=...)
if(NOT DEFINED VTK_DIR)
    find_path(VTK_CMAKE_AUTODETECT "vtk-config.cmake"
        PATHS
            # MSYS2/MinGW64 — Windows (multiple drive letters)
            "C:/msys64/mingw64/lib/cmake/vtk"
            "D:/msys64/mingw64/lib/cmake/vtk"
            "E:/msys64/mingw64/lib/cmake/vtk"
            # vcpkg — Windows
            "C:/vcpkg/installed/x64-windows/share/vtk"
            "D:/vcpkg/installed/x64-windows/share/vtk"
            # Linux system install
            "/usr/lib/cmake/vtk"
            "/usr/lib/x86_64-linux-gnu/cmake/vtk"
            "/usr/local/lib/cmake/vtk"
            # macOS Homebrew (Intel + Apple Silicon)
            "/usr/local/opt/vtk/lib/cmake/vtk"
            "/opt/homebrew/opt/vtk/lib/cmake/vtk"
        NO_DEFAULT_PATH
    )
    if(VTK_CMAKE_AUTODETECT)
        set(VTK_DIR "${VTK_CMAKE_AUTODETECT}" CACHE PATH "Auto-detected VTK cmake config path")
        message(STATUS "VTK_DIR auto-detected: ${VTK_DIR}")
    endif()
endif()

find_package(VTK QUIET)

if(VTK_FOUND)
    message(STATUS "Found VTK: version ${VTK_VERSION} in ${VTK_DIR}")
else()
    message(WARNING
        "\n"
        "  VTK not found — 3D visualization (AircraftPlotter / Plotter3D) will not be available.\n"
        "  Install VTK and re-run cmake:\n"
        "    MSYS2 : pacman -S mingw-w64-x86_64-vtk\n"
        "    apt   : sudo apt install libvtk9-dev\n"
        "    brew  : brew install vtk\n"
        "    vcpkg : vcpkg install vtk\n"
        "    Custom: cmake -DVTK_DIR=/path/to/vtk/cmake ..\n"
    )
endif()

# ==========================================
# Common libraries — assembled conditionally
# ==========================================
set(COMMON_LIBS
    OpenXLSX::OpenXLSX
    tinyxml2::tinyxml2
)

if(Boost_FOUND)
    list(APPEND COMMON_LIBS ${Boost_LIBRARIES})
endif()

if(Python3_FOUND)
    list(APPEND COMMON_LIBS Python3::Python)
endif()

if(OCCT_LIBS)
    list(APPEND COMMON_LIBS ${OCCT_LIBS})
endif()

if(VTK_FOUND)
    list(APPEND COMMON_LIBS ${VTK_LIBRARIES})
endif()

# ==========================================
# Copy ExcelFiles to build directory (if present)
# ==========================================
if(EXISTS ${CMAKE_SOURCE_DIR}/ExcelFiles)
    file(COPY ${CMAKE_SOURCE_DIR}/ExcelFiles
         DESTINATION ${CMAKE_BINARY_DIR})
endif()

# ==========================================
# Build all test executables
# ==========================================
set(TEST_SOURCES
    test/RegressionTest.cpp
    test/AircraftData.cpp
    test/TestLaunchVSP.cpp
    test/TestVSPCreator.cpp
)

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    if(EXISTS ${CMAKE_SOURCE_DIR}/${test_source})
        add_executable(${test_name} ${test_source})

        # Core libraries
        target_link_libraries(${test_name} PRIVATE ${COMMON_LIBS})

        # VTK — per-target include dirs + autoinit (mandatory for VTK >= 9.x)
        if(VTK_FOUND)
            target_include_directories(${test_name} PRIVATE ${VTK_INCLUDE_DIRS})
            vtk_module_autoinit(
                TARGETS  ${test_name}
                MODULES  ${VTK_LIBRARIES}
            )
        endif()

        message(STATUS "  Test target added: ${test_name}")
    else()
        message(WARNING "  Test source not found, skipping: ${test_source}")
    endif()
endforeach()

# ==========================================
# Enable testing
# ==========================================
enable_testing()
add_test(NAME RegressionTest COMMAND RegressionTest)
add_test(NAME AircraftData   COMMAND AircraftData)
add_test(NAME TestLaunchVSP  COMMAND TestLaunchVSP)
add_test(NAME TestVSPCreator COMMAND TestVSPCreator)

# ==========================================
# Configuration summary
# ==========================================
message(STATUS "")
message(STATUS "==============================================")
message(STATUS "  AERO++ Configuration Summary")
message(STATUS "==============================================")
message(STATUS "  C++ Standard    : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Boost           : ${Boost_VERSION}")
message(STATUS "  gnuplot         : ${GNUPLOT_EXECUTABLE}")
message(STATUS "  Python3         : ${Python3_VERSION}")
message(STATUS "  OpenCASCADE     : ${OpenCASCADE_VERSION}")
message(STATUS "  VTK             : ${VTK_VERSION}")
message(STATUS "  Include dir     : ${CMAKE_SOURCE_DIR}/include")
message(STATUS "==============================================")
message(STATUS "")