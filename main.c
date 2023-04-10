#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを示すトークン
} e_token_kind;

typedef struct s_token t_token;

struct s_token {
  e_token_kind kind; // トークンの種類
  t_token *next;     // 次のトークン
  int val;           // kindがTK_NUMの場合、その値
  char *str;         // トークン文字列
};

// 現在着目しているトーク
t_token *token;

// 入力プログラム
char *user_input;

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
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
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
t_token *new_token(e_token_kind kind, t_token *cur, char *str) {
  t_token *tok = calloc(1, sizeof(t_token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
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

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("	mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("	add rax, %d\n", expect_number());
      continue;
    }

    if (consume('-')) {
      printf("	sub rax, %d\n", expect_number());
      continue;
    }
    error("予期しない文字です");
  }

  printf("	ret\n");
  return 0;
}

