#include "WinConsoleHandler.hpp"

bool WinConsoleHandler::m_combinationCtrlCPressed = false;

bool WinConsoleHandler::installConsoleHandler()
{
    if (!SetConsoleCtrlHandler(WinConsoleHandler::ConsoleHandler, TRUE)) {
        fprintf(stderr, "error while installing console handler (to handle Ctrl+C)\n");
        return false;
    }
    return true;
}

bool WinConsoleHandler::combinationCtrlCPressed()
{
    return m_combinationCtrlCPressed;
}

BOOL WINAPI WinConsoleHandler::ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        printf("\n\nCtrl+C was pressed!\n\n");
        m_combinationCtrlCPressed = true;
    }
    return TRUE;
}