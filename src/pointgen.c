#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.c"

#define MAX_POINTS (MAX_MEMORY / sizeof(point2d_t))

// Xorshift32 rng. Adapted from <https://en.wikipedia.org/wiki/Xorshift>
typedef struct
{
    u32 a;
} xorshift32_state_t;

static xorshift32_state_t rng;

static inline void rng_init(u32 seed)
{
    if (seed == 0)
    {
        rng.a = time(NULL);
    }
    else
    {
        rng.a = seed;
    }
}

static inline u32 xorshift32(void)
{
    u32 x = rng.a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rng.a = x;
}
#define rand_u32 xorshift32

static inline s32 rand_s32_range(s32 min, s32 max)
{
    u32 extent = max - min + 1;
    u32 r = rand_u32() % extent;
    s32 result = (s32)r + min;
    return result;
}

static void write_to_txt(const char *filepath, point2d_t *points, size_t num_points)
{
    char txt_filepath[256];
    sprintf(txt_filepath, "%s.txt", filepath);

    FILE *outfile = fopen(txt_filepath, "w");
    if (!outfile)
    {
        fprintf(stderr, "ERROR: Failed to open \"%s\" for writing.\n", txt_filepath);
        return;
    }

    for (size_t point_index = 0; point_index < num_points; point_index++)
    {
        point2d_t *point = points + point_index;
        fprintf(outfile, "%d;%d\n", point->x, point->y);
    }
    fclose(outfile);
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Usage: %s num_points min max output_filename [--seed=seed_value] [--maketxt]\n", argv[0]);
        return 1;
    }

    u64 num_points = atoll(argv[1]);
    u32 min = atol(argv[2]);
    u32 max = atol(argv[3]);
    const char *filepath = argv[4];
    u32 seed = 0;  // Default seed value if not provided
    bool make_txt = false;
    for (u32 arg_index = 5; arg_index < (u32)argc; arg_index++)
    {
        const char *arg = argv[arg_index];

        if (strncmp(arg, "--seed=", 7) == 0)
        {
            seed = atoi(arg + 7);
        }
        else if (strcmp(arg, "--maketxt") == 0)
        {
            make_txt = true;
        }
    }

    init_memory();

    if (num_points > MAX_POINTS)
    {
        fprintf(stderr, "To avoid accidentally generating massive files, number of points must be less than %llu.\n",
                MAX_POINTS);
        return 1;
    }

    printf("Generating %llu 32-bit integer coordinates...", num_points);

    point2d_t *points = (point2d_t *)allocate_from_arena(num_points * sizeof(point2d_t));

    rng_init(seed);

    for (size_t point_index = 0; point_index < num_points; point_index++)
    {
        point2d_t *point = points + point_index;
        point->x = rand_s32_range(min, max);
        point->y = rand_s32_range(min, max);
    }

    printf("done.\n");

    FILE *outfile = fopen(filepath, "wb");
    if (!outfile)
    {
        fprintf(stderr, "ERROR: Failed to open \"%s\" for writing.\n", filepath);
        return 1;
    }
    fwrite(points, num_points * sizeof(point2d_t), 1, outfile);
    fclose(outfile);

    if (make_txt)
    {
        write_to_txt(filepath, points, num_points);
    }

    return 0;
}
