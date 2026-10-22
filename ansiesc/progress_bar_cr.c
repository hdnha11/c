#include <stdio.h>
#include <unistd.h>

int main(void) {
  for (int i = 1; i < 100; i++) {
    printf("\r%d%% / 100%%", i);
    fflush(stdout);
    usleep(40000);
  }
  printf("\nDone.\n");
  return 0;
}
