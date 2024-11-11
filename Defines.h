#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

#include <GLFW/glfw3.h>

// Unsigned int types.
typedef unsigned char U8;
typedef unsigned short U16;
typedef uint32_t U32;
typedef unsigned long long U64;

// Signed int types.
typedef signed char I8;
typedef signed short I16;
typedef signed int I32;
typedef signed long long I64;

// Floating point types
typedef float F32;
typedef double F64;

// Boolean types
typedef int B32;
typedef char B8;

// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size.
STATIC_ASSERT(sizeof(U8) == 1,  "Expected U8 to be 1 byte.");
STATIC_ASSERT(sizeof(U16) == 2, "Expected U16 to be 2 bytes.");
STATIC_ASSERT(sizeof(U32) == 4, "Expected U32 to be 4 bytes.");
STATIC_ASSERT(sizeof(U64) == 8, "Expected U64 to be 8 bytes.");

STATIC_ASSERT(sizeof(I8) == 1,  "Expected I8 to be 1 byte.");
STATIC_ASSERT(sizeof(I16) == 2, "Expected I16 to be 2 bytes.");
STATIC_ASSERT(sizeof(I32) == 4, "Expected I32 to be 4 bytes.");
STATIC_ASSERT(sizeof(I64) == 8, "Expected I64 to be 8 bytes.");

STATIC_ASSERT(sizeof(F32) == 4, "Expected F32 to be 4 bytes.");
STATIC_ASSERT(sizeof(F64) == 8, "Expected F64 to be 8 bytes.");


#define VK_CHECK(x, message){                                    \
    VkResult err = x;                                            \
    if (err != VK_SUCCESS) throw std::runtime_error(message);}

#define VK_CHECK(x, message)                                     \
{if (x) throw std::runtime_error(message);}

struct QueueFamilyIndices
{
    std::optional<U32> graphicsFamily;

    bool IsComplete()
    {
        return graphicsFamily.has_value();
    }
};