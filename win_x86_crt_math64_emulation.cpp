#ifdef _M_IX86

// Implementation for symbols which are called by MSVC when 64-bit math operations are used on x86.
// Symbols:
//     _alldiv
//     _allmul
//     _allrem
//     _allshl
//     _allshr
//     _aulldiv
//     _aullrem
//     _aullshr
// This implementation is from MSVC, but adapted to C code through inline assembly and definition of needed macros.

// x is expected to be an address of 64-bit value.
#define CUSTOM_CRT_LOWORD(x) dword ptr [x+0]
#define CUSTOM_CRT_HIWORD(x) dword ptr [x+4]

extern "C" {
    __declspec(naked) void _alldiv() {
        #define CUSTOM_CRT_DVND esp + 16
        #define CUSTOM_CRT_DVSR esp + 24

        __asm {
        push    edi
        push    esi
        push    ebx

//  Set up the local stack and save the index registers.  When this is done
//  the stack frame will look as follows (assuming that the expression a/b will
//  generate a call to lldiv(a, b)):
//
//                -----------------
//                |               |
//                |---------------|
//                |               |
//                |--divisor (b)--|
//                |               |
//                |---------------|
//                |               |
//                |--dividend (a)-|
//                |               |
//                |---------------|
//                | return addr** |
//                |---------------|
//                |      EDI      |
//                |---------------|
//                |      ESI      |
//                |---------------|
//        ESP---->|      EBX      |
//                -----------------
//
// Determine sign of the result (edi = 0 if result is positive, non-zero
// otherwise) and make operands positive.

        xor     edi,edi         // result sign assumed positive

        mov     eax,CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVND) // hi word of a
        or      eax,eax         // test to see if signed
        jge     short L1        // skip rest if a is already positive
        inc     edi             // complement result sign flag
        mov     edx,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVND) // lo word of a
        neg     eax             // make a positive
        neg     edx
        sbb     eax,0
        mov     CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVND),eax // save positive value
        mov     CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVND),edx
L1:
        mov     eax,CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVSR) // hi word of b
        or      eax,eax         // test to see if signed
        jge     short L2        // skip rest if b is already positive
        inc     edi             // complement the result sign flag
        mov     edx,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR) // lo word of a
        neg     eax             // make b positive
        neg     edx
        sbb     eax,0
        mov     CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVSR),eax // save positive value
        mov     CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR),edx
L2:

// Now do the divide.  First look to see if the divisor is less than 4194304K.
// If so, then we can use a simple algorithm with word divides, otherwise
// things get a little more complex.
// NOTE - eax currently contains the high order word of CUSTOM_CRT_DVSR

        or      eax,eax         // check to see if divisor < 4194304K
        jnz     short L3        // nope, gotta do this the hard way
        mov     ecx,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR) // load divisor
        mov     eax,CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVND) // load high word of dividend
        xor     edx,edx
        div     ecx             // eax <- high order bits of quotient
        mov     ebx,eax         // save high bits of quotient
        mov     eax,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVND) // edx:eax <- remainder:lo word of dividend
        div     ecx             // eax <- low order bits of quotient
        mov     edx,ebx         // edx:eax <- quotient
        jmp     short L4        // set sign, restore stack and return

// Here we do it the hard way.  Remember, eax contains the high word of CUSTOM_CRT_DVSR

L3:
        mov     ebx,eax         // ebx:ecx <- divisor
        mov     ecx,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR)
        mov     edx,CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVND) // edx:eax <- dividend
        mov     eax,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVND)
L5:
        shr     ebx,1           // shift divisor right one bit
        rcr     ecx,1
        shr     edx,1           // shift dividend right one bit
        rcr     eax,1
        or      ebx,ebx
        jnz     short L5        // loop until divisor < 4194304K
        div     ecx             // now divide, ignore remainder
        mov     esi,eax         // save quotient

// We may be off by one, so to check, we will multiply the quotient
// by the divisor and check the result against the original dividend
// Note that we must also check for overflow, which can occur if the
// dividend is close to 2**64 and the quotient is off by 1.

        mul     CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVSR) // QUOT * CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVSR)
        mov     ecx,eax
        mov     eax,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR)
        mul     esi             // QUOT * CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVSR)
        add     edx,ecx         // EDX:EAX = QUOT * CUSTOM_CRT_DVSR
        jc      short L6        // carry means Quotient is off by 1

// do long compare here between original dividend and the result of the
// multiply in edx:eax.  If original is larger or equal, we are ok, otherwise
// subtract one (1) from the quotient.

        cmp     edx,CUSTOM_CRT_HIWORD(CUSTOM_CRT_DVND) // compare hi words of result and original
        ja      short L6        // if result > original, do subtract
        jb      short L7        // if result < original, we are ok
        cmp     eax,CUSTOM_CRT_LOWORD(CUSTOM_CRT_DVND) // hi words are equal, compare lo words
        jbe     short L7        // if less or equal we are ok, else subtract
L6:
        dec     esi             // subtract 1 from quotient
L7:
        xor     edx,edx         // edx:eax <- quotient
        mov     eax,esi

// Just the cleanup left to do.  edx:eax contains the quotient.  Set the sign
// according to the save value, cleanup the stack, and return.

L4:
        dec     edi             // check to see if result is negative
        jnz     short L8        // if EDI == 0, result should be negative
        neg     edx             // otherwise, negate the result
        neg     eax
        sbb     edx,0

// Restore the saved registers and return.

L8:
        pop     ebx
        pop     esi
        pop     edi

        ret     16
        }

        #undef CUSTOM_CRT_DVND
        #undef CUSTOM_CRT_DVSR
    }

    __declspec(naked) void _allmul() {

    }

    __declspec(naked) void _allrem() {

    }

    __declspec(naked) void _allshl() {

    }

    __declspec(naked) void _allshr() {

    }

    __declspec(naked) void _aulldiv() {

    }

    __declspec(naked) void _aullrem() {

    }

    __declspec(naked) void _aullshr() {

    }
}

#undef CUSTOM_CRT_LOWORD
#undef CUSTOM_CRT_HIWORD

#endif
