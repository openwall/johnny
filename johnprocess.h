#ifndef JOHNPROCESS_H
#define JOHNPROCESS_H

#if defined(__DJGPP__) || defined(__CYGWIN32__)
#define OS_FORK				0
#else
#define OS_FORK				1
#endif

#include <QProcess>

#ifdef OS_FORK
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
};

#endif // JOHNPROCESS_H
