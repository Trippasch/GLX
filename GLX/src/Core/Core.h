#pragma once

#ifdef GL_DEBUG
    #define GL_ENABLE_ASSERTS
#endif

#ifdef GL_ENABLE_ASSERTS
    #if _MSC_VER
        #include <intrin.h>
        #define debugbreak() __debugbreak();
    #else
        #define debugbreak() __builtin_trap();
    #endif // _MSC_VER
    #define GL_ASSERT(x, ...) { if (!(x)) { GL_ERROR("Assertion Failed: {0}", __VA_ARGS__); debugbreak(); }}
#else
    #define GL_ASSERT(x, ...)
#endif // GL_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define GL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)