#include <windows.h>
#include <stdint.h>

// before: 105472
// after : 2560

void __stdcall WinMainCRTStartup() {
    int result = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{


    return 0;
}
