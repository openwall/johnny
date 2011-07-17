/*
 * Copyright (c) 2011 Shinnok <raydenxy at gmail.com>.  See LICENSE.
 */

#ifndef JOHNTHREAD_H
#define JOHNTHREAD_H

#include <QThread>
#include <QByteArray>
#include <QStringList>
#include <QProcess>
#include <QObject>
#include <QtDebug>

class JohnThread : public QThread
{
    Q_OBJECT

public:
    JohnThread(QByteArray &procOut,
               QByteArray &procErr,
               const QStringList parameters,
               QObject *parent = 0);

signals:
    void johnOutput(const QString, QByteArray, QByteArray);

private:
    QByteArray pout;
    QByteArray perr;
    QStringList paramList;

private slots:
    void readProcOutput();
    void stopProcess();
    void johnExit();
    void updateStatus();

protected:
    QProcess *proc;

    void run();
};

#endif // JOHNTHREAD_H
