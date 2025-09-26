#pragma once

#include <coreplugin/imode.h>

#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace UIInspector{

class UIInspectorMode : public Core::IMode
{
    Q_OBJECT

public:
    UIInspectorMode();
    QWidget *root() const;

private:
    void init();

private:
    QWidget *m_root;
};

} // namespace UIInspector
