#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef struct
{
    u8 *start;
    u8 *end;
    u8 *current;
} memory_arena_t;

typedef struct
{
    s32 x, y;
} point2d_t;

typedef struct
{
    point2d_t *data;
    size_t count;
} pointset_t;

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))

#define GIGABYTES(n) ((n) * 1024ULL * 1024ULL * 1024ULL)
#ifndef MAX_MEMORY
#define MAX_MEMORY GIGABYTES(8)
#endif

static memory_arena_t g_arena;

static void init_memory(void)
{
    void *memory = calloc(1, MAX_MEMORY);
    if (!memory)
    {
        fprintf(stderr, "ERROR: Failed to allocate the required memory for the program.\n");
        exit(1);
    }

    g_arena.start = memory;
    g_arena.end = g_arena.start + MAX_MEMORY;
    g_arena.current = g_arena.start;
}

static u8 *allocate_from_arena(size_t size)
{
    if (g_arena.current + size > g_arena.end)
    {
        fprintf(stderr, "ERROR: Tried to allocate more memory than reserved.\n");
        exit(1);
    }

    u8 *allocated_memory = g_arena.current;
    g_arena.current += size;
    return allocated_memory;
}
