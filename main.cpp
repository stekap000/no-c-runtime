#include <windows.h>
#include <stdint.h>

#include "win_custom_crt_math.cpp"
#include "win_custom_crt_float.cpp"

typedef uint8_t   u8;
typedef int8_t    s8;
typedef uint16_t  u16;
typedef int16_t   s16;
typedef uint32_t  u32;
typedef int32_t   s32;
typedef uint64_t  u64;
typedef int64_t   s64;
typedef float     f32;
typedef double    f64;

HANDLE std_out;

struct String {
    char* data;
    u64 size;
    String(char* data, u64 size) : data(data), size(size) {}
};
#define StringLiteral(s) String((char*)s, sizeof(s))

void print(String s) {
    DWORD count;
    WriteConsole(std_out, s.data, s.size, &count, NULL);
}

void __stdcall WinMainCRTStartup() {
    AttachConsole(ATTACH_PARENT_PROCESS);
    std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (std_out == NULL || std_out == INVALID_HANDLE_VALUE)
    {
        ExitProcess(ERROR_INVALID_HANDLE);
    }

    print(StringLiteral("Printing for debug works"));

    ExitProcess(0);
}
