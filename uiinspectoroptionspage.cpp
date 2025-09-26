#include "uiinspectoroptionspage.h"
#include "uiinspectorsettings.h"

#include <coreplugin/icore.h>
#include <utils/layoutbuilder.h>
#include <utils/pathchooser.h>
#include <debugger/debuggerinternalconstants.h>

#include <QGroupBox>

namespace UIInspector{

UIInspectorSettingsWidget::UIInspectorSettingsWidget()
{
    setupUI();
}

void UIInspectorSettingsWidget::setupUI()
{
    using namespace Layouting;

    m_pathChooser = new Utils::PathChooser;
    m_pathChooser->setExpectedKind(Utils::PathChooser::ExistingDirectory);
    m_pathChooser->setPromptDialogTitle(tr("选择 Probe 路径"));

    QString path = UIInspectorSettings::instance()->probePath();

    m_pathChooser->setPath(path);

    Form {
        Row { tr("Probe 路径:"), m_pathChooser }
    }.attachTo(this);
}

void UIInspectorSettingsWidget::apply()
{
    QString path = m_pathChooser->filePath().toFSPathString();
    UIInspectorSettings::instance()->setProbePath(path);
}

UIInspectorOptionsPage::UIInspectorOptionsPage()
{
    setId("UIInspector.General");
    setDisplayName("UI Inspector");
    setCategory(Debugger::Constants::DEBUGGER_SETTINGS_CATEGORY);
    setWidgetCreator([] { return new UIInspectorSettingsWidget; });
}


} // namespace UIInspector
