#ifndef JOHNPROCESS_H
#define JOHNPROCESS_H

#include <QtGlobal> // to define Q_OS_
#include <QProcess>

#if defined Q_OS_UNIX
#define OS_FORK				1
#else
#define OS_FORK				0
#endif

#if OS_FORK
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

class JohnProcess : public QProcess
{
public:
    JohnProcess();
    ~JohnProcess();

public slots:
    void terminate();

protected:
    virtual void setupChildProcess();

private:
#if defined Q_OS_WIN32
    QProcess m_helper;
#endif
};

#endif // JOHNPROCESS_H
