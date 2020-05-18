#include "user.h"
#include "user_sys.h"

void user_process() {
  call_sys_write("** user process executed **");
}
