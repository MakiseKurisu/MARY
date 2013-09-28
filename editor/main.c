#include <windows.h>
#include <tchar.h>
#include <Scintilla.h>
#include "resource.h"
#include "editor.h"

HINSTANCE hInstance;

HWND hWndScintilla;
LPSCINTILLAPROC lpScintillaProc;
LPVOID lpScintillaPointer;

LRESULT CALLBACK EditorProcCreate(
    _In_  HWND hWnd,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    hWndScintilla = CreateWindowEx(WS_EX_CLIENTEDGE, SCINTILLA_CLASS, NULL, WS_CHILD | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, 0, hInstance, NULL);

    //Get the WndProc
    lpScintillaProc = (LPSCINTILLAPROC) SendMessage(hWndScintilla, SCI_GETDIRECTFUNCTION, 0, 0);
    lpScintillaPointer = (LPVOID) SendMessage(hWndScintilla, SCI_GETDIRECTPOINTER, 0, 0);

    //Set the focus
    SetFocus(hWndScintilla);
    return 0;
}

LRESULT CALLBACK EditorProcNotify(
    _In_  HWND hWnd,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    if (((LPNMHDR) lParam)->hwndFrom == hWndScintilla)
    {
        switch (((LPNMHDR) lParam)->code)
        {
        case SCN_CHARADDED:
            /* Hey, Scintilla just told me that a new */
            /* character was added to the Edit Control.*/
            /* Now i do something cool with that char. */
            break;
        }
    }
    /*
    if (((LPNMHDR) lParam)->code == EN_MSGFILTER && ((MSGFILTER *) lParam)->msg == WM_RBUTTONDOWN)
    {
    POINT pt;

    HMENU hPopup = GetSubMenu(GetMenu(hWnd), 1);
    //PrepareEditMenu(hPopup);
    pt.x = LOWORD(((MSGFILTER *) lParam)->lParam);
    pt.y = HIWORD(((MSGFILTER *) lParam)->lParam);
    ClientToScreen(hWnd, &pt);
    TrackPopupMenu(hPopup, 0, pt.x, pt.y, 0, hWnd, NULL);
    }
    */
    return 0;
}

LRESULT CALLBACK EditorProcCommand(
    _In_  HWND hWnd,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    switch (LOWORD(wParam))
    {
    case IDM_OPEN:
        {
            TCHAR szFileName[MAX_PATH] = TEXT("");
            OPENFILENAME ofn;

            RtlZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.hInstance = hInstance;
            ofn.lpstrFilter = TEXT("ASM Source code (*.asm)\0*.asm\0All Files (*.*)\0*.*\0");
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = sizeof(szFileName) / sizeof(szFileName[0]);
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
            if (GetOpenFileName(&ofn))
            {

            }
            break;
                 }
    case IDM_CLOSE:
        break;
    case IDM_SAVE:
        break;
    case IDM_COPY:
        break;
    case IDM_CUT:
        break;
    case IDM_PASTE:
        break;
    case IDM_DELETE:
        break;
    case IDM_SELECTALL:
        break;
    case IDM_UNDO:
        break;
    case IDM_REDO:
        break;
    case IDM_PREFERENCE:
        break;
    case IDM_SAVEAS:
        {
            TCHAR szFileName[MAX_PATH] = TEXT("");
            OPENFILENAME ofn;

            RtlZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.hInstance = hInstance;
            ofn.lpstrFilter = TEXT("ASM Source code (*.asm)\0*.asm\0All Files (*.*)\0*.*\0");
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = sizeof(szFileName) / sizeof(szFileName[0]);
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
            if (GetSaveFileName(&ofn))
            {

            }
            break;
                   }
    case IDM_FIND:
        break;
    case IDM_REPLACE:
        break;
    case IDM_GOTOLINE:
        break;
    case IDM_FINDNEXT:
        break;
    case IDM_FINDPREV:
        break;
    case IDM_EXIT:
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        break;
    }
    return 0;
}

LRESULT CALLBACK EditorProc(
    _In_  HWND hWnd,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    switch (uMsg)
    {
    case WM_CREATE:
        EditorProcCreate(hWnd, wParam, lParam);
        break;
    case WM_NOTIFY:
        EditorProcNotify(hWnd, wParam, lParam);
        break;
    case WM_INITMENUPOPUP:
        break;
    case WM_COMMAND:
        EditorProcCommand(hWnd, wParam, lParam);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_SIZE:
        MoveWindow(hWndScintilla, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

int CALLBACK _tWinMain(
    _In_  HINSTANCE hInst,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPTSTR lpCmdLine,
    _In_  int nCmdShow
    )
{
    hInstance = hInst;

    HMODULE hScintilla = LoadLibrary(TEXT("SciLexer.dll"));
    if (!hScintilla)
    {
        MessageBox(NULL, TEXT("The Scintilla DLL could not be loaded. MARY will now close."), TEXT("Error loading Scintilla"), MB_OK | MB_ICONERROR);
        return -1;
    }

    WNDCLASSEX wcx;
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = EditorProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = hInstance;
    wcx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
    wcx.lpszClassName = EDITOR_CLASS;
    wcx.hIconSm = wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wcx);

    HWND hWnd = CreateWindowEx(0, EDITOR_CLASS, EDITOR_WINDOW, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINACCEL));

    MSG Msg;
    while (GetMessage(&Msg, 0, 0, 0))
    {
        //if (!IsDialogMessage(hSearch, &Msg))
        if (!TranslateAccelerator(hWnd, hAccel, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }

    FreeLibrary(hScintilla);
    return 0;
}