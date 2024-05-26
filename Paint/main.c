#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static POINTS ptsBegin;
    static int penSize = 20;       // Начальный размер кисти
    static BOOL isCleared = FALSE; // Переменная для отслеживания состояния очистки
    static float scale = 20.0f;    // Начальный масштаб

    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        ptsBegin = MAKEPOINTS(lParam);
        ptsBegin.x = (SHORT)(ptsBegin.x / scale);
        ptsBegin.y = (SHORT)(ptsBegin.y / scale);
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON && !isCleared)
        {
            HDC hdc = GetDC(hwnd);
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Черный цвет кисти
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            POINTS ptsEnd = MAKEPOINTS(lParam);
            ptsEnd.x = (SHORT)(ptsEnd.x / scale);
            ptsEnd.y = (SHORT)(ptsEnd.y / scale);

            RECT rect;
            rect.left = (int)(ptsEnd.x * scale);
            rect.top = (int)(ptsEnd.y * scale);
            rect.right = rect.left + (int)scale;
            rect.bottom = rect.top + (int)scale;

            FillRect(hdc, &rect, hBrush);

            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
            ReleaseDC(hwnd, hdc);
            ptsBegin = ptsEnd;
        }
        break;
    case WM_KEYDOWN:
        if (wParam == 'C' && (GetKeyState(VK_SHIFT) & 0x8000)) // Нажатие Shift+C для очистки окна
        {
            InvalidateRect(hwnd, NULL, TRUE);
            isCleared = TRUE; // Обновляем состояние очистки
        }
        else if (wParam == VK_ADD) // Нажатие + для увеличения масштаба
        {
            scale *= 1.1f;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (wParam == VK_SUBTRACT) // Нажатие - для уменьшения масштаба
        {
            scale /= 1.1f;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (wParam == VK_UP) // Нажатие стрелки вверх для увеличения размера кисти
        {
            penSize += 5;
        }
        else if (wParam == VK_DOWN) // Нажатие стрелки вниз для уменьшения размера кисти
        {
            if (penSize > 5)
            {
                penSize -= 5;
            }
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) // ID 1 для команды очистки
        {
            InvalidateRect(hwnd, NULL, TRUE);
            isCleared = TRUE; // Обновляем состояние очистки
        }
        break;
    case WM_CREATE:
    {
        HMENU hMenu = CreateMenu();
        AppendMenu(hMenu, MF_STRING, 1, "Clear");
        SetMenu(hwnd, hMenu);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (isCleared)
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1)); // Очистка области окна
            isCleared = FALSE;                                // Сбрасываем состояние очистки
        }
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Vadim's PAINT", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Установка курсора в виде крестика
    HCURSOR hCursor = LoadCursor(NULL, IDC_CROSS);
    SetCursor(hCursor);
    SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
