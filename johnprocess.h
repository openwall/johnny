#ifndef JOHNPROCESS_H
#define JOHNPROCESS_H

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#define OS_FORK				1
#else
#define OS_FORK				0
#endif

#include <QProcess>

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
};

#endif // JOHNPROCESS_H
