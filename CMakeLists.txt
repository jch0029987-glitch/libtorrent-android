cmake_minimum_required(VERSION 3.10...3.31)

project(libtorrent-android
        LANGUAGES C CXX)

# -------------------------------
# Android sanity checks
# -------------------------------
if (NOT ANDROID)
    message(FATAL_ERROR "This project is intended to be built for Android only")
endif()

message(STATUS "Building for Android ABI: ${ANDROID_ABI}")
message(STATUS "Android platform: ${ANDROID_PLATFORM}")

# -------------------------------
# Global build options
# -------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Shared library output
set(BUILD_SHARED_LIBS ON)

# Silence dev warnings from Android toolchain
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

# -------------------------------
# Performance flags (safe for Android)
# -------------------------------
add_compile_options(
    -O3
    -fvisibility=hidden
    -fvisibility-inlines-hidden
)

# -------------------------------
# libtorrent configuration
# -------------------------------
set(TORRENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_SIMULATOR OFF CACHE BOOL "" FORCE)

set(TORRENT_USE_BOOST ON CACHE BOOL "" FORCE)
set(TORRENT_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_LIBCRYPTO OFF CACHE BOOL "" FORCE)

# -------------------------------
# Force bundled Boost (CRITICAL)
# -------------------------------
set(BOOST_ROOT
    ${CMAKE_SOURCE_DIR}/third_party/libtorrent/deps/boost
    CACHE PATH "Bundled Boost path"
    FORCE
)

set(Boost_NO_SYSTEM_PATHS ON CACHE BOOL "" FORCE)
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "" FORCE)
set(Boost_NO_WARN_NEW_VERSIONS ON CACHE BOOL "" FORCE)

# libtorrent’s CMake macros expect this
set(BOOST_INCLUDEDIR ${BOOST_ROOT})

message(STATUS "Using bundled Boost from: ${BOOST_ROOT}")

# -------------------------------
# Add libtorrent
# -------------------------------
add_subdirectory(third_party/libtorrent)

# -------------------------------
# Export output .so cleanly
# -------------------------------
set_target_properties(torrent-rasterbar PROPERTIES
    OUTPUT_NAME "libtorrent"
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/output/${ANDROID_ABI}
)

# -------------------------------
# Strip symbols in Release
# -------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
        TARGET torrent-rasterbar POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:torrent-rasterbar>
    )
endif()

# -------------------------------
# Final status
# -------------------------------
message(STATUS "libtorrent Android build configured successfully")
