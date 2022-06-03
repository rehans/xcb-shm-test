cmake_minimum_required(VERSION 3.15.0)

include (FindPkgConfig)

project (xcb-shm-test)

add_executable(xcb-shm-test main.cpp)

pkg_check_modules(LIBXCB REQUIRED xcb)
pkg_check_modules(LIBXCB_UTIL REQUIRED xcb-util)
pkg_check_modules(LIBXCB_CURSOR REQUIRED xcb-cursor)
pkg_check_modules(LIBXCB_KEYSYMS REQUIRED xcb-keysyms)
pkg_check_modules(LIBXCB_XKB REQUIRED xcb-xkb)
pkg_check_modules(LIBXCB_IMAGE REQUIRED xcb-image)
pkg_check_modules(LIBXKB_COMMON REQUIRED xkbcommon)
pkg_check_modules(LIBXKB_COMMON_X11 REQUIRED xkbcommon-x11)

# message(FATAL_ERROR "rene ${LIBXCB_SHM}")

find_package(X11 REQUIRED )

target_link_libraries(xcb-shm-test
    PRIVATE
        ${X11_LIBRARIES}
        ${LIBXCB_LIBRARIES}
        ${LIBXCB_UTIL_LIBRARIES}
        ${LIBXCB_CURSOR_LIBRARIES}
        ${LIBXCB_KEYSYMS_LIBRARIES}
        ${LIBXCB_XKB_LIBRARIES}
        ${LIBXCB_IMAGE_LIBRARIES}
        ${LIBXKB_COMMON_LIBRARIES}
        ${LIBXKB_COMMON_X11_LIBRARIES}
)