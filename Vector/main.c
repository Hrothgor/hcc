#include <stdio.h>
#include "vector.h"

typedef struct {
    int x;
    int y;
} Point;
#define Point_Fmt "x=%d, y=%d"
#define Point_Arg(p) (p).x, (p).y

vec_define(IntVector, int);
vec_define(PointVector, Point);

void printIntVector(IntVector v)
{
    printf("--Actual State--\n");
    printf("vec_size: %lld\n", vec_size(&v));
    printf("vec_capacity: %lld\n", vec_capacity(&v));
    for (size_t i = 0; i < v.count; ++i) {
        printf("%d\n", v.items[i]);
    }
    printf("----\n");
}

void printPointVector(PointVector v)
{
    printf("--Actual State--\n");
    printf("vec_size: %lld\n", vec_size(&v));
    printf("vec_capacity: %lld\n", vec_capacity(&v));
    for (size_t i = 0; i < v.count; ++i) {
        printf(Point_Fmt"\n", Point_Arg(v.items[i]));
    }
    printf("----\n");
}

int main(void)
{
    // ====================
    // TEST WITH INTS
    // ====================
    printf("====================\n");
    printf("TEST WITH INTS\n");
    printf("====================\n\n");

    IntVector xs = {0};
    printf("Reserve: %d\n", 12);
    vec_reserve(&xs, 12);
    printf("Init 10 with 42\n");
    vec_init(&xs, 10, 42);

    printf("Push back 0, 1, 2, 3\n");
    for (int x = 0; x < 4; ++x) vec_push_back(&xs, x);

    printf("Insert 100 at index 1\n");
    vec_insert(&xs, 1, 100);

    printIntVector(xs);

    printf("vec_at(1): %d\n", vec_at(&xs, 1));
    printf("vec_front: %d\n", vec_front(&xs));
    printf("vec_back: %d\n", vec_back(&xs));

    printf("Clear and free vector\n");
    vec_clear(&xs);
    vec_free(&xs);

    printf("Emplace back 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n");
    for (int x = 0; x < 10; ++x) vec_emplace_back(&xs, x);

    printf("Loop using iterators\n");
    for (int *it = vec_begin(&xs); it != vec_end(&xs); ++it) {
        int data = *it;
        printf("%d\n", data);
    }

    printf("vec_empty: %s\n", vec_empty(&xs) ? "true" : "false" );
    printf("vec_size: %lld\n", vec_size(&xs));
    printf("vec_capacity: %lld\n", vec_capacity(&xs));

    printf("Erase element at index 2\n");
    vec_erase(&xs, 2);
    printIntVector(xs);
    printf("Erase 5 elements at index 2\n");
    vec_erase_many(&xs, 2, 5);
    printIntVector(xs);
    printf("Append many {100, 101, 102}\n");
    vec_append_many(&xs, ((int[]){100, 101, 102}), 3);
    printIntVector(xs);

    printf("Pop back 3 elements\n");
    for (int i = 0; i < 3; ++i) vec_pop_back(&xs);

    printf("Resize to 12 filling with 42\n");
    vec_resize_with_value(&xs, 12, 42);
    printIntVector(xs);

    vec_free(&xs);

    printf("\n\n\n");

    // ====================
    // TEST WITH CUSTOM STRUCT Point(x, y)
    // ====================
    printf("====================\n");
    printf("TEST WITH CUSTOM STRUCT Point(x, y)\n");
    printf("====================\n\n");

    PointVector ps = {0};
    printf("Reserve: %d\n", 12);
    vec_reserve(&ps, 12);
    printf("Init 10 with (42, 69)\n");
    vec_init(&ps, 10, ((Point){42, 69}));

    printf("Push back (0, 0), (1, 2), (2, 4), (3, 6)\n");
    for (int x = 0; x < 4; ++x) vec_push_back(&ps, ((Point){x, x*2}));

    printf("Insert (100, 200) at index 1\n");
    vec_insert(&ps, 1, ((Point){100, 200}));

    printPointVector(ps);

    printf("vec_at(1): "Point_Fmt"\n", Point_Arg(vec_at(&ps, 1)));
    printf("vec_front: "Point_Fmt"\n", Point_Arg(vec_front(&ps)));
    printf("vec_back: "Point_Fmt"\n", Point_Arg(vec_back(&ps)));

    printf("Clear and free vector\n");
    vec_clear(&ps);
    vec_free(&ps);

    printf("Emplace back (0, 0), (1, 2), (2, 4), (3, 6), (4, 8), (5, 10), (6, 12), (7, 14), (8, 16), (9, 18)\n");
    for (int x = 0; x < 10; ++x) vec_emplace_back(&ps, .x = x, .y = x * 2);

    printf("Loop using iterators\n");
    for (Point *it = vec_begin(&ps); it != vec_end(&ps); ++it) {
        Point data = *it;
        printf(Point_Fmt"\n", Point_Arg(data));
    }

    printf("vec_empty: %s\n", vec_empty(&ps) ? "true" : "false" );
    printf("vec_size: %lld\n", vec_size(&ps));
    printf("vec_capacity: %lld\n", vec_capacity(&ps));

    printf("Erase element at index 2\n");
    vec_erase(&ps, 2);
    printPointVector(ps);
    printf("Erase 5 elements at index 2\n");
    vec_erase_many(&ps, 2, 5);
    printPointVector(ps);
    printf("Append many (100, 200), (101, 202), (102, 204)\n");
    vec_append_many(&ps, ((Point[]){ {100, 200}, {101, 202}, {102, 204} }), 3);
    printPointVector(ps);

    printf("Pop back 3 elements\n");
    for (int i = 0; i < 3; ++i) vec_pop_back(&ps);

    printf("Resize to 12 filling with (42, 69)\n");
    vec_resize_with_value(&ps, 12, ((Point){42, 69}));
    printPointVector(ps);

    vec_free(&ps);

    return 0;
}