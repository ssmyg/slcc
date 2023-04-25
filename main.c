#include "slcc.h"
#include <stdio.h>
#include <stdlib.h>

// 現在着目しているトーク
t_token *token;

// 入力プログラム
char *user_input;

// ローカル変数
t_lvar *locals;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の数が正しくありません");
    return 1;
  }

  locals = calloc(1, sizeof(t_lvar));
  user_input = argv[1];
  token = tokenize();
  t_node *node = program();

  codegen(node);
  return 0;
}

