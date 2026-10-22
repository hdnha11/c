#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int data_ready = 0;

void *producer(void *arg) {
  sleep(1); // Simulate work
  pthread_mutex_lock(&mtx);
  data_ready = 1;
  printf("Producer: Data is ready. Signaling consumer...\n");
  pthread_cond_signal(&cond); // Wake up waiting thread
  pthread_mutex_unlock(&mtx);
  return NULL;
}

void *consumer(void *arg) {
  pthread_mutex_lock(&mtx);
  while (data_ready == 0) {
    printf("Consumer: Waiting for data...\n");
    // Atomically unlocks mtx and puts thread to sleep.
    // Re-locks mtx when awakened.
    pthread_cond_wait(&cond, &mtx);
  }
  printf("Consumer: Processing data!\n");
  pthread_mutex_unlock(&mtx);
  return NULL;
}

int main() {
  pthread_t prod, cons;

  pthread_create(&cons, NULL, consumer, NULL);
  pthread_create(&prod, NULL, producer, NULL);

  pthread_join(prod, NULL);
  pthread_join(cons, NULL);

  pthread_mutex_destroy(&mtx);
  pthread_cond_destroy(&cond);
  return 0;
}
