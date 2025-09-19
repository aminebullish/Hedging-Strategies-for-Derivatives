cmake_minimum_required(VERSION 3.0)
project(mc-pricer CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

if (CXX)
    set(CMAKE_CXX_COMPILER ${CXX})
endif (CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()


# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF)
endif()

# Add jlparser
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (WIN32)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()
add_subdirectory(src/3rdparty/jlparser)


### Ne rien modifier dans la partie au dessus de cette ligne.
add_library(Option src/AsianOption.cpp src/BasketOption.cpp src/PerformanceOption.cpp)
add_library(BlackScholesModel src/BlackScholesModel.cpp)
add_library(MonteCarlo src/MonteCarlo.cpp)
add_library(PricingResults src/PricingResults.cpp src/PnlVectToJson.cpp)
add_library(HedgingResults src/HedgingResults.cpp src/PnlVectToJson.cpp)

# Test the parser
add_executable(test-parser src/3rdparty/jlparser/test_parser.cpp)
target_link_libraries(test-parser jl_parser ${LIBS})

add_executable(testBasket src/tests/testBasketOption.cpp)
target_link_libraries(testBasket Option ${LIBS})

add_executable(testAsian src/tests/testAsianOption.cpp)
target_link_libraries(testAsian Option ${LIBS})

add_executable(testPerformance src/tests/testPerfOption.cpp)
target_link_libraries(testPerformance Option ${LIBS})

add_executable(testBlackScholesModel src/tests/testBlackscholesModel.cpp)
target_link_libraries(testBlackScholesModel BlackScholesModel ${LIBS})

add_executable(testMonteCarlo src/tests/testMonteCarlo.cpp)
target_link_libraries(testMonteCarlo MonteCarlo BlackScholesModel Option ${LIBS})

add_executable(price0 src/price0.cpp)
target_link_libraries(price0 jl_parser PricingResults MonteCarlo BlackScholesModel Option ${LIBS})

add_executable(hedge src/hedge.cpp)
target_link_libraries(hedge jl_parser HedgingResults MonteCarlo BlackScholesModel Option ${LIBS})

add_executable(testHedgingVanillaCall src/tests/testHedgingVanillaCall.cpp)
target_link_libraries(testHedgingVanillaCall MonteCarlo BlackScholesModel Option ${LIBS})

# Print compiler flags
get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_LINKER_EXE_FLAGS}" )
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_LINKER_EXE_FLAGS}" )
endif()
enable_testing()
