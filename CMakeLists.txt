cmake_minimum_required(VERSION 3.20)

project(AEROPlusPlus LANGUAGES C CXX)

# ============================================================
# AERO++ student/portable CMakeLists
# - Project can live in any folder, not only C:/msys64/home/...
# - No AERO++ FetchContent for OpenXLSX.
# - OpenXLSX must be vendored in third_party/OpenXLSX.
# - All .cpp files in root and test/ become buildable executables.
# - Runtime data folders are copied into build/ automatically.
# - VS Code/IntelliSense can use build/compile_commands.json.
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake policy compatibility" FORCE)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ------------------------------------------------------------
# MSYS2 / MinGW prefix detection
# ------------------------------------------------------------
if(DEFINED ENV{MINGW_PREFIX})
    file(TO_CMAKE_PATH "$ENV{MINGW_PREFIX}" MINGW_PREFIX_CMAKE)
elseif(EXISTS "C:/msys64/mingw64")
    set(MINGW_PREFIX_CMAKE "C:/msys64/mingw64")
else()
    set(MINGW_PREFIX_CMAKE "")
endif()

if(MINGW_PREFIX_CMAKE)
    list(PREPEND CMAKE_PREFIX_PATH "${MINGW_PREFIX_CMAKE}")
    list(PREPEND CMAKE_LIBRARY_PATH "${MINGW_PREFIX_CMAKE}/lib")
    list(PREPEND CMAKE_INCLUDE_PATH "${MINGW_PREFIX_CMAKE}/include")
    message(STATUS "Using MinGW/MSYS2 prefix: ${MINGW_PREFIX_CMAKE}")
endif()

add_compile_definitions(
    _USE_MATH_DEFINES
    _HAS_STD_BYTE=0
)

