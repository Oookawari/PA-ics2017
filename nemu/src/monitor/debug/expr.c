#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
enum {
   TK_NOTYPE = 256, TK_EQ, TK_D_NUM, TK_H_NUM, TK_REG, TK_UMINUS, TK_LOGIC_AND, TK_LOGIC_OR, TK_NOTEQUAL, TK_ADDRESS

  /* TODO: Add more token types */

};

/*优先级枚举*/
enum {
  LOGIC_OR = 1, LOGIC_AND, EQ_N_NEQ , PLUS_N_SUB, MULT_N_DIV, UNARY, 
};

/*字符串拷贝函数*/
int my_strcpy(char* dst, const char* src, int num) {
  for(int i = 0; i < num; i++){
    dst[i] = src[i];
  }
  dst[num] = '\0';
  return 0;
}

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // mult
  {"/", '/'},           // div
  {"\\(", '('},         // lpar
  {"\\)", ')'},         // rpar
  {"!=", TK_NOTEQUAL},         // notequal
  {"&&", TK_LOGIC_AND},         // land
  {"\\|\\|", TK_LOGIC_OR},         // lor
  {"!", '!'},         // lont
  {"0[xX][0-9a-fA-F]+", TK_H_NUM},// 十六进制整数
  {"0|[1-9][0-9]*", TK_D_NUM},     // 十进制整数
  {"\\$[a-zA-Z0-9]+", TK_REG},   // register
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        /*Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            /i, rules[i].regex, position, substr_len, substr_len, substr_start);
        */
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        //string substr = str.substr(position, substr_len);
        char substr[32];
        
        my_strcpy(substr, substr_start, substr_len);
        /*长度大于31的报错，暂时不处理*/
        if(substr_len > 31) {printf("expr.c : token length > 32\n");assert(0);}
        /*token数大于31的报错，暂时不处理*/
        if(nr_token > 31) {printf("expr.c : token num > 32\n");assert(0);}
        switch (rules[i].token_type) {
          case TK_NOTYPE:
              nr_token--;
              break;
          case TK_D_NUM:
          case TK_H_NUM:
          case TK_REG:
              strcpy(tokens[nr_token].str,substr);
          default: 
              tokens[nr_token].type = rules[i].token_type;
              break;
        }
        nr_token ++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int start, int end) {
  bool check_res = true;
  if(tokens[start].type != '(') {check_res = false;}
  if(tokens[end].type != ')') {check_res = false;}
  int depth = 0;
  for(int i = start; i <= end; i++) {
     if(tokens[i].type == '(') depth++;
     if(tokens[i].type == ')') {
      depth--;
      if(depth < 0) {
        check_res = false;
        printf("expr.c : parentheses don't match, bad expression");
        assert(0);
      }
      if(depth == 0 && i != end) {
        check_res = false;
      }
     }
  }
  if(depth > 0) {
    check_res = false;
    printf("expr.c : parentheses don't match, bad expression");
    assert(0);
  }
  return check_res;
}

uint32_t find_op(int start, int end){
  /*寻找op*/
  int paren_stack = 0;
  int min_prior_op = 99999999;
  int index_of_op = -1;
  for(int i = start; i <= end; i++) {
    if(tokens[i].type == '(') {
      paren_stack++;
      continue;
      }
    if(tokens[i].type == ')') {
      paren_stack--;
      continue;
      }
    if(paren_stack != 0) {
      continue;
    }  
    /*prior_1*/
    if(tokens[i].type == TK_LOGIC_OR) {
      min_prior_op = LOGIC_OR;
      index_of_op = i;
    }
    /*prior_2*/
    if(min_prior_op < LOGIC_AND) continue;
    if(tokens[i].type == TK_LOGIC_AND) {
      min_prior_op = LOGIC_AND;
      index_of_op = i;
    }
    /*prior_3*/
    if(min_prior_op < EQ_N_NEQ) continue;
    if(tokens[i].type == TK_EQ || tokens[i].type == TK_NOTEQUAL) {
      min_prior_op = EQ_N_NEQ;
      index_of_op = i;
    }
    /*prior_4*/
    if(min_prior_op < PLUS_N_SUB) continue;
    if(tokens[i].type == '+' || tokens[i].type == '-') {
      min_prior_op = PLUS_N_SUB;
      index_of_op = i;
    }
    /*prior_5*/
    if(min_prior_op < MULT_N_DIV) continue;
    if(tokens[i].type == '*' || tokens[i].type == '/') {
      min_prior_op = MULT_N_DIV;
      index_of_op = i;
    }
    /*prior_6*/
    if(min_prior_op <= UNARY) continue;
    if(tokens[i].type == TK_UMINUS || tokens[i].type == '!' || tokens[i].type == TK_ADDRESS) {
      min_prior_op = UNARY;
      index_of_op = i;
    }
    
  }
  return index_of_op;
}

