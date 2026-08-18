#ifdef _WIN32
#include <windows.h>

void SetWindowIconFromResource(void *hwnd)
{
    HWND h = (HWND)hwnd;
    HICON hIcon = (HICON)LoadImageA(
        NULL,
        "resource/Icon-01.ico",
        IMAGE_ICON,
        0, 0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE
    );
    if (hIcon) {
        SendMessageA(h, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessageA(h, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
}

#endif
