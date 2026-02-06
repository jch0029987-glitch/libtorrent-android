cmake_minimum_required(VERSION 3.22)
project(libtorrent_android LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Performance flags
add_compile_options(
    -O3
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    -fPIC
)

add_definitions(
    -DTORRENT_DISABLE_LOGGING
    -DTORRENT_DISABLE_EXTENSIONS
)

set(BUILD_SHARED_LIBS ON)
set(TORRENT_BUILD_TESTS OFF)
set(TORRENT_BUILD_EXAMPLES OFF)
set(TORRENT_BUILD_TOOLS OFF)

add_subdirectory(third_party/libtorrent)

set_target_properties(torrent-rasterbar PROPERTIES
    OUTPUT_NAME "torrent"
)
