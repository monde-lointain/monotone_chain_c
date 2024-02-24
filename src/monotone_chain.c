#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common.c"

static inline s32 orient2d(point2d_t *a, point2d_t *b, point2d_t *c)
{
    s32 result = (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
    return result;
}

static inline void swap(point2d_t *a, point2d_t *b)
{
    point2d_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static size_t partition(pointset_t *points, size_t low, size_t high)
{
    point2d_t *pivot = points->data + low;
    size_t i = low - 1;
    size_t j = high + 1;

    while (true)
    {
        do
        {
            i++;
        } while ((points->data[i].x < pivot->x) || (points->data[i].x == pivot->x && points->data[i].y < pivot->y));

        do
        {
            j--;
        } while ((points->data[j].x > pivot->x) || (points->data[j].x == pivot->x && points->data[j].y > pivot->y));

        if (i >= j)
        {
            return j;
        }

        swap(points->data + i, points->data + j);
    }
}

static void sort_by_x(pointset_t *points, size_t low, size_t high)
{
    if (low >= 0 && high >= 0 && low < high)
    {
        size_t pivot_index = partition(points, low, high);
        sort_by_x(points, low, pivot_index);
        sort_by_x(points, pivot_index + 1, high);
    }
}

static pointset_t *compute_convex_hull(pointset_t *points)
{
    sort_by_x(points, 0, points->count - 1);

    pointset_t *result = (pointset_t *)allocate_from_arena(sizeof(pointset_t));
    result->data = (point2d_t *)allocate_from_arena(points->count * sizeof(point2d_t));
    result->count = 0;

    size_t n = points->count;
    size_t k = 0;
    for (size_t i = 0; i < n; i++)
    {
        while (k >= 2 && orient2d(result->data + (k - 2), result->data + (k - 1), points->data + i) <= 0)
        {
            k--;
        }
        point2d_t *pk = result->data + k++;
        point2d_t *pi = points->data + i;
        pk->x = pi->x;
        pk->y = pi->y;
    }

    for (size_t i = n - 2, t = k + 1; i < SIZE_MAX; --i)
    {
        while (k >= t && orient2d(result->data + (k - 2), result->data + (k - 1), points->data + i) <= 0)
        {
            k--;
        }
        point2d_t *pk = result->data + k++;
        point2d_t *pi = points->data + i;
        pk->x = pi->x;
        pk->y = pi->y;
    }

    result->count = k;

    return result;
}

static pointset_t *load_input_data(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Failed to open \"%s\".\n", filepath);
        return NULL;
    }

#if _WIN32
    struct __stat64 fstats;
    _stat64(filepath, &fstats);
#else
    struct stat fstats;
    stat(filepath, &fstats);
#endif
    size_t file_size = fstats.st_size;

    pointset_t *result = (pointset_t *)allocate_from_arena(sizeof(pointset_t));
    result->data = (point2d_t *)allocate_from_arena(file_size);
    result->count = file_size / sizeof(point2d_t);

    if (fread(result->data, file_size, 1, file) != 1)
    {
        fprintf(stderr, "ERROR: Failed to read \"%s\".\n", filepath);
        return NULL;
    }

    fclose(file);

    return result;
}

static void write_to_file(const char *filepath, pointset_t *result)
{
    FILE *file = fopen(filepath, "w");
    if (!file)
    {
        fprintf(stderr, "ERROR: Failed to open \"%s\" for writing.\n", filepath);
        return;
    }

    for (size_t point_index = 0; point_index < result->count; point_index++)
    {
        point2d_t *point = result->data + point_index;
        fprintf(file, "%d;%d\n", point->x, point->y);
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s input_filename [--output=output_filename]\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    bool dump_output = false;
    const char *out_filepath = "";
    for (u32 arg_index = 2; arg_index < (u32)argc; arg_index++)
    {
        const char *arg = argv[arg_index];

        if (strncmp(arg, "--output=", 9) == 0)
        {
            dump_output = true;
            out_filepath = arg + 9;
        }
    }

    init_memory();

    pointset_t *input = load_input_data(filepath);
    if (!input)
    {
        return 1;
    }

    printf("%s (%llu points)\n", filepath, input->count);

    printf("Computing convex hull...");
    pointset_t *result = compute_convex_hull(input);
    printf("done\n");

    if (dump_output)
    {
        write_to_file(out_filepath, result);
        printf("Wrote results to \"%s\".", out_filepath);
    }

    return 0;
}
