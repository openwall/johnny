/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "tabwidget.h"

#include <QTabBar>

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    // tabBar() method is protected in old qt version and public in new qt
    // versions, so we had to
    // overwrite QTabWidget to install an event filter on the QTabBar
    tabBar()->installEventFilter(this);
}

TabWidget::~TabWidget()
{
}

bool TabWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(!event || !watched || !watched->isWidgetType())
        return false;
    QWidget *widget = (QWidget *)watched;
    switch(event->type())
    {
    case QEvent::Wheel:
        if(widget == tabBar())
        {
            event->ignore();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}
