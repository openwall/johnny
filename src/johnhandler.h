/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef JOHNHANDLER_H
#define JOHNHANDLER_H

#include "johnprocess.h"

#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QThread>

class JohnHandler : public QObject
{
    Q_OBJECT

public:
    JohnHandler();
    virtual ~JohnHandler();

    QProcess::ProcessState state() const;

    QByteArray readAllStandardOutput();
    QByteArray readAllStandardError();

    QStringList args() const;
    void setArgs(const QStringList &args);

    QProcessEnvironment env() const;
    void setEnv(const QProcessEnvironment &env);

    QString johnProgram() const;
    void setJohnProgram(const QString &johnProgram);

    void write(const QString &text);
    void closeWriteChannel();

signals:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void error(QProcess::ProcessError error);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void stateChanged(QProcess::ProcessState newState);

public slots:
    virtual void start();
    virtual void stop();

protected:
    void exec();
    bool terminate(bool kill = true);

private:
    JohnProcess         m_john;
    QThread             m_thread;
    QString             m_johnProgram;
    QStringList         m_args;
    QProcessEnvironment m_env;
};

#endif // JOHNHANDLER_H
