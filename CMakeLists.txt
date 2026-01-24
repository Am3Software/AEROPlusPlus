cmake_minimum_required(VERSION 3.10)

project(AEROPlusPlus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# Header-only library - Include directories
# ==========================================
include_directories(${CMAKE_SOURCE_DIR}/include)

# ==========================================
# OpenXLSX (optional - uncomment if needed)
# ==========================================
 find_package(OpenXLSX QUIET)
 
 if(NOT OpenXLSX_FOUND)
     message(STATUS "OpenXLSX not found, downloading via FetchContent")
     include(FetchContent)
     
     FetchContent_Declare(
         OpenXLSX
         GIT_REPOSITORY https://github.com/troldal/OpenXLSX.git
         GIT_TAG        v0.4.0
     )
     
     FetchContent_MakeAvailable(OpenXLSX)
 endif()

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
# Build test executable
# ==========================================
add_executable(RegressionTest test/RegressionTest.cpp)

# Link libraries to test
target_link_libraries(RegressionTest 
    ${Boost_LIBRARIES}
    OpenXLSX::OpenXLSX  # Uncomment if using OpenXLSX
)

# ==========================================
# Enable testing
# ==========================================
enable_testing()
add_test(NAME RegressionTest COMMAND RegressionTest)

message(STATUS "")
message(STATUS "==============================================")
message(STATUS "  AERo++ Configuration Summary")
message(STATUS "==============================================")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Boost version: ${Boost_VERSION}")
message(STATUS "  Include directory: ${CMAKE_SOURCE_DIR}/include")
message(STATUS "==============================================")
message(STATUS "")