# ------------------------------------------------------------
# MinGW compatibility shim for OpenXLSX  [MODIFICA 1: robusto]
# - Riscrive il file solo se manca o se il contenuto è cambiato
# - Evita touch inutili che forzerebbero ricompile di tutto OpenXLSX
# ------------------------------------------------------------
if(MINGW)
    set(AEROPP_OPENXLSX_COMPAT_HEADER "${CMAKE_BINARY_DIR}/AEROPlusPlusOpenXLSXCompat.h")
    set(AEROPP_OPENXLSX_COMPAT_CONTENT
"#pragma once
#include <cstdint>
#include <algorithm>
")
    set(_aeropp_write_compat ON)
    if(EXISTS "${AEROPP_OPENXLSX_COMPAT_HEADER}")
        file(READ "${AEROPP_OPENXLSX_COMPAT_HEADER}" _aeropp_existing_compat)
        if(_aeropp_existing_compat STREQUAL "${AEROPP_OPENXLSX_COMPAT_CONTENT}")
            set(_aeropp_write_compat OFF)
        endif()
    endif()
    if(_aeropp_write_compat)
        file(WRITE "${AEROPP_OPENXLSX_COMPAT_HEADER}" "${AEROPP_OPENXLSX_COMPAT_CONTENT}")
        message(STATUS "Generated OpenXLSX MinGW compat header: ${AEROPP_OPENXLSX_COMPAT_HEADER}")
    else()
        message(STATUS "OpenXLSX MinGW compat header up to date: ${AEROPP_OPENXLSX_COMPAT_HEADER}")
    endif()
    add_compile_options(-include "${AEROPP_OPENXLSX_COMPAT_HEADER}")
endif()

# ------------------------------------------------------------
# Project include paths
# ------------------------------------------------------------
set(AEROPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(NOT EXISTS "${AEROPP_INCLUDE_DIR}")
    message(FATAL_ERROR "AERO++ include directory not found: ${AEROPP_INCLUDE_DIR}")
endif()

set(AEROPP_MATPLOTLIB_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/matplotlib-cpp")

# ------------------------------------------------------------
# Dependencies from MSYS2/pacman
# ------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

find_package(Boost REQUIRED COMPONENTS
    atomic
    chrono
    container
    date_time
    serialization
    thread
)

find_package(OpenMP QUIET)

find_package(tinyxml2 CONFIG QUIET)
if(NOT tinyxml2_FOUND)
    find_package(tinyxml2 QUIET)
endif()
if(NOT tinyxml2_FOUND)
    message(FATAL_ERROR
        "tinyxml2 not found. Install it in MSYS2 MinGW64 with:\n"
        "  pacman -S --needed mingw-w64-x86_64-tinyxml2")
endif()

find_package(pugixml CONFIG QUIET)
if(NOT pugixml_FOUND)
    find_package(pugixml QUIET)
endif()

# ------------------------------------------------------------
# VTK: request only the components needed by AERO++ visualization
# ------------------------------------------------------------
set(AEROPP_VTK_COMPONENTS
    CommonCore
    CommonColor
    CommonDataModel
    CommonExecutionModel
    FiltersCore
    FiltersGeneral
    FiltersGeometry
    FiltersSources
    IOImage
    IOGeometry
    RenderingCore
    RenderingOpenGL2
    RenderingFreeType
    RenderingAnnotation
    InteractionStyle
)

find_package(VTK CONFIG REQUIRED COMPONENTS ${AEROPP_VTK_COMPONENTS})
message(STATUS "VTK found: ${VTK_VERSION}")
message(STATUS "VTK components: ${AEROPP_VTK_COMPONENTS}")

find_package(OpenCASCADE CONFIG QUIET)
if(OpenCASCADE_FOUND)
    message(STATUS "OpenCASCADE found")
else()
    message(STATUS "OpenCASCADE not found: STEP/IGES-related features are not linked")
endif()

# ------------------------------------------------------------
# Vendored OpenXLSX
# ------------------------------------------------------------
set(OPENXLSX_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/OpenXLSX")

if(NOT EXISTS "${OPENXLSX_LOCAL_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "Vendored OpenXLSX not found.\n"
        "Expected path:\n"
        "  ${OPENXLSX_LOCAL_DIR}\n\n"
        "Run from the AERO++ root:\n"
        "  mkdir -p third_party\n"
        "  git clone https://github.com/troldal/OpenXLSX.git third_party/OpenXLSX\n"
        "  git -C third_party/OpenXLSX checkout 5723411d47643ce3b5b9994064c26ca8cd841f13\n")
endif()

set(OPENXLSX_ENABLE_LIBZIP ON CACHE BOOL "Use system libzip for OpenXLSX" FORCE)
set(OPENXLSX_BUILD_TESTS OFF CACHE BOOL "Disable OpenXLSX tests" FORCE)
set(OPENXLSX_BUILD_EXAMPLES OFF CACHE BOOL "Disable OpenXLSX examples" FORCE)
set(OPENXLSX_BUILD_BENCHMARKS OFF CACHE BOOL "Disable OpenXLSX benchmarks" FORCE)

message(STATUS "Using bundled OpenXLSX from: ${OPENXLSX_LOCAL_DIR}")
add_subdirectory("${OPENXLSX_LOCAL_DIR}" "${CMAKE_BINARY_DIR}/_deps/openxlsx-build")

if(EXISTS "${OPENXLSX_LOCAL_DIR}/.git")
    execute_process(
        COMMAND git -C "${OPENXLSX_LOCAL_DIR}" rev-parse HEAD
        OUTPUT_VARIABLE OPENXLSX_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    message(STATUS "Bundled OpenXLSX commit: ${OPENXLSX_COMMIT}")
endif()

# ------------------------------------------------------------
# Interface target with common include dirs/libs/options
# ------------------------------------------------------------
add_library(AEROPlusPlusCommon INTERFACE)

target_include_directories(AEROPlusPlusCommon INTERFACE
    "${AEROPP_INCLUDE_DIR}"
    "${OPENXLSX_LOCAL_DIR}"
    "${OPENXLSX_LOCAL_DIR}/OpenXLSX"
    "${OPENXLSX_LOCAL_DIR}/OpenXLSX/headers"
)

if(EXISTS "${AEROPP_MATPLOTLIB_CPP_DIR}")
    target_include_directories(AEROPlusPlusCommon INTERFACE "${AEROPP_MATPLOTLIB_CPP_DIR}")
endif()

if(MINGW_PREFIX_CMAKE)
    target_include_directories(AEROPlusPlusCommon INTERFACE
        "${MINGW_PREFIX_CMAKE}/include"
        "${MINGW_PREFIX_CMAKE}/include/eigen3"
    )
endif()

target_compile_definitions(AEROPlusPlusCommon INTERFACE
    _USE_MATH_DEFINES
    _HAS_STD_BYTE=0
)

# [MODIFICA 2] Boost moderno: target nominati invece di ${Boost_LIBRARIES}
target_link_libraries(AEROPlusPlusCommon INTERFACE
    OpenXLSX
    Python3::Python
    Boost::atomic
    Boost::chrono
    Boost::container
    Boost::date_time
    Boost::serialization
    Boost::thread
    ${VTK_LIBRARIES}
)

if(TARGET tinyxml2::tinyxml2)
    target_link_libraries(AEROPlusPlusCommon INTERFACE tinyxml2::tinyxml2)
elseif(TARGET tinyxml2)
    target_link_libraries(AEROPlusPlusCommon INTERFACE tinyxml2)
else()
    target_link_libraries(AEROPlusPlusCommon INTERFACE tinyxml2)
endif()

if(TARGET pugixml::pugixml)
    target_link_libraries(AEROPlusPlusCommon INTERFACE pugixml::pugixml)
elseif(TARGET pugixml)
    target_link_libraries(AEROPlusPlusCommon INTERFACE pugixml)
else()
    message(STATUS "pugixml CMake target not found explicitly; relying on OpenXLSX/system link if needed")
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(AEROPlusPlusCommon INTERFACE OpenMP::OpenMP_CXX)
endif()

if(OpenCASCADE_FOUND)
    if(DEFINED OpenCASCADE_INCLUDE_DIR)
        target_include_directories(AEROPlusPlusCommon INTERFACE ${OpenCASCADE_INCLUDE_DIR})
    endif()
    if(DEFINED OpenCASCADE_LIBRARIES)
        target_link_libraries(AEROPlusPlusCommon INTERFACE ${OpenCASCADE_LIBRARIES})
    else()
        target_link_libraries(AEROPlusPlusCommon INTERFACE
            TKernel TKMath TKG2d TKG3d TKGeomBase TKGeomAlgo
            TKBRep TKTopAlgo TKXSBase TKDESTEP TKDEIGES TKDESTL
            TKMesh TKShHealing TKBool TKBO
        )
    endif()
endif()

target_compile_options(AEROPlusPlusCommon INTERFACE
    $<$<CONFIG:Debug>:-Og -g2 -fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# ------------------------------------------------------------
# Helper: create an executable target from any .cpp file
# ------------------------------------------------------------
function(aeropp_add_cpp_executable source_file target_prefix)
    get_filename_component(base_name "${source_file}" NAME_WE)

    if(target_prefix)
        set(target_name "${target_prefix}_${base_name}")
    else()
        set(target_name "${base_name}")
    endif()

    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" target_name "${target_name}")

    set(original_target_name "${target_name}")
    set(counter 1)
    while(TARGET "${target_name}")
        math(EXPR counter "${counter} + 1")
        set(target_name "${original_target_name}_${counter}")
    endwhile()

    add_executable("${target_name}" "${source_file}")
    target_link_libraries("${target_name}" PRIVATE AEROPlusPlusCommon)

    vtk_module_autoinit(
        TARGETS "${target_name}"
        MODULES ${VTK_LIBRARIES}
    )

    set_property(TARGET "${target_name}" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

    message(STATUS "AERO++ target added: ${target_name}  <--  ${source_file}")
endfunction()

# ------------------------------------------------------------
# Automatically build all .cpp files in root and test/
# ------------------------------------------------------------
file(GLOB AEROPP_ROOT_CPP CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB AEROPP_TEST_CPP CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

list(SORT AEROPP_ROOT_CPP)
list(SORT AEROPP_TEST_CPP)

foreach(cpp_file IN LISTS AEROPP_ROOT_CPP)
    aeropp_add_cpp_executable("${cpp_file}" "")
endforeach()

foreach(cpp_file IN LISTS AEROPP_TEST_CPP)
    get_filename_component(test_base "${cpp_file}" NAME_WE)
    if(TARGET "${test_base}")
        aeropp_add_cpp_executable("${cpp_file}" "test")
    else()
        aeropp_add_cpp_executable("${cpp_file}" "")
    endif()
endforeach()

# ------------------------------------------------------------
# Copy runtime data folders into build/  [MODIFICA 5: niente REMOVE_RECURSE]
# - file(COPY) di default non sovrascrive file più recenti
# - Non cancella più tutto a ogni configure: output di test/run preservati
# ------------------------------------------------------------
set(AEROPP_RUNTIME_DIRS
    AircraftSettings
    FuselagePreset
    NacellePreset
    ExcelFiles
    airfoil
)

foreach(runtime_dir IN LISTS AEROPP_RUNTIME_DIRS)
    set(runtime_src "${CMAKE_CURRENT_SOURCE_DIR}/${runtime_dir}")
    set(runtime_dst_parent "${CMAKE_BINARY_DIR}")

    if(EXISTS "${runtime_src}")
        file(COPY "${runtime_src}" DESTINATION "${runtime_dst_parent}")
        message(STATUS "Synced runtime directory to build/: ${runtime_dir}")
    else()
        message(STATUS "Runtime directory not present, skipping: ${runtime_dir}")
    endif()
endforeach()

# ------------------------------------------------------------
# Final summary
# ------------------------------------------------------------
message(STATUS "")
message(STATUS "================ AERO++ CONFIGURATION SUMMARY ================")
message(STATUS "Source dir:        ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Build dir:         ${CMAKE_BINARY_DIR}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "MSYS2/MinGW:       ${MINGW_PREFIX_CMAKE}")
message(STATUS "Boost:             ${Boost_VERSION}")
message(STATUS "Python3:           ${Python3_VERSION}")
message(STATUS "VTK:               ${VTK_VERSION}")
message(STATUS "OpenXLSX:          ${OPENXLSX_LOCAL_DIR}")
message(STATUS "OpenCASCADE found: ${OpenCASCADE_FOUND}")
message(STATUS "compile_commands:  ${CMAKE_BINARY_DIR}/compile_commands.json")
message(STATUS "===============================================================")
message(STATUS "")