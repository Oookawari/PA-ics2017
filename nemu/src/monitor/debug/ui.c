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

static int cmd_p(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_test(char *args) {
  char *arg = strtok(NULL, " ");
  while(arg != NULL){
    printf("arg: %s", arg);
    arg = strtok(NULL, " ");
  }
  return 0;
};

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single line executing", cmd_si },
  { "info", "Display the state of registers and checkpoints", cmd_info },
  { "p", "Compute expression", cmd_p },
  { "x", "Display the memory content", cmd_x },
  { "test", "输出接下来所有的内容", cmd_test },
  { "w", "Set checkpoint", cmd_w },
  { "d", "Unset checkpoint", cmd_d }

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

static int cmd_si(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int si = 1;
  /*arg != NULL, get si*/
  if(arg != NULL){
    si = atoi(arg);
    if(si == 0){
      printf("si [N], NaN !\n");
    }
  }
  cpu_exec(si);
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  if(strcmp(arg, "r") == 0) {
    printf("-----------------------\n");
    printf("registers:\n");
    printf("-----------------------\n");
    printf("eax: %08x\tax: %04x\tah: %02x\tal: %02x\n",cpu.gpr[0]._32,cpu.gpr[0]._16,cpu.gpr[0]._8[1],cpu.gpr[0]._8[0]);
    printf("ecx: %08x\tcx: %04x\tch: %02x\tcl: %02x\n",cpu.gpr[1]._32,cpu.gpr[1]._16,cpu.gpr[1]._8[1],cpu.gpr[1]._8[0]);
    printf("edx: %08x\tdx: %04x\tdh: %02x\tdl: %02x\n",cpu.gpr[2]._32,cpu.gpr[2]._16,cpu.gpr[2]._8[1],cpu.gpr[2]._8[0]);
    printf("ebx: %08x\tbx: %04x\tbh: %02x\tbl: %02x\n",cpu.gpr[3]._32,cpu.gpr[3]._16,cpu.gpr[3]._8[1],cpu.gpr[3]._8[0]);
    printf("esp: %08x\tsp: %04x\n",cpu.gpr[4]._32,cpu.gpr[4]._16);
    printf("ebp: %08x\tbp: %04x\n",cpu.gpr[5]._32,cpu.gpr[5]._16);
    printf("esi: %08x\tsi: %04x\n",cpu.gpr[6]._32,cpu.gpr[6]._16);
    printf("edi: %08x\tdi: %04x\n",cpu.gpr[7]._32,cpu.gpr[7]._16);
    printf("-----------------------\n");
  }
  else if(strcmp(arg, "w") == 0) {
    show_wps();
  }
  return 0;
}

static int cmd_p(char *args) {
  
  //printf("args:%s\n",args);
  bool success;
  int32_t res = expr(args, &success);
  if(success == true) printf("result: %d\n", res);
  else printf("expression computing doesnt success!");
  return 0;
}

static int cmd_x(char *args) {
  
  int cut_index = 0;
  for(int i = 0; i < strlen(args); i++) {
    if(args[i] == ' ') {cut_index = i;break;}
  }
    
  char my_expr[100];
  strcpy(my_expr, args + cut_index);
  char *arg1 = strtok(NULL, " ");
  int N = 1;

  /*arg1 != NULL, get N*/
  if(arg1 != NULL){
    N = atoi(arg1);
    if(N == 0){
      printf("x [N], NaN !\n");
    }
  }
  bool success = false;
  int expr_value = 0;
  
  expr_value = expr(my_expr, &success);
  //sscanf(expr, "%x", &expr_value);

  int content1 = 0x0;
  int content2 = 0x0;
  int content3 = 0x0;
  int content4 = 0x0;
  printf("-----------------------\n");
  for(int i = 0; i < N; i++) {
    content1 = vaddr_read(expr_value + 0, 1);
    content2 = vaddr_read(expr_value + 1, 1);
    content3 = vaddr_read(expr_value + 2, 1);
    content4 = vaddr_read(expr_value + 3, 1);
    printf("address %08x:\t%02x\t%02x\t%02x\t%02x\n",expr_value, content1, content2, content3, content4);
    content4 = vaddr_read(expr_value + 0, 4);
    printf("address %08x:\t%08x\n",expr_value, content4);
    expr_value += 4;
  }
  printf("-----------------------\n");
  return 0;
}

static int cmd_w(char *args) {
  WP* wp = new_wp();
  strcpy(wp->expr, args);
  bool success;
  wp->init_value = expr(args, &success);
  if(!success) {
    printf("expression computing doesnt success!");
    assert(0);
  }
  return 0;
};

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  int wno = -1;
  /*arg != NULL, get si*/
  if(arg != NULL){
    wno = atoi(arg);
    if(wno == -1){
      printf("error\n");
      assert(0);
    }
  }
  WP* temp = find_wp(wno);
  if(temp == NULL) {
    printf("watchpoint not found!\n");
    return 0;
  }
  free_wp(temp);
  return 0;
};

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