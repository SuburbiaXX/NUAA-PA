#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
	TK_EQ,
	TK_DEC,
	TK_HEX,
	TK_REG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
	{"[0-9]+", TK_DEC},					// decimal
	{"0x[0-9a-fA-F]+", TK_HEX},	// hexadecimal
	{"\\$e[a-z]{2}", TK_REG},		// register
	{"\\(", '('},								// left parenthesis
	{"\\)", ')'},								// right parenthesis
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
	{"\\-", '-'},					// sub
	{"\\*", '*'},					// mul
	{"\\/", '/'},					// div	
  {"==", TK_EQ}      	  // equal
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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
					case TK_NOTYPE:
						break;
					case TK_EQ:
						tokens[nr_token++].type=rules[i].token_type;
						break;
					case TK_DEC:
						tokens[nr_token].type=rules[i].token_type;
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						break;
					case TK_HEX:
						tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						break;
					case TK_REG:
						tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						break;
					case '+':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
					case '-':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
					case '*':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
					case '/':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
					case '(':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
					case ')':
						tokens[nr_token++].type=rules[i].token_type;
				    break;
          default: TODO();
        }

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

bool check_parentheses(int p,int q){
	if(tokens[p].type!='(' || tokens[q].type!=')'){
		return false;
	}
	int count=0;
	for(int i=p+1;i<q;i++){
		if(tokens[i].type=='('){
			count++;
		}
		if(tokens[i].type==')'){
			count--;
		}
		if(count<0){
			return false;
		}
	}
	if(count>0){
		return false;
	}
	printf("Branket FIT!!!!!!!!!!!\n");
	return true;
}

uint32_t eval(int p, int q) {
    if (p > q) {
			printf("This is a bad expression!!!\n");
			return 0;
    }
    else if (p == q) {
			if(tokens[p].type==TK_DEC){
				int len=strlen(tokens[p].str);
				int res=0;
				for(int i=0;i<len;i++){
					int num;
					num=tokens[p].str[i]-'0';
					res=res*10+num;	
				}
				return res;
			}
    }
    else if (check_parentheses(p, q) == true) {
        return eval(p + 1, q - 1);
    }
    else {
      printf("to be continue!!\n"); 
 /* We should do more things here. */
    }
		return 0;
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return eval(0,nr_token-1);
}
