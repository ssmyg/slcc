#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_node t_node;
typedef struct s_token t_token;

bool consume(char *op);
void expect(char *op);
t_node *expr();

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <
  ND_LE, // <=
  ND_NUM
} e_node_kind;

// 抽象構文木のノードの型
struct s_node {
  e_node_kind kind;
  t_node *lhs;
  t_node *rhs;
  int val;
};

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを示すトークン
} e_token_kind;

struct s_token {
  e_token_kind kind; // トークンの種類
  t_token *next;     // 次のトークン
  int val;           // kindがTK_NUMの場合、その値
  char *str;         // トークン文字列
  int len;           // トークンの長さ
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

t_node *new_node(e_node_kind kind, t_node *lhs, t_node *rhs) {
  t_node *node = calloc(1, sizeof(t_node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

t_node *new_node_num(int val) {
  t_node *node = calloc(1, sizeof(t_node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

// 次のトークンが期待している記号の時、トークンを一つ読み進めて真を返す
// それ以外の場合、偽を返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
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
t_token *new_token(e_token_kind kind, t_token *cur, char *str, int len) {
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
      cur = new_token(TK_NUM, cur, p, -1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, -1);
  return head.next;
}

t_node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    t_node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

t_node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

t_node *mul() {
  t_node *node = unary();

  while (true) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

t_node *add() {
  t_node *node = mul();

  while (true) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

t_node *relational() {
  t_node *node = add();

  while (true) {
    if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume(">=")) // >= は <= の両辺を入れ替えて実装
      node = new_node(ND_LE, add(), node);
    else if (consume(">")) // >  は <  の両辺を入れ替えて実装
      node = new_node(ND_LT, add(), node);
    else
      return node;
  }
}

t_node *equality() {
  t_node *node = relational();

  while (true) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

t_node *expr() {
  t_node *node = equality();

  return node;
}

void gen(t_node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }
  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize();
  t_node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("	ret\n");
  return 0;
}

