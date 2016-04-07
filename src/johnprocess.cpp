/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "johnprocess.h"

#ifdef Q_OS_WIN
#include <wincon.h>
#include <windows.h>
#endif

JohnProcess::JohnProcess()
{
}

JohnProcess::~JohnProcess()
{
}

void JohnProcess::setupChildProcess()
{
#ifdef Q_OS_UNIX
    // Create a group for children created by
    // JtR when -- fork option is available (unix-like)
    setsid();
#endif

    QProcess::setupChildProcess();
}

void JohnProcess::terminate()
{
#ifdef Q_OS_UNIX
    if(pid() != 0)
    {
        /* Send sigterm to all processes of the group
         * created previously with setsid()
         * This is done in Johnny because of a bug in john <= 1.8.0
         * where it didn't forward signals to children and we want to keep
         * compatibility with those versions of John for now */
        ::kill(-pid(), SIGTERM);
    }
#elif defined(Q_OS_WIN)
    Q_PID pid = this->pid();
    FreeConsole();
    AttachConsole(pid->dwProcessId);
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    FreeConsole();
#else
    QProcess::terminate();
#endif
}
