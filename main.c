#include "slcc.h"
#include <stdio.h>

// 現在着目しているトーク
t_token *token;

// 入力プログラム
char *user_input;

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

