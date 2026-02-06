cmake_minimum_required(VERSION 3.10...3.31)

project(libtorrent-android LANGUAGES C CXX)

if (NOT ANDROID)
    message(FATAL_ERROR "Android build only")
endif()

# -------------------------------
# C++ / Android config
# -------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

# -------------------------------
# Performance flags
# -------------------------------
add_compile_options(
    -O3
    -fvisibility=hidden
    -fvisibility-inlines-hidden
)

# -------------------------------
# libtorrent options
# -------------------------------
set(TORRENT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(TORRENT_BUILD_SIMULATOR OFF CACHE BOOL "" FORCE)

set(TORRENT_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_LIBCRYPTO OFF CACHE BOOL "" FORCE)
set(TORRENT_USE_BOOST ON CACHE BOOL "" FORCE)

# -------------------------------
# FORCE bundled Boost (THIS FIXES YOUR ERROR)
# -------------------------------
set(BOOST_ROOT
    ${CMAKE_SOURCE_DIR}/third_party/libtorrent/deps/boost
    CACHE PATH "Bundled Boost root"
    FORCE
)

set(Boost_INCLUDE_DIR
    ${BOOST_ROOT}
    CACHE PATH "Bundled Boost include dir"
    FORCE
)

set(Boost_NO_SYSTEM_PATHS ON CACHE BOOL "" FORCE)
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "" FORCE)
set(Boost_NO_WARN_NEW_VERSIONS ON CACHE BOOL "" FORCE)

message(STATUS "Using Boost headers from: ${Boost_INCLUDE_DIR}")

# -------------------------------
# Add libtorrent
# -------------------------------
add_subdirectory(third_party/libtorrent)

# -------------------------------
# Output control
# -------------------------------
set_target_properties(torrent-rasterbar PROPERTIES
    OUTPUT_NAME "libtorrent"
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/output/${ANDROID_ABI}
)

# -------------------------------
# Strip symbols
# -------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(
        TARGET torrent-rasterbar POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:torrent-rasterbar>
    )
endif()

message(STATUS "libtorrent configured successfully")
