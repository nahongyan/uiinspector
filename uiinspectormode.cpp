#include "uiinspectormode.h"

#include <coreplugin/icore.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/icontext.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QIcon>
#include <QString>


namespace UIInspector{

UIInspectorMode::UIInspectorMode()
{
    init();
    setWidget(m_root);
    setContext(Core::Context("UIInspector.MainView"));
    setDisplayName(QString::fromLatin1("UI Inspector"));
    setIcon(QIcon(QLatin1String(":/uiinspector.svg")));
    setPriority(10);
    setId("uiinspector.mode");
}

void UIInspectorMode::init()
{
    m_root = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_root);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setWidget(m_root);
}

QWidget *UIInspectorMode::root() const
{
    return m_root;
}

} // namespace UIInspector

#include "uiinspectormode.moc"
