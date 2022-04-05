#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Pause after N steps of single-step execution", cmd_si },
  { "info", "Print register status", cmd_info },
	{ "x", "Scan memory", cmd_x },
	{ "p", "Expression evaluation", cmd_p },
	{ "w", "Set watchpoint", cmd_w },
	{ "d", "Delete watchpoint", cmd_d },

  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
  char *ch= strtok(NULL," ");

  if (!ch){
    cpu_exec(1);
  }else{
    int stepnum = atoi(ch);
    cpu_exec(stepnum);
  }
  return 0;
}

static int cmd_info(char *args){
  char *ch=strtok(NULL," ");
  
  if (strcmp(ch,"r") == 0){
		printf("Name\tHexadecimal\tDecimal\n");
		printf("----------------------------------\n");
		printf("32-bit Register:\n");
  	for (int i=R_EAX; i<=R_EDI; i++){
			printf("%s:\t%#010x\t%010d\n",regsl[i-R_EAX], cpu.gpr[i-R_EAX]._32, cpu.gpr[i-R_EAX]._32);
		}
		printf("eip:\t%#010x\t%010d\n",cpu.eip,cpu.eip);
		printf("----------------------------------\n");
		printf("16-bit Register:\n");
		for (int i=R_AX; i<=R_DI; i++){
			printf("%s:\t%#06x\t\t%05d\n",regsw[i-R_AX], cpu.gpr[i-R_AX]._16, cpu.gpr[i-R_AX]._16);
		}
		printf("----------------------------------\n");
		printf("8-bit Register:\n");
		for(int i=R_AH;i<=R_BH;i++){
			printf("%s:\t%#04x\t\t%03d\n", regsb[i-R_AL], cpu.gpr[i-R_AH]._8[1], cpu.gpr[i-R_AH]._8[1]);
			printf("%s:\t%#04x\t\t%03d\n", regsb[i-R_AH], cpu.gpr[i-R_AH]._8[0], cpu.gpr[i-R_AH]._8[0]);
		}  
  }else if (strcmp(ch,"w") == 0){
		list_watchpoint();
	}
  return 0;
}


static int cmd_x(char *args){
	char *time=strtok(NULL," ");
	if (time==NULL){
		printf("Lack of two parameters!!!\n");
		return 0;
	}
	char *start=strtok(NULL," ");
	if(start==NULL){
		printf("Lack of one parameter!!!\n");
		return 0;
	}
	int nums=atoi(time);
	uint32_t address;
	if(start[0]!='0'){
		bool flag=false;
		address=expr(start,&flag);
		if(!flag){
			printf("The address is wrong!!!\n");
		}
	}else{
  	sscanf(start,"%x",&address);
	}
	printf("Address\t\tDword block\tByte sequence\n");
	printf("---------------------------------------------\n");
	for(int i=0;i<nums;i++){
		uint32_t temp=vaddr_read(address,4);
		printf("%#010x:\t%#010x\t",address,temp);
		address+=4;
		for(int j=0;j<4;j++){
			if(j!=3){
				printf("%02x ",temp & 0xff);
			}else{
				printf("%02x",temp & 0xff);
			}
			temp=temp >> 8;
		}
		printf("\n");
	}
	return 0;
}

static int cmd_p(char *args){
	if(args==NULL){
		printf("The Correct Format is: \"p EXPRESSION\"\n");
		return 0;
	}
	bool flag;
	int res = expr(args,&flag);
	if(flag){
	//	printf("The answer is %d\n",res);
		printf("(DEC) = %d\n",res);
		printf("(HEX) = %#010x\n",res);
	}else{
		printf("Error! Wrong Expression\n");
	}
	return 0;
}

static int cmd_w(char *args){
	char *expression=strtok(NULL," ");
	//printf("%s\n",expression);
	if(expression==NULL){
		printf("The Correct Format is: \"w Expression\"\n");
		return 0;
	}
	int NO = set_watchpoint(expression);
	if(NO==-1){
		printf("Bad expression OR watchpoint full!!!\nFailed to set!!!\n");
	}else{
		printf("Watchpoint #%d has been set!!!\n",NO);		
	}
	return 0;
}

static int cmd_d(char *args){
	char *ch=strtok(NULL," ");
	if(ch==NULL){
		printf("The Correct Format is: \"d NUMBER\"\n");
		return 0;
	}
	int NO=atoi(ch);
	//printf("%d\n",NO);
	bool flag=delete_watchpoint(NO);
	if(!flag){
		printf("The delete operation failed!!!\nNot found the watchpoint!!!\n");
		//printf("The watchpoint #%d has been deleted!!!\n",NO);
	}
	return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
