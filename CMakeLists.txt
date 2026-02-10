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
# Boost (required for gnuplot-iostream)
# ==========================================
find_package(Boost COMPONENTS iostreams system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# ==========================================
# Check for gnuplot executable
# ==========================================
find_program(GNUPLOT_EXECUTABLE gnuplot)
if(NOT GNUPLOT_EXECUTABLE)
    message(WARNING "gnuplot executable not found. Please install gnuplot to use plotting features.")
else()
    message(STATUS "Found gnuplot: ${GNUPLOT_EXECUTABLE}")
endif()

# ==========================================
# OpenXLSX (auto-download if not found)
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
# tinyxml2 (auto-download if not found)
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
# ==========================================
find_package(Python3 QUIET COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Found Python3: ${Python3_VERSION}")
else()
    message(STATUS "Python3 not found - Python bindings will be skipped")
endif()

# ==========================================
# Common libraries
# ==========================================
set(COMMON_LIBS
    ${Boost_LIBRARIES}
    OpenXLSX::OpenXLSX
    tinyxml2::tinyxml2
)

if(Python3_FOUND)
    list(APPEND COMMON_LIBS Python3::Python)
endif()

# ==========================================
# Copy ExcelFiles to build directory
# ==========================================
file(COPY ${CMAKE_SOURCE_DIR}/ExcelFiles
     DESTINATION ${CMAKE_BINARY_DIR})

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
        target_link_libraries(${test_name} PRIVATE ${COMMON_LIBS})
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
message(STATUS "  Boost version   : ${Boost_VERSION}")
message(STATUS "  gnuplot         : ${GNUPLOT_EXECUTABLE}")
message(STATUS "  Python3         : ${Python3_VERSION}")
message(STATUS "  Include dir     : ${CMAKE_SOURCE_DIR}/include")
message(STATUS "==============================================")
message(STATUS "")