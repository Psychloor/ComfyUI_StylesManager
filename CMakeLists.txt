cmake_minimum_required(VERSION 3.31)

# Version numbers that you can manually control
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)

# Get the latest commit count for the patch version
execute_process(
        COMMAND git rev-list --count HEAD
        OUTPUT_VARIABLE PROJECT_VERSION_PATCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the current git hash
execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Combine version numbers
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(StylesManager
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX
)

# Generate a header file with version information
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h
)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
        src/main.cpp
        src/main_window.cpp
        src/new_entry_dialog.cpp
        src/settings.cpp
)

# Header files
set(HEADERS
        include/main_window.h
        include/prompt_entry.h
        include/prompt_entry_list_model.h
        include/prompt_name_validator.h
        include/new_entry_dialog.h
        include/settings.h
)

# UI files
set(UI_FILES
        ui/main_window.ui
        ui/new_entry_dialog.ui
)

# Add MSVC-specific compiler flags
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif ()

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Specify UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_DIR "" CACHE PATH "Path to Qt installation")

if(QT_DIR)
    set(CMAKE_PREFIX_PATH ${QT_DIR})
else()
    # don't set and see how cmake does
endif()


if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_CXX_FLAGS "-O2")
    add_compile_options(-O2)
endif ()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

add_library(json INTERFACE)
add_library(csv INTERFACE)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(StylesManager WIN32
        ${HEADERS}
        ${SOURCES}
        ${UI_FILES}
)

target_link_libraries(StylesManager PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

# Include third-party headers
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/csv
        ${CMAKE_CURRENT_BINARY_DIR}
)


# Deploy Qt DLLs
if (WIN32 AND MSVC)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
    endif ()

    # Copy Qt plugins
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()

    # Copy Qt DLLs
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach ()
endif ()


