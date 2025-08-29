#include <string.h>
#include <stdlib.h>

// -------------------- [[ Vector API ]]

// Define vector type
#define vec_define(name, type)                                                       \
    typedef struct {                                                                 \
        type *items;                                                                 \
        size_t count;                                                                \
        size_t capacity;                                                             \
    } name

// -------------------- [[ Element access ]]

// Get element at index
#define vec_at(v, i) ((v)->items[i])

// Get first element
#define vec_front(v) ((v)->items[0])

// Get last element
#define vec_back(v)  ((v)->items[(v)->count-1])

// Get ptr to underlying array
#define vec_data(v)  ((v)->items)

// -------------------- [[ Iterators ]]

// Get ptr to first element
#define vec_begin(v) ((v)->items)

// Get ptr to past-the-end element (not last)
#define vec_end(v)   ((v)->items + (v)->count)

// -------------------- [[ Capacity ]]

// Check if vector is empty
#define vec_empty(v) ((v)->count == 0)

// Get number of elements
#define vec_size(v)  ((v)->count)

// Allocate capacity
#define vec_reserve(v, cap)                                                         \
    do {                                                                            \
        if ((v)->capacity < cap) {                                                  \
            (v)->capacity = cap;                                                    \
            (v)->items = realloc((v)->items, (v)->capacity * sizeof(*(v)->items));  \
        }                                                                           \
    } while (0)

// Get current capacity
#define vec_capacity(v) ((v)->capacity)

// Reduce capacity to fit current size
#define vec_shrink_to_fit(v)                                                        \
    do {                                                                            \
        if ((v)->count < (v)->capacity) {                                           \
            (v)->capacity = (v)->count;                                             \
            (v)->items = realloc((v)->items, (v)->capacity * sizeof(*(v)->items));  \
        }                                                                           \
    } while (0)

// Ensure the capacity is sufficient for new elements
#define vec_ensure_capacity(v, cap)                                                 \
    do {                                                                            \
        if (cap > (v)->capacity) {                                                  \
            if ((v)->capacity == 0) (v)->capacity = 256;                            \
            while (cap > (v)->capacity) (v)->capacity *= 2;                         \
            (v)->items = realloc((v)->items, (v)->capacity * sizeof(*(v)->items));  \
        }                                                                           \
    } while (0)

// -------------------- [[ Modifiers ]]

// Clear contents (does not free memory)
#define vec_clear(v) ((v)->count = 0)

// Free memory and reset vector
#define vec_free(v)                                                                 \
    do {                                                                            \
        if ((v)->capacity > 0) {                                                    \
            free((v)->items);                                                       \
            (v)->items = NULL;                                                      \
            (v)->count = 0;                                                         \
            (v)->capacity = 0;                                                      \
        }                                                                           \
    } while(0)

// Init the vector with a certain size fill with value
#define vec_init(v, s, value)                                                  \
    do {                                                                       \
        if ((v)->count == 0) {                                                 \
            vec_ensure_capacity(v, s);                                         \
            (v)->count = s;                                                    \
            for (size_t i = 0; i < (v)->count; ++i) (v)->items[i] = (value);   \
        }                                                                      \
    } while (0)

// At index insert one item
#define vec_insert(v, i, new_item)                                                                    \
    do {                                                                                              \
        if ((v)->count <= i) {                                                                        \
            vec_ensure_capacity(v, i + 1);                                                            \
            (v)->items[i] = (new_item);                                                               \
            (v)->count = i + 1;                                                                       \
        } else {                                                                                      \
            vec_ensure_capacity(v, (v)->count + 1);                                                   \
            memmove((v)->items + i + 1, (v)->items + i, ((v)->count - i) * sizeof(*(v)->items));      \
            (v)->items[i] = (new_item);                                                               \
            (v)->count++;                                                                             \
        }                                                                                             \
    } while (0)

