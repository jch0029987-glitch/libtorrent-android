cmake_minimum_required(VERSION 3.10...3.31)

project(libtorrent-android LANGUAGES C CXX)

# ------------------------------------------------------------------------------
# Android Guard & Basic Config
# ------------------------------------------------------------------------------
if (NOT ANDROID)
    message(FATAL_ERROR "This project is configured for Android cross-compilation only.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)

# Suppress warnings from submodules to keep build logs clean
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# Optimization & Binary Size Control
# ------------------------------------------------------------------------------
add_compile_options(
    -O3
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    -ffunction-sections
    -fdata-sections
)

if(NOT MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# ------------------------------------------------------------------------------
# Libtorrent Configuration
# ------------------------------------------------------------------------------
set(TORRENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_SIMULATOR OFF CACHE BOOL "" FORCE)

# For Android, we disable SSL to avoid external OpenSSL dependencies.
set(TORRENT_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_LIBCRYPTO OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_BOOST ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# Smart Boost Detection (The "Hunter" Logic)
# ------------------------------------------------------------------------------
# We search recursively for version.hpp inside the expected deps folder
file(GLOB_RECURSE BOOST_VERSION_PATH 
    "${CMAKE_SOURCE_DIR}/third_party/libtorrent/deps/boost/*/version.hpp"
    "${CMAKE_SOURCE_DIR}/third_party/libtorrent/deps/boost/version.hpp"
)

if (BOOST_VERSION_PATH)
    # Get the first match found
    list(GET BOOST_VERSION_PATH 0 _MATCH)
    # version.hpp is in .../boost/version.hpp. 
    # We need the parent of the folder containing 'version.hpp'
    get_filename_component(_BOOST_DIR "${_MATCH}" DIRECTORY)
    get_filename_component(BOOST_FINAL_INC "${_BOOST_DIR}" DIRECTORY)
    
    message(STATUS "Boost Detection: Found version.hpp at ${_MATCH}")
    message(STATUS "Boost Detection: Setting Include Dir to ${BOOST_FINAL_INC}")
else()
    message(FATAL_ERROR "Boost headers not found! Ensure submodules are checked out in third_party/libtorrent/deps/boost")
endif()

# Force the standard FindBoost module to use our discovered path
set(Boost_INCLUDE_DIR "${BOOST_FINAL_INC}" CACHE PATH "" FORCE)
set(BOOST_INCLUDEDIR "${BOOST_FINAL_INC}" CACHE PATH "" FORCE)
set(Boost_NO_SYSTEM_PATHS ON CACHE BOOL "" FORCE)
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# Add Libtorrent Source
# ------------------------------------------------------------------------------
add_subdirectory(third_party/libtorrent)

# ------------------------------------------------------------------------------
# Target Properties & Android Output
# ------------------------------------------------------------------------------
if (TARGET torrent-rasterbar)
    set_target_properties(torrent-rasterbar PROPERTIES
        OUTPUT_NAME "torrent"
        # This places the .so in a clean folder structure for the workflow to find
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/${ANDROID_ABI}"
    )

    # Allow libtorrent to log to Android Logcat
    target_link_libraries(torrent-rasterbar PRIVATE log)
endif()

# ------------------------------------------------------------------------------
# Post-Build: Strip Symbols
# ------------------------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
        TARGET torrent-rasterbar POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:torrent-rasterbar>
        COMMENT "Stripping debug symbols..."
    )
endif()

message(STATUS "Configuration complete for ABI: ${ANDROID_ABI}")
