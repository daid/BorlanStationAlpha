#include "frontend.h"
#ifdef CONSOLE_FRONTEND

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
#else

#if __linux__
#include <sys/ioctl.h>
#include <termios.h>

struct termios term_orig;
#endif

Frontend::Frontend()
{
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term_orig)) {
        printf("tcgetattr failed\n");
        exit(-1);
    }

    term = term_orig;

    cfmakeraw(&term);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term)) {
        printf("tcsetattr failed\n");
        exit(-1);
    }

    //Switch to alternative buffer and clear it, disable the cursor
    printf("\x1B[?1049h" "\x1B[?25l");
    fflush(stdout);
}

Frontend::~Frontend()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
    printf("\x1B[?1049l\x1B[23;0;0t");
    fflush(stdout);
}

uint32_t Frontend::getInput()
{
    char c;
    while(1)
    {
        if (!read(STDIN_FILENO, &c, sizeof(c)))
            return INPUT_QUIT;
        if (c == 3) return INPUT_QUIT;
        else if (c >= ' ' && c <= '~') return c;
        else if (c == 0x1B) {
            //Escape, check if we have a sequence or just the escape key pressed.
            fd_set set;
            struct timeval timeout;
            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
            if (select(1, &set, NULL, NULL, &timeout))
            {
                //Sequence
                char buf[16]{0};
                auto size = read(STDIN_FILENO, &buf, sizeof(buf));

                if (buf[0] == '[' && buf[1] == 'A') return INPUT_UP;
                if (buf[0] == '[' && buf[1] == 'B') return INPUT_DOWN;
                if (buf[0] == '[' && buf[1] == 'C') return INPUT_RIGHT;
                if (buf[0] == '[' && buf[1] == 'D') return INPUT_LEFT;

                printf("\x1B[0mESC ");
                for(int n=0; n<size; n++)
                    printf("%d ", buf[n]);
                fflush(stdout);
            } else {
                return 0; //Escape key.
            }
        } else {
            printf("\x1B[0m?%d ", c);
            fflush(stdout);
        }
    }
    return 0;
}

Vector2i Frontend::beginDrawing()
{
#ifdef __linux__
    struct winsize w;
    ioctl(1, TIOCGWINSZ, &w);
    Vector2i size{w.ws_col, w.ws_row};
#endif

    buffer.resize(size);
    return size;
}

void Frontend::present()
{
    for(int y=0; y<buffer.size().y; y++) {
        printf("\x1B[%d;1H", y + 1);
        for(int x=0; x<buffer.size().x; x++) {
            auto& t = buffer(x, y);
            printf("\x1B[38;2;%d;%d;%dm\x1B[48;2;%d;%d;%dm%c",
                int(t.forground_color.r * 255), int(t.forground_color.g * 255), int(t.forground_color.b * 255),
                int(t.background_color.r * 255), int(t.background_color.g * 255), int(t.background_color.b * 255),
                t.c);
        }
    }
    fflush(stdout);
}

#endif

#endif
