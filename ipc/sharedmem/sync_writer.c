#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  const int SIZE = 4096;
  const char *shm_name = "/my_shared_memory";
  const char *sem_name = "/my_semaphore";
  const char *message = "Hello, Synchronized Shared Memory!";

  // 1. Create and initialize the semaphore to 0
  sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 0);
  if (sem == SEM_FAILED) {
    perror("sem_open failed");
    exit(1);
  }

  // 2. Setup shared memory
  int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd, SIZE);
  void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

  // 3. Write data to shared memory
  printf("Writer: Writing data to shared memory...\n");
  sprintf(ptr, "%s", message);

  // 4. Signal the reader that data is ready (increments semaphore)
  printf("Writer: Data written. Signaling reader.\n");
  sem_post(sem);

  // 5. Clean up local semaphore reference
  sem_close(sem);

  return 0;
}
