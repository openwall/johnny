/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef JOHNATTACK_H
#define JOHNATTACK_H

#include "johnhandler.h"

#include <QDateTime>
#include <QObject>

class JohnAttack : public JohnHandler
{
    Q_OBJECT

public:
    JohnAttack();
    ~JohnAttack();

    // Nothing much to do here for now, there will be soon
    void start();
    void stop();

    QDateTime startTime() const;

private:
    QDateTime m_startTime;
};

#endif // JOHNATTACK_H
