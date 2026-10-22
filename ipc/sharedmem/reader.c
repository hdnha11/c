#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  const int SIZE = 4096;
  const char *name = "/my_shared_memory";

  // 1. Open the existing shared memory object (Read Only)
  int shm_fd = shm_open(name, O_RDONLY, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    exit(1);
  }

  // 2. Map the shared memory into this process's address space
  void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }

  // 3. Read and print the data
  printf("Reader: Read from shared memory: \"%s\"\n", (char *)ptr);

  // 4. Remove the shared memory object from the system
  if (shm_unlink(name) == -1) {
    perror("shm_unlink failed");
    exit(1);
  }

  return 0;
}
