/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include <QMetaType>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "johnThread.h"

JohnThread::JohnThread(QByteArray &procOut,
                       QByteArray &procErr,
                       const QStringList parameters, QObject *parent)
    : QThread(parent), pout(procOut), perr(procErr), paramList(parameters)
{
}

void JohnThread::run()
{
    // Process object should be created and then deleted. When process
    // object is parented by this warning occur: "QObject: Cannot
    // create children for a parent that is in a different
    // thread. (Parent is JohnThread(0x186798), parent's thread is
    // QThread(0x39948), current thread is JohnThread(0x186798)" One
    // way to solve it is to delete process in thread's
    // destructor. Other way is to place process object on stack.
    //
    // It is ok to place process object on stack because process
    // object should not live without a thread while method could be
    // finished only with thread.
    //
    // However it does not matter and app crashes on any status
    // update. Thread finishes and johnProcess is not a real
    // object. But there is code that still accesses it.
    // TODO: Fix crash.
    // TODO: Why is not signal disconnected when thread finishes?
    QProcess johnProcess;
    proc = &johnProcess;

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(johnExit()));

    connect(parent(), SIGNAL(killJohn()),
            this, SLOT(stopProcess()), Qt::QueuedConnection);

    connect(parent(), SIGNAL(johnStatus()),
            this, SLOT(updateStatus()));

    proc->start("/usr/sbin/john", paramList);

    exec();

}

void JohnThread::readProcOutput()
{
    pout = proc->readAllStandardOutput(); // read std buffer
    perr = proc->readAllStandardError(); // read error buffer
}

void JohnThread::updateStatus()
{
    proc->write("a\r\n");
    readProcOutput();
    emit johnOutput(paramList[paramList.size() - 1], pout, perr);
}

void JohnThread::johnExit()
{
    readProcOutput();
    emit johnOutput(paramList[paramList.size() - 1], pout, perr);
    exit(0);
}

void JohnThread::stopProcess()
{
    if (proc->state() == QProcess::Running) {
        proc->terminate();
//#ifdef Q_OS_WIN
//        QString abort_cmd;
// TODO: According to Qt coding style this cast should replaced by reinterpret_cast<...>(...).
//        PROCESS_INFORMATION *pinfo = (PROCESS_INFORMATION *)proc->pid();
//        abort_cmd = QString("cmd /c taskkill /PID %1 /F").arg(pinfo->dwProcessId);
//        QProcess::execute(abort_cmd);
//#endif
    }
}
