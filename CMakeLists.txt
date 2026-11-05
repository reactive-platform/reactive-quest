# include some defines automatically made by qpm
include(qpm_defines.cmake)

# Bump Android sdk version
set(ANDROID_PLATFORM 29)

# override mod id
set(MOD_ID "reactive")

# Enable link time optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

cmake_minimum_required(VERSION 3.21)
project(${COMPILE_ID})

# c++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# define that stores the actual source directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(YOGA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/yoga)

# compile options used
add_compile_options(-frtti -fexceptions)
# Reactive v2 props use C++20 designated initializers in any order, like the
# C# object initializers they mirror; clang accepts that and only warns.
add_compile_options(-Wno-reorder-init-list)
# Keep frame pointers + full unwind tables so libunwind can resolve backtraces
# accurately when we crash. Without these, the unwinder logs "malformed DWARF"
# and reports misleading symbols/offsets.
add_compile_options(-fno-omit-frame-pointer -funwind-tables -fasynchronous-unwind-tables)
add_compile_options(-O3)
# compile definitions used
add_compile_definitions(VERSION=\"${MOD_VERSION}\")
add_compile_definitions(MOD_ID=\"${MOD_ID}\")
add_compile_definitions(USE_CODEGEN_FIELDS)

# recursively get all src files
RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list ${SOURCE_DIR}/*.c)

RECURSE_FILES(src_inline_hook_beatsaber_hook_local_extra_c ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
RECURSE_FILES(src_inline_hook_beatsaber_hook_local_extra_cpp ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)

# add all src files to compile
add_library(
        ${COMPILE_ID}
        SHARED
        ${cpp_file_list}
        ${c_file_list}
        ${src_inline_hook_beatsaber_hook_local_extra_c}
        ${src_inline_hook_beatsaber_hook_local_extra_cpp}
)

target_include_directories(${COMPILE_ID} PRIVATE .)

# add src dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${SOURCE_DIR})
# add include dir as include dir
target_include_directories(${COMPILE_ID} PRIVATE ${INCLUDE_DIR})
# add shared dir as include dir (this is the public-headers tree consumers see)
target_include_directories(${COMPILE_ID} PUBLIC ${SHARED_DIR})
# codegen includes
target_include_directories(${COMPILE_ID} PRIVATE ${EXTERN_DIR}/includes/${CODEGEN_ID}/include)

target_link_libraries(${COMPILE_ID} PRIVATE -llog)

# Yoga is built as a static lib and linked into our .so.
if(NOT EXISTS ${YOGA_DIR}/yoga/CMakeLists.txt)
    message(FATAL_ERROR "Yoga is missing, run: git submodule update --init")
endif()
add_subdirectory(${YOGA_DIR}/yoga ${CMAKE_CURRENT_BINARY_DIR}/yoga)
target_link_libraries(${COMPILE_ID} PRIVATE yogacore)

# add extern stuff like libs and other includes
include(extern.cmake)

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_STRIP} -d --strip-all
        "lib${COMPILE_ID}.so" -o "stripped_lib${COMPILE_ID}.so"
        COMMENT "Strip debug symbols done on final binary.")

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory debug
        COMMENT "Make directory for debug symbols"
        )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename lib${COMPILE_ID}.so debug/lib${COMPILE_ID}.so
        COMMENT "Rename the lib to debug_ since it has debug symbols"
        )

add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename stripped_lib${COMPILE_ID}.so lib${COMPILE_ID}.so
        COMMENT "Rename the stripped lib to regular"
        )

###
### Laurie's fully™️-functional objcopy cmake script
### Kept for M6 (Reactive.BeatSaber asset bundle). Empty assets dir is a no-op.
###

set(ASSETS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(ASSET_BINARIES_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/binaryAssets)
set(ASSET_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/assets.hpp")

set(ASSET_HEADER_DATA "#define DECLARE_FILE(name, prefix) extern \"C\" uint8_t _binary_##name##_start[]; extern \"C\" uint8_t _binary_##name##_end[]; struct prefix##name { static size_t getLength() { return _binary_##name##_end - _binary_##name##_start; } static uint8_t* getData() { return _binary_##name##_start; } };\n")
file(MAKE_DIRECTORY ${ASSETS_DIRECTORY})
file(MAKE_DIRECTORY ${ASSET_BINARIES_DIRECTORY})
# CONFIGURE_DEPENDS makes CMake re-evaluate the glob at build time, so a
# newly-dropped asset gets picked up without a manual reconfigure.
file(GLOB ASSETS CONFIGURE_DEPENDS ${ASSETS_DIRECTORY}/*.*)

foreach(FILE IN LISTS ASSETS)
    get_filename_component(ASSET ${FILE} NAME)
    set(OUTPUT_FILE "${ASSET_BINARIES_DIRECTORY}/${ASSET}.o")

    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${CMAKE_OBJCOPY} ${ASSET} ${OUTPUT_FILE} --input-target binary --output-target elf64-aarch64
            DEPENDS ${ASSETS_DIRECTORY}/${ASSET}
            WORKING_DIRECTORY ${ASSETS_DIRECTORY}
    )
    list(APPEND BINARY_ASSET_FILES ${OUTPUT_FILE})

    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" FIXED_ASSET ${ASSET})
    set(ASSET_HEADER_DATA "${ASSET_HEADER_DATA}DECLARE_FILE(${FIXED_ASSET},)\n")
endforeach()
file(GENERATE OUTPUT ${ASSET_HEADER_PATH} CONTENT "${ASSET_HEADER_DATA}")

if(BINARY_ASSET_FILES)
    add_library(asset_files OBJECT ${BINARY_ASSET_FILES})
    set_target_properties(asset_files PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${COMPILE_ID} PRIVATE asset_files ${BINARY_ASSET_FILES})
endif()
