#pragma once

#include <coreplugin/dialogs/ioptionspage.h>
#include <utils/pathchooser.h>
#include <QWidget>


namespace UIInspector{

class UIInspectorSettingsWidget : public Core::IOptionsPageWidget
{
    Q_OBJECT
public:
    explicit UIInspectorSettingsWidget();
private:
    void setupUI();

private:
    Utils::PathChooser *m_pathChooser = nullptr;

protected:
    void apply() override;
};

class UIInspectorOptionsPage : public Core::IOptionsPage
{
public:
    explicit UIInspectorOptionsPage();

};

} // namespace UIInspector
