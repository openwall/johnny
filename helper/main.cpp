#include <QtGlobal>
#if defined Q_OS_WIN32
#include <Windows.h>
#include <wincon.h>
#endif

int main(int argc, char* argv[])
{
#if defined Q_OS_WIN32
    int processId = atoi(argv[1]);
    AttachConsole(processId); // attach to process console
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0); // generate Control+Break event
    FreeConsole();
#endif
    return 0;
}
