#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QEvent>
class TabWidget : public QTabWidget
{
    Q_OBJECT
    
public:
    TabWidget(QWidget *parent = 0);
    ~TabWidget();
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TABWIDGET_H
