#define EDITOR_CLASS TEXT("MARY_EDITOR_CLASS")
#define EDITOR_WINDOW TEXT("MARY Editor")

#define SCINTILLA_CLASS TEXT("Scintilla")
typedef int(__cdecl *LPSCINTILLAPROC)(
    _In_  LPVOID hWnd,
    _In_  UINT Msg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    );
#define SendEditor(Msg, wParam, lParam) lpScintillaProc(lpScintillaPointer, Msg, wParam, lParam)