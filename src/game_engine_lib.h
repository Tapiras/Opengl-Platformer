#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/stat.h>

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

#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

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

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size)
{
    char* result = nullptr;

    size_t allignedSize = (size + 7) & ~ 7; // first for bits are set to 0;
    if (bumpAllocator->used + allignedSize <= bumpAllocator->capacity)
    {
        result = bumpAllocator->memory + bumpAllocator->used;
        bumpAllocator->used += allignedSize;
    }
    else
    {
        SM_ASSERT(false, "BumpAllocator is full");
    }
    
    return result;
}

// --------------------------------------------------------------------------------------------------
//                                      File I/O
// --------------------------------------------------------------------------------------------------

long long get_timestanp(char* file)
{
    struct stat file_stat = {};
    stat(file, &file_stat);
    return file_stat.st_mtime;
}

bool file_exists(char* filePath)
{
    SM_ASSERT(filePath, "File path is null");

    auto file = fopen(filePath, "rb");
    if(!file)
    {
        return false;
    }
    fclose(file);

    return true;
}

long get_file_size(char* filePath)
{
    if (!file_exists(filePath))
    {
        SM_ERROR("File %s does not exist", filePath);
        return 0;
    }
    
    long fileSize = 0;
    auto file = fopen(filePath, "rb");
    
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    
    return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer)
{
    SM_ASSERT(buffer, "Buffer is null");

    if (!file_exists(filePath))
    {
        SM_ERROR("File %s does not exist", filePath);
        return nullptr;
    }

    *fileSize = 0;
    auto file = fopen(filePath, "rb");

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer, 0, *fileSize + 1);
    fread(buffer, sizeof(char), *fileSize, file);

    fclose(file);

    return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpalocator)
{
    char* file = nullptr;
    long fileSize2 = get_file_size(filePath);

    if (fileSize2)
    {
        char* buffer = bump_alloc(bumpalocator, fileSize2 + 1);

        file = read_file(filePath, fileSize, buffer);
    }

    return file;
}

void write_file(char* filePath, char* buffer, int size)
{
    SM_ASSERT(buffer, "Buffer is null");
    if (!file_exists(filePath))
    {
        SM_ERROR("File %s does not exist", filePath);
        return;
    }

    auto file = fopen(filePath, "wb");
    if (!file)
    {
        SM_ERROR("Failed to open file %s", filePath);
        return; 
    }

    fwrite(buffer, sizeof(char), size, file);  
    fclose(file);
}

bool copy_file(char* FileName, char* outputName, char* buffer)
{
    int fileSize = 0;
    char* data = read_file(FileName, &fileSize, buffer);

    auto outputFile = fopen(outputName, "wb");
    if(!outputFile)
    {
        SM_ERROR("Failed to open file %s", outputName);
        return false;
    }

    int result = fwrite(data, sizeof(char), fileSize, outputFile);
    if(!result)
    {
        SM_ERROR("Failed to open to file %s", outputName);
        return false;
    }

    fclose(outputFile);

    return true;
}

bool copy_file(char* FileName, char* outputName, BumpAllocator* bumpAllocator)
{
    char* file = 0;
    long fileSize2 = get_file_size(FileName);

    if (fileSize2)
    {
        char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);
        
        return copy_file(FileName, outputName, buffer);
    }

    return false;
}

// --------------------------------------------------------------------------------------------------
//                                          Math
// --------------------------------------------------------------------------------------------------
struct Vec2
{
    float x;
    float y;
};

struct IVec2
{
    int x;
    int y;
};