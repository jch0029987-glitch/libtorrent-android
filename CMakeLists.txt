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

# Suppress warnings from submodules
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

# Linker-level dead code elimination
if(NOT MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# ------------------------------------------------------------------------------
# Libtorrent Configuration (Pre-Subdirectory)
# ------------------------------------------------------------------------------
set(TORRENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_SIMULATOR OFF CACHE BOOL "" FORCE)

# For a lean Android build, we disable SSL by default. 
# If you need HTTPS trackers, set these to ON and provide OpenSSL for Android.
set(TORRENT_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_LIBCRYPTO OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_BOOST ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# Boost Submodule Detection (Fixes FindBoost Errors)
# ------------------------------------------------------------------------------
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/third_party/libtorrent/deps/boost")

# Libtorrent's internal FindBoost needs to see version.hpp.
# It usually lives in /boost/version.hpp OR /include/boost/version.hpp.
if (EXISTS "${BOOST_ROOT}/boost/version.hpp")
    set(BOOST_FINAL_INC "${BOOST_ROOT}")
elseif (EXISTS "${BOOST_ROOT}/include/boost/version.hpp")
    set(BOOST_FINAL_INC "${BOOST_ROOT}/include")
else()
    message(FATAL_ERROR "Boost headers not found at ${BOOST_ROOT}. Ensure submodules are initialized.")
endif()

# Force CMake to use our bundled Boost headers exclusively
set(Boost_INCLUDE_DIR "${BOOST_FINAL_INC}" CACHE PATH "" FORCE)
set(BOOST_INCLUDEDIR "${BOOST_FINAL_INC}" CACHE PATH "" FORCE)
set(Boost_NO_SYSTEM_PATHS ON CACHE BOOL "" FORCE)
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "" FORCE)

message(STATUS "Using Boost headers from: ${BOOST_FINAL_INC}")

# ------------------------------------------------------------------------------
# Add Libtorrent Source
# ------------------------------------------------------------------------------
add_subdirectory(third_party/libtorrent)

# ------------------------------------------------------------------------------
# Target Properties & Android Output
# ------------------------------------------------------------------------------
# libtorrent 2.0 uses 'torrent-rasterbar' as the target name
if (TARGET torrent-rasterbar)
    set_target_properties(torrent-rasterbar PROPERTIES
        OUTPUT_NAME "torrent"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/${ANDROID_ABI}"
    )

    # Link the Android logging library so libtorrent can output to logcat
    target_link_libraries(torrent-rasterbar PRIVATE log)
endif()

# ------------------------------------------------------------------------------
# Post-Build: Strip Symbols for Release
# ------------------------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
        TARGET torrent-rasterbar POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:torrent-rasterbar>
        COMMENT "Stripping debug symbols from libtorrent.so"
    )
endif()

message(STATUS "libtorrent-android configured for ABI: ${ANDROID_ABI}")
