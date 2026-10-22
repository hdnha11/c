#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  // fd[0] is for reading, fd[1] is for writing
  int fd[2];
  pid_t pid;
  const char *message = "Hello from the parent process through a pipe!";
  char buffer[256];

  // 1. Create the anonymous pipe BEFORE forking
  if (pipe(fd) == -1) {
    perror("pipe failed");
    exit(1);
  }

  // 2. Fork the process to create a child
  pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(1);
  }

  if (pid > 0) {
    // PARENT PROCESS (Sender)

    // Close the reading end because the parent only writes
    close(fd[0]);

    printf("Parent: Sending message...\n");
    // Write the message to the pipe
    write(fd[1], message, strlen(message) + 1);

    // Close the writing end when done to send an EOF to the reader
    close(fd[1]);

    // Wait for the child process to finish
    wait(NULL);
  } else {
    // CHILD PROCESS (Receiver)

    // Close the writing end because the child only reads
    close(fd[1]);

    printf("Child: Waiting for a message...\n");
    // Read the message from the pipe (blocks until data is available)
    read(fd[0], buffer, sizeof(buffer));

    printf("Child: Received message: \"%s\"\n", buffer);

    // Close the reading end when done
    close(fd[0]);
  }

  return 0;
}
