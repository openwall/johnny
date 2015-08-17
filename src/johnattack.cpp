/*
 * Copyright (c) 2015 Shinnok <admin at shinnok.com>.
 * See LICENSE for details.
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
