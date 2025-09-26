/*
  clientview.cpp

  Minimal embedded view for GammaRay client, without main window chrome.
*/

#include "clientview.h"

#include <ui/clienttoolmanager.h>

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QWidget>
#include <QString>
#include <QModelIndex>

using namespace GammaRay;

ClientView::ClientView(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_toolStack = new QStackedWidget(this);

    m_layout->addWidget(m_toolStack);
}

void ClientView::setToolManager(ClientToolManager *mgr)
{
    if (m_toolManager == mgr)
        return;
    m_toolManager = mgr;
    if (!m_toolManager)
        return;

    m_toolManager->setToolParentWidget(m_toolStack);

    connect(m_toolManager, &ClientToolManager::toolListAvailable,
            this, &ClientView::onToolListAvailable);
    connect(m_toolManager, &ClientToolManager::toolSelected,
            this, &ClientView::onToolSelectedById);
}

void ClientView::onToolListAvailable()
{
    if (m_toolManager && m_toolManager->isToolListLoaded() && m_toolStack->count() == 0) {
        int fallbackIndex = 0;

        //GammaRay::ObjectInspector GammaRay::MetaObjectBrowser GammaRay::MetaTypeBrowser
        const QString defaultId = QStringLiteral("GammaRay::WidgetInspector");
        int idx = m_toolManager->toolIndexForToolId(defaultId);
        if (idx < 0)
            idx = fallbackIndex;
        const QString toolId = m_toolManager->tools().value(idx).id();
        if (!toolId.isEmpty())
            showToolById(toolId);
    }
}

void ClientView::onToolSelectedById(const QString &toolId)
{
    showToolById(toolId);
}

void ClientView::showToolById(const QString &toolId)
{
    if (!m_toolManager)
        return;
    QWidget *w = m_toolManager->widgetForId(toolId);
    if (!w)
        return;
    attachWidget(toolId, w);
}

void ClientView::attachWidget(const QString & /*toolId*/, QWidget *w)
{
    if (!w)
        return;
    if (m_toolStack->indexOf(w) == -1)
        m_toolStack->addWidget(w);
    m_toolStack->setCurrentWidget(w);
    m_current = w;
}

