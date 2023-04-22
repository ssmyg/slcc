#include <stdbool.h>

typedef struct s_node t_node;
typedef struct s_token t_token;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_NUM     // 整数
} e_node_kind;

// 抽象構文木のノードの型
struct s_node {
  e_node_kind kind;
  t_node *lhs;
  t_node *rhs;
  t_node *next;
  int val;
};

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
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
extern t_token *token;

// 入力プログラム
extern char *user_input;

/////////////////
// codegen.c
/////////////////
void gen(t_node *node);

/////////////////
// tokenizer.c
/////////////////
bool consume(char *op);
void expect(char *op);
t_token *tokenize();

void error(char *fmt, ...);
int expect_number();

bool at_eof();

/////////////////
// parser.c
/////////////////
t_node *program();
