#

cmake_minimum_required(VERSION 3.17)

project(easystroke LANGUAGES C CXX)

file(GLOB SOURCES
    *.c *.cc
    trace/*.cc
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED
    x11 xi xext xfixes xtst
    gtkmm-3.0 atk
    dbus-glib-1
)

find_package(Boost REQUIRED COMPONENTS serialization filesystem)

add_executable(easystroke
    ${SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/gui.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/cellrenderertextish.c
)

target_link_libraries(easystroke PRIVATE
    ${MODULES_LIBRARIES}
    ${Boost_LIBRARIES}
)
target_include_directories(easystroke SYSTEM PRIVATE
    ${MODULES_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_compile_options(easystroke PRIVATE
    -DLOCALEDIR="${CMAKE_INSTALL_PREFIX}/share/locale/"
    -DGETTEXT_PACKAGE="easystroke"
    -I${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(OUTPUT gui.cpp
    COMMAND echo "const char* gui_buffer = R\\\"\\<\\>\\(" > gui.cpp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/gui.glade
        | sed -s "s/\\\"GtkWindow\\\"/\\\"GtkApplicationWindow\\\"/"
        >> gui.cpp
    COMMAND echo "\\)\\<\\>\\\"\\;" >> gui.cpp
    DEPENDS gui.glade
)

add_custom_command(OUTPUT cellrenderertextish.h cellrenderertextish.c
    COMMAND valac --compile --ccode -H cellrenderertextish.h --pkg gtk+-3.0 ${CMAKE_CURRENT_SOURCE_DIR}/cellrenderertextish.vala
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cellrenderertextish.vala
)

add_custom_command(OUTPUT easystroke.desktop
    COMMAND intltool-merge --utf8 --desktop-style ${CMAKE_CURRENT_SOURCE_DIR}/po ${CMAKE_CURRENT_SOURCE_DIR}/easystroke.desktop.in easystroke.desktop
    VERBATIM
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/easystroke.desktop.in
)

add_custom_command(OUTPUT desktop.cpp
    COMMAND echo "const char* desktop_file = R\\\"\\<\\>\\(" > desktop.cpp
    COMMAND cat easystroke.desktop
        | sed -s 's/Exec=easystroke/Exec=%1$$s/'
        >> desktop.cpp
    COMMAND echo "\\)\\<\\>\\\"\\;" >> desktop.cpp
    DEPENDS easystroke.desktop
)

install(
    TARGETS easystroke
    DESTINATION bin
)

install(
    FILES easystroke.svg
    DESTINATION share/icons/hicolor/scalable/apps
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/easystroke.desktop
    DESTINATION share/applications
)

#install(
#    FILES ${CMAKE_CURRENT_BINARY_DIR}/po/*/LC_MESSAGES/easystroke.mo
#    DESTINATION share/locale
#)
