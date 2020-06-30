#include "info.h"
#include "io.h"
#include "irq.h"
#include "map.h"
#include "mbox.h"
#include "power.h"
#include "string.h"
#include "time.h"
#include "timer.h"
#include "util.h"
#include "task.h"
#include "sys.h"
#include "process.h"
#include "fs.h"
#include "fat32.h"

#ifndef WITHOUT_LOADER
#include "loadimg.h"
#endif

#define BUFFER_SIZE 128

char *shell_read_line(char *ptr, char *buffer) {
  print("# ");
  char *beg = ptr--;
  do {
    *(++ptr) = getchar();
    switch (*ptr) {
      case 4:
        ptr = buffer + 1;
        *ptr = '\r';
        *buffer = 4;
        break;
      case 8:
      case 127:
        ptr--;
        if (ptr >= buffer) {
          ptr--;
          print("\b \b");
        }
        break;
      case 12:
        *ptr = 0;
        ptr--;
        print("\e[1;1H\e[2J");
        print("# ", beg);
        break;
      case 21:
        ptr--;
        while (ptr >= buffer) {
          if (*ptr == '\t')
            print("\b\b\b\b\b\b");
          else
            print("\b \b");
          ptr--;
        }
        break;
      default:
        putchar(*ptr);
    }

  } while (ptr < buffer || (!strchr("\r", *ptr)));
  while (ptr >= buffer && strchr(" \r\t", *ptr))
    ptr--;
  *(++ptr) = 0;
  puts("");
  while (beg < ptr && strchr(" \r\t\n", *beg))
    beg++;
  return beg;
}

int shell_execute(char *cmd, int el) {
  int ret = 0;
  FILE *fd = stdout;
  char *e = cmd + strlen(cmd);
  while(e > cmd){
    e--;
    if(*e == '>'){
      *e = 0;
      e++;
      while(*e == ' ') e++;
      char *p = e;
      while(*p && !strchr(" \r\t\n", *p)) p++;
      *p = 0;
      fd = vfs_open(e, O_CREAT);
    }
  }
  if(strbeg(cmd, "echo")){
    fprintf(fd, "%s" NEWLINE, cmd + 5);
  }
  else if(EQS("init", cmd)){
   fat32_init();
  }
  //else if(EQS("sdhost", cmd)){

  //}
  else if(EQS("file", cmd)){
    task_file_op(1);
  }
  else if(EQS("vnode", cmd)){
    task_vnode_op(1);
  }
  else if(EQS("dir", cmd)){
    task_read_dir(1);
  }
  else if(EQS("hello", cmd)) {
    println("Hello World!");
  }
  else if(EQS("fs", cmd)){
    vfs_show();
  }
  else if(strbeg(cmd, "ls")){
    char *p = cmd + 2;
    while(*p == ' ') p++;
    int i = 0;
    dirent *entry;
    DIR *dir = vfs_opendir(*p ? p : ".");
    if(dir){
      while((entry = vfs_readdir(dir))){
        printf(entry->type == dirent_dir ?
            "%d. {%s}" NEWLINE : "%d. <%s>" NEWLINE,
            i++, entry->name);
      }
    }
    vfs_closedir(dir);
  }
  else if(strbeg(cmd, "tree")){
    char *p = cmd + 4;
    while(*p == ' ') p++;
    DIR *dir = vfs_opendir(*p ? p : ".");
    if(dir){ list_dir(dir, 0); }
    vfs_closedir(dir);
  }
  else if(strbeg(cmd, "cat")){
    char *p = cmd + 3, buf[128];
    char *b = p + strlen(p);
    while(b > p && strchr(" \r\t\n", *b)) b--;
    *(b + 1) = 0;
    while(*p == ' ') p++;
    FILE *cat_fd = vfs_open(p, 0);
    if(cat_fd){
      buf[vfs_read(cat_fd, buf, 100)] = 0;
      fprintf(fd, buf);
      vfs_close(cat_fd);
    }
    else{
      puts("[shell] no such file or directory");
    }
  }
  else if(strbeg(cmd, "mount")){
    char *p = cmd + 5, *b = 0;
    while(*p == ' ') p++;
    b = p;
    while(*b != ' ' && *b) b++;
    if(*b) *b = 0, b++;
    while(*b == ' ') b++;
    if(*p && *b && vfs_mount(p, b, "tmpfs")){
      //printfmt("%s mounted to %s", p, b);
    }
    else printfmt("mount %s to %s failed", p, b);
  }
  else if(EQS("multilayer", cmd)){
    task_multilayer(1);
  }
  else if(EQS("user", cmd)){
    privilege_task_create(kexec_user_main, 0, current_task->priority);
  }
  else if(strbeg(cmd, "umount")){
    char *p = cmd + 6;
    while(*p == ' ') p++;
    if(!vfs_umount(p)) printfmt("unmount %s failed", p);
  }
  else if(strbeg(cmd, "touch")){
    char *p = cmd + 5;
    while(*p == ' ') p++;
    FILE *fd = vfs_open(p, O_CREAT);
    vfs_close(fd);
  }
  else if(strbeg(cmd, "mkdir")){
    char *p = cmd + 5;
    while(*p == ' ') p++;
    vfs_mkdir(p);
  }
  else if(strbeg(cmd, "cd")){
    char *p = cmd + 2;
    while(*p == ' ') p++;
    vfs_chdir(p);
  }
  else if (EQS("help", cmd)) {
    println("hello : print Hello World!");
    println("clear : clean the screen");
    println("timestamp : show timestamp");
    println("reboot : reboot the device");
    println("help : print all available commands");
#ifdef BUILD_STAMP
#define xstr(a) str(a)
#define str(a) #a
    println("BUILD @ ", xstr(BUILD_STAMP));
#endif
  }
  else if (EQS("timestamp", cmd)) {
    if(el) timestamp();
    else puts("not support timestamp on el0 currently");
  }
  else if (EQS("sleep", cmd)) {
    if(el) sleep(6);
    else puts("not support sleep on el0 currently");
  }
  else if (EQS("reboot", cmd)) {
    puts("rebooting...");
    reboot();
    ret = -1;
  }
  else if (EQS("exit", cmd) || cmd[0] == 4) {
    //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
    //__asm__ volatile ("msr daifset, #0xf":::"memory");
    //__asm__ volatile("stp x8, x9, [sp, #-16]!");
    //__asm__ volatile("mov x8, #2");
    //__asm__ volatile("svc #0");
    //__asm__ volatile("ldp x8, x9, [sp], #16");
    ret = -1;
  }
  else if (EQS("clear", cmd)) {
    print("\e[1;1H\e[2J");
  }
#ifndef WITHOUT_LOADER
  else if (EQS("loadimg", cmd)) {
    if(el) loadimg();
    else puts("not support timestamp on el0 currently");
  }
#endif
  else if (EQS("exc", cmd)) {

    if(!el) puts("cannot do exc on el0 currently");
    else{
#if defined(RUN_ON_EL1) || defined(RUN_ON_EL2)
      get_current_el();
#endif

#if 1
      int *sp;
      __asm__ volatile("mov %0, sp" : "=r"(sp));
      printf("sp = 0x%x\n", sp);
#endif
      __asm__ volatile("svc #1");
    }
  }
  else if (EQS("brk", cmd)) {
    if(!el) puts("cannot do exc on el0 currently");
    else{
#if 1
      int *sp;
      __asm__ volatile("mov %0, sp" : "=r"(sp));
      printf("sp = 0x%x\n", sp);
#endif
      __asm__ volatile("brk #1");
      puts("ret from brk");
    }
  }
  else if (EQS("irq", cmd)) {
    if(el){
      sys_timer_init();
      local_timer_init();
      core_timer_init();
    }
    else puts("not support timer on el0 currently");
  }
  else if (EQS("board", cmd)) {
    if (get_board_revision())
      printf("0x%x" NEWLINE, mbox[5]);
    else
      puts("get_board_reversion() failed");
  }
  else if (EQS("vcaddr", cmd)) {
    if (get_vc_memaddr())
      printf("0x%x\n", mbox[5]); // it should be 0xa020d3 for rpi3 b+
    else
      puts("get_vc_memaddr() failed");
  }
#ifdef TEST
  else if (EQS("bss", cmd)) {
    char *beg, *end;
    __asm__ volatile("adr %0, __bss_beg" : "=r"(beg));
    __asm__ volatile("adr %0, __bss_end" : "=r"(end));
    for (char *p = beg; p != end; p++) {
      print("0x");
      _print_ULL_as_number((unsigned long long)p, 16);
      println(": ", (int)*p);
    }
  }
#endif
  else if (strbeg(cmd, "kill")) {
    int pid = cmd[4] - '0';
    if(pid >= 0 && pid < TASK_SIZE){
      printf("kill pid %c" NEWLINE, cmd[4]);
      sys_signal(pid, SIGKILL);
    }
    else{
      printf("invalid pid %c" NEWLINE, cmd[4]);
    }
  }
  else if (EQS("buddy", cmd)) {
    task_buddy_aloc(1);
  }
  else if (EQS("fixed", cmd)) {
    task_fixed_aloc(1);
  }
  else if (EQS("varied", cmd)) {
    task_varied_aloc(1);
  }
  else if (strlen(cmd)) {
    print("command not found: ", cmd, NEWLINE);
    ret = 1;
  }
  if(fd) vfs_close(fd);
  return ret;
}

