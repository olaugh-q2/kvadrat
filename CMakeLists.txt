cmake_minimum_required(VERSION 3.14)

#Set the project name. This is not the executable program's name!
project(graphical_template)


#Some CMake tomfoolery
if(POLICY CMP0135)
cmake_policy(SET CMP0135 NEW)
endif()
#End CMake tomfoolery


#Try to find a locally installed raylib, but don't quit on fail
find_package(raylib 5.0 QUIET)

#This code downloads raylib into a directory called _deps and adds it as a subdirectory, compiling it with the program when running the build command
include(FetchContent)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

#We don't want raylib's examples built. This option is picked up by raylib's CMakeLists.txt
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#Here, the executable is declared with its sources. "main", or "main.exe" on windows will be the program's name
add_executable(main "main.c" "constants.h" "game_state.h" "game_state.c"
               "inputs.h" "inputs.c" "next.h" "next.c" "playfield.h" "playfield.c" "square.h" "tetrominos.h"
               "tetrominos.c" "apple_compat_endian.h" "fileproxy.h" "fileproxy.c" "kwg.h" "log.h" "log.c"
               "string_util.h" "string_util.c" "util.h" "util.c" "stats.h" "stats.c")

add_executable(word_bag_maker "word_bag_maker.c" "apple_compat_endian.h" "fileproxy.h" "fileproxy.c" "kwg.h" "log.h" "log.c"
               "string_util.h" "string_util.c" "util.h" "util.c" "bag.h")

add_executable(permutation_evaluator "permutation_evaluator.c" "apple_compat_endian.h" "fileproxy.h" "fileproxy.c" "kwg.h" "log.h" "log.c"
               "string_util.h" "string_util.c" "util.h" "util.c" "bag.h" "tetrominos.h" "tetrominos.c" "playfield.h" "playfield.c" "square.h")               

#Link raylib to main
target_link_libraries(main 
    raylib
)

#set_target_properties(main PROPERTIES
#    MACOSX_BUNDLE TRUE
#    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
#)

target_link_libraries(permutation_evaluator 
    raylib
)

#Make main find the <raylib.h> header (and others)
target_include_directories(main PUBLIC "${raylib_SOURCE_DIR}/src")