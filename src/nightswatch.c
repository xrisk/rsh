#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "external.h"
#include "main.h"
#include "nightswatch.h"

extern struct state shell_state;

void interrupt(int header) {

  FILE *f = fopen("/proc/interrupts", "r");
  long len = 1024, idx = 0;
  char *buffer = calloc(len, sizeof(char));
  int c;
  while ((c = fgetc(f)) != EOF) {
    if (idx == len) {
      buffer = realloc(buffer, 2 * len);
      len *= 2;
    }
    buffer[idx++] = c;
  }
  if (len == idx) {
    buffer = realloc(buffer, len + 1);
    ++len;
  }
  buffer[idx] = '\0';

  char *ptr = strtok(buffer, "\n");
  if (header)
    printf("%s\n", ptr);
  while (ptr != NULL) {
    char *og = ptr;
    while (*ptr == ' ')
      ++ptr;
    if (strncmp(ptr, "1:", strlen("1:")) == 0)
      printf("%s\n", og);
    ptr = strtok(NULL, "\n");
  }

  free(buffer);
  if (f != NULL)
    fclose(f);
}

void dirty() {

  FILE *f = fopen("/proc/meminfo", "r");
  long len = 4096, idx = 0;
  char *buffer = calloc(len, sizeof(char));
  int c;
  while ((c = fgetc(f)) != EOF) {
    if (idx == len) {
      buffer = realloc(buffer, 2 * len);
      len *= 2;
    }
    buffer[idx++] = c;
  }
  if (len == idx) {
    buffer = realloc(buffer, len + 1);
    ++len;
  }
  buffer[idx] = '\0';

  char *ptr = strtok(buffer, "\n");
  while (ptr != NULL) {
    if (strncmp(ptr, "Dirty", strlen("Dirty")) == 0) {
      ptr += strlen("Dirty:");
      while (*ptr == ' ' || *ptr == '\t' || *ptr == ':')
        ++ptr;
      printf("%s\n", ptr);
    }
    ptr = strtok(NULL, "\n");
  }
  free(buffer);
  if (f != NULL)
    fclose(f);
}

void nightswatch(process *p) {

  char **tokens = p->argv;

  if (p->n_tokens < 4) {
    fprintf(stderr, "incorrect usage\n");
    return;
  }

  int t = atoi(tokens[2]);

  pid_t fpid;

  switch (fpid = fork()) {

  case 0:;

    /*signal(SIGINT, SIG_DFL);*/
    /*signal(SIGQUIT, SIG_DFL);*/
    /*signal(SIGTSTP, SIG_DFL);*/
    /*signal(SIGTTIN, SIG_DFL);*/
    /*signal(SIGTTOU, SIG_DFL);*/

    bool header = 1;

    while (1) {
      if (strcmp(tokens[3], "interrupts") == 0) {
        interrupt(header);
        header = 0;
      } else if (strcmp(tokens[3], "dirty") == 0)
        dirty();
      else
        printf("unknown operation %s\n", tokens[3]);
      sleep(t);
    }

  default:;
    int c;
    while ((c = getchar() != 'q'))
      ;
    while ((c = getchar() != '\n'))
      ;

    kill(fpid, SIGKILL);
  }
}
