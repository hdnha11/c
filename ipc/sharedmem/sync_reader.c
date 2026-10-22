#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  const int SIZE = 4096;
  const char *shm_name = "/my_shared_memory";
  const char *sem_name = "/my_semaphore";

  // 1. Open the semaphore (create it if the reader starts first)
  sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 0);
  if (sem == SEM_FAILED) {
    perror("sem_open failed");
    exit(1);
  }

  // 2. Wait for the writer to signal BEFORE opening shared memory
  printf("Reader: Waiting for writer to finish...\n");
  sem_wait(sem);

  // 3. Now that the writer has signaled, the shared memory is guaranteed to
  // exist
  int shm_fd = shm_open(shm_name, O_RDONLY, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    exit(1);
  }

  // 4. Map the shared memory
  void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }

  // 5. Read and print the data safely
  printf("Reader: Read from shared memory: \"%s\"\n", (char *)ptr);

  // 6. Clean up and remove resources from the OS
  sem_close(sem);
  sem_unlink(sem_name);
  shm_unlink(shm_name);

  return 0;
}
