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

typedef struct pair {
  char *first, *second;
} pair;

char *print_longform(const char *dir, char *fname) {

  /* 1 path sep and 1 null */

  char *res = calloc(1024, sizeof(char));

  char *fullpath = calloc(strlen(dir) + strlen(fname) + 2, sizeof(char));
  strcpy(fullpath, dir);
  fullpath[strlen(fullpath)] = '/';
  strcpy(fullpath + strlen(dir) + 1, fname);
  struct stat buf;
  if (lstat(fullpath, &buf) < 0)
    perror("lstat");
  free(fullpath);

  sprintf(res + strcspn(res, "\0"), "%c",
          S_ISDIR(buf.st_mode) ? 'd' : (S_ISLNK(buf.st_mode) ? 'l' : '-'));
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IRUSR) ? 'r' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IWUSR) ? 'w' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IXUSR) ? 'x' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IRGRP) ? 'r' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IWGRP) ? 'w' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IXGRP) ? 'x' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IROTH) ? 'r' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IWOTH) ? 'w' : '-');
  sprintf(res + strcspn(res, "\0"), "%c", (buf.st_mode & S_IXOTH) ? 'x' : '-');

  sprintf(res + strcspn(res, "\0"), "\t%lu", (unsigned long)buf.st_nlink);

  struct passwd *pwEnt = getpwuid(buf.st_uid);
  sprintf(res + strcspn(res, "\0"), "\t%s", pwEnt->pw_name);

  struct group *grpEnt = getgrgid(buf.st_gid);
  sprintf(res + strcspn(res, "\0"), "\t%s", grpEnt->gr_name);

  sprintf(res + strcspn(res, "\0"), "\t%llu", (unsigned long long)buf.st_size);

  struct tm *tm_info = localtime(&buf.st_mtime);
  char fmt_time[128];
  strftime(fmt_time, 128, "%b %d %Y %H:%M", tm_info);
  sprintf(res + strcspn(res, "\0"), "\t%s", fmt_time);
  sprintf(res + strcspn(res, "\0"), "\t%s", fname);

  return res;
}

int dircmp(const void *a, const void *b) {
  return strcasecmp((*(pair **)a)->first, (*(pair **)b)->first);
}

void ls(process *p) {

  bool all = false, longform = false;

  char **tokens = p->argv;
  size_t n_tok = p->n_tokens;

  char **dirs = calloc(n_tok, sizeof(char *));
  size_t n_dirs = 0;

  for (size_t i = 1; i < n_tok; ++i) {
    if (tokens[i] == NULL)
      continue;
    if (tokens[i][0] == '-') {
      if (strcmp(tokens[i], "-la") == 0) {
        all = true, longform = true;
      } else if (strcmp(tokens[i], "-al") == 0) {
        all = true, longform = true;
      } else if (strcmp(tokens[i], "-a") == 0) {
        all = true;
      } else if (strcmp(tokens[i], "-l") == 0) {
        longform = true;
      } else {
        printf("error: unknown option: %s\n", tokens[i]);
      }
    } else {
      dirs[n_dirs] = strdup(tokens[i]);
      ++n_dirs;
    }
  }

  if (n_dirs == 0) {
    assert(n_tok != 0);
    dirs[0] = strdup(".");
    ++n_dirs;
  }

  for (size_t i = 0; i < n_dirs; ++i) {
    if (n_dirs != 1)
      printf("%s:\n", dirs[i]);
    DIR *dir = opendir(dirs[i]);
    if (dir == NULL) {
      perror("ls");
      return;
    }

    struct dirent *dirEnt;

    pair **files = calloc(256, sizeof(pair *));
    int size = 256, idx = 0;
    while ((dirEnt = readdir(dir)) != NULL) {
      if (dirEnt->d_name[0] == '.' && !all)
        continue;
      char *cur;
      if (!longform)
        cur = strdup(dirEnt->d_name);
      else {
        cur = print_longform(dirs[i], dirEnt->d_name);
      }
      if (idx == size) {
        files = realloc(files, 2 * size * sizeof(char *));
        size *= 2;
      }
      files[idx] = malloc(sizeof(pair));
      files[idx]->first = strdup(dirEnt->d_name);
      files[idx]->second = cur;
      idx++;
    }
    closedir(dir);

    qsort(files, idx, sizeof(pair *), dircmp);

    for (int i = 0; i < idx; i++)
      printf("%s\n", files[i]->second);

    for (int i = 0; i < idx; i++) {
      if (files[i] != NULL) {
        free(files[i]->first);
        files[i]->first = NULL;
        free(files[i]->second);
        files[i]->second = NULL;
        free(files[i]);
        files[i] = NULL;
      }
    }
    free(files);
    files = NULL;

    if (i != (n_dirs - 1))
      printf("\n");
  }

  for (size_t i = 0; i < n_tok; i++) {
    if (dirs[i] != NULL) {
      /*printf("%s\n", dirs[i]);*/
      free(dirs[i]);
      dirs[i] = NULL;
    }
  }

  if (dirs != NULL) {
    free(dirs);
    dirs = NULL;
  }
}
