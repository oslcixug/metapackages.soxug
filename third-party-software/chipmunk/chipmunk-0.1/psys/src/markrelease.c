#define MARKRELEASE_G

#include <stdio.h>
#include <p2c/p2c.h>
#include <p2c/markrelease.h>

struct record {
  struct record *prev;
} *mark_first_mallocced_info;

char *fakemalloc(size)
long size;
{
  struct record *retval;

#undef malloc
  retval = (struct record *) malloc(size + sizeof(struct record));
#define malloc(x) fakemalloc(x)

  retval->prev = mark_first_mallocced_info;
  mark_first_mallocced_info = retval;

  return ((char *)(retval) + sizeof(struct record) / sizeof(char));
}

void mark(state)
struct record **state;
{
  *state = mark_first_mallocced_info;
}

void release(state)
struct record **state;
{
  struct record *save;

  while (mark_first_mallocced_info != *state) {
    save = mark_first_mallocced_info;
    mark_first_mallocced_info = save->prev;
    free(save);
  }
}



