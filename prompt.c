#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"

#define handle_error(x)                                                        \
  do {                                                                         \
    fprintf(stderr, "Line %d", __LINE__);                                      \
    perror(x);                                                                 \
  } while (0)

extern struct state shell_state;

char *get_user_name(void) {
  uid_t uid = geteuid();
  struct passwd *pw;
  if ((pw = getpwuid(uid)) == NULL)
    handle_error("getpwuid");
  strncpy(shell_state.username, pw->pw_name, sizeof(shell_state.username));
  return &(shell_state.username[0]);
}

char *get_computer_name(void) {
  if (gethostname(shell_state.hostname, sizeof(shell_state.hostname)) < 0)
    handle_error("gethostname");
  return &(shell_state.hostname[0]);
}

char *get_cwd(void) {
  if (getcwd(shell_state.cwd, sizeof(shell_state.cwd)) == NULL)
    handle_error("get_current_dir");
  return &(shell_state.cwd[0]);
}

char *get_shortdir(void) {
  size_t homedir_len, cwd_len;

  char *homedir = shell_state.homedir;

  homedir_len = strlen(homedir);
  cwd_len = strlen(shell_state.cwd);

  if (cwd_len < homedir_len) {
    strcpy(shell_state.shortdir, shell_state.cwd);
    return &(shell_state.shortdir[0]);
  }

  bool matches = true;

  for (size_t i = 0; i < homedir_len; ++i) {
    if (homedir[i] != shell_state.cwd[i]) {
      matches = false;
      break;
    }
  }

  if (!matches) {
    strcpy(shell_state.shortdir, shell_state.cwd);
    return &(shell_state.shortdir[0]);
  }

  strcpy(shell_state.shortdir, "~");
  strncpy(shell_state.shortdir + 1, shell_state.cwd + homedir_len,
          sizeof(shell_state.shortdir) - 1);

  return &(shell_state.shortdir[0]);
}

void show_prompt(void) {
  char *user, *host, *cwd, *shortdir;
  user = get_user_name();
  host = get_computer_name();
  cwd = get_cwd();
  shortdir = get_shortdir();
  printf("<%s@%s:%s> ", user, host, shortdir);
}
