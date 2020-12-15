cmake_minimum_required (VERSION 2.6)

project (hello)

set(CMAKE_BUILD_TYPE Release)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpeNCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
find_package(websocketpp REQUIRED)
message(STATUS "Websocketpp library status:")
message(STATUS "    version: ${Websocketpp_VERSION}")
message(STATUS "    libraries: ${Websocketpp_LIBS}")
message(STATUS "    include path: ${WEBSOCKETPP_INCLUDE_DIRS}")
message(STATUS "    include path: ${WEBSOCKETPP_INCLUDE}")
message(STATUS "    include path: ${WEBSOCKETPP_PLATFORM_LIBS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

set(SOURCE_A proc_a.cpp)
set(SOURCE_B proc_b.cpp)
set(SOURCE_C proc_c.cpp)
set(SOURCE_D proc_d.cpp)

add_executable(proc_a ${SOURCE_A})
add_executable(proc_b ${SOURCE_B})
add_executable(proc_c ${SOURCE_C})
add_executable(proc_d ${SOURCE_D})


target_link_libraries(proc_a ${OpenCV_LIBS})
target_link_libraries(proc_b ${OpenCV_LIBS})
target_link_libraries(proc_c ${OpenCV_LIBS})
target_link_libraries(proc_c PUBLIC websocketpp)
install(TARGETS proc_a DESTINATION bin)
install(TARGETS proc_b DESTINATION bin)
install(TARGETS proc_c DESTINATION bin)
install(TARGETS proc_d DESTINATION bin)


