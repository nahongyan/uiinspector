cmake_minimum_required(VERSION 3.16.0)

file(STRINGS version.txt GAMMARAY_VERSION_FILE)
list(GET GAMMARAY_VERSION_FILE 0 GammaRay_VERSION)

project(
    GammaRay
    VERSION ${GammaRay_VERSION}
    LANGUAGES CXX C
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to ${default_build_type} as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE STRING "Choose the type of build." FORCE
    )
    # Set the possible values of build type for cmake-gui
    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
                 "Debug"
                 "Release"
                 "MinSizeRel"
                 "RelWithDebInfo"
    )
endif()

include(GenerateExportHeader)
# Minimal build toggles for embedded client
set(GAMMARAY_CLIENT_ONLY_BUILD ON)
set(GAMMARAY_BUILD_UI ON)

# Provide minimal stubs/macros used by plugin CMake files
set(GAMMARAY_OUTPUT_PREFIX ${PROJECT_BINARY_DIR}/out)
set(PROBE_PLUGIN_INSTALL_DIR plugins)
set(PLUGIN_INSTALL_DIR plugins)
set(TARGET_PLUGIN_INSTALL_DIR gammaray-target)
set(LIBEXEC_INSTALL_DIR libexec)
set(BUNDLE_INSTALL_DIR_PRIVATE .)
set(BIN_INSTALL_DIR bin)
set(PROBE_INSTALL_DIR probe)
set(TRANSLATION_INSTALL_DIR translations)
set(QCH_INSTALL_DIR doc)

## Plugin system removed in this embedding; UI“插件”改为内置到 gammaray_ui

# A flat inverse layout (for SelfLocator-based discovery)
set(GAMMARAY_INVERSE_BUNDLE_DIR .)
set(GAMMARAY_INVERSE_BIN_DIR .)
set(GAMMARAY_INVERSE_LIB_DIR .)
set(GAMMARAY_INVERSE_PROBE_DIR .)
set(GAMMARAY_INVERSE_LIBEXEC_DIR .)

# Simplify plugin version/ABI for client-only; use "0" as version and a generic abi
set(GAMMARAY_PLUGIN_VERSION 0)
set(GAMMARAY_PROBE_ABI generic)
set(GAMMARAY_PROBE_ABI_POSTFIX "")

function(gammaray_set_rpath target dir)
    # No-op (legacy helper retained as stub)
endfunction()


# Basic compiler sanity checks
set(BAD_CXX_MESSAGE "")
if(MSVC)
    if(MSVC_VERSION LESS 1900)
        set(BAD_CXX_MESSAGE "MSVC 2015 or higher")
    endif()
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.1.0)
        set(BAD_CXX_MESSAGE "Clang v3.1.0 or higher")
    endif()
endif()
if(BAD_CXX_MESSAGE)
    message(FATAL_ERROR "Compiler too old: ${BAD_CXX_MESSAGE}")
endif()

set(GAMMARAY_REQUIRED_CXX_FEATURES
    cxx_lambdas
    cxx_nullptr
    cxx_range_for
    cxx_raw_string_literals
    cxx_uniform_initialization
    cxx_variadic_templates
    cxx_rvalue_references
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_override
    cxx_final
    cxx_nonstatic_member_init
)

set(GAMMARAY_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(GAMMARAY_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(GAMMARAY_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(GAMMARAY_VERSION ${PROJECT_VERSION})
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")

# Omit git revision info in minimal tree

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray-version.h
)

# Silence banner output for minimal build

set(GAMMARAY_LIBRARY_TYPE STATIC)


#
# Compiler & linker settings
#
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-") #use strict C++ compliance
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244") #conversion from __int64 to int possible loss of data
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267") #conversion from size_t to int possible loss of data
endif()

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Minimal Qt dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060500)
add_definitions(-DGAMMARAY_DISABLE_FEEDBACK=1)

if(NOT DEFINED QT_LIBINFIX)
    set(QT_LIBINFIX "")
endif()

# debug suffixes for qmake compatibility
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
elseif(APPLE)
    set(CMAKE_DEBUG_POSTFIX "_debug")
else()
    set(CMAKE_DEBUG_POSTFIX "")
endif()


add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING)
if(NOT WIN32)
    # BIC on MSVC at least (see e.g. https://bugreports.qt.io/browse/AUTOSUITE-946)
    add_definitions(-DQT_STRICT_ITERATORS)
endif()


# Simple and flat output tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)
foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${OUTPUTCONFIG}" _uc)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_uc} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_uc} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_uc} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endforeach()

#
# actually build the stuff
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray.h)

add_subdirectory(3rdparty)

add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(host)

# Convenient aliases for embedding
# add_library(GammaRay::core ALIAS gammaray_core)
add_library(GammaRay::common ALIAS gammaray_common)
add_library(GammaRay::ui ALIAS gammaray_ui)
add_library(GammaRay::host ALIAS gammaray_host)
add_library(GammaRay::kitemmodels ALIAS gammaray_kitemmodels)
