#include <Windows.h>

#include <cstdio>
#include <io.h>
#include <fcntl.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#ifdef CONSOLE
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((HANDLE_PTR)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((HANDLE_PTR)handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

    FILE* ptrConout, * ptrConin;
    freopen_s(&ptrConout, "CONOUT$", "w", stdout);
    freopen_s(&ptrConin, "CONIN$", "r", stdin);

    printf("Welcome from Windows Main!\n");
#endif

    return 0;
}