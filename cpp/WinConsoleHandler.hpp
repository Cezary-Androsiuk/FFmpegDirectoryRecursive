#ifndef WIN_CONSOLE_HANDLER_HPP
#define WIN_CONSOLE_HANDLER_HPP

#include <cstdio>
#include <windows.h>

class WinConsoleHandler
{
public:
    static bool installConsoleHandler();
    static bool combinationCtrlCPressed();

private:
    static BOOL WINAPI ConsoleHandler(DWORD signal);
    static bool m_combinationCtrlCPressed;
};

#endif