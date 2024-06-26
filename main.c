#include "slcc.h"
#include <stdio.h>
#include <stdlib.h>

// 現在着目しているトーク
t_token *token;

// 入力プログラム
char *user_input;

// ローカル変数
t_lvar_list *locals;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize();
  t_function *prog = program();

  // オフセットを計算
  for (t_function *fn = prog; fn; fn = fn->next) {
    int offset = 0;
    for (t_lvar_list *vl = fn->locals; vl; vl = vl->next) {
      offset += 8;
      vl->lvar->offset = offset;
    }
    fn->stack_size = offset;
  }

  codegen(prog);
  return 0;
}
