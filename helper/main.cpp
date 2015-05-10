#include <QtGlobal>
#if defined Q_OS_WIN32
#include <Windows.h>
#include <wincon.h>
#endif

int main(int argc, char* argv[])
{
    /* On Windows, QProces::terminate() posts a WM_CLOSE message to all toplevel windows
     * of the process and then to the main thread of the process itself. Console applications like JtR
     * on Windows that do not run an event loop, or whose event loop does not handle the WM_CLOSE message,
     * can only be terminated by calling kill(). However, kill() doesn't give you a chance to do some cleanup.
     * So we use this helper to send a Control+Break event which will terminate cleanly
     *  the sender(helper.exe) and the receiver(john). In fact, in john
     * CTRL_XXX events will trigger the sig_handle_abord_ctrl method because of this
     * SetConsoleCtrlHandler(sig_handle_abort_ctrl, TRUE);
     * See this thread and next posts http://www.openwall.com/lists/john-dev/2015/05/08/11 */

#if defined Q_OS_WIN32
    int processId = atoi(argv[1]);
    AttachConsole(processId); // attach to process console
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0); // generate Control+Break event
    FreeConsole();
#endif

    return 0;
}
