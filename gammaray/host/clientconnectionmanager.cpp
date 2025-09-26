/*
  clientconnectionmanager.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "clientconnectionmanager.h"

#include "client.h"
#include "enumrepositoryclient.h"
#include "classesiconsrepositoryclient.h"
#include "remotemodel.h"
#include "selectionmodelclient.h"
#include "propertycontrollerclient.h"
#include "probecontrollerclient.h"

#include "paintanalyzerclient.h"
#include "remoteviewclient.h"
#include "favoriteobjectclient.h"
#include <toolmanagerclient.h>

#include <common/objectbroker.h>
#include <common/streamoperators.h>

#include <ui/clienttoolmanager.h>
// For embedded view factory
#include <ui/clientview.h>

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

using namespace GammaRay;

static QAbstractItemModel *modelFactory(const QString &name)
{
    return new RemoteModel(name, qApp);
}

static QItemSelectionModel *selectionModelFactory(QAbstractItemModel *model)
{
    return new SelectionModelClient(model->objectName() + ".selection", model, qApp);
}

static QObject *createPropertyController(const QString &name, QObject *parent)
{
    return new PropertyControllerClient(name, parent);
}

static QObject *createProbeController(const QString &name, QObject *parent)
{
    QObject *o = new ProbeControllerClient(parent);
    ObjectBroker::registerObject(name, o);
    return o;
}

static QObject *createToolManager(const QString &name, QObject *parent)
{
    QObject *o = new ToolManagerClient(parent);
    ObjectBroker::registerObject(name, o);
    return o;
}

static QObject *createPaintAnalyzerClient(const QString &name, QObject *parent)
{
    return new PaintAnalyzerClient(name, parent);
}

static QObject *createRemoteViewClient(const QString &name, QObject *parent)
{
    return new RemoteViewClient(name, parent);
}

static QObject *createEnumRepositoryClient(const QString &, QObject *parent)
{
    return new EnumRepositoryClient(parent);
}

static QObject *createClassesIconsRepositoryClient(const QString &, QObject *parent)
{
    return new ClassesIconsRepositoryClient(parent);
}

static QObject *createFavoriteObjectClient(const QString &, QObject *parent)
{
    return new FavoriteObjectClient(parent);
}

void ClientConnectionManager::init()
{
    StreamOperators::registerOperators();

    ObjectBroker::registerClientObjectFactoryCallback<PropertyControllerInterface *>(createPropertyController);
    ObjectBroker::registerClientObjectFactoryCallback<ProbeControllerInterface *>(createProbeController);
    ObjectBroker::registerClientObjectFactoryCallback<ToolManagerInterface *>(createToolManager);
    ObjectBroker::registerClientObjectFactoryCallback<PaintAnalyzerInterface *>(createPaintAnalyzerClient);
    ObjectBroker::registerClientObjectFactoryCallback<RemoteViewInterface *>(createRemoteViewClient);
    ObjectBroker::registerClientObjectFactoryCallback<EnumRepository *>(createEnumRepositoryClient);
    ObjectBroker::registerClientObjectFactoryCallback<ClassesIconsRepository *>(createClassesIconsRepositoryClient);
    ObjectBroker::registerClientObjectFactoryCallback<FavoriteObjectInterface *>(createFavoriteObjectClient);

    ObjectBroker::setModelFactoryCallback(modelFactory);
    ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
}

ClientConnectionManager::ClientConnectionManager(QObject *parent)
    : QObject(parent)
    , m_server(new Client(this))
    , m_toolManager(new ClientToolManager(this))
{
    connect(m_toolManager, &ClientToolManager::toolListAvailable, this, &ClientConnectionManager::ready);
}

ClientConnectionManager::~ClientConnectionManager() = default;

ClientToolManager *ClientConnectionManager::toolManager() const
{
    return m_toolManager;
}

bool ClientConnectionManager::isServerListening() const
{
    if (m_server) {
        return m_server->isListening();
    }
    return false;
}

QWidget *ClientConnectionManager::createEmbeddedView(QWidget *parent)
{
    auto *view = new GammaRay::ClientView(parent);
    view->setToolManager(m_toolManager);
    return view;
}

void ClientConnectionManager::startServer(const QUrl &url)
{
    Q_ASSERT(m_server);
    m_server->listen(url);
}
