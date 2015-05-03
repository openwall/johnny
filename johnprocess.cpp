#include "johnprocess.h"

JohnProcess::JohnProcess()
{

}

JohnProcess::~JohnProcess()
{


}

void JohnProcess::setupChildProcess()
{
#ifdef OS_FORK
    setsid();
#endif

    QProcess::setupChildProcess();
}

void JohnProcess::terminate()
{
#ifdef OS_FORK
    if(processId() != 0)
    {
        ::kill(-processId(),SIGTERM);
    }
#endif
    QProcess::terminate();
}
