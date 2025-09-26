/*
  config-gammaray.h.cmake

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#if !defined(CONFIG_GAMMARAY_H)
#define CONFIG_GAMMARAY_H

/* #undef GAMMARAY_INSTALL_QT_LAYOUT */
#define GAMMARAY_DEBUG_POSTFIX "d"

// relative install dirs
#define GAMMARAY_PLUGIN_INSTALL_DIR "plugins"
#define GAMMARAY_LIBEXEC_INSTALL_DIR "libexec"
#define GAMMARAY_BUNDLE_INSTALL_DIR "."
#define GAMMARAY_BIN_INSTALL_DIR "bin"
#define GAMMARAY_PROBE_INSTALL_DIR "probe"
#define GAMMARAY_TRANSLATION_INSTALL_DIR "translations"
#define GAMMARAY_QCH_INSTALL_DIR "doc"
#define GAMMARAY_TARGET_PLUGIN_INSTALL_DIR "gammaray-target"

#define GAMMARAY_PLUGIN_VERSION "0"
#define GAMMARAY_PROBE_ABI "generic"

// relative inverse install dirs
#define GAMMARAY_INVERSE_BUNDLE_DIR "."
#define GAMMARAY_INVERSE_BIN_DIR "."
#define GAMMARAY_INVERSE_LIB_DIR "."
#define GAMMARAY_INVERSE_PROBE_DIR "."
#define GAMMARAY_INVERSE_LIBEXEC_DIR "."

// probe name
#define GAMMARAY_PROBE_BASENAME ""

// library ABI suffix, as used with >= Qt 5.14 on Android
#define ANDROID_ABI ""

// default network properties
#define GAMMARAY_DEFAULT_LOCAL_ADDRESS "127.0.0.1"
#define GAMMARAY_DEFAULT_LOCAL_TCP_URL "tcp://127.0.0.1"
#define GAMMARAY_DEFAULT_ANY_ADDRESS "0.0.0.0"
#define GAMMARAY_DEFAULT_ANY_TCP_URL "tcp://0.0.0.0"

// build options
/* #undef HAVE_STDINT_H */
/* #undef HAVE_BACKTRACE */
/* #undef HAVE_CXA_DEMANGLE */

/* #undef HAVE_QT_WIDGETS */
/* #undef HAVE_QT_SCXML */
/* #undef HAVE_QT_SVG */
/* #undef HAVE_QT_DESIGNER */

/* #undef HAVE_SYNTAX_HIGHLIGHTING */

/* #undef HAVE_ELF_H */
/* #undef HAVE_SYS_ELF_H */
/* #undef HAVE_ELF */

/* #undef GAMMARAY_CORE_ONLY_LAUNCHER */
/* #undef GAMMARAY_STATIC_PROBE */
/* #undef GAMMARAY_DISABLE_FEEDBACK */

#define QT_LIBINFIX ""

#endif // CONFIG_GAMMARAY_H
