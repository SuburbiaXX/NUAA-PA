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
	TK_REG,			// register
	TK_NEG,			// negative, minus sign
	TK_UEQ,			// unequal
	TK_AND, 		// and
	TK_OR,  		// or
	TK_POINTER,	//pointer	(*)
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
  {"==", TK_EQ},      	// equal
	{"!=", TK_UEQ},				// unequal
	{"&&", TK_AND},				// and
	{"\\|\\|", TK_OR},		// or
	{"\\!", '!'},			// inverse
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
					case TK_EQ:	//==
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
					case TK_UEQ:	//!=
						tokens[nr_token++].type=rules[i].token_type;
						break;
					case TK_AND:	//&&
						tokens[nr_token++].type=rules[i].token_type;
						break;
					case TK_OR:		//||
						tokens[nr_token++].type=rules[i].token_type;
						break;

					case '!':
						tokens[nr_token++].type=rules[i].token_type;
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
					

          default:
						tokens[nr_token++].type=rules[i].token_type;
						break;
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

int find_dominated_op(int p, int q){
	int pos=p;
	int count=0;
	int level=-1;
	for (int i=p;i<=q;i++){
		if(tokens[i].type=='('){
			count++;
		}		
		else if(tokens[i].type==')'){
			count--;
		}		
	  else if(count==0){//the expression is not in a pair of parentheses
			if(tokens[i].type==TK_NEG || tokens[i].type=='!' || tokens[i].type==TK_POINTER){
				if(level<0){
					level=0;
					pos=i;
				}
			}
			if(tokens[i].type=='*' || tokens[i].type=='/'){
				if(level<1){
					level=1;
					pos=i;
				}
			}
			if(tokens[i].type=='+' || tokens[i].type=='-'){
				if(level<2){
					level=2;
					pos=i;
				}
			}
			if(tokens[i].type==TK_EQ || tokens[i].type==TK_UEQ){
				if(level<3){
					level=3;
					pos=i;
				}
			}
			if(tokens[i].type==TK_AND){
				if(level<4){
					level=4;
					pos=i;
				}
			}
			if(tokens[i].type==TK_OR){
				if(level<5){
					level=5;
					pos=i;
				}
			}
		}
	}
	//printf("the dominated_op's pos = %d\n",pos);
	return pos;
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
//printf("Branket FIT!!!!!!!!!!!\n");
	return true;
}

uint32_t eval(int p, int q) {
   // printf("%d %d\n",p,q);
		if (p > q) {
		//	printf("This is a bad expression!!!\n");
			return 0;
    }
    else if (p == q) {
			uint32_t res=0;
			if(tokens[p].type==TK_DEC){
				sscanf(tokens[p].str,"%d",&res);
			}
			if(tokens[p].type==TK_HEX){
				sscanf(tokens[p].str,"%x",&res);
			}
			if(tokens[p].type==TK_REG){
			//	printf("%s\n",tokens[p].str);
				char src[4];
			//	printf("%d\n",strlen(tokens[p].str));
	
				strncpy(src,tokens[p].str+1,3);
				src[3]='\0';
			//	printf("%s\n",src);
			//	printf("%d\n",strlen(src));
			//	printf("compare res = %d\n",strcmp(src,"eip"));
				if(strcmp(src,"eip")==0){
			//	printf("It's eip\n");
					res=cpu.eip;
				}else{
					for(int i=0;i<8;i++){
						if(strcmp(src,regsl[i])==0){
							res=cpu.gpr[i]._32;
						}		
					}
				}
			}
			return res;
    }
    else if (check_parentheses(p, q) == true) {
        return eval(p + 1, q - 1);
    }
    else{
			int op;
			//int val1=0,val2=0;
			op =find_dominated_op(p,q);
      //val1 = eval(p, op - 1);
		  //val2 = eval(op+1,q);

			//int n;

			//printf("val1 = %d  val2 = %d\n",val1,val2);
      switch(tokens[op].type){
				case TK_EQ:
					if(eval(p, op-1)== eval(op+1,q)){
						return true;
					}else{
						return false;
					}

				case TK_UEQ:
					if(eval(p, op-1) != eval(op+1,q)){
						return true;
					}else{
						return false;
					}

				case TK_POINTER:
					return vaddr_read(eval(op+1,q),4);

				case TK_AND:
					return eval(p,op-1) && eval(op+1,q);
				
				case TK_OR:
					return eval(p,op-1) || eval(op+1,q);

				case TK_NEG: return  -eval(op+1,q);

        case '+': 
					return eval(p, op-1) + eval(op+1,q);

				case '-': 
					return eval(p, op-1) - eval(op+1,q);
        
				case '*': 
					return eval(p, op-1) * eval(op+1,q);

				case '/': 
					return eval(p, op-1) / eval(op+1,q);

				case '!':
					if (eval(op+1,q)!=0){
						return 0;
					}else{
						return 1;
					}
				/*case TK_NEG: n=-val2; printf("get %d\n",n); return n;
				case '+': n= val1 + val2; printf("get %d\n",n); return n;
        case '-': n= val1 - val2; printf("get %d\n",n); return n;
        case '*': n= val1 * val2; printf("get %d\n",n); return n;
        case '/': n= val1 / val2; printf("get %d\n",n); return n;*/
        default: assert(0);
        }
    } 
		return 0;
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
   *success = false;
    return 0;
  }else *success=true;
	
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
	for(int i=0;i<nr_token;i++){
		if(tokens[i].type=='-' && (i==0 || (tokens[i-1].type!=')' && tokens[i-1].type!=TK_DEC && tokens[i-1].type!=TK_HEX ))){
			tokens[i].type=TK_NEG;
			//printf("minus sign pos is %d\n",i);
		}
		if(tokens[i].type=='*' && (i==0 || (tokens[i-1].type!=')' && tokens[i-1].type!=TK_DEC && tokens[i-1].type!=TK_HEX ))){
			tokens[i].type=TK_POINTER;
			printf("Pointer's pos is %d\n",i);
		}
	}
  return eval(0,nr_token-1);
}