char *shell_stuff_line(char c, char **ptr, char *buffer) {

  *(++(*ptr)) = c;
  switch (**ptr) {
    case 4:
      *ptr = buffer + 1;
      **ptr = '\r';
      *buffer = 4;
      break;
    case 8:
    case 127:
      (*ptr)--;
      if (*ptr >= buffer) {
        (*ptr)--;
        print("\b \b");
      }
      break;
    case 12:
      **ptr = 0;
      (*ptr)--;
      print("\e[1;1H\e[2J");
      print("# ", buffer);
      break;
    case 21:
      (*ptr)--;
      while (*ptr >= buffer) {
        if (**ptr == '\t')
          print("\b\b\b\b\b\b");
        else
          print("\b \b");
        (*ptr)--;
      }
      break;
    default:
      putchar(**ptr);
  }

  char *p = 0;
  if ((*ptr) >= buffer && strchr("\r\n", **ptr)) {
    p = buffer;
    while ((*ptr) >= buffer && strchr(" \r\t", **ptr))
      (*ptr)--;
    *(++(*ptr)) = 0;
    puts("");
    while (p < (*ptr) && strchr(" \r\t\n", *p))
      p++;
    (*ptr) = buffer - 1;
  }
  else if ((*ptr) >= buffer + BUFFER_SIZE) {
    puts("buffer size isn't enough... cleared.");
    (*ptr) = buffer - 1;
  }
  return p;
}

int busy_shell_loop(int el) {
  char buffer[BUFFER_SIZE];
  while (shell_execute(shell_read_line(buffer, buffer), el) >= 0);
  return 0;
}

void irq_shell_loop(int el){
  char buffer[BUFFER_SIZE], *exec_ptr, *ptr;
  ptr = buffer - 1;
  print("# ");
  while (1) {
    exec_ptr = shell_stuff_line(sys_read(), &ptr, buffer);
    if (exec_ptr) {
      shell_execute(exec_ptr, el);
      exec_ptr = 0;
      print("# ");
    }
  }
}
