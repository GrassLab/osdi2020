void uart_test(){
    char buffer[16]="writewrite\n";
    buffer[12]='\0';  
    
    int size;
    size = uart_write(buffer,sizeof(buffer));
    uart_puts("size: 0x"); 
    uart_hex(size);
    uart_puts("\n"); 
   
    size = uart_read(buffer,sizeof(buffer));
    uart_puts("size: 0x"); 
    uart_hex(size);
    uart_puts("\n"); 
}

void foo(){
  int tmp = 5;
  uart_puts("Task ");
  uart_hex(get_taskid());
  uart_puts(" after exec, tmp address 0x");
  uart_hex(&tmp);
  uart_puts(", tmp value ");
  uart_hex(tmp);
  uart_puts("\n");
  
  uart_test();  

  exit(0);
  uart_puts("Should not be printed\n");
}

void test() {
  int cnt = 1;
  if (fork() == 0) {
    fork();
    delay(100000);
    fork();
    while(cnt < 10) {
      if(cnt == 6){
        fork();
      }
      uart_puts("Task id: ");
      uart_hex(get_taskid());
      uart_puts(", cnt: ");
      uart_hex(cnt);
      uart_puts("\n");

      delay(10000);
      ++cnt;
    }
    exit(0);
    uart_puts("Should not be printed\n");
  } else {
    uart_puts("Task ");
    uart_hex(get_taskid());
    uart_puts(" before exec, cnt address 0x");
    uart_hex(&cnt);
    uart_puts(", cnt value ");
    uart_hex(cnt);
    uart_puts("\n");
  
    exec(foo);
  }
}

void test_command1() { // test fork functionality
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      printf("task id: %d, sp: 0x%llx cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      delay(1000000);
    }
    exit(0); // all childs exit
  }
}

void test_command2() { // test page fault
  if(fork() == 0) {
    int* a = 0x0; // a non-mapped address.
    printf("%d\n", *a); // trigger simple page fault, child will die here.
  }
}

void test_command3() { // test page reclaim.
  printf("Remaining page frames : %d\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}


void main(){
}
