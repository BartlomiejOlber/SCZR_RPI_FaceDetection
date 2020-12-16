cmake_minimum_required (VERSION 2.6)

project (cmake_opencv)

#set(CMAKE_BUILD_TYPE Release)


SET(OpenCV_DIR /home/bartlomiej/installation/OpenCV-3.4.4/share/OpenCV/)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

#file(GLOB SOURCES "src/*.cpp")
set(SOURCE_A src/proc_a.cpp)
set(SOURCE_B src/proc_b.cpp)
set(SOURCE_D src/proc_d.cpp)
set(SOURCE_C src/proc_c.cpp)

include_directories("cppcodec")

set(BOOST_ROOT /opt/boost)
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
find_package(Boost COMPONENTS system)
#add_library(testOpencv STATIC ${SOURCES})
include_directories(${Boost_INCLUDE_DIR})

add_executable(proc_a ${SOURCE_A})
add_executable(proc_b ${SOURCE_B})
add_executable(proc_d ${SOURCE_D})
add_executable(proc_c ${SOURCE_C})

# Link your application with OpenCV libraries
target_link_libraries(proc_a ${OpenCV_LIBS})
target_link_libraries(proc_b ${OpenCV_LIBS})
target_link_libraries(proc_c ${OpenCV_LIBS})
target_link_libraries(proc_c ${Boost_LIBRARIES})
