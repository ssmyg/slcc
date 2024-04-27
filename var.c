#include "slcc.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

bool is_var_char_begin(char p) {
  return isalpha(p) || p == '_';
}

bool is_var_char(char p) {
  return isalpha(p) || isdigit(p) || p == '_';
}

size_t var_len(char *p) {
  char *q = p;
  if (is_var_char_begin(*p)) {
    while (is_var_char(*p))
      p++;
  }
  return p - q;
}

t_lvar *find_lvar(t_token *tok) {
  for (t_lvar *var = locals; var; var = var->next) {
    size_t len = strlen(var->name);
    if (len == tok->len && !memcmp(tok->str, var->name, len)) {
      return var;
    }
  }
  return NULL;
}

t_lvar *new_lvar(char *name) {
  t_lvar *lvar = calloc(1, sizeof(t_lvar));
  lvar->next = locals;
  lvar->name = name;
  locals = lvar;
  return lvar;
}
