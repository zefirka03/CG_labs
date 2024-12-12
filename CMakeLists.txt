cmake_minimum_required(VERSION 3.10)
project(CGlabs)

set(PNG_SHARED OFF)
set(SDL_STATIC ON)

set(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)
set(PNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644)

set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})
set(ZLIB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/Release/zlibstatic.lib)

set(PNG_INCLUDE_DIR ${PNG_DIR})
set(PNG_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/lpng1644/Release/libpng16_static.lib)

set(SDL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL/include)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/zconf.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)  
file(COPY ${PNG_DIR}/scripts/pnglibconf.h.prebuilt DESTINATION ${PNG_DIR})  
file(RENAME ${PNG_DIR}/pnglibconf.h.prebuilt ${PNG_DIR}/pnglibconf.h)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/*.h"
)

add_executable(CGlab_5 "src/lab5.cpp")

target_include_directories(CGlab_5 PRIVATE ${SDL_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/glm")
target_link_libraries(CGlab_5 PRIVATE SDL3::SDL3-static)

