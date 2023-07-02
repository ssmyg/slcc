#include "slcc.h"
#include <stdlib.h>
#include <string.h>

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

t_node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    t_node *node = expr();
    expect(")");
    return node;
  }

  t_token *tok = consume_ident();
  if (tok) {
    if (consume("(")) {
      // 関数
      t_node *node = calloc(1, sizeof(t_node));
      int i = 0;
      if (!consume(")")) {
        node->args[i] = expr();
        i++;
        while (consume(",")) {
          if (i >= 6)
            error("function too many args.");
          node->args[i] = assign();
          i++;
        }
        expect(")");
      }
      node->kind = ND_FUNC;
      node->func = calloc(tok->len + 1, sizeof(char));
      strncpy(node->func, tok->str, tok->len);
      return node;
    }

    // ローカル変数
    t_node *node = calloc(1, sizeof(t_node));
    node->kind = ND_LVAR;

    t_lvar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = new_lvar(tok);
      node->offset = lvar->offset;
    }
    return node;
  }

  // 数値
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

t_node *assign() {
  t_node *node = equality();

  while (true) {
    if (consume("="))
      node = new_node(ND_ASSIGN, node, assign());
    else
      return node;
  }
}

t_node *expr() {
  t_node *node = assign();

  return node;
}

t_node *stmt() {
  t_node *node;
  if (consume("{")) {
    t_node body_head;
    t_node *body_curr = &body_head;
    while (!consume("}")) {
      body_curr->next = stmt();
      body_curr = body_curr->next;
    }
    node = calloc(1, sizeof(t_node));
    node->kind = ND_BLOCK;
    node->body = body_head.next;
    return node;
  }
  if (consume("if")) {
    node = calloc(1, sizeof(t_node));
    node->kind = ND_IF;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else")) {
      node->els = stmt();
    }
    return node;
  }
  if (consume("while")) {
    node = calloc(1, sizeof(t_node));
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }
  if (consume("for")) {
    t_node *node = calloc(1, sizeof(t_node));
    node->kind = ND_FOR;
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->incl = expr();
      expect(")");
    }
    node->then = stmt();
    return node;
  }
  if (consume("return")) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }

  expect(";");
  return node;
}

int count_lvar_num() {
  int num = 0;
  t_lvar *tmp = locals;
  while (tmp) {
    num++;
    tmp = tmp->next;
  }
  return num;
}

t_function *func() {
  locals = calloc(1, sizeof(t_lvar));
  char *func_name = expect_func();
  expect("(");
  expect(")");
  expect("{");

  t_node head;
  head.next = NULL;
  t_node *cur = &head;
  while (!consume("}")) {
    t_node *node = stmt();
    cur->next = node;
    cur = node;
  }

  t_function *func = (t_function *)calloc(1, sizeof(t_function));
  func->name = func_name;
  func->node = head.next;
  func->lvar_num = count_lvar_num();
  return func;
}

t_function *program() {
  t_function head;
  head.next = NULL;
  t_function *cur = &head;

  while (!at_eof()) {
    t_function *fn = func();
    cur->next = fn;
    cur = fn;
  }

  return head.next;
}

