#include "frontend.h"
#ifdef CONSOLE_FRONTEND

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN        0x0008
#endif

static HANDLE handle_stdin;
static HANDLE handle_stdout;

Frontend::Frontend()
{
    handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
    handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD mode{};
    if (!GetConsoleMode(handle_stdin, &mode))
    {
        printf("Failed to get stdin...");
        exit(1);
    }
    SetConsoleMode(handle_stdin, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(handle_stdout, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(handle_stdout, &info);
    //printf("%d %d\n", info.dwSize.X, info.dwSize.Y);
    //printf("%d %d %d %d\n", info.srWindow.Top, info.srWindow.Left, info.srWindow.Bottom, info.srWindow.Right);

    WriteFile(handle_stdout, "\x1B[2J", 4, nullptr, nullptr);
    char buffer[16];
    for(int n=0; n<256; n++)
    {
        sprintf(buffer, "\x1B[%d;%dH\x1B[38;2;%d;0;0m%c", n % 16, n / 16, n, '#');
        WriteFile(handle_stdout, buffer, strlen(buffer), nullptr, nullptr);
    }
    while(1) {
        INPUT_RECORD input_record;
        DWORD count;
        if (!ReadConsoleInput(handle_stdin, &input_record, 1, &count))
        {
            printf("%d\n", GetLastError());
            break;
        }
        printf("%d %d\n", count, input_record.EventType);
        switch(input_record.EventType)
        {
        case KEY_EVENT:
            printf("KEY: %d %d %d %d %d %d %d\n",
                input_record.Event.KeyEvent.bKeyDown,
                input_record.Event.KeyEvent.wRepeatCount,
                input_record.Event.KeyEvent.wVirtualKeyCode,
                input_record.Event.KeyEvent.wVirtualScanCode,
                input_record.Event.KeyEvent.uChar.UnicodeChar,
                input_record.Event.KeyEvent.uChar.AsciiChar,
                input_record.Event.KeyEvent.dwControlKeyState);
            break;
        case WINDOW_BUFFER_SIZE_EVENT:
            printf("SIZE: %d %d\n", input_record.Event.WindowBufferSizeEvent.dwSize.X, input_record.Event.WindowBufferSizeEvent.dwSize.Y);
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(handle_stdout, &info);
    printf("%d %d\n", info.dwSize.X, info.dwSize.Y);
    printf("%d %d %d %d\n", info.srWindow.Top, info.srWindow.Left, info.srWindow.Bottom, info.srWindow.Right);
            break;
        }
    }
}

#endif
