#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_power_of_two(uintptr_t x) { return (x & (x - 1)) == 0; }

uintptr_t align_foward(uintptr_t ptr, size_t align) {
  uintptr_t p, a, modulo;

  assert(is_power_of_two(align));

  p = ptr;
  a = (uintptr_t)align;
  // Same as (p % a) but faster as 'a' is the power of two
  modulo = p & (a - 1);

  if (modulo != 0) {
    // If 'p' address is not aligned, push the address to
    // the next valule which is aligned
    p += a - modulo;
  }

  return p;
}

typedef struct Arena Arena;
struct Arena {
  unsigned char *buf;
  size_t buf_len;
  size_t prev_offset;
  size_t curr_offset;
};

void *arena_alloc_align(Arena *a, size_t size, size_t align) {
  // Align 'curr_offset' forward to the specified alignment
  uintptr_t curr_ptr = (uintptr_t)a->buf + a->curr_offset;
  uintptr_t offset = align_foward(curr_ptr, align);
  offset -= (uintptr_t)a->buf; // Change to relative offset

  // Check to see if the backing memory has space left
  if (offset + size <= a->buf_len) {
    void *ptr = &a->buf[offset];
    a->prev_offset = offset;
    a->curr_offset = offset + size;

    // Zero new memory by default
    memset(ptr, 0, size);
    return ptr;
  }
  // Return NULL if the arena is out of memory (or handle differently)
  return NULL;
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

// Because C doesn't have default parameters
void *arena_alloc(Arena *a, size_t size) {
  return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_length) {
  a->buf = backing_buffer;
  a->buf_len = backing_buffer_length;
  a->curr_offset = 0;
  a->prev_offset = 0;
}

void arena_free(Arena *a, void *ptr) {
  // Do nothing
}

void arena_free_all(Arena *a) {
  a->curr_offset = 0;
  a->prev_offset = 0;
}

void *arena_resize_align(Arena *a, void *old_memory, size_t old_size,
                         size_t new_size, size_t align) {
  unsigned char *old_mem = (unsigned char *)old_memory;

  assert(is_power_of_two(align));

  if (old_mem == NULL || old_size == 0) {
    return arena_alloc_align(a, new_size, align);
  } else if (a->buf <= old_mem && old_mem < a->buf + a->buf_len) {
    if (a->buf + a->prev_offset == old_mem) {
      a->curr_offset = a->prev_offset + new_size;
      if (new_size > old_size) {
        // Zero the new memory by default
        memset(&a->buf[a->prev_offset + old_size], 0, new_size - old_size);
        // Buggy code
        // memset(&a->buf[a->curr_offset], 0, new_size - old_size);
      }
      return old_memory;
    } else {
      void *new_memory = arena_alloc_align(a, new_size, align);
      size_t copy_size = old_size < new_size ? old_size : new_size;
      // Copy across old memory to the new memory
      memmove(new_memory, old_memory, copy_size);
      return new_memory;
    }
  } else {
    assert(0 && "Memory is out of bounds of the buffer in this arena");
    return NULL;
  }
}

// Because C doesn's have default parameters
void *arena_resize(Arena *a, void *old_memory, size_t old_size,
                   size_t new_size) {
  return arena_resize_align(a, old_memory, old_size, new_size,
                            DEFAULT_ALIGNMENT);
}

// Testing

char *append(Arena *a, char *dst, char *src) {
  size_t size = strlen(dst) + strlen(src) + 1;
  char *s = arena_alloc(a, size);
  snprintf(s, size, "%s%s", dst, src);
  return s;
}

int main(void) {
  printf("Default alignment: %lu\n", DEFAULT_ALIGNMENT);

  unsigned char backing_buffer[256];
  Arena arena = {0};
  arena_init(&arena, backing_buffer, 256);

  char *buf = arena_alloc(&arena, 6);
  printf("curr_offset=%ld, prev_offset=%ld\n", arena.curr_offset,
         arena.prev_offset);
  sprintf(buf, "Hello");
  printf("%s", buf);

  char *buf2 = append(&arena, "Hi, ", "there\n");
  printf("%s", buf2);
  printf("curr_offset=%ld, prev_offset=%ld\n", arena.curr_offset,
         arena.prev_offset);

  buf = arena_resize(&arena, buf, 6, 14);
  buf = append(&arena, buf, ", world\n");
  printf("%s", buf);
  printf("%s", buf2);
  printf("curr_offset=%ld, prev_offset=%ld\n", arena.curr_offset,
         arena.prev_offset);

  char *buf3 = arena_alloc(&arena, 6);

  // Try to fill the unused memory section [6,16)
  for (int i = 6; i < 16; i++) {
    buf3[i] = 'X';
  }

  buf3 = arena_resize(&arena, buf3, 6, 14);

  // Check if the resize has correct zero out logic
  for (int i = 6; i < 16; i++) {
    printf("buf3[%i]=%d\n", i, buf3[i]);
  }

  arena_free_all(&arena);
  printf("curr_offset=%ld, prev_offset=%ld\n", arena.curr_offset,
         arena.prev_offset);

  return 0;
}
