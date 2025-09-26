/*
  clientview.h

  Minimal embedded view for GammaRay client, without main window chrome.
*/

#pragma once

#if defined(__has_include)
#  if __has_include("gammaray_ui_export.h")
#    include "gammaray_ui_export.h"
#  else
#    define GAMMARAY_UI_EXPORT
#  endif
#else
#  include "gammaray_ui_export.h"
#endif

#include <QObject>
#include <QString>
#include <QWidget>
#include <QHash>
#include <QItemSelectionModel>
#include <QStackedWidget>
class QHBoxLayout;
class QStackedWidget;

namespace GammaRay {
class ClientToolManager;
class ClientToolFilterProxyModel; // legacy, not used now

class GAMMARAY_UI_EXPORT ClientView : public QWidget
{
    Q_OBJECT
public:
    explicit ClientView(QWidget *parent = nullptr);

    void setToolManager(ClientToolManager *mgr);
    void showToolById(const QString &toolId);

private slots:
    void onToolListAvailable();
    void onToolSelectedById(const QString &toolId);

private:
    void attachWidget(const QString &toolId, QWidget *w);

    ClientToolManager *m_toolManager = nullptr;
    QHBoxLayout *m_layout = nullptr;
    QStackedWidget *m_toolStack = nullptr;
    QWidget *m_current = nullptr;
};

} // namespace GammaRay
