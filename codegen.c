#include "slcc.h"
#include <stdio.h>

static int label_seq = 0;
static char *ARG_REG[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lvar(t_node *node) {
  if (node->kind != ND_LVAR)
    error("左辺値が変数ではありません\n");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(t_node *node) {
  switch (node->kind) {
  case ND_BLOCK: {
    for (t_node *ni = node->body; ni; ni = ni->next) {
      gen(ni);
      printf("  pop rax\n"); // TODO これは不要？
    }
  }
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_FUNC: {
    int n = 0;
    while (node->args[n]) {
      gen(node->args[n]);
      n++;
    }
    for (int i = n - 1; i >= 0; i--)
      printf("  pop %s\n", ARG_REG[i]);
    // RSPが16の倍数になるように調整してから関数呼び出しを行う
    // 可変長引数のためにRAXを0に設定
    int seq = label_seq++;
    printf("  mov rax, rsp\n");
    printf("  and rax, 15\n");
    printf("  jnz .L.call.%d\n", seq);
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->func);
    printf("  jmp .L.end.%d\n", seq);
    printf(".L.call.%d:\n", seq);
    printf("  sub rsp, 8\n");
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->func);
    printf("  add rsp, 8\n");
    printf(".L.end.%d:\n", seq);
    printf("  push rax\n");
    return;
  }
  case ND_LVAR:
    gen_lvar(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lvar(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n"); // 計算した右辺の値
    printf("  pop rax\n"); // 左辺の変数のアドレス
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_IF: {
    int seq = label_seq++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    if (node->els) {
      printf("  je .L.else.%d\n", seq);
      gen(node->then);
      printf("  jmp .L.end.%d\n", seq);
      printf(".L.else.%d:\n", seq);
      gen(node->els);
    } else {
      printf("  je .L.end.%d\n", seq);
      gen(node->then);
    }
    printf(".L.end.%d:\n", seq);
    return;
  }
  case ND_WHILE: {
    int seq = label_seq++;
    printf(".L.begin.%d:\n", seq);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.end.%d\n", seq);
    gen(node->then);
    printf("  jmp .L.begin.%d\n", seq);
    printf(".L.end.%d:\n", seq);
    return;
  }
  case ND_FOR: {
    int seq = label_seq++;
    if (node->init)
      gen(node->init);
    printf(".L.begin.%d:\n", seq);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .L.end.%d\n", seq);
    }
    gen(node->then);
    if (node->incl)
      gen(node->incl);
    printf("  jmp .L.begin.%d\n", seq);
    printf(".L.end.%d:\n", seq);
    return;
  }
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  jmp .L.return\n");
    return;
  default:
    // 何もしない
    break;
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
  default:
    error("不正なノードです\n");
  }
  printf("  push rax\n");
}

void codegen(t_function *func) {
  printf(".intel_syntax noprefix\n");
  printf(".global %s\n", func->name);
  printf("%s:\n", func->name);

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", func->lvar_num * 8);
  t_node *node = func->node;
  while (node) {
    gen(node);
    printf("  pop rax\n");
    node = node->next;
  }
  printf(".L.return:\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