uint32_t eval(int start, int end) {
  /**/
  //printf("eval: %d, %d\n",start,end);
    if (start > end) {
      return 0;
        /* Bad expression */
    }
    else if (start == end) {
      Token temp = tokens[start];
      int value = -1;
      switch (temp.type)
      {
      case TK_D_NUM:
        /* 获取十进制数 */
        value = atoi(temp.str);
        break;
      case TK_H_NUM:
        /* 获取十六进制数 */
        value = 0;
        //string Num = temp.str.substr(2);
        for(int i = 2; i < strlen(temp.str); i++){
          char ch = temp.str[i];
          if (ch >= 'a' && ch <= 'f') {
            value = value * 16 + 10 + ch - 'a';
          }
          else if (ch >= 'A' && ch <= 'F') {
            value = value * 16 + 10 + ch - 'A';
          }
          else if (ch >= '0' && ch <= '9') {
            value = value * 16 + ch - '0';
          }
          else{
            printf("Unexpected character in expr TK_H_NUM");
            assert(0);
          }
        }
        break;
      case TK_REG:
        /* 获取寄存器 */
        value = 0;
        char reg[8];
        strcpy(reg, temp.str + 1);
        //string reg = temp.str.substr(1);
        if(strcmp(reg, "eax") == 0){value = cpu.gpr[0]._32;}
        else if(strcmp(reg, "ecx") == 0){value = cpu.gpr[1]._32;}
        else if(strcmp(reg, "edx") == 0){value = cpu.gpr[2]._32;}
        else if(strcmp(reg, "ebx") == 0){value = cpu.gpr[3]._32;}
        else if(strcmp(reg, "esp") == 0){value = cpu.gpr[4]._32;}
        else if(strcmp(reg, "ebp") == 0){value = cpu.gpr[5]._32;}
        else if(strcmp(reg, "esi") == 0){value = cpu.gpr[6]._32;}
        else if(strcmp(reg, "edi") == 0){value = cpu.gpr[7]._32;}
        else if(strcmp(reg, "ah") == 0){value = cpu.gpr[0]._8[1];}
        else if(strcmp(reg, "ch") == 0){value = cpu.gpr[1]._8[1];}
        else if(strcmp(reg, "dh") == 0){value = cpu.gpr[2]._8[1];}
        else if(strcmp(reg, "bh") == 0){value = cpu.gpr[3]._8[1];}
        else if(strcmp(reg, "al") == 0){value = cpu.gpr[1]._8[0];}
        else if(strcmp(reg, "cl") == 0){value = cpu.gpr[2]._8[0];}
        else if(strcmp(reg, "dl") == 0){value = cpu.gpr[3]._8[0];}
        else if(strcmp(reg, "bl") == 0){value = cpu.gpr[3]._8[0];}
        else if(strcmp(reg, "ax") == 0){value = cpu.gpr[0]._16;}
        else if(strcmp(reg, "cx") == 0){value = cpu.gpr[1]._16;}
        else if(strcmp(reg, "dx") == 0){value = cpu.gpr[2]._16;}
        else if(strcmp(reg, "bx") == 0){value = cpu.gpr[3]._16;}
        else if(strcmp(reg, "sp") == 0){value = cpu.gpr[4]._16;}
        else if(strcmp(reg, "bp") == 0){value = cpu.gpr[5]._16;}
        else if(strcmp(reg, "si") == 0){value = cpu.gpr[6]._16;}
        else if(strcmp(reg, "di") == 0){value = cpu.gpr[7]._16;}
        else if(strcmp(reg, "eip") == 0){value = cpu.eip;}
        break;
      default:
        value = 0;
        break;
      }
      return value;
    }
    else if (check_parentheses(start, end) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
        * If that is the case, just throw away the parentheses.
        */
        return eval(start + 1, end - 1);
    }
    else {
        int op_index = find_op(start, end);
        int op_type = tokens[op_index].type;
        int val1 = eval(start, op_index - 1);
        int val2 = eval(op_index + 1, end);
        switch (op_type) {
          case '+': return val1 + val2;
          case '-': return val1 - val2;
          case '*': return val1 * val2;
          case '/': return val1 / val2;
          case '!': return (val2 ? 0 : 1);
          case TK_EQ: return ((val2 == val1) ? 1 : 0);
          case TK_NOTEQUAL: return ((val2 != val1) ? 1 : 0);
          case TK_LOGIC_AND: return val1 && val2;
          case TK_LOGIC_OR: return val1 || val2;
          case TK_UMINUS: return -val2;
          case TK_ADDRESS: return vaddr_read(val2, 4);
          default: 
          printf("op_type: %d\n",op_type);
          assert(0);
        }
    }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  bool has_operator = false;
  for(int i = 0; i <= nr_token - 1; i++) {
    if((tokens[i].type == '-') || (tokens[i].type == TK_UMINUS)) {
      if(!has_operator) {tokens[i].type = TK_UMINUS;continue;}
    }
    if((tokens[i].type == '*') || (tokens[i].type == TK_ADDRESS)) {
      if(!has_operator) {tokens[i].type = TK_ADDRESS;continue;}
    }
    if((tokens[i].type == '+') || (tokens[i].type == '-') || (tokens[i].type == '*') || (tokens[i].type == '/') || 
    (tokens[i].type == TK_NOTEQUAL) || (tokens[i].type == TK_LOGIC_AND) || (tokens[i].type == TK_LOGIC_OR) || (tokens[i].type == TK_EQ)) {
      has_operator = false;
    }
    if((tokens[i].type == TK_D_NUM) || (tokens[i].type == TK_H_NUM) || (tokens[i].type == TK_REG) || (tokens[i].type == ')')) {
      has_operator = true;
    }
  }

  //printf("TK_NUM:%d\n",nr_token);
  *success = true;
  return eval(0, nr_token - 1);
}