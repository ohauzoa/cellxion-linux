cmake_minimum_required(VERSION 3.0.0)
project(cellxion)
set(CMAKE_C_STANDARD 11)#C11
#set(CMAKE_CXX_STANDARD 17)#C17
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE INCLUDES "images/*.h" "./lv_drivers/*.h" "./lv_libs/*.h"  "./lvgl/*.h"  "./*.h" )
file(GLOB_RECURSE SOURCES  "images/*.c" "./lv_drivers/*.c" "./lv_libs/*.c"  "./lvgl/*.c"  )

#include(${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_rlottie/CMakeLists.txt)
#include(${CMAKE_CURRENT_SOURCE_DIR}/lv_libs)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
SET(CMAKE_CXX_FLAGS "-O3")

#find_package(lua51 REQUIRED)
find_package(SDL2 REQUIRED SDL2)
find_package(Freetype REQUIRED)
#find_package(Rlottie REQUIRED)

if(NOT FREETYPE_FOUND)
  message(STATUS "freetype2:   NO")
else()
  message(STATUS "freetype2:   YES..   Version:${FREETYPE_VERSION_STRING}")
endif()


include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${LOTTIE_INCLUDE_DIRS}
)

link_directories (
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
    ${OPENGL_LIBRARY_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${LOTTIE_INCLUDE_DIRS}
)
    
    #add_executable(main main.c lv_lib_rlottie/lv_rlottie_example_fingerprint.c ${SOURCES} ${INCLUDES})
add_executable(${PROJECT_NAME} main.c  ${SOURCES} ${INCLUDES})
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
#target_link_libraries(main PRIVATE SDL2 lv_lib_rlottie)


target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 freetype rlottie)



add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
