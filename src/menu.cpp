/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "menu.h"

#include <QHelpEvent>
#include <QToolTip>

Menu::Menu(QWidget *parent) : QMenu(parent)
{
}

Menu::~Menu()
{
}

bool Menu::event(QEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
    // By default QMenu's tooltip are not visible to the user, which isn't what
    // we want for the session menu
    // The setToolTipsVisible(bool) method only appears in Qt >= 5.1, so this is
    // needed.
    const QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    if((helpEvent->type() == QEvent::ToolTip) && (activeAction() != 0))
        QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
    else
        QToolTip::hideText();
#endif
    return QMenu::event(event);
}