// At index insert several items
#define vec_insert_many(v, i, new_items, new_items_count)                                                        \
    do {                                                                                                         \
        if ((v)->count <= i) {                                                                                   \
            vec_ensure_capacity(v, i + (new_items_count));                                                       \
            memcpy((v)->items + i, (new_items), ((new_items_count)) * sizeof(*(v)->items));                      \
            (v)->count = i + (new_items_count);                                                                  \
        } else {                                                                                                 \
            vec_ensure_capacity(v, (v)->count + (new_items_count));                                              \
            memmove((v)->items + i + (new_items_count), (v)->items + i, ((v)->count - i) * sizeof(*(v)->items)); \
            memcpy((v)->items + i, (new_items), ((new_items_count)) * sizeof(*(v)->items));                      \
            (v)->count += (new_items_count);                                                                     \
        }                                                                                                        \
    } while (0)


// At index construct and insert one item
#define vec_emplace(v, i, ...)                                                                        \
    do {                                                                                              \
        if ((v)->count <= i) {                                                                        \
            vec_ensure_capacity(v, i + 1);                                                            \
            (v)->count = i + 1;                                                                       \
            (v)->items[i] = (typeof(*(v)->items)){__VA_ARGS__};                                       \
        } else {                                                                                      \
            vec_ensure_capacity(v, (v)->count + 1);                                                   \
            memmove(&(v)->items[i + 1], &(v)->items[i], ((v)->count - i) * sizeof(*(v)->items));      \
            (v)->items[i] = (typeof(*(v)->items)){__VA_ARGS__};                                       \
            (v)->count++;                                                                             \
        }                                                                                             \
    } while (0)

// At index erase one item from the vector
#define vec_erase(v, i)                                                                               \
    do {                                                                                              \
        if ((v)->count > 0 && i < (v)->count) {                                                       \
            memmove(&(v)->items[i], &(v)->items[i + 1], ((v)->count - i - 1) * sizeof(*(v)->items));  \
            (v)->count--;                                                                             \
        }                                                                                             \
    } while(0)

// At index erase several items from the vector
#define vec_erase_many(v, i, erase_count)                                                                                           \
    do {                                                                                                                      \
        if ((v)->count > 0 && i < (v)->count) {                                                                               \
            int clamped_count = (erase_count) > ((v)->count - i) ? ((v)->count - i) : (erase_count);                                      \
            memmove(&(v)->items[i], &(v)->items[i + clamped_count], ((v)->count - i - clamped_count) * sizeof(*(v)->items));  \
            (v)->count -= clamped_count;                                                                                      \
        }                                                                                                                     \
    } while(0)

// Append an item at the end of the vector
#define vec_push_back(v, new_item)                                                  \
    do {                                                                            \
        vec_ensure_capacity(v, (v)->count + 1);                                     \
        (v)->items[(v)->count++] = (new_item);                                      \
    } while (0)

// Construct and append an item at the end of the vector
#define vec_emplace_back(v, ...)                                                    \
    do {                                                                            \
        vec_ensure_capacity(v, (v)->count + 1);                                     \
        (v)->items[(v)->count++] = (typeof(*(v)->items)){__VA_ARGS__};              \
    } while (0)

// Append several items at the end of the vector
#define vec_append_many(v, new_items, new_items_count)                                          \
    do {                                                                                        \
        vec_ensure_capacity(v, (v)->count + (new_items_count));                                 \
        memcpy((v)->items + (v)->count, (new_items), (new_items_count) * sizeof(*(v)->items));  \
        (v)->count += (new_items_count);                                                        \
    } while(0)

// Remove the last element of the vector
#define vec_pop_back(v)         \
    do {                        \
        if ((v)->count > 0) {   \
            (v)->count--;       \
        }                       \
    } while (0)

// Resize vector and append copies of {0} if resize is greater than old size
#define vec_resize(v, new_size)                                                                   \
    do {                                                                                          \
        vec_ensure_capacity(v, (new_size));                                                       \
        if ((new_size) > (v)->count) {                                                            \
            memset((v)->items + (v)->count, 0, ((new_size) - (v)->count) * sizeof(*(v)->items));  \
        }                                                                                         \
        (v)->count = (new_size);                                                                  \
    } while (0)

// Resize vector and append copies of value if resize is greater than old size
#define vec_resize_with_value(v, new_size, value)               \
    do {                                                        \
        vec_ensure_capacity(v, (new_size));                     \
        if ((new_size) > (v)->count) {                          \
            for (size_t i = (v)->count; i < (new_size); ++i) {  \
                (v)->items[i] = (value);                        \
            }                                                   \
        }                                                       \
        (v)->count = (new_size);                                \
    } while (0)
