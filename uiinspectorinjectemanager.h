#pragma once

#include <QObject>
#include <QString>
#include <QUrl>

namespace UIInspector{

class UIInspectorSettings;
class UIInspectorInjecteManager : public QObject
{
    Q_OBJECT
public:
    explicit UIInspectorInjecteManager(UIInspectorSettings *settings, QObject *parent = nullptr);
    ~UIInspectorInjecteManager() override;

    void attachToProcess(qint64 pid);

signals:
    void logMessage(const QString &msg);
    void errorMessage(const QString &msg);
    void serverStarted(const QUrl &address);

private:
    bool initializeEnvironment(qint64 pid, QString &probeName, QString &probeDllPath, QString &error);
    bool buildProbeName(QString &probeName, QString &error);
    bool locateProbeDll(const QString &probeName, QString &probeDllPath, QString &error);
    bool performInjection(qint64 pid, const QString &probeDllPath, QString &error);

private:
    UIInspectorSettings *m_settings = nullptr;
    qint64 m_targetPid = 0;
};
}
