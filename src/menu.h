/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef MENU_H
#define MENU_H

#include <QMenu>

class Menu : public QMenu
{
    Q_OBJECT

public:
    Menu(QWidget *parent = 0);
    ~Menu();
    bool event(QEvent *event);
};

#endif // MENU_H
