#include "uiinspectorinjectemanager.h"
#include "uiinspectorsettings.h"

#include <projectexplorer/buildconfiguration.h>
#include <qtsupport/qtkitaspect.h>
#include <ProjectExplorer/projectmanager.h>
#include <ProjectExplorer/toolchain.h>
#include <ProjectExplorer/toolchainkitaspect.h>
#include <projectexplorer/target.h>
#include <ProjectExplorer/projectexplorerconstants.h>

#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <string>

#if defined(Q_OS_WIN)
#  include "injector/basicwindllinjector.h"
#endif

namespace UIInspector{
UIInspectorInjecteManager::UIInspectorInjecteManager(UIInspectorSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

UIInspectorInjecteManager::~UIInspectorInjecteManager()
{
    // 清理资源
}

inline QString getCompilerType(ProjectExplorer::Toolchain *toolchain) {
    if (!toolchain) return "unknown";
    Utils::Id type = toolchain->typeId();
    if (type == ProjectExplorer::Constants::MSVC_TOOLCHAIN_TYPEID) return "MSVC";
    if (type == ProjectExplorer::Constants::MINGW_TOOLCHAIN_TYPEID) return "MinGW";
    if (type == ProjectExplorer::Constants::GCC_TOOLCHAIN_TYPEID) return "GCC";
    if (type == ProjectExplorer::Constants::CLANG_TOOLCHAIN_TYPEID) return "Clang";
    return "Other";
}

inline QString getMsvcToolsetVersion(const ProjectExplorer::Abi &abi) {
    switch (abi.osFlavor()) {
    case ProjectExplorer::Abi::WindowsMsvc2022Flavor: return "143";
    case ProjectExplorer::Abi::WindowsMsvc2019Flavor: return "142";
    case ProjectExplorer::Abi::WindowsMsvc2017Flavor: return "141";
    case ProjectExplorer::Abi::WindowsMsvc2015Flavor: return "140";
    default: return "unknown";
    }
}

inline QString getArchitectureString(const ProjectExplorer::Abi &abi) {
    if (abi.architecture() == ProjectExplorer::Abi::X86Architecture) {
        return abi.wordWidth() == 64 ? QString("x86_64") : QString("x86");
    }
    return "unknown";
}

inline QString getBuildTypeSuffix(ProjectExplorer::Target *target) {
    if (!target) return QString();
    auto *buildConfig = target->activeBuildConfiguration();
    if (!buildConfig) return QString();

    switch (buildConfig->buildType()) {
    case ProjectExplorer::BuildConfiguration::Debug: return "d";
    case ProjectExplorer::BuildConfiguration::Release: return "";
    case ProjectExplorer::BuildConfiguration::Profile: return "p";
    default: return "";
    }
}

void UIInspectorInjecteManager::attachToProcess(qint64 pid)
{
    if (pid <= 0) {
        emit errorMessage(tr("无效的 PID: %1").arg(pid));
        return;
    }

    m_targetPid = pid;

    QString error;
    QString probeName, probeDllPath;
    if (!initializeEnvironment(pid, probeName, probeDllPath, error)) {
        QMessageBox::warning(nullptr,"ui inspector" , error);
        return;
    }

    qDebug() << "Probe ABI:" << probeName;
    emit logMessage(tr("开始附加到进程 %1...").arg(m_targetPid));

#if defined(Q_OS_WIN)
    if (!performInjection(pid, probeDllPath, error)) {
        emit errorMessage(error);
        return;
    }

    emit logMessage(tr("DLL注入成功，等待 probe 连接..."));

#else
    emit errorMessage(tr("当前仅支持 Windows 平台"));
#endif
}

bool UIInspectorInjecteManager::initializeEnvironment(qint64 pid, QString &probeName, QString &probeDllPath, QString &error) {
    auto *kit = ProjectExplorer::ProjectManager::startupTarget()->kit();
    auto *toolchain = ProjectExplorer::ToolchainKitAspect::cxxToolchain(kit);
    auto abi = toolchain->targetAbi();

    if (!abi.isValid()) {
        error = tr("无法检测到目标进程的 ABI，注入失败 %1").arg(pid);
        return false;
    }

    if (!buildProbeName(probeName, error)) {
        return false;
    }

    return locateProbeDll(probeName, probeDllPath, error);
}

bool UIInspectorInjecteManager::buildProbeName(QString &probeName, QString &error) {
    auto *kit = ProjectExplorer::ProjectManager::startupTarget()->kit();
    auto *qtVersion = QtSupport::QtKitAspect::qtVersion(kit);
    auto *toolchain = ProjectExplorer::ToolchainKitAspect::cxxToolchain(kit);
    auto *target = ProjectExplorer::ProjectManager::startupTarget();

    if (!qtVersion || !toolchain) {
        error = tr("无法获取 Qt 版本或工具链");
        return false;
    }

    QString compiler = getCompilerType(toolchain);
    QString msvcVersion = getMsvcToolsetVersion(toolchain->targetAbi());
    QString architecture = getArchitectureString(toolchain->targetAbi());
    QString buildType = getBuildTypeSuffix(target);

    probeName = QString("qt%1_%2-%3-%4-%5%6")
                    .arg(qtVersion->qtVersion().majorVersion())
                    .arg(qtVersion->qtVersion().minorVersion())
                    .arg(compiler)
                    .arg(msvcVersion)
                    .arg(architecture)
                    .arg(buildType);

    return true;
}
bool UIInspectorInjecteManager::locateProbeDll(const QString &probeName, QString &probeDllPath, QString &error)
{
    QString probePath= m_settings->probePath();
    if (probePath.isEmpty()) {
        error = tr("未配置 Probe DLL 路径，请在设置中配置");
        return false;
    }

    QDir probeDir = QDir(probePath);
    probeDir.exists(probeName);
    if (!probeDir.exists()) {
        error = tr("无法找到 Probe DLL 目录: %1").arg(probeDir.absolutePath());
        return false;
    }

    bool success = probeDir.cd(probeName);
    if (!success) {
        error =tr("未检测到匹配probe");
        return false;
    }

    auto *target = ProjectExplorer::ProjectManager::startupTarget();
    QString buildType = getBuildTypeSuffix(target);

    QString dllName = QString("gammaray_winloader.dll");
    probeDllPath = probeDir.absoluteFilePath(dllName);

    if (!QFile::exists(probeDllPath)) {
        error = tr("gammaray_winloader 不存在: %1").arg(probeDllPath);
        return false;
    }

    return true;
}

inline wchar_t* qstringTowchar_t(const QString &path) {
    if (path.isEmpty()) {
        wchar_t* p = new wchar_t[1]{L'\0'};
        return p;
    }
    std::wstring wstr = path.toStdWString();
    wchar_t* result = new wchar_t[wstr.size() + 1];
    std::wcscpy(result, wstr.c_str());
    return result;
}

bool UIInspectorInjecteManager::performInjection(qint64 pid, const QString &probeDllPath, QString &error)
{
    auto *kit = ProjectExplorer::ProjectManager::startupTarget()->kit();
    auto *qtVersion = QtSupport::QtKitAspect::qtVersion(kit);
#if defined(Q_OS_WIN)
    wchar_t* binPath = qstringTowchar_t(qtVersion->binPath().path());
    wchar_t* dllPath = qstringTowchar_t(probeDllPath);
    BOOL ok = BasicWinDllInjector::injectProcess(pid, binPath, dllPath);
    return ok;
#else
    Q_UNUSED(pid)
    Q_UNUSED(probeDllPath)
    error = tr("当前仅支持 Windows 平台");
    return false;
#endif
}
}
