# Qt Creator plugin that embeds the minimal GammaRay client+UI

cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_path(QtCreatorAPI_DIR QtCreatorAPI.cmake
  HINTS
    $ENV{QTC_SOURCE}/cmake
    $ENV{QTCREATOR_SOURCE}/cmake
    $ENV{QTC_SOURCE_DIR}/cmake
    $ENV{QTCREATOR_SOURCE_DIR}/cmake
    ${CMAKE_PREFIX_PATH}/lib/cmake/QtCreator
    ${CMAKE_PREFIX_PATH}/share/cmake/QtCreator
)

if(QtCreatorAPI_DIR)
  list(APPEND CMAKE_MODULE_PATH "${QtCreatorAPI_DIR}")
endif()

include(QtCreatorAPI OPTIONAL RESULT_VARIABLE QTC_API_INCLUDED)
if(NOT QTC_API_INCLUDED)
  message(FATAL_ERROR "QtCreatorAPI.cmake not found. Set CMAKE_PREFIX_PATH or QTC_SOURCE to Qt Creator sources.")
endif()

set(PLUGIN_NAME uiinspector)


set(gammaray_launcher_shared_srcs
)

if(WIN32)
    list(
        APPEND
        gammaray_launcher_shared_srcs
        injector/basicwindllinjector.cpp
        injector/basicwindllinjector.h
    )
else()
    if(APPLE)
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_mac.cpp)
    elseif(UNIX)
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_elf.cpp)
    else()
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_dummy.cpp)
    endif()
endif()


add_qtc_plugin(${PLUGIN_NAME}
  PLUGIN_DEPENDS Core ProjectExplorer QtSupport
  DEPENDS Qt::Core Qt::Widgets Qt::Network
  SOURCES
  ${gammaray_launcher_shared_srcs}
  uiinspector.json.in
  uiinspectorplugin.cpp
  uiinspectorplugin.h
  uiinspectormode.cpp
  uiinspectormode.h
  uiinspectorsettings.h
  uiinspectorsettings.cpp
  uiinspectorinjectemanager.h
  uiinspectorinjectemanager.cpp
  uiinspectoroptionspage.h
  uiinspectoroptionspage.cpp
  $<$<BOOL:${WIN32}>:injector/basicwindllinjector.cpp>
  $<$<BOOL:${WIN32}>:injector/basicwindllinjector.h>
  icons/uiinspector.qrc
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  QtCreator::Core QtCreator::Utils QtCreator::ProjectExplorer QtCreator::QtSupport
)

if(WIN32)
  # For OpenProcess/VirtualAllocEx/CreateRemoteThread/FormatMessageW
  target_link_libraries(${PLUGIN_NAME} PRIVATE
    kernel32
    user32
    advapi32
    psapi
    # version
  )
elseif(APPLE)
    target_link_libraries(gammaray_launcher PRIVATE "-framework CoreFoundation")
endif()

set(QTC_PLUGIN_DIR "$<TARGET_FILE_DIR:${PLUGIN_NAME}>")

add_subdirectory(gammaray)
target_link_libraries(${PLUGIN_NAME} PRIVATE gammaray_host gammaray_common gammaray_ui)

