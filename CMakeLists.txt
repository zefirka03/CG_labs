cmake_minimum_required(VERSION 3.10)
project(CGlabs)

set(PNG_SHARED OFF)

set(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)
set(PNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644)

set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})
set(ZLIB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/Release/zlibstatic.lib)

set(PNG_INCLUDE_DIR ${PNG_DIR})
set(PNG_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/lpng1644/Release/libpng16_static.lib)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/zconf.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)  
file(COPY ${PNG_DIR}/scripts/pnglibconf.h.prebuilt DESTINATION ${PNG_DIR})  
file(RENAME ${PNG_DIR}/pnglibconf.h.prebuilt ${PNG_DIR}/pnglibconf.h)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/*.h"
)

file(GLOB_RECURSE SRC_GFRAME
${CMAKE_CURRENT_SOURCE_DIR}/deps/GFrameW32/GFrameW32/GF.cpp
${CMAKE_CURRENT_SOURCE_DIR}/deps/GFrameW32/GFrameW32/DIBSection.cpp
${CMAKE_CURRENT_SOURCE_DIR}/deps/GFrameW32/GFrameW32/stdafx.cpp
${CMAKE_CURRENT_SOURCE_DIR}/deps/GFrameW32/GFrameW32/GFrameW32.cpp
)

set(INCLUDE_GFRAME ${CMAKE_CURRENT_SOURCE_DIR}/deps/GFrameW32/GFrameW32)

add_executable(CGlab_1 "src/lab1.cpp" "src/png_files.h")
add_executable(CGlab_2 "src/lab2.cpp" "src/png_files.h")
add_executable(CGlab_dz3 "src/dz3.cpp" "src/png_files.h")

target_include_directories(CGlab_1 PRIVATE ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
target_link_libraries(CGlab_1 PRIVATE ${ZLIB_LIBRARY} ${PNG_LIBRARY})

target_include_directories(CGlab_2 PRIVATE ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
target_link_libraries(CGlab_2 PRIVATE ${ZLIB_LIBRARY} ${PNG_LIBRARY})

target_include_directories(CGlab_dz3 PRIVATE ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
target_link_libraries(CGlab_dz3 PRIVATE ${ZLIB_LIBRARY} ${PNG_LIBRARY})

