#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int shared_counter = 0;

void *increment_counter(void *arg) {
  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&lock);
    shared_counter++; // Critical section
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main() {
  pthread_t t1, t2;

  pthread_create(&t1, NULL, increment_counter, NULL);
  pthread_create(&t2, NULL, increment_counter, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("Final counter value: %d\n", shared_counter);
  pthread_mutex_destroy(&lock);
  return 0;
}
