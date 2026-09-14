#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *arena_buffer;
static size_t arena_buffer_length;
static size_t arena_offset;

void *arena_alloc(size_t size) {
  // Check to see if the backing memory has space left
  if (arena_offset + size <= arena_buffer_length) {
    void *ptr = &arena_buffer[arena_offset];
    arena_offset += size;
    // Zero new memory by default
    memset(ptr, 0, size);
    return ptr;
  }
  // Return NULL if the arena is out of memory
  return NULL;
}

char *append(char *dst, char *src) {
  size_t size = strlen(dst) + strlen(src) + 1;
  char *s = arena_alloc(size);
  snprintf(s, size, "%s%s", dst, src);
  return s;
}

int main(void) {
  arena_buffer_length = 256;
  arena_buffer = malloc(arena_buffer_length);

  char *buf = arena_alloc(14);
  sprintf(buf, "Hello, world\n");
  printf("%s", buf);

  printf("%s\n", append("Hi, ", "there"));

  printf("offset=%ld\n", arena_offset);

  free(arena_buffer);
  return 0;
}
