#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sig_handler(int signo) {
  printf("received SIGHUP\n");
  exit(1);
}

int main(void) {

  signal(SIGHUP, sig_handler);
  // A long long wait so that we can easily issue a signal to this process
  while (1)
    sleep(1);
  return 0;
}
