extern "C" {
    int _fltused;

    #ifdef _M_IX86

    // =========================================================================
    // These are for x86 mode for IA32 arch (no sse extensions).
    // This simple implementation does not handle NAN values and exceptions.

    __declspec(naked) void _ftol2() {
        __asm {
            fistp qword ptr [esp-8]
            mov   edx, [esp-4]
            mov   eax, [esp-8]
            ret
        }
    }

    __declspec(naked) void _ftol2_sse() {
        __asm {
            fistp dword ptr [esp-4]
            mov   eax, [esp-4]
            ret
        }
    }

    __declspec(naked) void _ftoul2_legacy() {}

    // =========================================================================
    // These are for x86 mode when there are sse extensions.

    __declspec(naked) void _dtol3() {}
    __declspec(naked) void _dtoui3() {}
    __declspec(naked) void _dtoul3() {}
    __declspec(naked) void _dtoul3_legacy() {}
    __declspec(naked) void _ftol3() {}
    __declspec(naked) void _ftoui3() {}
    __declspec(naked) void _ftoul3() {}
    __declspec(naked) void _ftoul3_legacy() {}
    __declspec(naked) void _ltof3() {}
    __declspec(naked) void _ultof3() {}
    __declspec(naked) void _ltod3() {}
    __declspec(naked) void _ultod3() {}

    #endif
}
