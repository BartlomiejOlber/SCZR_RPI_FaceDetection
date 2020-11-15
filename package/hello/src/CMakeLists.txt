cmake_minimum_required (VERSION 2.6)

project (hello)

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()


add_executable(hello hello.cpp)

# Link your application with OpenCV libraries
target_link_libraries(hello ${OpenCV_LIBS})
install(TARGETS hello DESTINATION bin)


