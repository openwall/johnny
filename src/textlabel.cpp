/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "textlabel.h"

TextLabel::TextLabel(QWidget *parent) : QLabel(parent), _elide(Qt::ElideNone)
{
}

TextLabel::~TextLabel()
{
}

Qt::TextElideMode TextLabel::elide() const
{
    return _elide;
}

void TextLabel::setElide(const Qt::TextElideMode &elide)
{
    _elide = elide;
    emit elideChanged(_elide);
}

QString TextLabel::text()
{
    return _origText;
}

void TextLabel::setText(const QString &text)
{
    _origText = text;
    QLabel::setText(elideText(_origText));
    setToolTip(_origText);
}

void TextLabel::clear()
{
    _origText.clear();
    QLabel::clear();
}

void TextLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::setText(elideText(_origText));

    if(event)
        event->ignore();
}

QString TextLabel::elideText(const QString &text)
{
    QFontMetrics metrics(this->font());
    return metrics.elidedText(text, _elide, this->width());
}
