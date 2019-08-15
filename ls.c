#include <assert.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>

#include "ls.h"
#include "main.h"

extern struct state shell_state;

void print_longform(const char *dir, char *fname) {

  /* 1 path sep and 1 null */
  char *res = calloc(strlen(dir) + strlen(fname) + 2, sizeof(char));
  strcpy(res, dir);
  res[strlen(dir)] = '/';
  strcpy(res + strlen(dir) + 1, fname);
  struct stat buf;
  if (lstat(res, &buf) < 0)
    perror("lstat");

  printf("%c", S_ISDIR(buf.st_mode) ? 'd' : (S_ISLNK(buf.st_mode) ? 'l' : '-'));
  printf("%c", (buf.st_mode & S_IRUSR) ? 'r' : '-');
  printf("%c", (buf.st_mode & S_IWUSR) ? 'w' : '-');
  printf("%c", (buf.st_mode & S_IXUSR) ? 'x' : '-');
  printf("%c", (buf.st_mode & S_IRGRP) ? 'r' : '-');
  printf("%c", (buf.st_mode & S_IWGRP) ? 'w' : '-');
  printf("%c", (buf.st_mode & S_IXGRP) ? 'x' : '-');
  printf("%c", (buf.st_mode & S_IROTH) ? 'r' : '-');
  printf("%c", (buf.st_mode & S_IWOTH) ? 'w' : '-');
  printf("%c", (buf.st_mode & S_IXOTH) ? 'x' : '-');

  printf("\t%lu", (unsigned long)buf.st_nlink);

  struct passwd *pwEnt = getpwuid(buf.st_uid);
  printf("\t%s", pwEnt->pw_name);

  struct group *grpEnt = getgrgid(buf.st_gid);
  printf("\t%s", grpEnt->gr_name);

  printf("\t%llu", (unsigned long long)buf.st_size);

  struct tm *tm_info = localtime(&buf.st_mtime);
  char fmt_time[128];
  strftime(fmt_time, 128, "%b %d %Y %H:%M", tm_info);
  printf("\t%s", fmt_time);
  printf("\t%s\n", fname);
}

void ls(void) {

  bool all = false, longform = false;

  // TODO: actually count dirs instead of allocating extra
  char **dirs = calloc(shell_state.n_tok, sizeof(char *));
  size_t n_dirs = 0;

  for (size_t i = 1; i < shell_state.n_tok; ++i) {
    if (shell_state.tokens[i] == NULL)
      continue;
    if (shell_state.tokens[i][0] == '-') {
      if (strcmp(shell_state.tokens[i], "-la") == 0) {
        all = true, longform = true;
      } else if (strcmp(shell_state.tokens[i], "-al") == 0) {
        all = true, longform = true;
      } else if (strcmp(shell_state.tokens[i], "-a") == 0) {
        all = true;
      } else if (strcmp(shell_state.tokens[i], "-l") == 0) {
        longform = true;
      } else {
        printf("error: unknown option: %s\n", shell_state.tokens[i]);
      }
    } else {
      dirs[n_dirs] = strdup(shell_state.tokens[i]);
      ++n_dirs;
    }
  }

  if (n_dirs == 0) {
    assert(shell_state.n_tok != 0);
    dirs[0] = ".";
    ++n_dirs;
  }

  // TODO: sort entries by alphabetical order

  for (size_t i = 0; i < n_dirs; ++i) {
    if (n_dirs != 1)
      printf("%s:\n", dirs[i]);
    DIR *dir = opendir(dirs[i]);
    if (dir == NULL) {
      perror("ls");
      return;
    }

    struct dirent *dirEnt;
    while ((dirEnt = readdir(dir)) != NULL) {
      if (dirEnt->d_name[0] == '.' && !all)
        continue;
      if (!longform)
        printf("%s\n", dirEnt->d_name);
      else
        print_longform(dirs[i], dirEnt->d_name);
    }
    closedir(dir);

    if (i != n_dirs)
      printf("\n");
  }
}
