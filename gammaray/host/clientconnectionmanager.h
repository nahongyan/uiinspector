/*
  clientconnectionmanager.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTCONNECTIONMANAGER_H
#define GAMMARAY_CLIENTCONNECTIONMANAGER_H

#include "gammaray_client_export.h"

#include <QObject>
#include <QUrl>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QWidget;
QT_END_NAMESPACE

namespace GammaRay {
class Client;
class MainWindow;
class ClientToolManager;

/*! Pre-MainWindow connection setup logic.
 *
 * This has been modified to work as a server that accepts probe connections
 * rather than connecting as a client to a remote server.
 *
 * @since 2.3
 */
class GAMMARAY_CLIENT_EXPORT ClientConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnectionManager(QObject *parent = nullptr);
    ~ClientConnectionManager() override;

    ClientToolManager *toolManager() const;
    /*! Create an embedded client view (no main window chrome). */
    QWidget *createEmbeddedView(QWidget *parent);

    /*! Start GammaRay server to accept probe connections at @p url. */
    void startServer(const QUrl &url);

    /*! Check if server is listening for connections. */
    bool isServerListening() const;

    /*! One-time initialization of stream operators and factory callbacks. */
    static void init();

signals:
    /*! Emitted when the connection is established and the tool model is populated.
     *  If you want to bring up the standard main window, connect this to createMainWindow(),
     *  otherwise use this to show your own UI at this point.
     */
    void ready();

    /*! Emitted when there has been a persistent connection error.
     *  You can connect this to handlePersistentConnectionError() for a standard
     *  message box and application exit handling.
     */
    void persistentConnectionError(const QString &msg);

    /*! Emitted when the connection to the target has been closed, for whatever reason.
     *  For a stand-alone client you probably want to connect this to QApplication::quit().
     */
    void disconnected();

private:
    Client *m_server;
    ClientToolManager *m_toolManager;
};
}

#endif // GAMMARAY_CLIENTCONNECTIONMANAGER_H
