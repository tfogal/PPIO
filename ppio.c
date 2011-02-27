#define _POSIX_C_SOURCE 200112L
#include "ppio_internal.h"

struct mapping {
  const void* from;
  void* to;
  struct mapping* next;
  struct mapping* prev;
};
static struct mapping* head = NULL;

void p_hash_add(const void* from, void* to)
{
  struct mapping* entry = calloc(1, sizeof(struct mapping));
  entry->from = from;
  entry->to = to;
  entry->prev = NULL;
  entry->next = head;
  head = entry;
}

void p_hash_remove(const void* from)
{
  struct mapping* e = head;
  while(e && e->from != from) { e = e->next; }

  if(!e) { return; } /* not found. */

  if(e->prev) { e->prev->next = e->next; }
  if(e->next) {
    e->next->prev = e->prev;
  } else {
    e->next = NULL;
  }
  free(e);
}

void* p_hash_lookup(const void* from)
{
  struct mapping* e = head;
  while(e && e->from != from) { e = e->next; }

  if(e) { return e->to; }

  return NULL;
}
