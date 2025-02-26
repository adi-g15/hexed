#include "MainWindow.h"
#include "ConsoleBuffer.h"
#include <stdio.h>
#include <string>
#include "Error.h"
#include "SaveRestoreConsole.h"

using namespace std;

bool s_running = false;

void ProcessInput(const INPUT_RECORD& inputRecord);
void RemapColours(HANDLE stdoutHandle);

int main(int argc, char** argv)
{
    char *fname = NULL;
    if (argc != 2)
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof ofn);
        ofn.lStructSize = sizeof ofn;
        ofn.lpstrFilter = "All files\0*.*\0\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFile = fname = new char[MAX_PATH];
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_FORCESHOWHIDDEN;

        *fname = '\0';
        if (GetOpenFileName(&ofn) == FALSE)
        {
            printf("usage: hexed <filename>\n");
            delete[] fname;
            return 0;
        }
    }
    else
    {
        fname = argv[1];
    }

    HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdinHandle == INVALID_HANDLE_VALUE)
        Error("Couldn't get input handle");

    HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdoutHandle == INVALID_HANDLE_VALUE)
        Error("Couldn't get output handle");

    SaveConsole(stdoutHandle, stdinHandle);

    if (!SetConsoleMode(stdinHandle, ENABLE_WINDOW_INPUT))
        Error("Couldn't set console mode");

    static const int INPUT_BUFFER_SIZE = 128;
    INPUT_RECORD inputBuffer[INPUT_BUFFER_SIZE];

    RemapColours(stdoutHandle);

    int width = 0;
    int height = 0;

    ConsoleBuffer buffer(stdoutHandle);
    Window::SetConsoleBuffer(&buffer);
    File file;
    file.Open(fname);
    MainWindow mainWindow(file);

    s_running = true;

    while (s_running)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(stdoutHandle, &info))
        {
            int newWidth = info.dwSize.X;
            int newHeight = info.dwSize.Y;
            if (newWidth != width || newHeight != height)
            {
                width = newWidth;
                height = newHeight;
                Window::Resize(width, height);
                Window::Refresh(true);
            }
        }

        DWORD numEventsAvailable;
        GetNumberOfConsoleInputEvents(stdinHandle, &numEventsAvailable);
        if (numEventsAvailable > 0)
        {
            DWORD numEventsRead;
            ReadConsoleInput(stdinHandle, inputBuffer, INPUT_BUFFER_SIZE, &numEventsRead);

            for (DWORD i = 0; i < numEventsRead; i++)
            {
                INPUT_RECORD& e = inputBuffer[i];
                ProcessInput(e);
            }
        }
    }

    RestoreConsole(stdoutHandle, stdoutHandle);
    if (fname != argv[1])
        delete[] fname;
}

void ProcessInput(const INPUT_RECORD& inputRecord)
{
    switch (inputRecord.EventType)
    {
        case KEY_EVENT:
        {
            const KEY_EVENT_RECORD& ker = inputRecord.Event.KeyEvent;
            KeyEvent keyEvent(ker.bKeyDown == TRUE, ker.wVirtualKeyCode, ker.wVirtualScanCode, ker.uChar.AsciiChar, ker.dwControlKeyState);
            Window::ProcessKeyInput(keyEvent);
            if (!keyEvent.WasHandled() && ker.wVirtualKeyCode == VK_ESCAPE && !ker.bKeyDown)
                s_running = false;
            break;
        }
    }
}

void RemapColours(HANDLE stdoutHandle)
{
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(stdoutHandle, &info))
        Error("Failed to get console info");

    // TODO: Configurable colours.

    info.ColorTable[0] = 0x00362b00;
    info.ColorTable[1] = 0x00423607;
    info.ColorTable[2] = 0x00808000;
    info.ColorTable[3] = 0x00a48231;
    info.ColorTable[4] = 0x00164bcb;
    info.ColorTable[5] = 0x00b6369c;
    info.ColorTable[6] = 0x00009985;
    info.ColorTable[7] = 0x00d5e8ee;
    info.ColorTable[8] = 0x00a1a193;
    info.ColorTable[9] = 0x00d28b26;
    info.ColorTable[10] = 0x0036b64f;
    info.ColorTable[11] = 0x0098a12a;
    info.ColorTable[12] = 0x002f32dc;
    info.ColorTable[13] = 0x008236d3;
    info.ColorTable[14] = 0x000089b5;
    info.ColorTable[15] = 0x00e3f6fd;

    // HACK: Workaround for a bug where SetConsoleScreenBufferInfoEx seems to interpret
    // the right and bottom as exclusive, so the window size shrinks.
    info.srWindow.Right += 1;
    info.srWindow.Bottom += 1;

    if (!SetConsoleScreenBufferInfoEx(stdoutHandle, &info))
        Error("Failed to set console info");
}
