#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 100;

void *read_data(void *arg) {
  int id = *(int *)arg;
  pthread_rwlock_rdlock(&rwlock);
  printf("Reader %d read data: %d\n", id, shared_data);
  pthread_rwlock_unlock(&rwlock);
  return NULL;
}

void *write_data(void *arg) {
  pthread_rwlock_wrlock(&rwlock);
  shared_data += 50;
  printf("Writer updated data to: %d\n", shared_data);
  pthread_rwlock_unlock(&rwlock);
  return NULL;
}

int main() {
  pthread_t readers[3], writer;
  int ids[3] = {1, 2, 3};

  pthread_create(&writer, NULL, write_data, NULL);
  for (int i = 0; i < 3; i++) {
    pthread_create(&readers[i], NULL, read_data, &ids[i]);
  }

  pthread_join(writer, NULL);
  for (int i = 0; i < 3; i++) {
    pthread_join(readers[i], NULL);
  }

  pthread_rwlock_destroy(&rwlock);
  return 0;
}
