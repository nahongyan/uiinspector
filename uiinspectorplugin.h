#pragma once

#include <extensionsystem/iplugin.h>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QObject>


namespace ProjectExplorer
{
class RunControl;
}

namespace GammaRay {
class ClientConnectionManager;
}

namespace UIInspector{

class UIInspectorMode;
class UIInspectorOptionsPage;
class UIInspectorInjecteManager;

class UIInspectorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "uiinspector.json")

public:
    UIInspectorPlugin();
    ~UIInspectorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    UIInspectorMode *m_mode = nullptr;
    UIInspectorOptionsPage *m_optionsPage = nullptr;
    UIInspectorInjecteManager *m_injecteMgr = nullptr;
    GammaRay::ClientConnectionManager *m_connectMgr = nullptr;

    bool m_injectOnNextStart = false;
    int m_injectPort = 0;

    void updateAvailability();
    void setupInjectionForRunControl(ProjectExplorer::RunControl *rc);
};

}
