cmake_minimum_required(VERSION 3.28)
project(ImageProcessor VERSION 1.0 LANGUAGES CXX)


# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(OpenCV REQUIRED)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0)


# Include directories
include_directories(
    ${GST_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)
link_directories(
    ${GST_LIBRARY_DIRS}
    ${OpenCV_LIBS}
)

############################################################
# SOURCE FILES
############################################################
set(SOURCE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Src/main.cpp
)

############################################################
# Create and executable
############################################################
add_executable(ImageProcessor ${SOURCE_FILES})


############################################################
# Link Dependencies
############################################################
target_include_directories(ImageProcessor PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
)

target_link_libraries(ImageProcessor
    ${GST_LIBRARIES}
    ${OpenCV_LIBS}
)
