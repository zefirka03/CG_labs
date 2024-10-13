cmake_minimum_required(VERSION 3.10)
project(CGlabs)

set(PNG_SHARED OFF)

# Укажите путь к папке deps
set(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)
set(PNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644)

# Найти zlib
set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})
set(ZLIB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/Release/zlibstatic.lib) # Укажите правильный путь к zlib.lib, если он существует

# Находим libpng
set(PNG_INCLUDE_DIR ${PNG_DIR})
set(PNG_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/lpng1644/Release/libpng16_static.lib) # Укажите правильный путь к libpng.lib, если он существует

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/lpng1644")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/build/deps/zlib-1.3.1/zconf.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib-1.3.1)  
file(COPY ${PNG_DIR}/scripts/pnglibconf.h.prebuilt DESTINATION ${PNG_DIR})  
file(RENAME ${PNG_DIR}/pnglibconf.h.prebuilt ${PNG_DIR}/pnglibconf.h)

file(GLOB_RECURSE SRC_FILES
	"src/*.cpp"
    "src/*.h"
)

add_executable(CGlabs ${SRC_FILES})

target_include_directories(CGlabs PRIVATE ${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
target_link_libraries(CGlabs PRIVATE ${ZLIB_LIBRARY} ${PNG_LIBRARY})