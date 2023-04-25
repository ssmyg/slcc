#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slcc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号の時、トークンを一つ読み進めて真を返す
// それ以外の場合、偽を返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

t_token *consume_ident() {
  if (token->kind == TK_IDENT) {
    t_token *ret = token;
    token = token->next;
    return ret;
  }
  return NULL;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "expected '%s'", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを読み進めてその数値を返す
// それ以外にはエラーを報告する
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
t_token *new_token(e_token_kind kind, t_token *cur, char *str, size_t len) {
  t_token *tok = calloc(1, sizeof(t_token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool is_starts_with(char *s1, char *s2, int len) {
  return strncmp(s1, s2, len) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
t_token *tokenize() {
  char *p = user_input;
  t_token head;
  head.next = NULL;
  t_token *cur = &head;

  while (*p) {
    // 空白文字ならスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_var_char(*(p + 6))) {
      cur = new_token(TK_RESERVED, cur, p, 6);
      p += 6;
      continue;
    }

    size_t v_len = var_len(p);
    if (v_len) {
      cur = new_token(TK_IDENT, cur, p, v_len);
      p += v_len;
      continue;
    }

    if (is_starts_with(p, "==", 2) || is_starts_with(p, "!=", 2) || is_starts_with(p, "<=", 2) || is_starts_with(p, ">=", 2)) {

      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (ispunct(*p)) {
      cur = new_token(TK_RESERVED, cur, p, 1);
      p += 1;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
