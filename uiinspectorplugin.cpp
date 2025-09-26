#include "uiinspectorplugin.h"
#include "uiinspectorplugin.h"
#include "uiinspectormode.h"
#include "uiinspectorinjectemanager.h"
#include "uiinspectoroptionspage.h"
#include "uiinspectorsettings.h"

#include <coreplugin/imode.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/dialogs/ioptionspage.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/runcontrol.h>
#include <QPushButton>
#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QMainWindow>
#include <QAction>
#include <QDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QString>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>

#include <extensionsystem/pluginmanager.h>

#include <host/clientconnectionmanager.h>
#include <host/toolmanagerclient.h>
#include <common/paths.h>
#include <common/selflocator.h>
#include <ui/clientview.h>

using namespace Core;
namespace UIInspector{

UIInspectorPlugin::UIInspectorPlugin() = default;

UIInspectorPlugin::~UIInspectorPlugin()
{
    delete m_optionsPage;
}

bool UIInspectorPlugin::initialize(const QStringList &args, QString *error)
{
    Q_UNUSED(args)
    Q_UNUSED(error)
    m_mode = new UIInspectorMode;

    GammaRay::ClientConnectionManager::init();
    m_connectMgr = new GammaRay::ClientConnectionManager(this);
    m_optionsPage = new UIInspectorOptionsPage();
    QWidget *w = m_mode->root();
    GammaRay::ClientView *view = static_cast<GammaRay::ClientView *>(m_connectMgr->createEmbeddedView(w));
    if(view)
    {
        while(!w->layout()->isEmpty())
        {
            QLayoutItem* item = w->layout()->itemAt(0);
            w->layout()->removeItem(item);
        }
        w->layout()->addWidget(view);
    }

    UIInspectorSettings * settings = UIInspectorSettings::instance();
    settings->fromSettings(Core::ICore::settings());

    m_injecteMgr = new UIInspectorInjecteManager(settings, this);

    connect(m_connectMgr,&GammaRay::ClientConnectionManager::ready,this,[&]{
        Core::ModeManager::activateMode(m_mode->id());
        qDebug() << "setupInjectionForRunControl ok" ;
    });

    connect(m_injecteMgr, &UIInspectorInjecteManager::logMessage, this, [](const QString &m){ qInfo().noquote() << m; });
    connect(m_injecteMgr, &UIInspectorInjecteManager::errorMessage, this, [](const QString &m){ qWarning().noquote() << m; });

    ActionContainer *toolsMenu = ActionManager::actionContainer(Core::Constants::M_TOOLS);
    ActionContainer *menu = ActionManager::createMenu("UIInspector.Menu");
    menu->menu()->setTitle(tr("UI Inspector"));
    toolsMenu->addMenu(menu);

    QAction *uiDebugAction = new QAction(tr("Widget 调试"), this);
    Command *cmdUiDebug = ActionManager::registerAction(uiDebugAction,
                                                        "UIInspector.UiDebug", Context(Core::Constants::C_GLOBAL));
    menu->addAction(cmdUiDebug);
    Core::ModeManager::addAction(cmdUiDebug->action(), /*priority*/ 90);

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(),
            &ProjectExplorer::ProjectExplorerPlugin::runActionsUpdated,
            this, [this]{ updateAvailability(); });
    updateAvailability();

    connect(uiDebugAction, &QAction::triggered, this, [this]{
        const auto rcs = ProjectExplorer::ProjectExplorerPlugin::allRunControls();
        for (auto rc : rcs) {
            if (rc && rc->isRunning()) {
                setupInjectionForRunControl(rc);
                return;
            }
        }

        if (ProjectExplorer::ProjectExplorerPlugin::canRunStartupProject(ProjectExplorer::Constants::NORMAL_RUN_MODE)) {
            auto connection = connect(ProjectExplorer::ProjectExplorerPlugin::instance(),
                                      &ProjectExplorer::ProjectExplorerPlugin::runControlStarted,
                                      this, [this](ProjectExplorer::RunControl *rc) {
                                          setupInjectionForRunControl(rc);
                                          disconnect(ProjectExplorer::ProjectExplorerPlugin::instance(), &ProjectExplorer::ProjectExplorerPlugin::runControlStarted,
                                                              this, nullptr);
                                      });

            ProjectExplorer::ProjectExplorerPlugin::runStartupProject(ProjectExplorer::Constants::NORMAL_RUN_MODE);
        }
    });
    connect(Core::ICore::instance(), &Core::ICore::saveSettingsRequested,
            this, [=] { settings->toSettings(Core::ICore::settings()); });

    return true;
}

void UIInspectorPlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag UIInspectorPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void UIInspectorPlugin::updateAvailability()
{
    const auto canRun = ProjectExplorer::ProjectExplorerPlugin::canRunStartupProject(
        ProjectExplorer::Constants::NORMAL_RUN_MODE);
    if (auto *cmd = Core::ActionManager::command("UIInspector.UiDebug"))
        cmd->action()->setEnabled(canRun);
}

void UIInspectorPlugin::setupInjectionForRunControl(ProjectExplorer::RunControl *rc)
{
    if (!rc || !m_injecteMgr || !m_mode)
        return;

    const Utils::ProcessHandle h = rc->applicationProcessHandle();
    if (!h.isValid() || h.pid() <= 0)
        return;
    const qint64 pid = h.pid();

    QUrl url(QString("local://gammaray-%1").arg(pid));
    m_connectMgr->startServer(url);
    bool listened = m_connectMgr->isServerListening();
    if (!listened) {
        // qWarning() << "UI Inspector: Unable to start server at" << url.toString() <<  "injection aborted.";
        return;
    }
    // QThread::msleep(1500); // wait for the server to be ready
    m_injecteMgr->attachToProcess(pid);
}
}
