#include "tabwidget.h"

#include <QTabBar>

TabWidget::TabWidget(QWidget *parent)
    :QTabWidget(parent)
{
    // tabBar() method is protected in old qt version and public in new qt versions, so we had to
    // overwrite QTabWidget to install an event filter on the QTabBar
    tabBar()->installEventFilter(this);
}

TabWidget::~TabWidget()
{
    
}

bool TabWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (!event)
        return false;
    if (!watched || !watched->isWidgetType())
        return false;
    QWidget* widget = (QWidget*) watched;
    switch (event->type())
    {
    case QEvent::Wheel:
        if (widget == tabBar()) {
            event->ignore();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}