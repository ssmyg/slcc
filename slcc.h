#include <stdbool.h>
#include <stddef.h>

typedef struct s_node t_node;
typedef struct s_token t_token;
typedef struct s_lvar t_lvar;
typedef struct s_function t_function;

// 現在着目しているトーク
extern t_token *token;

// 入力プログラム
extern char *user_input;

// ローカル変数
extern t_lvar *locals;

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
  ND_NUM,    // 整数
  ND_RETURN, // リターン
  ND_IF,     // if
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_BLOCK,  // block
  ND_FUNC,   // func
} e_node_kind;

// 抽象構文木のノードの型
struct s_node {
  e_node_kind kind;
  t_node *lhs;
  t_node *rhs;
  t_node *next;

  // if (cond) then else els;
  // while (cond) then;
  // for (init; cond; incl) then;
  t_node *cond;
  t_node *then;
  t_node *els;
  t_node *init;
  t_node *incl;

  // block
  t_node *body;

  int val;
  int offset;

  // function
  char *func;
  t_node *args[6];
};

struct s_function {
  char *name;
  t_node *node;
  int lvar_num;
  t_function *next;
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
  size_t len;        // トークンの長さ
};

// ローカル変数の型
struct s_lvar {
  t_lvar *next; // 次の変数かNULL
  char *name;   // 変数名
  int offset;   // RBPからのオフセット
};

/////////////////
// codegen.c
/////////////////
void codegen(t_function *func);

/////////////////
// tokenizer.c
/////////////////
bool consume(char *op);
t_token *consume_ident();
char *expect_func();
void expect(char *op);
t_token *tokenize();

void error(char *fmt, ...);
int expect_number();

bool at_eof();

char *my_strndup(char *str, size_t len);

/////////////////
// parser.c
/////////////////
t_node *assign();
t_node *expr();
t_function *program();

/////////////////
// var.c
/////////////////
size_t var_len(char *p);
t_lvar *new_lvar(char *name);
bool is_var_char(char p);
t_lvar *find_lvar(t_token *tok);
