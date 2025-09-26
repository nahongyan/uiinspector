

set(gammaray_hostlib_srcs
    classesiconsrepositoryclient.cpp
    classesiconsrepositoryclient.h
    clientconnectionmanager.cpp
    clientconnectionmanager.h
    enumrepositoryclient.cpp
    enumrepositoryclient.h
    favoriteobjectclient.cpp
    favoriteobjectclient.h
    messagestatisticsmodel.cpp
    messagestatisticsmodel.h
    paintanalyzerclient.cpp
    paintanalyzerclient.h
    probecontrollerclient.cpp
    probecontrollerclient.h
    propertycontrollerclient.cpp
    propertycontrollerclient.h
    remotemodel.cpp
    remotemodel.h
    remoteviewclient.cpp
    remoteviewclient.h
    selectionmodelclient.cpp
    selectionmodelclient.h
    toolmanagerclient.cpp
    toolmanagerclient.h
    localserverdevice.cpp
    localserverdevice.h
    client.cpp
    client.h
    serverdevice.cpp
    serverdevice.h
    multisignalmapper.cpp
    multisignalmapper.h
    gammaray_client_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/gammaray.qrc
)

add_library(
    gammaray_host STATIC
    ${gammaray_hostlib_srcs}
)

generate_export_header(gammaray_host)
# cmake-lint: disable=E1120
if(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES)
    set_target_properties(gammaray_host PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
endif()
target_compile_features(gammaray_host PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

target_include_directories(
    gammaray_host
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty>
)
target_compile_definitions(gammaray_host PRIVATE gammaray_host_ONLY_BUILD=1)
target_link_libraries(
    gammaray_host
    PUBLIC Qt::Core
    PRIVATE gammaray_ui
            gammaray_common
            Qt::Gui
            Qt::Widgets
            Qt::Network
)
if(GAMMARAY_USE_PCH)
    target_precompile_headers(gammaray_host REUSE_FROM gammaray_pch_core_gui)
endif()

