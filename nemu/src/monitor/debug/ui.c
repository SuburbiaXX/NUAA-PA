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
    //default
    cpu_exec(1);
  }else{
    int stepnum = atoi(ch);
    if (stepnum < 0){
      printf("Error, N cannot be a negative number!!!\n");
      return 0;
    }
    cpu_exec(stepnum);
  }
  return 0;
}

static int cmd_info(char *args){
  char *ch=strtok(NULL," ");
  
  if (strcmp(ch,"r") == 0){
		printf("Name\tHexadecimal\tDecimal\n");
		printf("----------------------------------\n");
  	for (int i=R_EAX; i<=R_EDI; i++){
			printf("%s:\t0x%08x\t%010d\n",regsl[i-R_EAX], cpu.gpr[i-R_EAX]._32, cpu.gpr[i-R_EAX]._32);
		}  
  }else if (strcmp(ch,"w") == 0){
    
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
  sscanf(start,"%x",&address);

//	printf("test\n");
//	printf("%d\t%x\n",nums,address);

	printf("Address\t\tDword block\tByte sequence\n");
	for(int i=0;i<nums;i++){
		uint32_t temp=vaddr_read(address,4);
		printf("0x%08x:\t0x%08x\t",address,temp);
		address+=4;
		for(int j=0;j<4;j++){
			printf("%x ",temp&0xff);
			temp=temp>>8;
		}
		printf("\n");
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
