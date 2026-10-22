#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  const int SIZE = 4096;                  // Size of shared memory (1 page)
  const char *name = "/my_shared_memory"; // Name of the shared memory object
  const char *message =
      "Hello, Shared Memory! This is inter-process communication at work.";

  // 1. Create the shared memory object
  // O_CREAT: Create if it doesn't exist. O_RDWR: Read/Write permissions.
  int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    exit(1);
  }

  // 2. Configure the size of the shared memory
  ftruncate(shm_fd, SIZE);

  // 3. Map the shared memory into this process's address space
  void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }

  // 4. Write data to the shared memory
  sprintf(ptr, "%s", message);
  printf("Writer: Data written to shared memory.\n");

  return 0;
}
