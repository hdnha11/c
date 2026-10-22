#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define THREAD_COUNT 4

// Custom Barrier Structure
typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int count;
  int threshold;
} mac_barrier_t;

mac_barrier_t phase_barrier;

void barrier_init(mac_barrier_t *barrier, int count) {
  pthread_mutex_init(&barrier->mutex, NULL);
  pthread_cond_init(&barrier->cond, NULL);
  barrier->count = 0;
  barrier->threshold = count;
}

void barrier_wait(mac_barrier_t *barrier) {
  pthread_mutex_lock(&barrier->mutex);
  barrier->count++;

  if (barrier->count >= barrier->threshold) {
    // Last thread arrived, wake everyone up
    barrier->count = 0; // Reset for potential reuse
    pthread_cond_broadcast(&barrier->cond);
  } else {
    // Wait for other threads
    pthread_cond_wait(&barrier->cond, &barrier->mutex);
  }

  pthread_mutex_unlock(&barrier->mutex);
}

void barrier_destroy(mac_barrier_t *barrier) {
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);
}

void *phase_worker(void *arg) {
  int id = *(int *)arg;
  printf("Thread %d: Phase 1 complete. Waiting at barrier...\n", id);

  barrier_wait(&phase_barrier);

  printf("Thread %d: Starting Phase 2...\n", id);
  return NULL;
}

int main() {
  pthread_t threads[THREAD_COUNT];
  int ids[THREAD_COUNT] = {1, 2, 3, 4};

  barrier_init(&phase_barrier, THREAD_COUNT);

  for (int i = 0; i < THREAD_COUNT; i++) {
    pthread_create(&threads[i], NULL, phase_worker, &ids[i]);
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
  }

  barrier_destroy(&phase_barrier);
  return 0;
}
