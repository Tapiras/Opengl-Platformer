#pragma

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------------------------------------------
//                                      Defines
// --------------------------------------------------------------------------------------------------
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

// --------------------------------------------------------------------------------------------------
//                                      Logging
// --------------------------------------------------------------------------------------------------
enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_COUNT,
};

template<typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args)
{
    static char* TextColorTable[TEXT_COLOR_COUNT] =
    {
        "\x1b[30m",
        "\x1b[31m",
        "\x1b[32m",
        "\x1b[33m",
        "\x1b[34m",
        "\x1b[35m",
        "\x1b[36m",
        "\x1b[37m",
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE : "  , msg, TEXT_COLOR_GREEN , ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN  : "    , msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR : "  , msg, TEXT_COLOR_RED   , ##__VA_ARGS__);

#define SM_ASSERT(condition, msg, ...)              \
{                                                   \
    if(!(condition))                                \
    {                                               \
        SM_ERROR(msg, ##__VA_ARGS__);               \
        DEBUG_BREAK();                              \
    }                                               \
}                                                   \
// --------------------------------------------------------------------------------------------------
//                                      Bump Allocator
// --------------------------------------------------------------------------------------------------
struct BumpAllocator
{
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size)
{
    BumpAllocator allocator = {};
    allocator.memory = (char*) malloc(size);
    if (allocator.memory)
    {
        allocator.capacity = size;
        memset(allocator.memory, 0, size); // sets memory to 0
    }
    else
    {
        SM_ERROR("Failed to allocate memory for BumpAllocator");
    }    

    return allocator;
}
