/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */
#include "johnhandler.h"

#include <QMetaType>

JohnHandler::JohnHandler() : m_env(QProcessEnvironment::systemEnvironment())
{
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");

    moveToThread(&m_thread);
    m_thread.start();

    connect(&m_john, SIGNAL(started()), this, SIGNAL(started()),
            Qt::QueuedConnection);
    connect(&m_john, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SIGNAL(finished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
    connect(&m_john, SIGNAL(error(QProcess::ProcessError)), this,
            SIGNAL(error(QProcess::ProcessError)), Qt::QueuedConnection);
    connect(&m_john, SIGNAL(readyReadStandardError()), this,
            SIGNAL(readyReadStandardError()), Qt::QueuedConnection);
    connect(&m_john, SIGNAL(readyReadStandardOutput()), this,
            SIGNAL(readyReadStandardOutput()), Qt::QueuedConnection);
    connect(&m_john, SIGNAL(stateChanged(QProcess::ProcessState)), this,
            SIGNAL(stateChanged(QProcess::ProcessState)), Qt::QueuedConnection);
}

JohnHandler::~JohnHandler()
{
    terminate();
    m_thread.quit();
    m_thread.wait();
}

void JohnHandler::exec()
{
    m_john.start(m_johnProgram, m_args);
}

bool JohnHandler::terminate(bool kill)
{
    bool success = false;
    if((m_john.state() == QProcess::Running) ||
       (m_john.state() == QProcess::Starting))
    {
        m_john.terminate();
        if(m_john.waitForFinished(1000))
        {
            success = true;
        }
        else if(kill)
        {
            m_john.kill();
            success = true;
        }
    }
    return success;
}

QProcess::ProcessState JohnHandler::state() const
{
    return m_john.state();
}

QByteArray JohnHandler::readAllStandardOutput()
{
    return m_john.readAllStandardOutput();
}

QByteArray JohnHandler::readAllStandardError()
{
    return m_john.readAllStandardError();
}

QStringList JohnHandler::args() const
{
    return m_args;
}

void JohnHandler::setArgs(const QStringList &args)
{
    m_args = args;
}
QProcessEnvironment JohnHandler::env() const
{
    return m_env;
}

void JohnHandler::setEnv(const QProcessEnvironment &env)
{
    m_env = env;
    m_john.setProcessEnvironment(m_env);
}
QString JohnHandler::johnProgram() const
{
    return m_johnProgram;
}

void JohnHandler::setJohnProgram(const QString &johnProgram)
{
    m_johnProgram = johnProgram;
}

void JohnHandler::start()
{
    // Give a chance to terminate cleanly
    if(m_john.state() != QProcess::NotRunning)
        terminate();
    JohnHandler::exec();
}

void JohnHandler::stop()
{
    JohnHandler::terminate();
}

void JohnHandler::write(const QString &text)
{
    m_john.write(text.toUtf8());
}

void JohnHandler::closeWriteChannel()
{
    m_john.closeWriteChannel();
}
