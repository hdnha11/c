#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arena Arena;
struct Arena {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
};

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_len) {
  a->buf = backing_buffer;
  a->buf_len = backing_buffer_len;
  a->prev_offset = 0;
  a->curr_offset = 0;
}

void arena_free_all(Arena *a) {
  a->prev_offset = 0;
  a->curr_offset = 0;
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

bool is_power_of_two(size_t x) { return (x & (x - 1)) == 0; }

uintptr_t align_forward(uintptr_t ptr, size_t align) {
  assert(is_power_of_two(align));

  uintptr_t p, a, m;
  p = ptr;
  a = (uintptr_t)align;
  m = (p & (a - 1)); // p % a

  if (m == 0) { // already aligned
    return ptr;
  }

  p += (a - m);
  return p;
}

void *arena_alloc_align(Arena *a, size_t size, size_t align) {
  assert(is_power_of_two(align));

  uintptr_t curr_ptr = (uintptr_t)a->buf + a->curr_offset;
  uintptr_t offset = align_forward(curr_ptr, align);
  offset -= (uintptr_t)a->buf; // back to relative

  if (offset + size > a->buf_len) {
    // out of memeory
    return NULL;
  }

  void *ptr = &a->buf[offset];
  a->prev_offset = offset;
  a->curr_offset = offset + size;

  // zero by default
  memset(ptr, 0, size);

  return ptr;
}

void *arena_alloc(Arena *a, size_t size) {
  return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void *arena_realloc_align(Arena *a, void *old_memory, size_t old_size,
                          size_t new_size, size_t align) {
  assert(is_power_of_two(align));
  // TODO implement
  return NULL;
}

void *arena_realloc(Arena *a, void *old_memory, size_t old_size,
                    size_t new_size) {
  return arena_realloc_align(a, old_memory, old_size, new_size,
                             DEFAULT_ALIGNMENT);
}

// tesing

void arena_dump(Arena *a, bool dumpbuf) {
  printf("===== Arena Dump Begin =====\n");
  printf("Buffer Length: %ld\n", a->buf_len);
  printf("Prev Offset  : %ld\n", a->prev_offset);
  printf("Curr Offset  : %ld\n", a->curr_offset);

  if (!dumpbuf || a->curr_offset == 0) {
    printf("===== Arena Dump End =====\n");
    return;
  }

  printf("Buffer       :\n");

  size_t rows = a->curr_offset / DEFAULT_ALIGNMENT;
  if (a->curr_offset % DEFAULT_ALIGNMENT) {
    rows++;
  }
  for (size_t row = 0; row < rows; row++) {
    printf("%013lx:", row * DEFAULT_ALIGNMENT);
    for (size_t col = 0; col < DEFAULT_ALIGNMENT; col++) {
      printf(" %02x", a->buf[row * DEFAULT_ALIGNMENT + col]);
    }
    printf("\n");
  }

  printf("===== Arena Dump End =====\n");
}

char *append(Arena *a, char *dst, char *src) {
  size_t size = strlen(dst) + strlen(src) + 1; // \0 terminated
  char *new_str = arena_alloc(a, size);
  if (!new_str) {
    return NULL;
  }

  snprintf(new_str, size, "%s%s", dst, src);
  return new_str;
}

int main(void) {
  // init arena with stack based buffer
  const size_t arena_buf_len = 256;
  unsigned char arena_buf[arena_buf_len];
  Arena a = {0};
  arena_init(&a, arena_buf, arena_buf_len);

  arena_dump(&a, true);

  char *msg = "Hello";
  msg = append(&a, msg, ", world!");
  printf("%s\n", msg);

  arena_dump(&a, true);

  arena_free_all(&a);
  arena_dump(&a, true);

  return 0;
}
