#define _GNU_SOURCE

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"
#include "pinfo.h"

extern struct state shell_state;

void disp_pinfo(pid_t pid) {
  printf("pid -- %d\n", pid);

#ifdef __APPLE__
  fprintf(stderr, "pinfo not supported on macOS\n");
  return;
#endif

  char *path;

  asprintf(&path, "/proc/%d/status", pid);
  FILE *f = fopen(path, "r");

  if (f == NULL) {
    perror("fopen");
    free(path);
    path = NULL;
    return;
  }

  int len = 4096;
  char *buffer = calloc(len, sizeof(char));
  int c, idx = 0;
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

  char *state;
  char *mem;
  while (ptr != NULL) {
    if (strncmp(ptr, "State", strlen("State")) == 0) {
      ptr += strlen("State:");
      while (*ptr == ' ' || *ptr == '\t')
        ptr++;
      state = strdup(ptr);
    }
    if (strncmp(ptr, "VmRSS", strlen("VmRSS")) == 0) {
      ptr += strlen("VmRSS:");
      while (*ptr == ' ' || *ptr == '\t')
        ptr++;
      mem = strdup(ptr);
    }
    ptr = strtok(NULL, "\n");
  }

  char *tmp, *exe_path = NULL;
  exe_path = calloc(1024, sizeof(char));
  asprintf(&tmp, "/proc/%d/exe", pid);
  if (readlink(tmp, exe_path, 1024) < 0) {
    warn("readlink failed");
  }

  printf("state: %s\nmem: %s\nexe path:%s\n", state, mem, exe_path);

  free(path);
  path = NULL;
  free(buffer);
  buffer = NULL;
  free(state);
  state = NULL;
  free(mem);
  mem = NULL;
  free(exe_path);
  exe_path = NULL;
  free(tmp);
  tmp = NULL;

  fclose(f);
}

void pinfo(process *p) {

  pid_t pid;
  if (p->n_tokens == 1)
    pid = getpid();
  else
    pid = atoi(p->argv[1]);

  disp_pinfo(pid);
}
