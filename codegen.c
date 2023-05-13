#include "slcc.h"
#include <stdio.h>

static int label_seq = 0;

void gen_lvar(t_node *node) {
  if (node->kind != ND_LVAR)
    error("左辺値が変数ではありません\n");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(t_node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
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
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
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

void codegen(t_node *node) {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  // ローカル変数100個固定
  printf("  sub rsp, %d\n", 100 * 8);
  while (node) {
    gen(node);
    printf("  pop rax\n");
    node = node->next;
  }
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
