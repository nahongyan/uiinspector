/*
  client.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "client.h"
#include "serverdevice.h"
#include "LocalServerDevice.h"
#include "messagestatisticsmodel.h"

#include <common/message.h>
#include <common/objectbroker.h>
#include <common/propertysyncer.h>

#include <QDebug>
#include <QUrl>

using namespace GammaRay;

Client::Client(QObject *parent)
    : Endpoint(parent)
    , m_serverDevice(nullptr)
    , m_statModel(new MessageStatisticsModel(this))
    , m_initState(0)
{
    Message::resetNegotiatedDataVersion();

    connect(this, &Endpoint::disconnected, this, &Client::socketDisconnected);

    m_propertySyncer->setRequestInitialSync(true);

    ObjectBroker::registerModelInternal(QStringLiteral(
                                            "com.kdab.GammaRay.MessageStatisticsModel"),
                                        m_statModel);
}

Client::~Client()
{
    socketDisconnected();
    disconnect(this, &Endpoint::disconnected, this, &Client::socketDisconnected);
}

Client *Client::instance()
{
    return static_cast<Client *>(s_instance);
}

bool Client::isRemoteClient() const
{
    return true;
}

bool Client::listen(const QUrl &url)
{
    m_statModel->clear();
    m_serverDevice = ServerDevice::create(url, this);
    if (!m_serverDevice) {
        emit persisitentConnectionError(tr("Unsupported transport protocol."));
        return false;
    }

    static_cast<LocalServerDevice*>(m_serverDevice)->setServerAddress(url);
    connect(m_serverDevice, &ServerDevice::newConnection, this, &Client::socketConnected);
    m_serverDevice->listen();
    m_serverAddress = url;
    return true;
}

bool Client::isListening() const
{
    return m_serverDevice->isListening();
}

QUrl Client::serverAddress() const
{
    return m_serverAddress;
}

void Client::socketConnected()
{
    m_initState = 0;
    //Q_ASSERT(m_serverDevice->nextPendingConnection());
    setDevice(m_serverDevice->nextPendingConnection());
}

void Client::resetClientDevice()
{
    if (m_serverDevice) {
        m_serverDevice->deleteLater();
        m_serverDevice = nullptr;
    }
}

void Client::socketDisconnected()
{
    foreach (const auto &objInfo, objectAddresses())
        removeObjectNameAddressMapping(objInfo.second);
    ObjectBroker::clear();
    resetClientDevice();
}

void Client::messageReceived(const Message &msg)
{
    // qDebug() << "Client::messageReceived" << msg.type() << msg.address() << msg.size();
    m_statModel->addMessage(msg.address(), msg.type(), msg.size());
    // server version must be the very first message we get
    if (!(m_initState & VersionChecked)) {
        if (msg.address() != endpointAddress() || msg.type() != Protocol::ServerVersion) {
            emit persisitentConnectionError(tr(
                "Protocol violation, first message is not the server version."));
            socketDisconnected();
        }
        qint32 serverVersion;
        msg >> serverVersion;
        if (serverVersion != Protocol::version()) {
            emit persisitentConnectionError(tr("Gammaray Protocol Mismatch.\n"
                                               "Probe version is %1, was expecting %2.")
                                                .arg(
                                                    serverVersion)
                                                .arg(Protocol::version()));
            socketDisconnected();
        }
        m_initState |= VersionChecked;
        return;
    }

    if (msg.address() == endpointAddress()) {
        switch (msg.type()) {
        case Protocol::ObjectAdded: {
            QString name;
            Protocol::ObjectAddress addr;
            msg >> name >> addr;
            addObjectNameAddressMapping(name, addr);
            m_statModel->addObject(addr, name);
            break;
        }
        case Protocol::ObjectRemoved: {
            QString name;
            msg >> name;
            removeObjectNameAddressMapping(name);
            break;
        }
        case Protocol::ObjectMapReply: {
            QVector<QPair<Protocol::ObjectAddress, QString>> objects;
            msg >> objects;
            for (auto it = objects.constBegin(); it != objects.constEnd(); ++it) {
                if (it->first != endpointAddress())
                    addObjectNameAddressMapping(it->second, it->first);
                m_statModel->addObject(it->first, it->second);
            }

            m_propertySyncer->setAddress(objectAddress(QStringLiteral(
                "com.kdab.GammaRay.PropertySyncer")));
            Q_ASSERT(m_propertySyncer->address() != Protocol::InvalidObjectAddress);
            Endpoint::registerMessageHandler(
                m_propertySyncer->address(), m_propertySyncer, "handleMessage");

            m_initState |= ObjectMapReceived;
            break;
        }
        case Protocol::ServerInfo: {
            QString label;
            QString key;
            qint64 pid;
            quint8 dataVersion;
            msg >> label >> key >> pid >> dataVersion;
            setLabel(label);
            setKey(key);
            setPid(pid);

            {
                const quint8 version = qMin(dataVersion, Message::highestSupportedDataVersion());
                Message msg(endpointAddress(), Protocol::ClientDataVersionNegotiated);
                msg << version;
                send(msg);
            }

            m_initState |= ServerInfoReceived;
            break;
        }
        case Protocol::ServerDataVersionNegotiated: {
            quint8 version;
            msg >> version;
            Message::setNegotiatedDataVersion(version);

            m_initState |= ServerDataVersionNegotiated;
            break;
        }
        default:
            qWarning() << Q_FUNC_INFO << "Got unhandled message:" << msg.type();
            return;
        }
        if (m_initState == InitComplete) {
            m_initState |= ConnectionEstablished;
            emit connectionEstablished();
        }
    } else {
        dispatchMessage(msg);
    }
}

Protocol::ObjectAddress Client::registerObject(const QString &name, QObject *object)
{
    Q_ASSERT(isConnected());
    Protocol::ObjectAddress address = Endpoint::registerObject(name, object);
    m_propertySyncer->addObject(address, object);
    m_propertySyncer->setObjectEnabled(address, true);

    monitorObject(address);
    return address;
}

void Client::registerMessageHandler(Protocol::ObjectAddress objectAddress, QObject *receiver,
                                    const char *messageHandlerName)
{
    Q_ASSERT(isConnected());
    Endpoint::registerMessageHandler(objectAddress, receiver, messageHandlerName);
    monitorObject(objectAddress);
}

void Client::unregisterMessageHandler(Protocol::ObjectAddress objectAddress)
{
    Endpoint::unregisterMessageHandler(objectAddress);
    unmonitorObject(objectAddress);
}

void Client::objectDestroyed(Protocol::ObjectAddress objectAddress, const QString & /*objectName*/,
                             QObject * /*object*/)
{
    unmonitorObject(objectAddress);
}

void Client::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString & /*objectName*/)
{
    unmonitorObject(objectAddress);
}

void Client::monitorObject(Protocol::ObjectAddress objectAddress)
{
    if (!isConnected())
        return;
    Message msg(endpointAddress(), Protocol::ObjectMonitored);
    msg << objectAddress;
    send(msg);
}

void Client::unmonitorObject(Protocol::ObjectAddress objectAddress)
{
    if (!isConnected())
        return;
    Message msg(endpointAddress(), Protocol::ObjectUnmonitored);
    msg << objectAddress;
    send(msg);
}

void Client::doSendMessage(const GammaRay::Message &msg)
{
    m_statModel->addMessage(msg.address(), msg.type(), msg.size());
    Endpoint::doSendMessage(msg);
}
