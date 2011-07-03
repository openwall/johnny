/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#include <QMetaType>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "johnThread.h"

JohnThread::JohnThread(QByteArray& procOut, QByteArray& procErr,
                       const QStringList parameters, QObject *parent)
    : pout(procOut),
      perr(procErr),
      paramList(parameters),
      parent(parent)
{
}

void JohnThread::run() {

    proc = new QProcess();

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(johnExit()));

    connect(parent, SIGNAL(killJohn()),
            this, SLOT(stopProcess()),Qt::QueuedConnection);

    connect(parent, SIGNAL(johnStatus()),
            this, SLOT(updateStatus()));

    proc->start("/usr/sbin/john", paramList);

    exec();

}

void JohnThread::readProcOutput() {
    pout  = proc->readAllStandardOutput(); // read std buffer
    perr  = proc->readAllStandardError(); // read error buffer
}

void JohnThread::updateStatus() {
    proc->write("a\r\n");
    readProcOutput();
    emit johnOutput(paramList[paramList.size()-1], pout, perr);
}

void JohnThread::johnExit() {
    readProcOutput();
    emit johnOutput(paramList[paramList.size()-1], pout, perr);
    exit(0);
}

void JohnThread::stopProcess() {
    if(proc->state() == QProcess::Running) {
        proc->terminate();
//#ifdef Q_OS_WIN
//        QString abort_cmd;
//        PROCESS_INFORMATION *pinfo = (PROCESS_INFORMATION*)proc->pid();
//        abort_cmd = QString("cmd /c taskkill /PID %1 /F").arg(pinfo->dwProcessId);
//        QProcess::execute(abort_cmd);
//#endif
    }
}
