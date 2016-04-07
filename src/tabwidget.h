/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QEvent>
#include <QTabWidget>

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = 0);
    ~TabWidget();
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TABWIDGET_H
