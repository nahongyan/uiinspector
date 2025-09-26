/*
  widgetattributetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETATTRIBUTETAB_H
#define GAMMARAY_WIDGETATTRIBUTETAB_H

#include <QWidget>
// Needed for std::unique_ptr
#include <memory>
// Ensure model type is known in all translation units (including moc)
#include <QAbstractItemModel>

namespace GammaRay {

namespace Ui {
class WidgetAttributeTab;
}

class WidgetAttributeTab : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetAttributeTab(QWidget *parent = nullptr);
    ~WidgetAttributeTab() override;

    void setModel(QAbstractItemModel *model);

private:
    std::unique_ptr<Ui::WidgetAttributeTab> ui;
};
}

#endif // GAMMARAY_WIDGETATTRIBUTETAB_H
