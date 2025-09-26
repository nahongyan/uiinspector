#pragma once

#include <QString>

namespace Utils {
class QtcSettings;
}

namespace UIInspector{
class UIInspectorSettings
{
public:
    void toSettings(Utils::QtcSettings *s) const;
    void fromSettings(Utils::QtcSettings *s);

    static UIInspectorSettings *instance();

    QString probePath() const;
    void setProbePath(const QString &path);

private:
    UIInspectorSettings() = default;

    QString m_probePath;
};
}
