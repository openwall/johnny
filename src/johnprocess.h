/*
 * Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>.
 * Copyright (c) 2015 Shinnok <admin at shinnok.com>.
 * See LICENSE for details.
 */

#ifndef JOHNPROCESS_H
#define JOHNPROCESS_H

#include <QtGlobal> // to define Q_OS_
#include <QProcess>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

class JohnProcess : public QProcess
{
    Q_OBJECT
    
public:
    JohnProcess();
    ~JohnProcess();

public slots:
    void terminate();

protected:
    virtual void setupChildProcess();
};

#endif // JOHNPROCESS_H
