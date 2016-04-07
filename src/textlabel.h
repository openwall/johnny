/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QLabel>
#include <QMouseEvent>

class TextLabel : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(Qt::TextElideMode elide READ elide WRITE setElide NOTIFY
                   elideChanged DESIGNABLE true)
    Q_ENUMS(Qt::TextElideMode)

public:
    TextLabel(QWidget *parent = 0);
    ~TextLabel();

    Qt::TextElideMode elide() const;
    void setElide(const Qt::TextElideMode &elide);
    QString text(); // returns original text

public slots:
    void setText(const QString &text);
    void clear();

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void elideChanged(Qt::TextElideMode elide);

private:
    Qt::TextElideMode _elide;
    QString           _origText;

    QString elideText(const QString &text);
};

#endif // TEXTLABEL_H
