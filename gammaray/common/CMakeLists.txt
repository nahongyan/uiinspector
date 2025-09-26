# generate the class icon index map
file(READ "../ui/resources/classiconindex.map" _map_content)
string(REPLACE "\n" ";" _map_content ${_map_content})
set(CLASS_ICON_INDEX_CODE "")
foreach(_icon_data IN LISTS _map_content)
    if(_icon_data)
        string(REPLACE " " ";" _icon_data ${_icon_data})
        list(GET _icon_data 0 _icon_name)
        set(CLASS_ICON_INDEX_CODE "${CLASS_ICON_INDEX_CODE}\n    ADD_ICON(\"${_icon_name}\")")
    endif()
endforeach()
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsindex_data.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/classesiconsindex_data.cpp
)

add_library(
    gammaray_lz4 OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/lz4/lz4.c
)
set_target_properties(gammaray_lz4 PROPERTIES POSITION_INDEPENDENT_CODE ON)
set(gammaray_common_srcs
    classesiconsrepository.cpp
    classesiconsrepository.h
    commonutils.cpp
    commonutils.h
    endpoint.cpp
    endpoint.h
    enumdefinition.cpp
    enumdefinition.h
    enumrepository.cpp
    enumrepository.h
    enumvalue.cpp
    enumvalue.h
    message.cpp
    message.h
    methodargument.cpp
    methodargument.h
    modelevent.cpp
    modelevent.h
    modelutils.cpp
    modelutils.h
    objectbroker.cpp
    objectbroker.h
    objectid.h
    objectidfilterproxymodel.cpp
    objectidfilterproxymodel.h
    paintanalyzerinterface.cpp
    paintanalyzerinterface.h
    paths.cpp
    paths.h
    propertysyncer.cpp
    propertysyncer.h
    protocol.cpp
    protocol.h
    remoteviewframe.cpp
    remoteviewframe.h
    remoteviewinterface.cpp
    remoteviewinterface.h
    selflocator.cpp
    selflocator.h
    sourcelocation.cpp
    sourcelocation.h
    transferimage.cpp
    transferimage.h
    translator.cpp
    translator.h
)

add_library(
    gammaray_common
    STATIC ${gammaray_common_srcs}
)
generate_export_header(gammaray_common)
# cmake-lint: disable=E1120
if(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES)
    set_target_properties(gammaray_common PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
endif()
set_target_properties(gammaray_common PROPERTIES OUTPUT_NAME gammaray_common)
target_compile_features(gammaray_common PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

target_include_directories(
    gammaray_common
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    # Explicit (non-generator) paths to ensure config-gammaray.h is found in nested build
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty>
)
target_link_libraries(
    gammaray_common
    PUBLIC Qt::Core
    PRIVATE Qt::Network Qt::Gui Qt::GuiPrivate
)
target_link_libraries(
    gammaray_common
    PRIVATE $<BUILD_INTERFACE:gammaray_lz4> gammaray_kitemmodels ${CMAKE_DL_LIBS}
)
if(GAMMARAY_USE_PCH)
    target_precompile_headers(gammaray_common REUSE_FROM gammaray_pch_core_gui)
endif()

set(gammaray_common_internal_srcs
    favoriteobjectinterface.cpp
    favoriteobjectinterface.h
    networkselectionmodel.cpp
    networkselectionmodel.h
    plugininfo.cpp
    plugininfo.h
    pluginmanager.cpp
    pluginmanager.h
    probecontrollerinterface.cpp
    probecontrollerinterface.h
    propertycontrollerinterface.cpp
    propertycontrollerinterface.h
    proxyfactorybase.cpp
    proxyfactorybase.h
    streamoperators.cpp
    streamoperators.h
    toolmanagerinterface.cpp
    toolmanagerinterface.h
    tools/messagehandler/messagehandlerinterface.cpp
    tools/messagehandler/messagehandlerinterface.h
    tools/metatypebrowser/metatypebrowserinterface.cpp
    tools/metatypebrowser/metatypebrowserinterface.h
    tools/objectinspector/connectionsextensioninterface.cpp
    tools/objectinspector/connectionsextensioninterface.h
    tools/objectinspector/methodsextensioninterface.cpp
    tools/objectinspector/methodsextensioninterface.h
    tools/objectinspector/propertiesextensioninterface.cpp
    tools/objectinspector/propertiesextensioninterface.h
    tools/problemreporter/problemreporterinterface.cpp
    tools/problemreporter/problemreporterinterface.h
    tools/resourcebrowser/resourcebrowserinterface.cpp
    tools/resourcebrowser/resourcebrowserinterface.h
)

add_library(
    gammaray_common_internal OBJECT
    ${gammaray_common_internal_srcs}
)
set_target_properties(gammaray_common_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_common_internal PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

target_link_libraries(
    gammaray_common_internal gammaray_common
)
target_link_libraries(
    gammaray_common_internal Qt::Core Qt::Network
)


