#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t slots_available;

void *consume_resource(void *arg) {
  int id = *(int *)arg;
  sem_wait(&slots_available); // Decrement counter, block if 0
  printf("Thread %d using resource...\n", id);
  sleep(1); // Simulate work
  printf("Thread %d releasing resource...\n", id);
  sem_post(&slots_available); // Increment counter, wake waiting thread
  return NULL;
}

int main() {
  pthread_t threads[5];
  int ids[5] = {1, 2, 3, 4, 5};

  // Initialize with 2 available slots
  sem_init(&slots_available, 0, 2);

  for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, consume_resource, &ids[i]);
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(threads[i], NULL);
  }

  sem_destroy(&slots_available);
  return 0;
}
