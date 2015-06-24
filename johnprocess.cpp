#include "johnprocess.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <wincon.h>
#endif

JohnProcess::JohnProcess()
{
}

JohnProcess::~JohnProcess()
{
}

void JohnProcess::setupChildProcess()
{
#if OS_FORK
    // Create a group for children created by
    // JtR when -- fork option is available (unix-like)
    setsid();
#endif

    QProcess::setupChildProcess();
}

void JohnProcess::terminate()
{
#if OS_FORK
    if (processId() != 0) {
        /* Send sigterm to all processes of the group
         * created previously with setsid()
         * This is done in Johnny because of a bug in john <= 1.8.0
         * where it didn't forward signals to children and we want to keep
         * compatibility with those versions of John for now */
        ::kill(-processId(), SIGTERM);
    }
#elif defined Q_OS_WIN
    FreeConsole();
    AttachConsole(processId());
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    FreeConsole();
#else
    QProcess::terminate();
#endif
}
