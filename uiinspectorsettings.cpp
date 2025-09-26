#include "uiinspectorsettings.h"

#include <coreplugin/icore.h>
#include <utils/qtcsettings.h>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

const char probePathsKeyC[] = "ProbePath";
namespace UIInspector{
QString UIInspectorSettings::probePath() const
{
    return m_probePath;
}

void UIInspectorSettings::setProbePath(const QString &path)
{
    m_probePath = path;
    toSettings(Core::ICore::settings());
}

void UIInspectorSettings::toSettings(Utils::QtcSettings *s) const
{
    if (s) {
        s->beginGroup("UIInspector");
        s->setValue(probePathsKeyC, m_probePath);
        s->endGroup();
    }
}

void UIInspectorSettings::fromSettings(Utils::QtcSettings *s)
{
    if (s) {
        s->beginGroup("UIInspector");
        m_probePath = s->value(probePathsKeyC, QString()).toString();
        s->endGroup();
    }
}

UIInspectorSettings *UIInspectorSettings::instance()
{
    static UIInspectorSettings settings;
    return &settings;
}
}
#include "uiinspectorsettings.moc"
