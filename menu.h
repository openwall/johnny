#ifndef MENU_H
#define MENU_H

#include <QMenu>

class Menu : public QMenu
{
public:
    Menu(QWidget *parent = 0);
    bool event(QEvent *event);
};

#endif // MENU_H
