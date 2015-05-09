#include <Windows.h>
#include <wincon.h>

int main(int argc, char* argv[])
{
    int processId = atoi(argv[1]);
    AttachConsole(6808); // attach to process console
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0); // generate Control+C event
    FreeConsole();
}
