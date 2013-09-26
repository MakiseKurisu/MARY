#include <windows.h>
#include <richedit.h>
#include <tchar.h>
#include "resource.h"

const TCHAR szClassName [] = TEXT("MARY_EDITOR_CLASS");
const TCHAR szWindowName [] = TEXT("MARY Editor");

#define COLOR_COMMENT 0x007F00

HINSTANCE hInstance;
HMODULE hRichEdit;
HWND hWndRichEdit;
WNDPROC procRichEdit;

LRESULT CALLBACK RichEditProc(
    _In_  HWND hWnd,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    switch (uMsg)
    {
    case WM_PAINT:
        {
            HDC hDC;
            RECT rect;
            TEXTRANGE tr;
            HRGN hRgn, hOldRgn;
            LRESULT nText;
            LRESULT FirstChar;

            HideCaret(hWnd);
            CallWindowProc(procRichEdit, hWnd, uMsg, wParam, lParam);

            hDC = GetDC(hWnd);
            SetBkMode(hDC, TRANSPARENT);

            SendMessage(hWnd, EM_GETRECT, 0, (LPARAM) &rect);
            FirstChar = tr.chrg.cpMin = SendMessage(hWnd, EM_LINEINDEX, SendMessage(hWnd, EM_LINEFROMCHAR, SendMessage(hWnd, EM_CHARFROMPOS, 0, (LPARAM) &rect), 0), 0);
            tr.chrg.cpMax = SendMessage(hWnd, EM_CHARFROMPOS, 0, (LPARAM) &rect.right);

            hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
            hOldRgn = SelectObject(hDC, hRgn);
            SetTextColor(hDC, COLOR_COMMENT);
            tr.lpstrText = (LPTSTR) GlobalAlloc(GPTR, (tr.chrg.cpMax - tr.chrg.cpMin + 1) * sizeof(TCHAR));
            if (nText = SendMessage(hWnd, EM_GETTEXTRANGE, 0, (LPARAM) &tr))
            {
                LRESULT i;
                LPTSTR pString;

                //Search for comments first
                i = 0;
                do
                {
                    for (; (i < nText) && (tr.lpstrText[i] != TEXT(';')); i++);
                    if (tr.lpstrText[i] != TEXT(';'))   //No comment has been found
                    {
                    }
                    pString = &tr.lpstrText[i];
                    tr.chrg.cpMin = FirstChar + i;

                    //Search the end of line or the end of buffer
                    for (; (i < nText) && (tr.lpstrText[i] != TEXT('\x0D')); i++);
                    //if (i < nText)
                    {
                        tr.lpstrText[i] = TEXT('\0');
                }
                    tr.chrg.cpMax = FirstChar + i;

                    //Now we must search the range for the tabs
                    if (tr.chrg.cpMax - tr.chrg.cpMin)
                    {
                        for (i = tr.chrg.cpMin; i < tr.chrg.cpMax; i++)
                        {
                            if (tr.lpstrText[i] == TEXT('\t'))
                            {
                                tr.lpstrText[i] = TEXT('\0');
                            }
                        }

                        i = tr.chrg.cpMin;
                        while (i < tr.chrg.cpMax)
                        {
                            if (tr.lpstrText[i])
                            {
                                SIZE_T length = lstrlen(&tr.lpstrText[i]);
                                SendMessage(hWnd, EM_POSFROMCHAR, (WPARAM) &rect, FirstChar + i);
                                DrawText(hDC, &tr.lpstrText[i], -1, &rect, 0);
                                i += length;
                            }
                            else
                            {
                                i++;
                            }
                        }
                    }
                } while (tr.chrg.cpMax - tr.chrg.cpMin);

            }
            GlobalFree(tr.lpstrText);
            SelectObject(hDC, hOldRgn);
            DeleteObject(hRgn);
            ReleaseDC(hWnd, hDC);
            ShowCaret(hWnd);
            break;
                 }
    case WM_CLOSE:
        SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR) procRichEdit);
        break;
    default:
        return CallWindowProc(procRichEdit, hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK EditorProcCreate(
    _In_  HWND hWnd,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    unsigned int TabStops [] = { 36 };
    hWndRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE, MSFTEDIT_CLASS, NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, 0, hInstance, NULL);
    SendMessage(hWndRichEdit, EM_SETEDITSTYLE, SES_EMULATESYSEDIT, SES_EMULATESYSEDIT);  //Make it emulate system edit control so the text color update doesn't take very long

    //Subclass the richedit control.
    procRichEdit = (WNDPROC) SetWindowLongPtr(hWndRichEdit, GWL_WNDPROC, (LONG_PTR) RichEditProc);

    //Set the text limit. The default is 64K.
    SendMessage(hWndRichEdit, EM_LIMITTEXT, -1, 0);

    //Set the default text/background color.
    //SetColor();
    SendMessage(hWndRichEdit, EM_SETMODIFY, FALSE, 0);

    //Set the event mask.
    SendMessage(hWndRichEdit, EM_SETEVENTMASK, FALSE, ENM_MOUSEEVENTS);
    //SendMessage(hWndRichEdit, EM_EMPTYUNDOBUFFER, 0, 0);

    //Set the tab width
    SendMessage(hWndRichEdit, EM_SETTABSTOPS, 1, (LPARAM) &TabStops);

    //Set the focus
    SetFocus(hWndRichEdit);

    return 0;
}

LRESULT CALLBACK EditorProcNotify(
    _In_  HWND hWnd,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
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
        SendMessage(hWndRichEdit, WM_COPY, 0, 0);
        break;
    case IDM_CUT:
        SendMessage(hWndRichEdit, WM_CUT, 0, 0);
        break;
    case IDM_PASTE:
        SendMessage(hWndRichEdit, WM_PASTE, 0, 0);
        break;
    case IDM_DELETE:
        SendMessage(hWndRichEdit, EM_REPLACESEL, TRUE, 0);
        break;
    case IDM_SELECTALL:
        {
            CHARRANGE cr;
            cr.cpMin = 0;
            cr.cpMax = -1;
            SendMessage(hWndRichEdit, EM_EXSETSEL, 0, (LPARAM) &cr);
            break;
                      }
    case IDM_UNDO:
        SendMessage(hWndRichEdit, EM_UNDO, 0, 0);
        break;
    case IDM_REDO:
        SendMessage(hWndRichEdit, EM_REDO, 0, 0);
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
        MoveWindow(hWndRichEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
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

    if (hRichEdit = LoadLibrary(TEXT("Msftedit.dll")))  // Using RichEdit 4.1
    {
        WNDCLASSEX wcx;
        HWND hWnd;
        HACCEL hAccel;
        MSG Msg;

        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW;
        wcx.lpfnWndProc = EditorProc;
        wcx.cbClsExtra = 0;
        wcx.cbWndExtra = 0;
        wcx.hInstance = hInstance;
        wcx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
        wcx.lpszClassName = szClassName;
        wcx.hIconSm = wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassEx(&wcx);

        hWnd = CreateWindowEx(0, szClassName, szWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
        ShowWindow(hWnd, SW_SHOWNORMAL);
        UpdateWindow(hWnd);

        hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINACCEL));

        while (GetMessage(&Msg, 0, 0, 0))
        {
            //if (!IsDialogMessage(hSearch, &Msg))
            if (!TranslateAccelerator(hWnd, hAccel, &Msg))
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
        }

        FreeLibrary(hRichEdit);
        return 0;
    }
    else
    {
        MessageBox(NULL, TEXT("Cannot find riched20.dll."), NULL, 0);
        return -1;
    }
}