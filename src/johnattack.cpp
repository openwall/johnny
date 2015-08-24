/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "johnattack.h"

JohnAttack::JohnAttack()
{
}

JohnAttack::~JohnAttack()
{
}

void JohnAttack::start()
{
    m_startTime = QDateTime::currentDateTime();
    JohnHandler::start();
}

void JohnAttack::stop()
{
    JohnHandler::stop();
}

QDateTime JohnAttack::startTime() const
{
    return m_startTime;
}
