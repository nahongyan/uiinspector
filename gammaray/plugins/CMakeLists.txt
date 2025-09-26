# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# Hardcoded minimal plugin set for embedded client in Qt Creator
set(GAMMARAY_SELECTED_PLUGINS
    modelinspector
    widgetinspector
    guisupport
    eventmonitor
    sysinfo
    mimetypes
    network
    localeinspector
)

foreach(_p IN LISTS GAMMARAY_SELECTED_PLUGINS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_p}/CMakeLists.txt)
        add_subdirectory(${_p})
    else()
        message(STATUS "GammaRay plugin '${_p}' not present in this minimal tree, skipping.")
    endif()
endforeach()
