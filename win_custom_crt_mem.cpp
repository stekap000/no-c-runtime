extern "C" {
    // These are definitions for memset and memcpy that are not intrinsic.
    // Also, these implementations are not efficient.
    // Alternative definition would use winapi ZeroMemory, or directly SIMD.

    #pragma function(memset)
    void* memset(void* destination, int byte_value, size_t byte_count) {
        char* dest = (char*)destination;

        while(byte_count--) {
            *dest++ = (char)byte_value;
        }

        return destination;
    }

    #pragma function(memcpy)
    void* memcpy(void *destination, const void *source, size_t byte_count) {
        char* dest = (char*)destination;
        char* src = (char*)source;

        while(byte_count--) {
            *dest++ = *src++;
        }

        return destination;
    }
}
