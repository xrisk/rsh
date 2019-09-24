#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "parse.h"

extern struct state shell_state;

line *parse_line(char *str) {
  line *l = calloc(1, sizeof(line));
  job **head = &(l->first_job);
  char *ptr;

  str = strdup(str);
  char *to_free = str;

  while ((ptr = strsep(&str, ";")) != NULL) {
    *head = parse_job(ptr);
    head = &((*head)->next_job);
  }

  free(to_free);
  return l;
}

job *parse_job(char *s) {
  job *j = calloc(1, sizeof(job));
  process **head = &(j->first_process);
  char *ptr;

  char *str = strdup(s);
  char *to_free = str;

  j->fg = true;
  int idx = strlen(str) - 1;
  while (idx >= 0) {
    if (str[idx] == '&') {
      j->fg = false;
      str[idx] = '\0';
    } else if (!(str[idx] == ' ' || str[idx] == '\t'))
      break;
    idx--;
  }

  while ((ptr = strsep(&str, "|")) != NULL) {
    *head = parse_process(ptr);
    head = &((*head)->next_process);
  }

  free(to_free);
  return j;
}

process *parse_process(char *s) {
  process *p = calloc(1, sizeof(process));
  p->infile = NULL;
  p->outfile = NULL;
  p->append = false;
  char *ptr;
  int n_tokens = 32;
  char **argv = calloc(32, sizeof(char *));
  int cur_tok = 0;

  char *str = strdup(s);
  char *to_free = str;

  while ((ptr = strsep(&str, " \t")) != NULL) {
    if (ptr[0] == '\0')
      continue;

    if (strncmp(ptr, ">>", 2) == 0) {
      if (strlen(ptr) > 2) {
        p->outfile = strdup(ptr + 2);
        p->append = true;
      } else {
        while (1) {
          ptr = strsep(&str, " \t");
          if (ptr == NULL || ptr[0] != '\0')
            break;
        }
        if (ptr == NULL) {
          fprintf(stderr, "missing file after >>");
          exit(1);
        }
        p->outfile = strdup(ptr);
        p->append = true;
      }
      continue;
    } else if (ptr[0] == '>') {
      if (strlen(ptr) != 1) {
        p->outfile = strdup(ptr + 1);
      } else {
        while (1) {
          ptr = strsep(&str, " \t");
          if (ptr == NULL || ptr[0] != '\0')
            break;
        }
        if (ptr == NULL) {
          fprintf(stderr, "missing file after >");
          exit(1);
        }
        p->outfile = strdup(ptr);
      }
      continue;
    } else if (ptr[0] == '<') {
      if (strlen(ptr) != 1) {
        p->infile = strdup(ptr + 1);
      } else {
        while (1) {
          ptr = strsep(&str, " \t");
          if (ptr == NULL || ptr[0] != '\0')
            break;
        }
        if (ptr == NULL) {
          fprintf(stderr, "missing file after <");
          exit(1);
        }
        p->infile = strdup(ptr);
      }
      continue;
    }

    if (cur_tok == n_tokens) {
      n_tokens *= 2;
      argv = realloc(argv, sizeof(char *) * n_tokens);
    }

    argv[cur_tok] = strdup(ptr);
    cur_tok++;
  }

  if (cur_tok == n_tokens) {
    argv = realloc(argv, sizeof(char *) * (n_tokens + 1));
  }

  argv[cur_tok + 1] = NULL;

  p->n_tokens = cur_tok;
  p->argv = argv;

  free(to_free);

  return p;
}
