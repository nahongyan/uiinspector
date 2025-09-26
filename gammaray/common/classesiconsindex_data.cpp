/*
  classesiconsindex_data.cpp.in

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <QHash>
#include <QString>

namespace GammaRay {
namespace ClassesIconsIndex {
static QHash<int, QString> s_numberToKey;
static QHash<QString, int> s_keyToNumber;

static void initializeClassesIconsIndex()
{
    Q_ASSERT(s_numberToKey.isEmpty());
    Q_ASSERT(s_keyToNumber.isEmpty());
    int i = 0;

#define ADD_ICON(x) \
    s_numberToKey[i] = QLatin1String(":/gammaray/icons/ui/classes/" x); \
    s_keyToNumber[s_numberToKey[i]] = i; \
    ++i;

    
    ADD_ICON("QQuickBorderImage/default.png")
    ADD_ICON("QQuickFlickable/default.png")
    ADD_ICON("QQuickFlipable/default.png")
    ADD_ICON("QQuickFocusScope/default.png")
    ADD_ICON("QQuickGridView/default.png")
    ADD_ICON("QQuickImage/default.png")
    ADD_ICON("QQuickItem/default.png")
    ADD_ICON("QQuickListView/default.png")
    ADD_ICON("QQuickMouseArea/default.png")
    ADD_ICON("QQuickPathView/default.png")
    ADD_ICON("QQuickRectangle/default.png")
    ADD_ICON("QQuickTextEdit/default.png")
    ADD_ICON("QQuickText/default.png")
    ADD_ICON("QQuickTextInput/default.png")
    ADD_ICON("QQuickWebView/default.png")
    ADD_ICON("QDateTimeEdit/default.png")
    ADD_ICON("QTextEdit/default.png")
    ADD_ICON("QCalendarWidget/default.png")
    ADD_ICON("QProgressBar/default.png")
    ADD_ICON("QAbstractScrollArea/default.png")
    ADD_ICON("QLineEdit/default.png")
    ADD_ICON("QDockWidget/default.png")
    ADD_ICON("QScrollBar/default.png")
    ADD_ICON("QComboBox/default.png")
    ADD_ICON("QCommandLinkButton/default.png")
    ADD_ICON("QWizard/default.png")
    ADD_ICON("QWidget/default.png")
    ADD_ICON("QSpinBox/default.png")
    ADD_ICON("QScrollBar/orientation=Horizontal.png")
    ADD_ICON("QCheckBox/default.png")
    ADD_ICON("QTabWidget/default.png")
    ADD_ICON("QDateEdit/default.png")
    ADD_ICON("QLabel/default.png")
    ADD_ICON("QListView/default.png")
    ADD_ICON("QColumnView/default.png")
    ADD_ICON("QDial/default.png")
    ADD_ICON("QFrame/default.png")
    ADD_ICON("QTabBar/default.png")
    ADD_ICON("QSlider/default.png")
    ADD_ICON("QGroupBox/default.png")
    ADD_ICON("QGraphicsView/default.png")
    ADD_ICON("QMdiArea/default.png")
    ADD_ICON("QFontComboBox/default.png")
    ADD_ICON("QDialogButtonBox/default.png")
    ADD_ICON("QToolBox/default.png")
    ADD_ICON("QTimeEdit/default.png")
    ADD_ICON("QPlainTextEdit/default.png")
    ADD_ICON("QSlider/orientation=Vertical.png")
    ADD_ICON("QDoubleSpinBox/default.png")
    ADD_ICON("QLCDNumber/default.png")
    ADD_ICON("QToolButton/default.png")
    ADD_ICON("QTableView/default.png")
    ADD_ICON("QStackedWidget/default.png")
    ADD_ICON("QRadioButton/default.png")
    ADD_ICON("QPushButton/default.png")
    ADD_ICON("QAbstractItemView/default.png")
    ADD_ICON("QSplitter/orientation=Horizontal.png")
    ADD_ICON("QHBoxLayout/default.png")
    ADD_ICON("QGridLayout/default.png")
    ADD_ICON("QSplitter/default.png")
    ADD_ICON("QVBoxLayout/default.png")
    ADD_ICON("QFormLayout/default.png")

#undef ADD_ICON
}
}
}
