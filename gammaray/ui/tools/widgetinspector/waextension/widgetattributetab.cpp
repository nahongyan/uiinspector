/*
  widgetattributetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "widgetattributetab.h"
#include "ui_widgetattributetab.h"
// Ensure Qt types are visible here regardless of include order
#include <QWidget>
#include <QAbstractItemModel>

using namespace GammaRay;

WidgetAttributeTab::WidgetAttributeTab(QWidget *parent)
    : QWidget(parent)
    , ui(std::unique_ptr<Ui::WidgetAttributeTab>(new Ui::WidgetAttributeTab))
{
    ui->setupUi(this);
}

WidgetAttributeTab::~WidgetAttributeTab() = default;

void WidgetAttributeTab::setModel(QAbstractItemModel *model)
{
    ui->attributeView->setModel(model);
}
