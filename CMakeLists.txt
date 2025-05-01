cmake_minimum_required(VERSION 3.16)

# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(eggsizerML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(DEFINED ENV{GITHUB_ACTIONS})
    IF(WIN32)
        set(OpenCV_DIR "C:/tools/opencv/build")
        set(QT_DIR "C:/6.8.2/msvc2022_64/lib/cmake/Qt6")
        set(Qt6_DIR "C:/6.8.2/msvc2022_64/lib/cmake/Qt6")
        set(CMAKE_CXX_COMPILER "gcc")
    ENDIF()
ENDIF()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/eggsizerml.cpp
        src/eggsizerml.ui
        src/otsuThreshold.cpp
        src/blobDetect.cpp
        src/measureEdges.cpp
        include/eggsizerml.h
        include/asmOpenCV.h
        include/otsuThreshold.h
        include/blobDetect.h
        include/measureEdges.h
)
enable_testing()

if(DEFINED ENV{GITHUB_ACTIONS})
    if(WIN32)
        set(Boost_NO_SYSTEM_PATHS ON)
        set(Boost_USE_STATIC_LIBS OFF)
        set(Boost_USE_MULTITHREADED ON)
        set(Boost_USE_STATIC_RUNTIME OFF)
    endif()
endif()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# ----------------------------
# Accuracy Test Setup
# ----------------------------
add_executable(accuracy_test
    tests/accuracy_test.cpp
    src/measureImage.cpp
    src/blobDetect.cpp
    src/measureEdges.cpp
    src/otsuThreshold.cpp
)

target_include_directories(accuracy_test PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    include
)

target_link_libraries(accuracy_test PRIVATE
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
)

add_test(NAME AccuracyTest COMMAND accuracy_test)

# ----------------------------
# Main Application (eggsizerML)
# ----------------------------
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(eggsizerML
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/measureImage.cpp
        src/blobDetect.cpp
        src/measureEdges.cpp
        include/measureImage.hpp
        include/blobDetect.h
        include/measureEdges.h
        include/otsuThreshold.h
    )
else()
    if(ANDROID)
        add_library(eggsizerML SHARED
            ${PROJECT_SOURCES}
        )
        # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(eggsizerML
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(eggsizerML PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    include
)

target_link_libraries(eggsizerML PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.eggsizerML)
endif()
set_target_properties(eggsizerML PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS eggsizerML
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(eggsizerML)
endif()

# runs doxygen documentation upon build (can remove if this is redundant)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Doxygen configuration goes here
else()
    message(STATUS "Doxygen not found. Documentation will not be generated.")
endif()

set(CPACK_PACKAGE_NAME "EggSizerML")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "eggsizerml")
set(CPACK_GENERATOR WIX)
include(CPack)
