#include "slcc.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool is_var_char_begin(char p) {
  return isalpha(p) || p == '_';
}

bool is_var_char(char p) {
  return isalpha(p) || isdigit(p) || p == '_';
}

int var_len(char *p) { 
  char *q = p;
  if (is_var_char_begin(*p)) {
    while(is_var_char(*p))
      p++;
  }
  return p - q;
}

t_lvar *find_lvar(t_token *tok) {
  for(t_lvar *var = locals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

t_lvar *new_lvar(t_token *tok) {
  t_lvar *lvar = calloc(1, sizeof(t_lvar));
  lvar->next = locals;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->offset = locals->offset + 8;
  locals = lvar;
  return lvar;
}

