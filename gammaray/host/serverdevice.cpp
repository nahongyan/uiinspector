/*
  serverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "serverdevice.h"

#include "localserverdevice.h"

#include <QDebug>
#include <QUrl>

using namespace GammaRay;

ServerDevice::ServerDevice(QObject *parent)
    : QObject(parent)
{
}

ServerDevice::~ServerDevice() = default;

void ServerDevice::broadcast(const QByteArray &data)
{
    Q_UNUSED(data);
}

ServerDevice *ServerDevice::create(const QUrl &serverAddress, QObject *parent)
{
    ServerDevice *device = new LocalServerDevice(parent);
    if (!device) {
        qWarning() << "Unsupported transport protocol:" << serverAddress.toString();
        return nullptr;
    }
    return device;
}
