# generate the class icon qrc file
file(READ "classiconindex.map" _map_content)
string(REPLACE "\n" ";" _map_content ${_map_content})
set(QRC_CONTENT "")
foreach(_icon_data IN LISTS _map_content)
    if(_icon_data)
        string(REPLACE " " ";" _icon_data ${_icon_data})
        list(GET _icon_data 0 _icon_name)
        list(GET _icon_data 1 _icon_file)
        # cmake-lint: disable=C0301
        set(QRC_CONTENT
            "${QRC_CONTENT}\n        <file alias=\"${_icon_name}\">${CMAKE_CURRENT_SOURCE_DIR}/gammaray/ui/icons/classes/${_icon_file}</file>"
        )
    endif()
endforeach()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/classicons.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/classicons.qrc)
